.data
.text
sum:
    lw $t1, -4($sp)
    lw $t2, -8($sp)
    add $t0, $t1, $t2
    move $t1, $t0
    lw $t2, -12($sp)
    add $t0, $t1, $t2
    move $t1, $t0
    lw $t2, -16($sp)
    add $t0, $t1, $t2
    move $t1, $t0
    lw $t2, -20($sp)
    add $t0, $t1, $t2
    sw $t0, 4($sp)
    move $v0, $t0
    jal f1
    li $v0, 10
    syscall
main:
    li $v0, 5
    syscall
    move $t0, $v0
    sw $t0, 8($sp)
    li $v0, 5
    syscall
    move $t0, $v0
    sw $t0, 12($sp)
    li $v0, 5
    syscall
    move $t0, $v0
    sw $t0, 16($sp)
    li $v0, 5
    syscall
    move $t0, $v0
    sw $t0, 20($sp)
    li $v0, 5
    syscall
    move $t0, $v0
    sw $t0, 24($sp)
    lw $t0, 8($sp)
    sw $t0, -4($sp)
    lw $t0, 12($sp)
    sw $t0, -8($sp)
    lw $t0, 16($sp)
    sw $t0, -12($sp)
    lw $t0, 20($sp)
    sw $t0, -16($sp)
    lw $t0, 24($sp)
    sw $t0, -20($sp)
    jal sum
f1:
    move $t0, $v0
    sw $t0, 28($sp)
    lw $t0,28($sp)
    li $v0, 1
    move $a0, $t0
    syscall
    li $v0, 10
    syscall
