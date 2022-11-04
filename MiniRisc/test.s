DEF LD  0x80                ; LED adatregiszter (�rhat�/olvashat�)


data
    DIGIT_CODE:
       DB 0x7d, 0x6b,0x57,0x26,0x47,0x3d,0x10 ;Kib�v�tett Digit k�d
    org 0x10  
    SUM_LUT: ;R�szeredm�nyek
        DB 0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03
        DB 0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x03
code 
begin:
    mov r0, #DIGIT_CODE ;Digit k�d c�m�nek beolvas�sa				0
    mov r1, #0x07 ;Adatsz�ml�l� inicializ�l�sa					1
    mov r2, #0x00 ;Bitsz�ml�l� null�z�sa					2
    mov 0x00, r1							;	3
beolvas:
    mov r8, (r0) ;Byte beolvas�sa						4	
    and r8, #0x0f ;Als� n�gy bit maszkol�sa					5
    mov r9, #SUM_LUT ;R�szeredm�nyek bet�lt�se					6
    add r9, r8 ;Megfelel� c�m megkeres�se					7
    mov r10, (r9) ;�rt�k kim�spl�sa						8
    add r2, r10 ;Bitsz�ml�l� n�vel�se a r�szeredm�nnyel				9
    mov r8, (r0) ;Ugyanaz mint el�bb						10
    swp r8 ;Fels�-als� 4 bit felcser�l�se					11
    and r8, #0x0f;								12
    mov r9, #SUM_LUT;								13
    add r9, r8;									14
    mov r10, (r9);								15
    add r2, r10;								16
    add r0, #0x01 ;C�mregiszter n�vel�se, hogy a k�vvetkez� adatra mutasson	17
    sub r1, #0x01 ;Adatsz�ml�l� cs�kkent�se					18
    jnz beolvas ;Ha van m�g h�tra adat, akkor olvassuk be �ket			19
vege:
    mov LD, r2 ;Eredm�ny kiirat�sa						20
    jmp begin