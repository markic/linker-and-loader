# linker-and-loader
Linker and loader for elf relocatable files. Developed in C for ARM architecture.

This was a project for the course ”System Programming” on my faculty.


Department of Computer Engineering and Information Theory.


School of Electrical Engineering, University of Belgrade, Serbia.


Developed by Marin Markić. No licence. April - June 2013.
- marinmarkic@mail.com


### How to use: (see example)

- Application works with binary relocatable files in elf format, called program. Application links program with other libraries, performs necessary relocations, loads it to operating memory and executes program code from entry point. Programs are acccepted as ARM assembly or C programs. In case of C program additionally external functions printf and scanf are supported. 
- Use QEMU, machine emulator and virtualizer for ARM architecture on linux. Start application by executing run.sh in bash.

### Create binaries:
```asm
arm-linux-gnueabi-as test1.s -o test1.o // creates elf from ARM assembly file
arm-linux-gnueabi-gcc -r -c -nostdlib -marm test3.c -o test3.o // creates elf from C file
```

Pass created .o files to application to perform execute program with linking and loading.