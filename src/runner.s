@ file: runner.s - Course: System Programming
@ author: Marin Markic
.text
	.global run
	.extern runAddr
	.extern stack
.func run
run:
	push {r8, r9, lr}

	ldr r8, =oldsp
	str sp, [r8]    @ save sp

	ldr r9, =stack
	ldr sp, [r9]

	ldr r9, =runAddr
	ldr r9, [r9]
	blx r9          @ jump to main

    	ldr r8, =oldsp
	ldr sp, [r8] 	@ restore sp

	pop {r8, r9, pc}

.endfunc
.data
    oldsp:.long 0

.end
