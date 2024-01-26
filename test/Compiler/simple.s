.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "This line should be printed. A bunch of expressions:\n"
	str1: .asciz "This line should not be printed\n"

.global main
.balign 4
main:
	push {lr}

	mov r1, #2
	push {r1}
	mov r1, #3
	pop {r2}
	cmp r2, r1
	bge if0
e0:
	ldr r0, =str0
	bl printf

	mov r1, #5
	ldr r0, =pln
	bl printf

	mov r1, #4
	add r1, r1, #3
	add r1, r1, #5
	add r1, r1, #35
	ldr r0, =pln
	bl printf

	mov r1, #32
	push {r1}
	mov r1, #70
	pop {r2}
	sub r1, r2, r1
	push {r1}
	mov r1, #2
	pop {r2}
	sub r1, r2, r1
	add r1, r1, #9
	ldr r0, =pln
	bl printf

	mov r1, #3
	add r1, r1, #5
	push {r1}
	mov r1, #17
	pop {r2}
	mul r1, r2, r1
	ldr r0, =pln
	bl printf
	b el0
if0:
	ldr r0, =str1
	bl printf

el0:

	pop {pc}
