.ORIG x3000
    ADD R1, R1, #3
    ADD R1, R1,   #-2
    ADD R0, R1, #6
    AND R1, R1, R0
    AND R0, R0, x2
    NOT R1, R0
    AND R0, R0, #0
    AND R1, R1, #0
    ADD R1, R1, #4
    ADD R0, R0, #8
    LSHF R5, R1, #5
    RSHFL R6, R0, #1
    RSHFA R7, R0, #3
    XOR R7, R1, R0
    XOR R7, R1, x2


    LEA R3, A
    LDW R3, R3, #0
    LEA R4, Y
    LDW R4, R4, #0

    STB R3, R4, #0
    STW R3, R4, #1
    LDB R0, R4, #0
    LDW R1, R4, #1
    HALT
A	.FILL x1272
Y	.FILL x1234
	.END

