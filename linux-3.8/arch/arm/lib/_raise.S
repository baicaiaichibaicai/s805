#include <linux/linkage.h>
#include <asm/assembler.h>
#include <asm/unwind.h>

ENTRY(raise)
		PUSH {r7} @aaa
		mov	ip, r0
		MRC 15, 0, r2, cr13, cr0, 3
		ldr	r0, [r2, #-1108]
		ldr	r3, [r2, #-1112]
		cmp	r3, #0
		bne	off30
		mov	r7, #224
		svc	0x00000000
		mov	r3, r0
		str	r0, [r2, #-1112]
		b	off48
off30:  cmp	r0, #0
		bgt	off48
		bic	r2, r0, #-2147483648
		cmp	r2, #0
		rsbne	r0, r0, #0
		moveq	r0, r3
off48:	mov	r2, ip
		mov	r1, r3
		mov	r7, #268
		svc	0x00000000
		mov	r2, r0
		cmn	r0, #4096
		bls	off7c
		ldr	r1, [pc, #108]
		ldr	r1, [pc, r1]
		rsb	r2, r0, #0
		mrc	15, 0, r0, cr13, cr0, 3
		str	r2, [r0, r1]
		b	off84
off7c:  cmn	r2, #1
		bne	offd0
off84:	ldr	r2, [pc, #80]
		ldr	r2, [pc, r2]
		mrc	15, 0, r1, cr13, cr0, 3
		ldr	r2, [r1, r2]
		cmp	r2, #38	
		mvnne	r0, #0
		bne	offd0
		mov	r1, ip
		mov	r0, r3
		mov	r7, #238
		svc	0x00000000
		cmn	r0, #4096
		bls	offd0
		ldr	r3, [pc, #32]
		ldr	r3, [pc, r3]
		rsb	r0, r0, #0
		mrc	15, 0, r2, cr13, cr0, 3
		str	r0, [r2, r3]
		mvn	r0, #0
offd0:	POP	{r7}
		bx	lr
		.word	0x0006e9fc
		.word	0x0006e9dc
		.word	0x0006e9a8
ENDPROC(raise)

