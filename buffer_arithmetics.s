.text
	.global add_buffers
	.global sub_buffers
add_buffers:
# rcx - addr of first input buffer
# rdx - addr of second input buffer
# r8 - addr of output buffer
# r9 - len of each buffer
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
# r9 - len of each buffer
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
# r8 - addr of output buffer
# r9 - len of input buffers(B) = half length of output buffer

	movq %rdx, %r12 # saving rdx to r12, because rdx will be used by mulq
	movq %r9, %r10
	iter_mul_1: andq %r10, %r10
	jz end_iter_mul_1
	subq $0x8, %r10
		movq %r9, %r11
		iter_mul_2: andq %r11, %r11
		# jz ??? !!!!!!!!!!!!!!!!!!!!!!!!!
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
			
			movq (%rdi), %rbx # Loading c[l+k] to rbx
			
			
			
			
			
			# movq (%rdi), %rbx
			# addq (%rsi), %rbx
			
			
			
			#end loop content
			
	
	end_iter_mul_1: ret
