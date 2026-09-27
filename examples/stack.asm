cstk    START   0
        EXTDEF  sinit, spop, spush, stkp

sinit   LDB    #stk
        STB     stkp
        RSUB

spush   
        STA stkA
        LDA stkp
        ADD #3
        STA stkp
        LDA stkA
        RSUB

spop    STA stkA
        LDA stkp
        SUB #3
        STA stkp
        LDA stkA
        RSUB

.podatki za sklad
stkp    WORD    0
stk    RESW    1000
stkA    WORD    0



    END prog