prog    START   0
        EXTREF  sinit, spop, spush, stkp

        +JSUB    sinit
        LDA #10
        JSUB    rsum

halt    J   halt

.argument, rezultat v registru A
rsum    +STL @stkp
        +JSUB spush
        +STB @stkp
        +JSUB    spush

        COMP #2
        JLT rsumEx
        RMO A,B
        SUB #1
        JSUB    rsum
        ADDR B,A


rsumEx  +JSUB spop
        +LDB @stkp
        +JSUB   spop
        +LDL    @stkp
        RSUB

        END prog

        