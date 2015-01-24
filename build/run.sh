 #!/bin/bash  

export QEMU_LD_PREFIX=/usr/arm-linux-gnueabi
arm-linux-gnueabi-gcc -marm runner.s main.c -o main
./main   

read -p "Press any key to continue... " -n1 -s
