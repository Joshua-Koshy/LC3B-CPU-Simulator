        .ORIG x3000

; Initialize registers
        LD R0, VAL0
        LD R1, VAL1
        LD R2, VAL2
        LD R3, VAL3
        LD R4, VAL4
        LD R5, VAL5
        LD R6, VAL6
        LD R7, VAL7

; Loop 1: Add
LOOP1   ADD R0, R0, R1
        ADD R2, R2, R3
        ADD R4, R4, R5
        ADD R6, R6, R7
        BRz DONE1
        BR LOOP1

DONE1   ; Loop 2: AND
LOOP2   AND R0, R0, R2
        AND R1, R1, R3
        AND R4, R4, R6
        AND R5, R5, R7
        BRp LOOP2

        ; Loop 3: NOT
LOOP3   NOT R0, R0
        NOT R1, R1
        NOT R2, R2
        NOT R3, R3
        BRnzp LOOP3

; Data section
VAL0    .FILL x0001
VAL1    .FILL x0002
VAL2    .FILL x0003
VAL3    .FILL x0004
VAL4    .FILL x0005
VAL5    .FILL x0006
VAL6    .FILL x0007
VAL7    .FILL x0008

        .END
