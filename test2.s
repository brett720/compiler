.data
.text
main:
    # Getting user input
    li $v0, 5
    syscall

    # Moving the integer input to another register
    move $t1, $v0

    sw $t1, 4($sp)



    li $v0, 5
    syscall
    move $t2, $v0

    sw $t2, 8($sp)

    lw $t0, 4($sp)
    lw $t1, 8($sp)
    slt $s0, $t0, $t1
    beq $s0, 1, L0
    j L1

L0:
    # Printing out the number
    lw $t1, 8($sp)

    li $v0, 1
    move $a0, $t1
    syscall


L1:
    lw $t0, 4($sp)
    lw $t1, 8($sp)

    slt $s0, $t1, $t0
    beq $s0, 1, L2
    j L3

L2:
    lw $t0, 4($sp)
    
    li $v0, 1
    move $a0, $t0
    syscall
    # End Program
L3: 
    li $v0, 10
    syscall
