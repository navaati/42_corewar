.name "fork_test2"
.comment "fork_test2"

ld %1, r2
test:
	add r2, r3, r3
	st r3, 42
	live %-1
	fork %:test
