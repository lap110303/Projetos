#include <sys/mman.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// We only need to interface with the MMU module.
#include "mmu.h"
#include "pager.h"

// Macros for address conversion
#define PAGE_SIZE           sysconf(_SC_PAGESIZE)

#define ADDR_TO_PAGE(addr)  ((intptr_t) (addr - UVM_BASEADDR) / PAGE_SIZE)
#define PAGE_TO_ADDR(page)  (UVM_BASEADDR + page * PAGE_SIZE)

// Pager data
struct frame_data {
	pid_t pid;
	int page; // Reverse index
	int prot; /* Starts with PROT_READ. Clock assigns PLOT_NONE. 1 fault for PROT_NONE -> PROT_READ. 1 extra fault for PROT_READ -> PROT_WRITE*/
	int is_free; /* 1 if available */
    int dirty; // 1 if dirty (was assigned PROT_WRITE) at some point, need to write block when erasing frame
};

struct page_data {
	int block; /* block assigned to page. **EVERY PAGE HAS A BLOCK ASSOCIATED, EVEN IF NEVER USED, therefore this is never -1** */
	int on_disk; /* 1 indicates page was written to disk -> Determines if we call fill() or not when pulling from block to frame*/ 
	int frame; /* -1 indicates non-resident, otherwise index of pager->frames*/
};

struct proc {
	pid_t pid;
	int npages;
	int maxpages;
	struct page_data *pages;
};

struct pager {
    // Mutex for synchronization
	pthread_mutex_t mutex;

    // Memory info
	int nframes;
	int frames_free;
    int nblocks;
	int blocks_free;

    // For page replacement
    int clock; 

    // Frame and process info
    struct frame_data *frames;
	pid_t *block2pid;
	struct proc **pid2proc;
};

// Local pager
static struct pager pager;

/*
* Auxiliary functions
*/

/**
 * @brief Converts a PID to an index for pid2proc lookup.
 * This is necessary since PIDs can be arbitrary (can't index with them directly).
 * 
 * If no process with this PID is found, returns the first free index in pid2proc.
 * 
 * @param pid Process PID to convert to index.
 * @return Index of the process in pid2proc or the first free index.
 */
int pid2idx(pid_t pid) {
    int free_p = -1;

    for (int p = 0; p < pager.nblocks + pager.nframes; p++) {
        if (pager.pid2proc[p] == NULL && free_p == -1)
            free_p = p;

        if (pager.pid2proc[p] != NULL && pager.pid2proc[p]->pid == pid)
            return p;
    }

    return free_p;
}


/**
 * @brief Clock algorithm for page replacement.
 * 
 * Selects a victim frame to be evicted and sends it to disk.
 * 
 * @return Index of frame freed.
 */
int clock_algorithm() {
    int victim = -1;

    while (victim == -1) {
        // Check if frame has permissions -> has been accessed
        int current_frame_in_algorithm = pager.clock;
        struct frame_data* frame = &pager.frames[current_frame_in_algorithm];

        // Give second chance by resetting permissions (this way we will know if this frame was accessed)
        if (frame->prot != PROT_NONE) { 
            frame->prot = PROT_NONE;
            mmu_chprot(frame->pid, (void *) PAGE_TO_ADDR(frame->page), frame->prot);
        } 
        else { // Found victim
            victim = current_frame_in_algorithm;
            struct proc* proc = pager.pid2proc[pid2idx(frame->pid)];

            // Set non-resident
            proc->pages[frame->page].frame = -1; 
            mmu_nonresident(frame->pid, (void *) PAGE_TO_ADDR(frame->page));

            // If dirty, write to associated block
            if (frame->dirty) { 
                int block = proc->pages[frame->page].block;
                mmu_disk_write(victim, block);
                proc->pages[frame->page].on_disk = 1; // Do not fill with 0s anymore!
            }

            // Frame sent to disk
            pager.frames_free++;
        }

        // Advance clock
        pager.clock = (pager.clock + 1) % pager.nframes;
    }

    return victim;
}


/**
 * @brief Selects a new free frame to be used. 
 * 
 * If there is no free frame (memory full), the clock algorithm is applied to select a victim frame to be evicted.
 * 
 * @return Index of the frame to be used (either a free frame or a victim frame).
 */
int get_free_frame() {
    int frame = -1;
    if (pager.frames_free > 0) { // Find a free one
        for (frame = 0; frame < pager.nframes; frame++) {
            if (pager.frames[frame].is_free == 1) break;
        }
    }
    else { // No free frame, apply clock algorithm
        frame = clock_algorithm();
    }

    return frame;
}


