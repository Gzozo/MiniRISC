DEF LD 0x80
DEF SW 0x81
DEF BT 0x84

code:
   mov r0, #0x00
   mov LD, r0
 ciklus:
   mov r1, BT
   and r1, #0x0f
   jnz reset
   mov r2, SW
   mov LD, r2
   jmp ciklus
 reset:
   mov r0, #0x00
   mov LD, r0
   jmp ciklus
