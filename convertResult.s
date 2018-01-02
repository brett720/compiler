.data
.text
main:
    li $v0, 5
    syscall
    move $t1, $v0
    sw $t1, 4($sp)
    li $t0, 0
    sw $t0, 8($sp)
    li $t0, 0
    sw $t0, 12($sp)
    li $t0, 1
    sw $t0, 8($sp)
L0:
    lw $t0, 8($sp)
    lw $t1, 4($sp)
    slt $s0, $t1, $t0
    beq $s0, 0, L1
    j L2
L1:
    lw $t0, 12($sp)
    lw $t1, 8($sp)
    add $t3, $t0, $t1
    sw $t3, 12($sp)
    lw $t0, 8($sp)
    li $t1,1
    add $t4, $t0, $t1
    sw $t4, 8($sp)
    j L0
L2:
    lw $t0,12($sp)
    li $v0, 1
    move $a0, $t0
    syscall
    li $v0, 10
    syscall