/**
 * @brief Switches the permissions of a frame between PROT_NONE and PROT_READ | PROT_WRITE.
 * 
 * DOES NOT update MMU permissions. This function is only used internally. Be sure to use mmu_chprot() afterwards.
 * 
 * @param frame Frame index to update permissions.
 */
void update_frame_permissions(int frame) {
    if (pager.frames[frame].prot == PROT_NONE) {
        pager.frames[frame].prot = PROT_READ;
    }
    else {
        pager.frames[frame].prot = PROT_READ | PROT_WRITE;
        pager.frames[frame].dirty = 1;
    }
}

/*
* Main pager functions
*/

void pager_init(int nframes, int nblocks) {
    // Init mutex
    pthread_mutex_init(&pager.mutex, NULL);

    // Available memory
    pager.nframes = pager.frames_free = nframes;
    pager.nblocks = pager.blocks_free = nblocks;

    pager.clock = 0;

    // Allocate memory for frames, blocks and lookup tables
    pager.frames = malloc(nframes * sizeof(struct frame_data));
    pager.block2pid = malloc(nblocks * sizeof(pid_t));

    // Max of nblocks + nframes processes
    pager.pid2proc = malloc((nblocks + nframes) * sizeof(struct proc*));

    // Init frames
    for (int f = 0; f < nframes; f++) {
        pager.frames[f].prot = PROT_NONE;
        pager.frames[f].is_free = 1;
        pager.frames[f].page = -1;
        pager.frames[f].pid = -1;
        pager.frames[f].dirty = 0;
    }

    // Init lookup tables
    for (int b = 0; b < nblocks; b++)
        pager.block2pid[b] = -1;

    for (int p = 0; p < nblocks + nframes; p++)
        pager.pid2proc[p] = NULL;
}

void pager_create(pid_t pid) {
    pthread_mutex_lock(&pager.mutex);

    // We will not initialize the memory for the process yet
    // We will only initialize its proc struct
    struct proc* proc = malloc(sizeof(struct proc));

    proc->pid = pid;
    proc->npages = 0;
    proc->maxpages = (UVM_MAXADDR - UVM_BASEADDR + 1) / PAGE_SIZE; // Max pages for this address space
    proc->pages = (struct page_data*) malloc(sizeof(struct page_data) * proc->maxpages);

    // Add to lookup table
    int idx_pid = pid2idx(pid);
    pager.pid2proc[idx_pid] = proc;

    pthread_mutex_unlock(&pager.mutex);
}

void *pager_extend(pid_t pid) {
    pthread_mutex_lock(&pager.mutex);

    // Get proc struct
    int idx_pid = pid2idx(pid);
    struct proc* proc = pager.pid2proc[idx_pid];

    // Allocating the block
    int block = -1;
    for (int b = 0; b < pager.nblocks; b++) {
        if (pager.block2pid[b] == -1) { // Find free block
            block = b;
            break;
        }
    }

    // No free blocks
    if (block == -1) {
        pthread_mutex_unlock(&pager.mutex);
        return NULL;
    }
    
    int proc_num_of_pages = proc->npages;

    // Check if new page is within bounds
    if (proc_num_of_pages + 1 > proc->maxpages) {
        pthread_mutex_unlock(&pager.mutex);
        return NULL;
    }

    // Assign page a block, but not a frame. We will init its frame when faulting
    proc->pages[proc_num_of_pages].block = block;
    proc->pages[proc_num_of_pages].on_disk = 0;
    proc->pages[proc_num_of_pages].frame = -1;
    proc->npages++;

    pager.blocks_free--;
    pager.block2pid[block] = proc->pid;

    void* vaddr = (void *) PAGE_TO_ADDR((proc->npages - 1));

    pthread_mutex_unlock(&pager.mutex);
    
    return vaddr;
}

/**
 * @brief Handles a page fault for a process, selecting a new free frame if the page is not on disk OR
 * changing the frame permissions if the page is already in memory.
 * 
 * @param pid Process PID that caused the fault.
 * @param addr Address that caused the fault.
 */
