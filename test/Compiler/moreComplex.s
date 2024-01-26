.data

.text
	pln: .asciz "%d\n"
	p: .asciz "%d"
	str0: .asciz "This line should be printed.\n"
	str1: .asciz "This line should not be printed\n"
	str2: .asciz "This line should be printed: 3 > 4 || true\n"
	str3: .asciz "This line should not be printed\n"
	str4: .asciz "This line should not be printed\n"
	str5: .asciz "This line should be printed: 4 >= 5 || 5 <= (6*9 - 100)\n"
	str6: .asciz "This line should not be printed\n"
	str7: .asciz "This line should be printed: (0 > 1) && (2 > 3) || false\n"
	str8: .asciz "This line should not be printed\n"
	str9: .asciz "This line should be printed: (0 > 1) && (3 > 2) || false\n"
	str10: .asciz "This line should be printed: !!true\n"
	str11: .asciz "This line should not be printed\n"
	str12: .asciz "This line should not be printed\n"
	str13: .asciz "This line should be printed: !!(3 > 5)\n"
	str14: .asciz "This line should be printed: !(3 > 5)\n"
	str15: .asciz "This line should not be printed\n"

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

		mov r1, #3
	push {r1}
	mov r1, #4
	pop {r2}
	cmp r2, r1
	ble e_or_S1
	b e_or_E2
e_or_S1:
e_or_E2:
	ldr r0, =str2
	bl printf
	b el1
if1:
	ldr r0, =str3
	bl printf

el1:

		mov r1, #2
	add r1, r1, #1
	push {r1}
	mov r1, #5
	pop {r2}
	cmp r2, r1
	blt e_or_S3
	b e_or_E4
e_or_S3:
	mov r1, #5
	push {r1}
	mov r2, #6
	mov r1, #9
	mul r1, r2, r1
	sub r1, r1, #100
	pop {r2}
	cmp r2, r1
	bgt if2
e_or_E4:
	ldr r0, =str4
	bl printf
	b el2
if2:
	ldr r0, =str5
	bl printf

el2:

		mov r1, #0
	push {r1}
	mov r1, #1
	pop {r2}
	cmp r2, r1
	ble e_or_S5
	mov r1, #2
	push {r1}
	mov r1, #3
	pop {r2}
	cmp r2, r1
	ble e_or_S5
e7:
	b e_or_E6
e_or_S5:
	b if3
e_or_E6:
	ldr r0, =str6
	bl printf
	b el3
if3:
	ldr r0, =str7
	bl printf

el3:

		mov r1, #0
	push {r1}
	mov r1, #1
	pop {r2}
	cmp r2, r1
	ble e_or_S8
	mov r1, #3
	push {r1}
	mov r1, #2
	pop {r2}
	cmp r2, r1
	ble e_or_S8
e10:
	b e_or_E9
e_or_S8:
	b if4
e_or_E9:
	ldr r0, =str8
	bl printf
	b el4
if4:
	ldr r0, =str9
	bl printf

el4:

		b not11
not12:
	b if5
not11:
	ldr r0, =str10
	bl printf
	b el5
if5:
	ldr r0, =str11
	bl printf

el5:

		mov r1, #3
	push {r1}
	mov r1, #5
	pop {r2}
	cmp r2, r1
	ble not14
	b not13
not14:
	b if6
not13:
	ldr r0, =str12
	bl printf
	b el6
if6:
	ldr r0, =str13
	bl printf

el6:

		mov r1, #3
	push {r1}
	mov r1, #5
	pop {r2}
	cmp r2, r1
	ble not15
	b if7
not15:
	ldr r0, =str14
	bl printf
	b el7
if7:
	ldr r0, =str15
	bl printf

el7:


	pop {pc}
