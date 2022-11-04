DEF LD  0x80                ; LED adatregiszter (írható/olvasható)


data
    DIGIT_CODE:
       DB 0x7d, 0x6b,0x57,0x26,0x47,0x3d,0x10 ;Kibõvített Digit kód
    org 0x10  
    SUM_LUT: ;Részeredmények
        DB 0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03
        DB 0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x03
code 
begin:
    mov r0, #DIGIT_CODE ;Digit kód címének beolvasása				0
    mov r1, #0x07 ;Adatszámláló inicializálása					1
    mov r2, #0x00 ;Bitszámláló nullázása					2
    mov 0x00, r1							;	3
beolvas:
    mov r8, (r0) ;Byte beolvasása						4	
    and r8, #0x0f ;Alsó négy bit maszkolása					5
    mov r9, #SUM_LUT ;Részeredmények betöltése					6
    add r9, r8 ;Megfelelõ cím megkeresése					7
    mov r10, (r9) ;Érték kimásplása						8
    add r2, r10 ;Bitszámláló növelése a részeredménnyel				9
    mov r8, (r0) ;Ugyanaz mint elõbb						10
    swp r8 ;Felsõ-alsó 4 bit felcserélése					11
    and r8, #0x0f;								12
    mov r9, #SUM_LUT;								13
    add r9, r8;									14
    mov r10, (r9);								15
    add r2, r10;								16
    add r0, #0x01 ;Címregiszter növelése, hogy a kövvetkezõ adatra mutasson	17
    sub r1, #0x01 ;Adatszámláló csökkentése					18
    jnz beolvas ;Ha van még hátra adat, akkor olvassuk be õket			19
vege:
    mov LD, r2 ;Eredmény kiiratása						20
    jmp begin