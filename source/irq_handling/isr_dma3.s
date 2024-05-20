

.section .iwram,"ax", %progbits
    .align  2
    .code   32
	.global DMA3ISR
	.type DMA3ISR STT_FUNC
	
DMA3ISR:
	pop		{r0, r12, r14}				@restore the registers that were pushed earlier
	msr		cpsr_c, #0x92				@disable interrupts, switch to irq mode
	msr		spsr, r12					@restore the spsr in case another interrupt trashed it
	mov		r1, #0xffff3fff				@prepare to fill IE with all ones
	strh	r1, [r0]					@enable every interrupt in IE
	pop		{r0-r3, r12, r14}			@restore the registers pushed by the BIOS
	msr		cpsr_c, #0x9f				@keep interrupts disabled, but switch to system mode
	push	{r0-r5, r12, r15}			@push some of the registers
	mov		r0, #0x4000000				@get the address for IME
	mov		r1, #0x0					@prepare to disable IME
	strb	r1, [r0, #0x208]			@disable IME
	msr		cpsr_c,	#0x92				@switch back to irq mode, interrupts still disabled
	mov		r1, r14						@put the system r15 into r1
	mrs		r0, spsr					@get the spsr
	msr		cpsr_c,	#0x1f				@switch back to system mode, enable interrupts again
	str		r1, [r13, #28]				@store the system pc into the spot we left for it
	push	{r0, r14}					@push the spsr and r14
	ldr 	r15, =gameLoop				@start at the beginning of the game loop
















@let me scroll down please
