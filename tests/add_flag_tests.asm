.device ATmega328P
ldi r16, 0x01
ldi r17, 0xFF
add r16, r17   	;r16 = 0x00, C=1, Z=1, V=0

ldi r16, 0x7F
ldi r17, 0x01
add r16, r17	;r16 = 0x80, C=0, Z=0, V=1

ldi r16, 0x80
ldi r17, 0x80
add r16, r17	;r16 = 0x00, C=1, Z=1, V=1
