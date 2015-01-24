@test2.s - square root

.section .text
	.extern printf
	.extern scanf
	.global main
main:
	push {r0, r1, r2, lr}
	
	@ input message
	ldr r0, =input
	bl printf
	
	@ citanje vrednosti
	ldr r0, =value
	ldr r1, =number
	bl scanf

	ldr r1, =number
	ldr r1, [r1] @ number is in r1
	mul r2, r1, r1 @ square root is in r2
	
	@result
	ldr r0, =output
	bl printf
	
	pop {r0, r1, r2, pc}

.section .data
	input: .asciz "Enter integer:"
	value: .asciz "%d"
	number: .long 0
	output: .asciz "Square root of %d is %d."
.end
