.name "arno"
.comment "arnoarnoarno"

live:
	add r1, r2, r3
	ld 12345678, r2
	st r3, 0
	st r2, 84
	sub r3, r1, r2
	add r1, r2, r3
	sub r3, r1, r2
	st r3, 42
	st r2, 0
	sub r3, r1, r2
	st r3, 42
	st r2, 0
	ld %0, r2
	zjmp %:live
