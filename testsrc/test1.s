@test1.s - writes number of arguments

.section .text
	.global main
	.extern printf
main:
	push {r0, r1, lr}
	mov r1, r0 @r1 = argc
	ldr r0, =format @r0 = format
	bl printf
	pop {r0, r1, pc}

.section .data
format: .asciz "Number of arguments is: %d\n"
.end
