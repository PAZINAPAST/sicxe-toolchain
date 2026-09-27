poly    START   0
        LDT     s
        LDX     cap
        LDA k1
        STA res
        COMPR   T,X
        JLT loop
        LDA c
        MUL k2
        ADD res
        STA res
        ADD



halt    J   halt

loop    LDS ls
        LDB inc
        J zmnoziKoef
        COMPR S,T
        JLT loop
        RSUB
        


zmnoziKoef  LDA x
            MUL x
            STA c
            RSUB



.podatki   
k1   WORD    5
k2  WORD    4
k3  WORD    3
k4  WORD    2
x   WORD    2
s   WORD    1
cap WORD    5
res RESW    1 
c   RESW    1
inc WORD    1 
ls  WORD    0 