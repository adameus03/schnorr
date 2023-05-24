.text
	.global add_buffers
	.global sub_buffers
	.global mul_buffers
	.global mod_buffer
add_buffers:
# rcx - addr of first input buffer
# rdx - addr of second input buffer
# r8 - addr of output buffer
# r9 - len of each buffer(8B blks)
	andq %r9, %r9
	jz fin_add

	xorq %rax, %rax

	#movq (%rcx), %rbx
	#addq (%rdx), %rbx

	#lahf
	#rorw $0x8, %ax
	#movq %rbx, (%r8)

	#iter: decq %r9
	iter_add: andq %r9, %r9
	jz fin_add
	#addq $0x8, %rcx
	#addq $0x8, %rdx
	#addq $0x8, %r8
	movq %rax, %rbx
	addq (%rcx), %rbx
	lahf
	movb %ah, %al
	addq (%rdx), %rbx
	lahf
	orb %ah, %al #xorb
	andw $0x1, %ax
	#rorw $0x8, %ax
	movq %rbx, (%r8)
	#
	addq $0x8, %rcx
	addq $0x8, %rdx
	addq $0x8, %r8
	decq %r9
	#
	jmp iter_add

	fin_add: ret


sub_buffers:
# rcx - addr of first input buffer
# rdx - addr of second input buffer
# r8 - addr of output buffer
# r9 - len of each buffer(8B blks)
	andq %r9, %r9
	jz fin_sub

	xorq %rsi, %rsi
	xorq %rax, %rax

	#movq (%rcx), %rbx
	#addq (%rdx), %rbx

	#lahf
	#rorw $0x8, %ax
	#movq %rbx, (%r8)

	#iter: decq %r9
	iter_sub: andq %r9, %r9
	jz fin_sub
	#addq $0x8, %rcx
	#addq $0x8, %rdx
	#addq $0x8, %r8
	# movq %rax, %rbx
	movq (%rcx), %rbx

	subq (%rdx), %rbx
	lahf
	movb %ah, %al
	subq %rsi, %rbx
	lahf
	orb %ah, %al #xorb
	andw $0x1, %ax
	movq %rax, %rsi
	#rorw $0x8, %ax
	movq %rbx, (%r8)
	#
	addq $0x8, %rcx
	addq $0x8, %rdx
	addq $0x8, %r8
	decq %r9
	#
	jmp iter_sub

	fin_sub: ret


mul_buffers:
# rcx - addr of first input buffer
# rdx - addr of second input buffer
# r8 - addr of 0-initialized output buffer (additional 64-bit 0x0 suffix(MS BLK) required)
# r9 - len of input buffers(B) = half length of output buffer (w/o suffix)

	movq %rdx, %r12 # saving rdx to r12, because rdx will be used by mulq

					#######################################
	movq %r8, %r14  #                                     #
	addq %r9, %r14  # Load output buffer tail-1 into r14  #
	decq %r14       #                                     #
					#######################################
	movq %r9, %r10
	iter_mul_1: andq %r10, %r10
	jz end_iter_mul_1
	subq $0x8, %r10
		movq %r9, %r11
		iter_mul_2: andq %r11, %r11
		jz iter_mul_1 # !!!!!!!!!!!!!!!!!!!!!!!!!
		subq $0x8, %r11
			#begin loop content
			movq %r8, %rdi    ######################################
			addq %r10, %rdi   # Making rdi hold address of c[l+k]  #
			addq %r11, %rdi   ######################################

							  ###################################
			movq %rcx, %rsi   # Making rsi hold address of a[l] #
			addq %r10, %rsi   #                                 #
							  ###################################

			movq (%rsi), %rax # Loading a[l] to rax

			                  ###################################
			movq %r12, %rsi   # Making rsi hold address of b[k] #
			addq %r11, %rsi   #                                 #
			                  ###################################

							  ##########################################
			movq (%rsi), %rbx # Results in having a[l]*b[k] in rdx:rax #
			mulq %rbx         #                                        #
							  ##########################################

			movq %rdi, %r13  # save rdi in r13 for after-propagation

			prop1: movq (%r13), %rbx # Loading c[l+k+propagation] to rbx
			addq %rax, %rbx
			lahf
			movb %ah, %al
			andq $0x1, %rax
			movq %rbx, (%r13) # storing rbx to c[l+k+propagation]
			jz end_prop1
			addq $0x8, %r13
			jmp prop1

			end_prop1: movq %rdi, %r13 #restore initial r13 from rdi
			incq %r13

			prop2: movq (%r13), %rbx # Loading c[l+k+propagation] to rbx
			addq %rdx, %rbx
			lahf
			movb %ah, %al
			andq $0x1, %rax
			movq %rax, %rdx
			movq %rbx, (%r13) # storing rbx to c[l+k+propagation]
			jz end_prop2
			addq $0x8, %r13
			jmp prop2

			end_prop2: jmp iter_mul_2

			#end loop content


	end_iter_mul_1: ret

