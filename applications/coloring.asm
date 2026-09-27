branje START   0
        JSUB    priprava
        JSUB sub

sub     CLEAR   A
        LDCH   @keyboard
        STA     readKey
        CLEAR   A
        STCH     @keyboard
        JSUB    chklocc
        JSUB    keyPres
        ...JSUB    chklocc
        JSUB    checkInboundX
        JSUB    checkInboundY
        JSUB    comput          .izracuna lokacijo
        JSUB    chkloc
        LDCH     curr           .pobarva lokacijo kjer je
        STCH     @loc           .pobarva lokacijo kje je
        ...JSUB    chkloc         .popravi prejsnjo lokacijo
        .JSUB    reset           .resetira vrednost na keyboardu
        J   sub
        RSUB

priprava CLEAR   A
        LDCH     blue
        STCH    @posLoc
        LDCH    darkViolet
        STCH    @posLoc1
        LDCH    violet
        STCH    @posLoc2
        LDCH    red
        STCH    @posLoc3
        LDCH    orange
        STCH    @posLoc4
        LDCH    yellow
        STCH    @posLoc5
        LDCH    green
        STCH    @posLoc6
        LDCH    green
        STCH    @loc
        RSUB



changeCol  CLEAR   A
        LDCH     prevCol
        STCH    @prevLoc
        LDA     loc
        STA     prevLoc
        LDCH    @prevLoc
        STCH    prevCol
        RSUB

changeL CLEAR   A
        LDCH     @loc
        STCH     prevCol
        CLEAR   A
        LDA     loc
        STA     prevLoc
        RSUB


chkloc  CLEAR   A
        LDA     loc
        COMP    prevLoc
        JGT     changeCol
        JLT     changeCol
        RSUB

chklocc  CLEAR   A
        LDA     loc
        COMP    prevLoc
        JGT     changeL
        JLT     changeL
        RSUB

keyPres CLEAR   A
        LDA     readKey
        COMP    pW
        JEQ     movUp
        ...LDA     readKey
        COMP    pS
        JEQ     movDown
        ...LDA     readKey
        COMP    pL
        JEQ    movLeft
        ...LDA     readKey
        COMP    pR
        JEQ     movRight
        COMP    save
        JEQ     preberi
        COMP    paste
        JEQ     prilepi
        RSUB

preberi CLEAR   A
        LDCH     prevCol
        STCH     color
        RSUB

prilepi CLEAR   A
        LDCH     color
        STCH    prevCol
        RSUB

reset   CLEAR   A
        LDA     res
        STCH     @keyboard
        RSUB


.izracuna trenutno lokacijo
comput  CLEAR A
        LDA locY
        MUL     cols
        STA     cy
        LDA     origin
        ADD     cy
        ADD     locX
        STA     loc
        RSUB



.preverjanje ali je znotraj screena
checkInboundX    CLEAR   A
        LDA     locX
        COMP    maxScreen
        JGT     xRepUp
        COMP    res
        JLT     xRepDwn
        RSUB


xRepUp   LDA     res
        STA     locX
        RSUB
        
xRepDwn LDA     maxScreen
        STA     locX
        RSUB

checkInboundY    CLEAR   A
        LDA     locY
        COMP    maxScreen
        JGT     yRepUp
        COMP    res
        JLT     yRepDwn
        RSUB


yRepUp   LDA     res
        STA     locY
        RSUB
        
yRepDwn LDA     maxScreen
        STA     locY
        RSUB

movUp   CLEAR   A
        LDA     locY
        SUB     #1
        STA     locY
        RSUB

movDown   CLEAR   A
        LDA     locY
        ADD    #1
        STA     locY
        RSUB

movLeft   CLEAR   A
        LDA     locX
        SUB     #1
        STA     locX
        RSUB

movRight   CLEAR   A
        LDA     locX
        ADD     #1
        STA     locX
        RSUB

halt    J   halt

        END branje


.prejsnji
prevLoc RESW    1
prevCol RESB    1

.tipkovnica
keyboard WORD   X'00C000'
readKey RESW    1
res     WORD    0

.velikostZaslona
maxScreen       WORD    63
cols    WORD    64

.barva
curr   BYTE    X'FF'
violet  BYTE    X'A6'
red     BYTE    X'F0'
green   BYTE    X'DC'
yellow  BYTE    X'FC'
blue    BYTE    X'D3'
darkViolet BYTE    X'D1'
orange  BYTE    X'F8'
color   RESB    1
...posCh1   BYTE    X'60'
...posCh2   BYTE    X'61'
...posCh3   BYTE    X'62'
...posCh4   BYTE    X'63'
...posCh5   BYTE    X'64'
...posCh6   BYTE    X'65'
...posCh7   BYTE    X'66'
...posCh8   BYTE    X'67'
...posCh9   BYTE    X'68'
...posCh10   BYTE    X'69'
...posCh11   BYTE    X'6A'
...posCh12   BYTE    X'6B'
...posCh13   BYTE    X'6C'
...posCh14   BYTE    X'6D'
...posCh15   BYTE    X'6E'


.lokacija
loc    WORD     X'00A000'    
origin  WORD    X'00A000'
locX    WORD    0
locY    WORD    0
cy      RESW    1
posLoc  WORD    X'00AF16'
posLoc1  WORD    X'00AF17'
posLoc2  WORD    X'00AF18'
posLoc3  WORD    X'00AF19'
posLoc4  WORD    X'00AF1A'
posLoc5  WORD    X'00AF1B'
posLoc6  WORD    X'00AF1C'
posLoc7  WORD    X'00AF1D'
posLoc8  WORD    X'00AF1E'
posLoc9  WORD    X'00AF1F'
posLoc10  WORD    X'00AF20'
posLoc11  WORD    X'00AF21'
posLoc12  WORD    X'00AF22'
posLoc13  WORD    X'00AF23'
posLoc14  WORD    X'00AF24'

.premiki
pS      WORD    X'000053'   .s
pW      WORD    X'000057'     .w
pR   WORD    X'000044'     .d
pL    WORD    X'000041'     .a
save    WORD    X'000051'       .q
paste   WORD    X'000045'      .e
