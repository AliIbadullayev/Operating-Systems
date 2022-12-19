# Lab3 
 ## Writing kernel modules and inserting them into kernel space and getting info from kernel space using user space programms. 

 ### Used Commands 
``` bash
1. make # compile kernel objects 
2. sudo insmod procmod.ko # insert kernel object in kernel space
3. dmesg # to view logs of kernel space
4. sudo lsmod # to view all running kernel modules 
5. sudo rmmod procmod
6. gcc user.c -o user # to compile user programm
7. ./user <struct number(0 - page, 1 - thread_struct)> <pid - number> # to run user programm and get result
```
