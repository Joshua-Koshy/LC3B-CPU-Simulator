.orig x3000
      lea r0, target
      ldb r1, r0, #-1
      ldw r2, r0, #-2
      and r3, r2, r1
      xor r3, r2, r1
      add r3, r2, r1
      lea r0, start
      lea r0, target
      stw r2, r0, #-1
      stb r1, r0, #-2
      jsr queue
      jmp queue
      halt

      queue and r1, r1, #0
          add r1, r1, #-5
          lshf r1, r1, #2
          rshfa r1, r1, #2
          rshfl r1, r1, #2
          ret

      .fill xA987
      .fill x6565
      target .fill x301c
      .END