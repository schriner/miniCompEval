.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "This line should be printed. A bunch of expressions:\n"
	str1: .asciz "This line should not be printed\n"
	str2: .asciz "If true passes\n"
	str3: .asciz "If true fails\n"
	str4: .asciz "If false fails\n"
	str5: .asciz "If false passes\n"

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
	sub r1, r1, #70
	sub r1, r1, #2
	add r1, r1, #9
	ldr r0, =pln
	bl printf

	mov r1, #3
	add r1, r1, #5
	mov r2, #17
	mul r1, r2, r1
	ldr r0, =pln
	bl printf
	b el0
if0:
	ldr r0, =str1
	bl printf

el0:

		ldr r0, =str2
	bl printf
	b el1
if1:
	ldr r0, =str3
	bl printf

el1:

		b if2
	ldr r0, =str4
	bl printf
	b el2
if2:
	ldr r0, =str5
	bl printf

el2:


	pop {pc}
