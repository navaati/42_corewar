.name "live_test1"
.comment "live_test1"

live:
	live %:live
	ld 0, r2
	zjmp %:live
