.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "This is a single print statement.\nWith newlines\nTo add interest.\nHuzzah! :D\n"

.global main
.balign 4
main:
	push {lr}

	ldr r0, =str0
	bl printf

	pop {pc}
