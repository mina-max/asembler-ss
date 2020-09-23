.global a
.extern c

.section text:
.word c
a: .byte 4
b:
.skip 4

.section data:
.skip 6
d:
.global d

.end



