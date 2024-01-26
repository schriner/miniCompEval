.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "The answer\n"
	str1: .asciz "to the meaning\n"
	str2: .asciz "of life\n"
	str3: .asciz "is:\n"
	str4: .asciz "42\n"

.global main
.balign 4
main:
	push {lr}

	ldr r0, =str0
	bl printf

	ldr r0, =str1
	bl printf

	ldr r0, =str2
	bl printf

	ldr r0, =str3
	bl printf

	ldr r0, =str4
	bl printf


	pop {pc}
