;---------------------------------------------------
; FILE:		utils.asm
;
; TITLE:	Support Functions.
;---------------------------------------------------
	.def	_CopyByteBuf;


;---------------------------------------------------
;	Memory copy	(Byte)

	.def  _CopyByteBuf

_CopyByteBuf:
        PUSH    XAR2            ; Context Save
        PUSH    XAR1
        PUSH    XAR0 
		
		MOV		AR0, AL			; load destination offset
		MOV		AR1, AH			; load source offset
		MOV		AL, *-SP[9]		; load bytes length
		BF		_CopyByteBuf_Exit, EQ
		MOVZ	AR2, AL
		DEC		AR2

_CopyByteBuf_Loop:
		MOVB	AL.LSB, *+XAR5[AR1]
		MOVB	*+XAR4[AR0], AL.LSB
		
		INC		AR0
		INC		AR1

		BANZ	_CopyByteBuf_Loop, AR2--

_CopyByteBuf_Exit:
		POP		XAR0
		POP		XAR1
		POP		XAR2
		LRETR

