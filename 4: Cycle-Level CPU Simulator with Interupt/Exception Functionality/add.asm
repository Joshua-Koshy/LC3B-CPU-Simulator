.ORIG x3000

            LEA R0, PTR4000;
            LDW R1, R0, #0;     R1 = x4000
            AND R2, R2, #0
            ADD R2, R2, #1;     R2 = 1
            STW R2, R1, #0;     MEM[x4000] = 1

            LEA R0, PTRC000
            LDW R4, R0, #0;     R4 = xC000
            AND R3, R3, #0;     R3 = sum = 0

            AND R0, R0, #0;     R0 = 0
            ADD R0, R0, #10;
            ADD R0, R0, #10;    R0 = 20;

AGAIN       LDB R2, R4, #0
            ADD R3, R3, R2
            ADD R4, R4, #1
            ADD R0, R0, #-1;
            BRp AGAIN;          loop while R0 > 0

            STW R3, R4, #0;     MEM[xC014] = R3 = sum

            AND R0, R0, #0
            STW R3, R0, #0

            ;ADD R4, R4, #3;     R4 = xC014 + 3 = xC017
            ;STW R3, R4, #0

            ;.FILL xA000

            HALT

PTR4000     .FILL x4000
PTRC000     .FILL xC000

            .END
