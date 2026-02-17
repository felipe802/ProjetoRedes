/*
The illusion of Memory - Dissecting fork() and Copy-on-Write (CoW)
Demonstration of how the kernel lies to processes using identical virtual
addresses that map to different physical memory only when necessary. 
Linux/BSD kernel space -> user space via syscalls
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // Provides the OS API (fork, getpid)
#include <sys/wait.h>   // Provides the wait() syscall

int main() {
    // 1. Allocate memory on the heap for a shared variable 
    // The 'shared_var' pointer holds the Virtual Adress of this allocation
    int *shared_var = malloc(sizeof(int));
    if (shared_var == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    *shared_var = 42; // The universal truth value

    printf("\n[PARENT - PID: %d] Phase 1: Before cloning.\n", getpid());
    printf("  -> Pointer 'shared_var' points to Virtual Address: %p\n", (void*)shared_var);
    printf("  -> Stored Value: %d\n\n", *shared_var);

    /*
    The magic happens here
    The fork() syscall clones the current process. From this line onward,
    Two processes will execute the next instruction in parallel.
    But physical memory (RAM) HAS NOT BEEN DUPLICATED YET. The kernel uses
    "Copy-on-Write" (Cow) for efficiency.
    */
    pid_t pid = fork();

    if (pid < 0) {
        // Catastrophic kernel failure (e.g. process limit exceeded)
        perror("fork() failed");
        return 2;
    }

    if (pid == 0) {
        // Child process context
        // The child receives pid = 0. It has an exact copy of the parent's memory map
        printf("[CHILD - PID: %d] Phase 2: Right after birth.\n", getpid());

        // The kernel's illusion: notice that the printed address will be EXACTLY THE SAME as the parent's!
        // They shate the exact same physical RAM in this milisecond
        printf("  -> Adress of 'shared_var' seen by child: %p (Identical to parent!)\n", (void*)shared_var);
        printf("  -> Inherited value: %d\n\n", *shared_var);

        /*
        Breaking the matrix (triggered copy-on-write)
        When attempting to write (modify) the data, the processor's Memory
        Management Unit (MMU) issues an alert (Page Fault).
        The kernel intercepts it, pauses the child process for microseconds,
        silently allocates a new physical block of RAM, and updates the mapping.
        */
       printf("[CHILD - PID: %d] Action: Overwriting the value to 99...\n", getpid());
       *shared_var = 99; // <--- copy-on-write happens exaclty here

       printf("[CHILD - PID: %d] Phase 3: After the modification.\n", getpid());
       // The Virtual address remains the same so as not to break C pointers,
       // but the PHYSICAL RAM behind it is now exclusive to the child
       printf("  -> Virtual Address: %p (Still the same)\n", (void*)shared_var);
       printf("  -> New value: %d\n\n", *shared_var);

       free(shared_var);
       exit(EXIT_SUCCESS); // Child terminates execution
    }
    /*
    When call fork(), the OS creates a new process by duplicanting the current one.
    From that point forward, you have two independent processes running the same program.
    In the child process, fork() returns 0. That is why the if(pid==0) branch executes there.
    In the parent process, fork() return the PID of the child (a positive integer). That is
    why the else branch executes in the parent.
    Both branches do not run in the same process, but two processes exist, and each one takes 
    a different path depending on the return value of fork(). 
    */
    else {
        // Parent process context
        // The parent receives the actual PID of the child
        // The parent must wait for the child to finish to avoid "zombie" processes
        wait(NULL); // Take off this line to test!!
        printf("[PARENT - PID: %d] Phase 4: The child finished and died.\n", getpid());
        
        // The parent was unaffected by the child's write operation. Physical operation
        // was guaranteed by the kernel via copy-on-write
        printf("  -> Virtual Adress: %p\n", (void*)shared_var);
        printf("  -> Intact value: %d (The proof of isolation)\n\n", *shared_var);

        free(shared_var);
    }
    return(EXIT_SUCCESS);
}