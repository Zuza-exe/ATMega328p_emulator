.device ATmega328P
;jumps between A and B in loop, skipping nop instructions

rjmp A
nop
nop
A:
	rjmp B
	nop
B:
	rjmp A
