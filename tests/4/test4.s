.global b

.section text:
	mov $2, %r0
	mov $a, (%r1)
	mov 2(%r2), b(%r3)
	movw a(%pc), b
	movb a, %r0h

.section data:
	.skip 4
	a: .skip 4
	b:

.end
