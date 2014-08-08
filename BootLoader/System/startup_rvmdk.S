        AREA    ||.text||, code, readonly, align=2
        EXPORT  CallApplication
CallApplication
        mov     r1, #0xE0000000
        orr     r1, r1, #0x0000ED00
        str     r0, [r1,#8]
        ldr     sp, [r0], #4
        ldr     r0, [r0]
        bx      r0

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END
