.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "20 + 22 = "
	str1: .asciz "\n1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 =\n"

.global main
.balign 4
main:
	push {lr}

	ldr r0, =str0
	bl printf

	mov r1, #22
	add r1, r1, #20
	ldr r0, =p
	bl printf

	ldr r0, =str1
	bl printf

	mov r1, #2
	add r1, r1, #1
	add r1, r1, #3
	add r1, r1, #4
	add r1, r1, #5
	add r1, r1, #6
	add r1, r1, #7
	add r1, r1, #8
	add r1, r1, #9
	add r1, r1, #10
	ldr r0, =pln
	bl printf


	pop {pc}
