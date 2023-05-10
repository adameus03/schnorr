.text
	.global add_buffers
	.global sub_buffers
	.global mul_buffers
	.global smod_buffer
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
# r8 - addr of 0-initialized output buffer (additional 64-bit 0x0 suffix required)
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

smod_buffer:
# rcx - addr of first input buffer = output buffer
# rdx - addr of buffer to divide by (second buffer) - non zero!
#       //OBSOLETE: 64bit 0x0 additional suffix mandatory
# OBSOLETE: r8 - addr of output buffer
# r9 - len of each buffer(B) //OBSOLETE: (w/0 suffix)

	movq %rdx, %rsi   #####################################
	addq %r9, %rsi    # Load tail of second buffer to rsi #
	# addq $0x8, %rsi #####################################

	movq %rcx, %rdi  ######################################
	addq %r9, %rdi   # Load tail of io buffer to rdi      #
	# addq $0x8, %rdi######################################

	movq %rdi, %r12  # Copy rdi to r12, to access output tail later
	subq $0x8, %r12  # Make it the tail-1 of output

	zero_run: subq $0x8, %rsi
	subq $0x8, %rdi
	movq (%rsi), %rbx # Load buffer blk to rbx
	andq %rbx, %rbx
	jz zero_run

	movq $0xffffffffffffffff, %r11 # run mask
	movq $0x8000000000000000, %r10 #####################################
	zero_walk: movq %r10, %rax     #                                   #
	andq %rbx, %rax                #  Inside-blk walk to non-zero bit  #
	jnz end_zero_walk              #                                   #
	shrq $0x1, %r10                #                                   #
	shrq $0x1, %r11                #                                   #
	jmp zero_walk                  #####################################

	end_zero_walk: andq %r11, (%rdi) # zero mid-blk using r11 mask

	zero_big: cmpq %rdi, %r12  ###################### output tail-1 comparison #
	jz end_zero_big             #                  #
	addq $0x8, %rdi             #  Zero high blks  #
	movq $0x0, (%rdi)           #                  #
	jmp zero_big              ########################

	end_zero_big: ret

