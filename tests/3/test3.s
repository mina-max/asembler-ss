.global b

.section text:
	jmp 3
	jmp a
	jmp *%r0
	jmp *(%r1)
	jmp *4(%r2)
	jmp *a(%r3)
	jmp *a(%pc)
	jmp *b(%pc)
	jmp *c(%pc)
	.skip 6
	c:
	jmp *10
	jmp *a

.section data:
	.skip 4
	a: .skip 4
	b:

.end

