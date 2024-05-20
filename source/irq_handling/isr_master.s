@register usage
	@r0: 0x4000000/0x4000200
	@r1: IE
	@r2: priority list ptr
	@r3: scratch
	@r12: scratch

.section .iwram,"ax", %progbits
    .align  2
    .code   32
	.global irqMasterServiceRoutine
	.global irqPriorityTable
	.type irqMasterServiceRoutine STT_FUNC

irqMasterServiceRoutine:
	ldr 	r1, [r0, #0x200]!			@load IE and IF				***NOTE: We assume every bit in IE is set
	str		r1, [r0]					@awknowledge the IF flags of every active interrupt
	and		r1, r1, r1, ror #16			@duplicate the active interrupts to both halves of the register
	adr		r2, irqPriorityTable		@ready a pointer to the priority list
findHighestInterrupt:
	ldr		r3, [r2], #12				@load 2 entries from the priority list
interruptReentryPoint:					@this is where we reenter after processing an interrupt but there are more to do
	tst		r1, r3						@test if either interrupt is enabled
	beq		findHighestInterrupt		@repeat this loop if neither find a match
matchFound:
	ands	r12, r3, r1, lsr #16		@check if the first entry is a match
	bic		r3, r3, r12					@clear the found interrupt from the priority list
	strh	r3, [r0]					@disable all interrupts equal or lower than this one
	orreq	r12, r3, r3, lsr #16		@duplicate the found interrupt to both halves of the register if the second entry matched
	ldrne	r3, [r2, #-8]				@load the first function pointer if the first entry found a match
	ldreq	r3, [r2, #-4]				@load the second function pointer if the second entry found a match
	orrne	r12, r12, r12, lsl #16		@if the first entry found a match, duplicate the found interrupt to both halves of the register
	bics	r1, r1, r12					@clear the found interrupt from the active interrupts. Check if this is the last interrupt
	mrs		r12, spsr					@load the spsr into r3
	msr		cpsr_c, #0x1f				@enable interrupts, switch to system mode
	pushne	{r0 - r2, r12, r14}			@push all registers that I need to keep once the function returns
	adrne	r14, userISRReturn			@set the return address if we have more interrupts to handle after this
	pusheq	{r0, r12, r14}				@if this is the last interrupt, we dont need to push as many registers
	adreq	r14, userISRExit			@set the address if we can just leave right afterwards
	bx		r3							@branch to the interrupt service routine for this particular interrupt
userISRReturn:
	pop		{r0 - r2, r12, r14}			@restore all the registers that still have useful data
	msr		cpsr_c, #0x92				@disable interrupts, switch to irq mode
	msr		spsr, r12					@restore the spsr in case another interrupt trashed it
	ldr		r3, [r2, #-12]				@load the same priority list as before, in case they were both set
	b		interruptReentryPoint		@reenter the interrupt search loop
userISRExit:
	pop		{r0, r12, r14}				@restore the registers that were pushed earlier
	msr		cpsr_c, #0x92				@disable interrupts, switch to irq mode
	msr		spsr, r12					@restore the spsr in case another interrupt trashed it
	mov		r1, #0xffff3fff				@prepare to fill IE with all ones
	strh	r1, [r0]					@enable every interrupt in IE
	pop		{r0-r3, r12, r14}			@restore the registers pushed by the BIOS
	subs	r15, r14, #0x4				@return to previous execution

irqPriorityTable:						@first word has the two priority checks, second and third word are corresponding function pointers
	.word	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
