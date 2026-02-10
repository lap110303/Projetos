# getcnt syscall on xv6

by Lucas Pedras and Lucas Affonso - developed as an assignment for the Operating Systems course at UFMG

## Install dependencies 

```sh
sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

## How to run

```sh
cd xv6
make qemu
```

## Assignment [instructions](https://docs.google.com/document/d/1RPtNYEmhi5r3JXTT8LiylOG4LGp0XHGGFfbohQ8TYWM/edit)

### Instructions on getting Xv6-RISCV running
- Installing dependencies: [Tools Used in 6.1810](https://pdos.csail.mit.edu/6.1810/2023/tools.html)
- xv6 RISC-V repository: [xv6 RISC-V](https://github.com/mit-pdos/xv6-riscv)

### Resources
- Recommended reading: [xv6 book](https://pdos.csail.mit.edu/6.S081/2020/xv6/book-riscv-rev1.pdf) Chapters 1 and 2 plus Sections 4.3 and 4.4.
- Using GDB do help with debugging:
    - See the first part of this page: [Using gdb (easy)](https://pdos.csail.mit.edu/6.1810/2023/labs/syscall.html)
- Similar assignment:
    - [Intro To Kernel Hacking](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/initial-xv6/README.md)

### Tasks
- Implement a getcnt syscall
    - Modify xv6 so the kernel keeps track of how many times each syscall has been called
    - The getcnt syscall receives one integer parameter. It returns the number of times the syscall with the given number has been called. The userspace declaration of getcnt should be **int getcnt(int)**
    - Relevant files
        - Modified kernel/syscall.h
        - Modified kernel/syscall.c
        - Modified kernel/sysproc.c
        - Modified kernel/proc.c

        TODO: add lock to prevent data races and ask Italo about the following
        - sysfile.c (not modified, ask Italo about it)
        - proc.h (for trapframe)

- Implement a user-space program to call the syscall. The program should be named **getcnt** and receive a single integer as parameter corresponding to the target syscall.
    - Relevant files
        - Modified user/user.h
        - Modified user/usys.pl (auto-generates user/usys.S)
        - Modified Makefile (add your program to the UPROGS variable)
        - Created file user/getcnt.c
    - Example execution (22 is the number of the getcnt syscall in this implementation)

```
$ ./getcnt 1
syscall 1 has been called 4 times
$ ./getcnt 22
syscall 22 has been called 2 times
$ ./getcnt 22
syscall 22 has been called 3 times
$ ./getcnt 22
syscall 22 has been called 4 times
```

###  What to submit

You should upload a zip containing the complete version of your modified xv6 with the getcnt syscall. You should also upload a documentation in PDF format with at least the following information:

- Show and discuss four (4) modifications you made to the xv6 code; use the [git diff] command to compute the changes between the original xv6 code and your modified version.
    - Show the diff and discuss the data structure you use to keep track of the number of calls to each syscall
    - Show the diff and discuss how you update the data structure
    - Choose two more parts of the code you find relevant to discuss using diffs
- How you tested your syscall