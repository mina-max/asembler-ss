.global b

.section text:
.equ a, 4+b
.skip 4
b:

.section data:
.equ c, -e+2
.skip 2
d: .equ e, d+5

.end

