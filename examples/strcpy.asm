strcpy  START   0
    LDA nas
    RMO A, T
    LDB dve
    LDS ZERO
    J POD
halt J halt


POD     CLEAR A
        LDCH  nas
        ADDR A, B
        SHIFTL B, 8
        LDA nas
        STCH #0
        SHIFTL A, 8
        RMO A, T
        STT nas
        COMPR T, L
        JLT POD
        JGT POD 
        RSUB        





.
nas     WORD    1234567
dve     RESW    1
ZERO    WORD    0
dest    WORD    0