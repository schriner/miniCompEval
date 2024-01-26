.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"

.global main
.balign 4
main:
	push {lr}

	mov r2, #-1
	mov r1, #1
	mul r1, r2, r1
	sub r1, r1, #2
	mov r2, #3
	mul r1, r2, r1
	ldr r0, =pln
	bl printf

	mov r1, #-1
	sub r1, r1, #2
	rsb r1, r1, #0
	mov r2, #3
	mul r1, r2, r1
	ldr r0, =pln
	bl printf

	mov r1, #1
	sub r1, r1, #1
	sub r1, r1, #2
	sub r1, r1, #3
	ldr r0, =pln
	bl printf

	mov r2, #-1
	mov r1, #1
	mul r1, r2, r1
	sub r1, r1, #2
	mov r2, #3
	mul r1, r2, r1
	ldr r0, =pln
	bl printf


	pop {pc}
