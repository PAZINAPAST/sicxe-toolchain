cat     START   0
        CLEAR   X
        JSUB    beri
        JSUB    pisi


halt    J   halt

beri    RD  #0
        STA     txt
        COMP    #0
        JLT beri
        RSUB


pisi    WD  #1
        LDCH    txt, X
        COMP #0
        JLT pisi
        RSUB

txt     BYTE   C'a'
X       BYTE    0

        END    cat
