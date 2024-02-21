.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "Calling a method:\n"
	str1: .asciz "This line should not be printed\n"
	str2: .asciz "Function Called\n"
	str3: .asciz "Huzzah!\n"

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

	bl dummyFunctionfunction
	mov r1, r0
	ldr r0, =pln
	bl printf
	b el0
if0:
	ldr r0, =str1
	bl printf

el0:

	pop {pc}
dummyFunction:
dummyFunctionfunction:
	push {lr}

	mov r1, #49
	ldr r0, =pln
	bl printf

	ldr r0, =str2
	bl printf

	ldr r0, =str3
	bl printf


	mov r2, #9
	mov r1, #9
	mul r1, r2, r1
	mov r2, #1
	mul r1, r2, r1
	mov r2, #2
	mul r1, r2, r1
	mov r2, #67
	mul r1, r2, r1
	push {r1}
	mov r2, #10
	mov r1, #2
	mul r1, r2, r1
	pop {r2}
	add r1, r2, r1
	sub r1, r1, #39
	push {r1}
	mov r2, #10
	mov r1, #8
	mul r1, r2, r1
	pop {r2}
	add r1, r2, r1

	mov r0, r1

	pop {pc}
