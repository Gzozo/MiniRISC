DEF LD 0x80
DEF SW 0x81
DEF BT 0x84

code:
start:
    mov r0, SW
    mov r1, r0

    and r0, #0x0f
    and r1, #0xf0

    swp r1
    add r0, r1

    mov LD, r0
    jmp start