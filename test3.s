.data
.text
main:
	li $v0, 5
    syscall
	move $t0, $v0
	sw $t0, 4($sp)

	li $t0, 0
	sw $t0, 8($sp)

	li $t0, 1
	sw $t0, 12($sp)

L0:
	#a
	lw $t0, 4($sp)
	#b
    lw $t1, 12($sp)
	
	slt $s0, $t0, $t1
	beq $s0, 0, L1
	j L2

L1:
	lw $t1, 8($sp)
	lw $t2, 12($sp)
	add $t3, $t1, $t2
	sw $t3, 8($sp)

	lw $t0, 12($sp)
	addi $t0, $t0, 1
	sw $t0, 12($sp)
	
	j L0



L2: 
	#print result
	lw $t1, 8($sp)

    li $v0, 1
    move $a0, $t1
    syscall

	li $v0, 10
    syscall