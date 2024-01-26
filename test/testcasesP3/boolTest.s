.data
	sReg0: .word 0
	sReg1: .word 0

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"

.global main
.balign 4
main:
	push {lr}

	bl Factorialcompute10
	mov r1, r0
	ldr r0, =pln
	bl printf

	bl Factorialcompute10
	mov r1, r0
	ldr r0, =pln
	bl printf


	pop {pc}
Factorial:
Factorialcompute10:
	push {lr}

	mov r0, #0
	push {r1}
	mov r1, #1
	pop {r2}
	cmp r2, r1
	mov r1, #0
	moveq r1, #1
	eor r1, r1, #1
	push {r1}
	mov r1, #0
	pop {r2}
	orr r1, r1, r2

	ldr r0, =sReg0
	str r1, [r0]

	ldr r0, =sReg0
	ldr r1, [r0]
	ldr r0, =pln
	bl printf

	mov r1, #1
	push {r1}
	mov r1, #1
	pop {r2}
	cmp r2, r1
	mov r1, #0
	moveq r1, #1
	push {r1}
	mov r1, #0
	pop {r2}
	orr r1, r1, r2

	ldr r0, =sReg0
	str r1, [r0]

		ldr r0, =sReg0
	ldr r1, [r0]

	mov r0, r1
	pop {pc}

	ldr r0, =sReg0
	ldr r1, [r0]
	ldr r0, =pln
	bl printf

	mov r1, #0
	push {r1}
	mov r1, #1
	pop {r2}
	cmp r2, r1
	mov r1, #0
	moveq r1, #1
	push {r1}
	mov r1, #1
	pop {r2}
	and r1, r1, r2

	ldr r0, =sReg1
	str r1, [r0]


	ldr r0, =sReg1
	ldr r1, [r0]

	mov r0, r1

	pop {pc}
