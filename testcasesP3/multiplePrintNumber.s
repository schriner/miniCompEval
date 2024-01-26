.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "Single Number: "
	str1: .asciz "\nSingle Number:\n"

.global main
.balign 4
main:
	push {lr}

	ldr r0, =str0
	bl printf

	mov r1, #42
	ldr r0, =p
	bl printf

	ldr r0, =str1
	bl printf

	mov r1, #42
	ldr r0, =pln
	bl printf


	pop {pc}
