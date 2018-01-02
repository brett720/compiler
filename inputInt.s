.data
.text
 main:
    # Getting user input
    li $v0, 5
    syscall

    # Moving the integer input to another register
    move $t0, $v0

    # Printing out the number
    li $v0, 1
    move $a0, $t0
    syscall

    # End Program
    li $v0, 10
    syscall
