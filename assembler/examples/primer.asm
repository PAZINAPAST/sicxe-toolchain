prog     START   1000
STIRI    WORD    4     .stiri
PET      WORD    5      .sse en komentar
DEVET    WORD    0
CHARBUF  BYTE    C'A'
ZERO     WORD    0

.glavna funkcija
MAIN     LDA     STIRI
         ADD     PET
         STA     DEVET
         CLEAR   X
         LDB     #3
         RMO     B, A
         SHIFTL  A,1
         ADDR    A,B
         RMO     B,A
         COMP    DEVET
         JEQ     EQUAL
         JLT     LESS
         JGT     GREATER

.jeenako
EQUAL    LDCH    CHARBUF
         STCH    CHARBUF
         J       DONE

.manjse
LESS     LDA     ZERO .......dodaj 
         J       DONE

GREATER  SUB     PET
         J       DONE

SURB     ADD     #1
         RSUB

DONE     J      DONE
         END     MAIN