mod_buffer:
# rcx - addr of dividend input buffer
# rdx - addr of divisor input buffer - non zero!
#	Most significant blk of divisor buffer should be 64-bit 0x0 
# r8 - addr of remainder output buffer - (r9+2*r9+1) 64-bit BLKS 0-initialized!!
# r9 - len of input buffers(64-bit blks)
	
	movq %rcx, %r10 #dividend buffer head
	movq %rdx, %r11 #divisor buffer head
	movq %r8, %r12  #output buffer head
	movq %r9, %r13  #length counter
	
	# Find MS non-zero blk of divisor buffer
	movq %r9, %rdi
	shlq $0x3, %rdi  #length in bytes
	addq %rdi, %r11
	msnb_find: subq $0x8, %r11
		cmpq $0x0, (%r11)
		jz msnb_find
		
	#[BIG ENDIAN BLOCKWISE]
	#abcdefghi   dividend 
	#^       
	#zzzzznzzz   divisor
	#xxxxxxxxx   output
    #    ^   
	
	movq %r11, %rax
	subq %rdx, %rax   #hold index*8 of divisor msnb 
	
	addq %rdi, %r10
	addq %rdi, %r12
	
	subq %rax, %r12
	# subq $0x8, %r12
	
	blk_as: cmpq %r12, %r8
	jz end_blk_as
		subq $0x8, %r10
		subq $0x8, %r12
		movq (%r10), %rbx
		movq %rbx, (%r12)
		jmp blk_as
	end_blk_as: nop
	
	#[BIG ENDIAN BITWISE]
	#abcdefgh ijklmnop	dividend adjacent blks
	#         00000101  divisor msnb (critical blk)
	#abcdefgh ijklmnop  output adjacent blks  >>
	
	# 000ijklm
	# fgh00000
	
	movq %rcx, %r10 # save rcx in r10 / needed?
	movq %rdx, %r11 # save rdx in r11 / needed?
	movb $0x7, %cl
	# r11 still contains address of divisor critical blk
	movq (%r11), %rsi
	scan_critical: 
		movq $0x1, %rbx
		shlq %cl, %rbx
		andq %rsi, %rbx
		jnz end_scan_critical
		decb %cl
		jmp scan_critical
	end_scan_critical: nop
	incb %cl # critical offset
	
	# if critical offset == 64 then shift 1 blk more
	# else perform critical shift
	# (or maybe unify it?) YES
	
	# r12 is at r8
	
	critical_shift: jz end_critical_shift
		movq (%r12), %rbx
		shrq %cl, %rbx
		
		movq %r12, %rsi
		addq $0x8, %rsi
		movq (%rsi), %rax
		
		movb $0x8, %ch
		subb %cl, %ch
		xchgb %ch, %cl
		shlq %cl, %rax
		movb %ch, %cl
		
		orq %rax, %rbx
		movq %rbx, (%r12)
		
		addq $0x8, %r12
		decq %r13
		jmp critical_shift #
	end_critical_shift: nop
	
	#restore rcx, rdx
	movq %r10, %rcx #r10
	movq %r11, %rdx #r11 
	
	#check final condition
	#check if buffer at r8 is zeroed
	movq %r8, %r12
	movq %r9, %rdi
	shlq $0x3, %rdi
	addq %rdi, %r12
	zeroed_check: cmpq %r12, %r8
	jz end_zeroed_check
	subq $0x8, %r12
	movq (%r12), %rbx
	andq %rbx, %rbx
	jz zeroed_check
	
	jmp proceed
	end_zeroed_check: nop #movq %rcx, %r8
	
	#if a<b return a, else return a-b
	call buffer_gte
	andb %al, %al
	jnz fin_modsub
	
	# copy rcx to r8
	movq %rcx, %r10
	movq %r8, %r12
	addq %rdi, %r10
	addq %rdi, %r12
	fin_copy: cmpq %r10, %rcx
	jz end_fin_copy
		subq $0x8, %r10
		subq $0x8, %r12
		movq (%r10), %rbx
		movq %rbx, (%r12)
		jmp fin_copy
	end_fin_copy: nop
	#zero lower r8?
	ret
	
	fin_modsub: call sub_buffers
	
	ret
	
	
	proceed: movq %r8, %r12  #prepare for zeroing upper r8 buffer
	movq %r9, %r13
	shlq $0x1, %r13
	addq %r9, %r13
	incq %r13
	movq %r13, %rdi
	shlq $0x3, %rdi
	addq %rdi, %r12
	subq %r9, %r13    #keep the lower r8 buffer
	
	zero_lay: andq %r13, %r13
	jz end_zero_lay
		decq %r13
		subq $0x8, %r12
		movq $0x0, (%r12)
		jmp zero_lay
	end_zero_lay: nop
		
	movq %rcx, %r15 # preparing for calls
	pushq %rdx      ####
	
	
	movq %r8, %rcx # factor1
	addq %r9, %r8  # product
	
	
	
	pushq %rdi
	shlq $0x3, %r9
	call mul_buffers
	shrq $0x3, %r9
	popq %rdi
	movq %r8, %rdx
	movq %r15, %rcx
	subq %r9, %r8
	pushq %r8
	pushq %r9
	#shrq $0x3, %r9
	call sub_buffers
	popq %r9
	popq %r8
	popq %rdx
	
	
	###################################
	#need to repeat the shift division#
	###################################
	# Copy r8 to rcx
	movq %r15, %rcx #restore rcx   #bug between 364 and 181
	movq %rcx, %r10
	movq %r8, %r12
	movq %r9, %rdi
	shlq $0x3, %rdi
	addq %rdi, %r10
	addq %rdi, %r12
	prep_copy: cmpq %r10, %rcx
	jz end_prep_copy
		subq $0x8, %r10
		subq $0x8, %r12
		movq (%r12), %rbx
		movq %rbx, (%r10)
		jmp prep_copy
	end_prep_copy: nop
	
	#zero r8
	movq %r8, %r12 #or check if here? (replace br. 364 with 379)
	movq %r9, %r13
	shlq $0x1, %r13
	addq %r9, %r13
	incq %r13
	movq %r13, %rdi
	shlq $0x3, %rdi
	addq %rdi, %r12
	zero_prep: andq %r13, %r13
	jz end_zero_prep
		decq %r13
		subq $0x8, %r12
		movq $0x0, (%r12)
		jmp zero_prep
	end_zero_prep: nop
	
	jmp mod_buffer
	
	
	
	# zero output suffix after multiplication! Here or in multiplication
	# or maybe not really?
	
buffer_gte:
#rcx - first input buffer
#rdx - second input buffer
#r9  - buffer length (blks)
#al  - 0x1 if first>=second else 0x0
	movq %rcx, %r10
	movq %rdx, %r12
	movq %r9, %rdi
	shlq $0x3, %rdi
	addq %rdi, %r10
	addq %rdi, %r12
	comparing: cmpq %r10, %rcx
	jz end_comparing
		subq $0x8, %r10
		subq $0x8, %r12
		cmpq $0x0, (%r10)
		jnz end_comparing
		cmpq $0x0, (%r12)
		jnz end_comparing
		jmp comparing
	end_comparing: nop
	movq (%r10), %rax
	movq (%r12), %rbx
	cmpq %rbx, %rax #
	jge result_ge
	xorb %al, %al
	ret
	
	result_ge: xorb %al, %al
	incb %al
	ret
	