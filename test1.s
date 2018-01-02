.data
.text
 main:
    # Getting user input
    li $v0, 5
    syscall

    # Moving the integer input to another register
    move $t1, $v0

    sw $t1, 4($sp)
    # Printing out the number
    
    lw $t0, 4($sp)

    li $v0, 1
    move $a0, $t0
    syscall

    # End Program
    li $v0, 10
    syscall
