	.arch msp430g2553
	.p2align 1,0
	.text

	.extern buzzer_set_period
	.global playTune

jt:
	.word default
	.word case1
	.word case2
	.word case3
	.word case4

playTune:
	cmp #4, r12
	jhs default

	add r12, r12
	mov jt(r12), r0

case1:
	mov #500, r12
	call #buzzer_set_period
	pop r0

case2:
	mov #1000, r12
	call #buzzer_set_period
	pop r0

case3:
	mov #1500, r12
	call #buzzer_set_period
	pop r0

case4:
	mov #2000, r12
	call #buzzer_set_period
	pop r0

default:
	pop r0
