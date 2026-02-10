#define NPROC        64  // maximum number of processes
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS 10         // max # of blocks any FS op writes
#define LOGBLOCKS (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF (MAXOPBLOCKS*3)       // size of disk block cache
#define FSSIZE 2000            // size of file system in blocks
#define MAXPATH      128   // maximum file path name


/* --- adicionados para compatibilidade mkfs / log --- */
#ifndef MAXOPBLOCKS
#define MAXOPBLOCKS 10             /* max # of blocks any FS op writes */
#endif

#ifndef LOGSIZE
#define LOGSIZE (MAXOPBLOCKS*3)    /* max data blocks in on-disk log (kernel uses LOGSIZE) */
#endif

#ifndef LOGBLOCKS
#define LOGBLOCKS LOGSIZE          /* compatibilidade com mkfs.c que usa LOGBLOCKS */
#endif

#ifndef NBUF
#define NBUF (MAXOPBLOCKS*3)       /* cache de blocos do disco */
#endif

#ifndef FSSIZE
#define FSSIZE 2000                /* tamanho do FS em blocos (ajuste se precisar) */
#endif
/* --- fim --- */

