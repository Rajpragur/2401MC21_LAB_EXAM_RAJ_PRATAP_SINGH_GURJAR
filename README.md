## Name : Raj Pratap Singh Gurjar
## Roll No : 2401MC21
## Set No : Shift 1 - 2 To 4 PM

In Question 1 :  
I added the function definitions in "defs.h".  
I added the function initialization in "main.c".  
I added sem.o and synctest in the Makefile so it can run in xv6.  
In "sem.c" I created the functions for semaphores using a spinlock, sleep and wakeup.  
I added the system calls and wrote "synctest.c" to print A, B and C in order.

In Question 2 :  
I added the free page count in "kalloc.c" and update it when pages are allocated or freed.  
I added the address translation function in "vm.c" to check the page table and return the physical address.  
I added the function definitions in "defs.h" and connected both system calls.  
I added memtest in the Makefile so it can run in xv6.  
In "memtest.c" I checked the free pages and the parent and child addresses before and after fork.