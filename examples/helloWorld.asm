hello   START   0
        CLEAR   X
loop    LDCH    txt, X
        WD      #1
        TIX     #txtlen
        COMP    #0
        JLT     loop

.        LDCH    #0x68
.        WD      #1
.        LDCH    #0x65
.        WD      #1
.        LDCH    #0x6C
.        WD      #1
.        WD      #1

halt    J   halt

txt     BYTE C'hello world\n'
        BYTE 0

txtend  EQU *
txtlen  EQU txtend-txt

        END hello