void pager_fault(pid_t pid, void *addr) {
    pthread_mutex_lock(&pager.mutex);

    int idx_pid = pid2idx(pid);
    struct proc* proc = pager.pid2proc[idx_pid];
    int page = ADDR_TO_PAGE(addr);

    // Check if page is valid (exists)
    if (page >= proc->npages) {
        exit(EXIT_FAILURE); // This should never happen, MMU/UVM should only call fault for valid pages
    }

    // Page exists. Either it's not on disk OR needs read/write protection
    if (proc->pages[page].frame == -1) { // Not on memory
        int frame = get_free_frame();

        // Update frame
        pager.frames[frame].pid = pid;
        pager.frames[frame].is_free = 0;
        pager.frames[frame].page = page;
        pager.frames[frame].prot = PROT_READ;
        pager.frames[frame].dirty = 0;
        pager.frames_free--;

        proc->pages[page].frame = frame;

        // Get actual frame data
        if (proc->pages[page].on_disk) {
            mmu_disk_read(proc->pages[page].block, frame);
            proc->pages[page].on_disk = 1;
        }
        else { // If frame is not on disk yet -> fill with 0s
            mmu_zero_fill(frame);
        }

        // Set as resident
        void *vaddr = (void *) PAGE_TO_ADDR(page);
        mmu_resident(pid, vaddr, frame, pager.frames[frame].prot);
    }
    else { // In memory, but needs permissions
        /* 
        * If the page fault is due to a write operation, we need to change the permissions to PROT_WRITE:
        * IF NONE -> SET READ  else  IF READ -> SET WRITE + ASSIGN DIRTY BIT
        */

        int frame = proc->pages[page].frame;
        
        update_frame_permissions(frame);

        // Update permissions in MMU
        void *vaddr = (void *) PAGE_TO_ADDR(page);
        mmu_chprot(pid, vaddr, pager.frames[frame].prot);
    }
    
    pthread_mutex_unlock(&pager.mutex);
}

int pager_syslog(pid_t pid, void *addr, size_t len) {
    pthread_mutex_lock(&pager.mutex);

    int idx_pid = pid2idx(pid);
    struct proc* proc = pager.pid2proc[idx_pid];

    if (proc == NULL) {
        pthread_mutex_unlock(&pager.mutex);
        return -1;
    }

    int start_page = ADDR_TO_PAGE(addr);

    // Check if address is valid
    // - cannot be negative
    // - cannot be bigger than the number of pages for this process
    // - len cannot be bigger than the available memory for this process    
    int negative_address = start_page < 0;
    int address_out_of_bounds = start_page >= proc->npages;
    int len_out_of_bounds = len > (proc->npages - start_page) * PAGE_SIZE;

    if (negative_address || address_out_of_bounds || len_out_of_bounds) {
        pthread_mutex_unlock(&pager.mutex);
        return -1;
    }

    // Init buffer
    unsigned char* buf = malloc(len);

    if (buf == NULL) {
        pthread_mutex_unlock(&pager.mutex);
        return -1;
    }

    // Copy memory to buffer, abort if non-resident
    int page_idx = start_page;

    for (int bytes_copied = 0; bytes_copied < len; bytes_copied += PAGE_SIZE) {
        int frame = proc->pages[page_idx].frame;

        // Non-resident page, free buffer and return error. UVM can handle the fault.
        if (frame == -1) {
            free(buf);
            pthread_mutex_unlock(&pager.mutex);
            return -1;
        }

        // Copy memory to buffer
        void* dest = buf + bytes_copied;
        const void* src = pmem + frame * PAGE_SIZE;

        // Default size to copy
        int size = PAGE_SIZE; 

        // Last page
        if (bytes_copied + PAGE_SIZE > len) 
            size = len - bytes_copied;

        memcpy(dest, src, size);

        page_idx++; // Next page (loop will exit on bytes_copied)
    }

    // Memory is valid, so we print it
    for(size_t i = 0; i < len; i++)
        printf("%02x", buf[i]);
    printf("\n");

    free(buf);

    pthread_mutex_unlock(&pager.mutex);

    // Syslog OK
    return 0;
}

void pager_destroy(pid_t pid) {
    pthread_mutex_lock(&pager.mutex);

    // We don't need to call MMU here
    // we will just free every frame and block this process has used
    int idx_pid = pid2idx(pid);
    struct proc* proc = pager.pid2proc[idx_pid];

    if (proc == NULL) {
        pthread_mutex_unlock(&pager.mutex);
        return;
    }

    // Loop through all pages using pointer math yay :)
    const struct page_data* max_number_of_pages = proc->pages + proc->npages;

    for (struct page_data* page = proc->pages; page < max_number_of_pages; page++) {
        if (page->frame != -1) {
            // Free frame
            pager.frames[page->frame].pid = -1;
            pager.frames[page->frame].page = -1;
            pager.frames[page->frame].prot = PROT_NONE;
            pager.frames[page->frame].is_free = 1;
            pager.frames[page->frame].dirty = 0;
            pager.frames_free++;
        }

        // Every page is associated with a block, so also free it
        pager.block2pid[page->block] = -1; 
        pager.blocks_free++;
    }

    // Free proc struct
    free(proc);
    pager.pid2proc[idx_pid] = NULL;

    pthread_mutex_unlock(&pager.mutex);
}