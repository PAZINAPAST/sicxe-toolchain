TEST    START   1000

        LDA     #5          . immediate, format 3
        STA     VALUE       . PC-relative

        +JSUB   SUBRTN      . format 4 → M record REQUIRED
        LDA     @VALUE      . indirect, PC-relative

        BASE    BIGBUF      . enable BASE-relative
        LDA     BIGBUF      . BASE-relative (forced)
        NOBASE

        LDA     FAR         . PC-relative FAILS → format 4 required
        +LDA    FAR         . format 4, M record REQUIRED

        RSUB

VALUE   WORD    123

        RESB    32          . gap → T record break

BIGBUF  RESW    512         . force BASE-relative

FAR     WORD    999         . out of PC range

SUBRTN  LDA     #0
        RSUB

        END     TEST
