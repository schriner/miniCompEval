.data
	sReg0: .word 0
	sReg1: .word 0
	sReg2: .word 0

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "Factorial of 10 is:"

.global main
.balign 4
main:
	push {lr}

	ldr r0, =str0
	bl printf

	bl Factorialcompute10
	mov r1, r0
	ldr r0, =pln
	bl printf


	pop {pc}
Factorial:
Factorialcompute10:
	push {lr}

	mov r1, #1

	ldr r0, =sReg1
	str r1, [r0]

	mov r1, #1

	ldr r0, =sReg0
	str r1, [r0]

lp0:
	ldr r0, =sReg1
	ldr r1, [r0]
	push {r1}
	mov r1, #10
	pop {r2}
	cmp r2, r1
	bgt d0
	ldr r0, =sReg0
	ldr r1, [r0]
	push {r1}
	ldr r0, =sReg1
	ldr r1, [r0]
	pop {r2}
	mul r1, r2, r1

	ldr r0, =sReg0
	str r1, [r0]

	ldr r0, =sReg1
	ldr r1, [r0]
	add r1, r1, #1

	ldr r0, =sReg1
	str r1, [r0]
	b lp0
d0:


	ldr r0, =sReg0
	ldr r1, [r0]

	mov r0, r1

	pop {pc}
