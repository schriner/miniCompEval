.data
	sReg0: .word 0
	sReg1: .word 0

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "Divide 7/3:"
	str1: .asciz "Divide 6/3:"
	str2: .asciz "Divide 0/3:"
	str3: .asciz "Divide 2/3:"
	str4: .asciz "Divide -2/3:"
	str5: .asciz "Divide -6/3:"
	str6: .asciz "Divide -7/3:"
	str7: .asciz "Factorial of 10 is:"

.global main
.balign 4
main:
	push {lr}

	ldr r0, =str0
	bl printf

	mov r1, #7
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div0
	mov r4, #1
	rsb r2, #0
div0:
	cmp r2, r1
	blt d0
	sub r2, r2, r1
	add r0, r0, #1
	b div0
d0:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str1
	bl printf

	mov r1, #6
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div1
	mov r4, #1
	rsb r2, #0
div1:
	cmp r2, r1
	blt d1
	sub r2, r2, r1
	add r0, r0, #1
	b div1
d1:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str2
	bl printf

	mov r1, #0
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div2
	mov r4, #1
	rsb r2, #0
div2:
	cmp r2, r1
	blt d2
	sub r2, r2, r1
	add r0, r0, #1
	b div2
d2:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str3
	bl printf

	mov r1, #2
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div3
	mov r4, #1
	rsb r2, #0
div3:
	cmp r2, r1
	blt d3
	sub r2, r2, r1
	add r0, r0, #1
	b div3
d3:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str4
	bl printf

	mov r1, #-2
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div4
	mov r4, #1
	rsb r2, #0
div4:
	cmp r2, r1
	blt d4
	sub r2, r2, r1
	add r0, r0, #1
	b div4
d4:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str5
	bl printf

	mov r1, #-6
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div5
	mov r4, #1
	rsb r2, #0
div5:
	cmp r2, r1
	blt d5
	sub r2, r2, r1
	add r0, r0, #1
	b div5
d5:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str6
	bl printf

	mov r1, #-7
	push {r1}
	mov r1, #3
	pop {r2}
	mov r0, #0
	mov r4, #0
	cmp r2, #0
	bge div6
	mov r4, #1
	rsb r2, #0
div6:
	cmp r2, r1
	blt d6
	sub r2, r2, r1
	add r0, r0, #1
	b div6
d6:
	mov r1, r0
	cmp r4, #1
	rsbeq r1, #0
	ldr r0, =pln
	bl printf

	ldr r0, =str7
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

lp7:
	ldr r0, =sReg1
	ldr r1, [r0]
	push {r1}
	mov r1, #10
	pop {r2}
	cmp r2, r1
	bgt d7
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
	b lp7
d7:


	ldr r0, =sReg0
	ldr r1, [r0]

	mov r0, r1

	pop {pc}
