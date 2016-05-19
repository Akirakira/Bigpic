; TODO INSERT CONFIG CODE HERE USING CONFIG BITS GENERATOR
#INCLUDE <p18f14k50.inc>
    
    CONFIG FOSC = IRC,PLLEN = ON,WDTEN = ON,WDTPS = 32768
    
RES_VECT  CODE    0x0000	    ;	processor reset vector
    GOTO    START		    ;	go to beginning of program
    
MOJICOUNT   EQU H'0005'		    ;	RX counter
SLEEP5	    EQU H'0006'		    ;	SLEEP moji
MAXMOJI	    EQU H'0007'
MOJILF	    EQU H'0008'		    ;	Moji LF address
MOJIS	    EQU H'0009'		    ;	Moji S address
MOJIL	    EQU H'000A'		    ;	Moji L address
MOJIE	    EQU H'000B'		    ;	Moji E address
MOJIP	    EQU H'000C'		    ;	Moji P address

    CBLOCK  H'000D'		    ;	RX 16 Moji Address series
MOJI00	    
MOJI01
MOJI02
MOJI03
MOJI04
MOJI05
MOJI06
MOJI07
MOJI08
MOJI09
MOJI10
MOJI11
MOJI12
MOJI13
MOJI14
MOJI15			    ;	H'00'
    ENDC
    
; TODO ADD INTERRUPTS HERE IF USED

MAIN_PROG CODE                      ;	let linker place main program

START
    ; SFC Setting
    MOVLW   b'01010010'		    ;	IRC...Internal Osilator && clock 4MHz
    MOVWF   OSCCON,0
    CLRF    ANSEL		          ;	DIGITAL
    MOVLW   b'10010000'		    ;	RX setting
    MOVWF   RCSTA,0
    MOVLW   b'00100100'		    ;	TX setting
    MOVWF   TXSTA,0
    MOVLW   b'00001000'		    ;	Boud rate setting
    MOVWF   BAUDCON,0
    
    ; MOJI setting
    MOVLW   b'00001010'		    ;	LF ASCII code
    MOVWF   MOJILF,0
    MOVLW   b'01010011'		    ;	S ASCII code
    MOVWF   MOJIS,0
    MOVLW   b'01001100'		    ;	L ASCII code
    MOVWF   MOJIL,0
    MOVLW   b'01000101'		    ;	E ASCII code
    MOVWF   MOJIE,0
    MOVLW   b'01010000'		    ;	P ASCII code
    MOVWF   MOJIP,0
    MOVLW   b'00000101'		    ;	SLEEP 5moji setting
    MOVWF   SLEEP5,0
    
    ; Counter Clear
    CLRF    MOJICOUNT
    MOVLW   b'00010000'		    ;	MAX16 moji
    MOVWF   MAXMOJI,0
    
RECEIV
    BTFSS   PIR1,RCIF,0		    ;	UART receive data?
    GOTO    UARTRX		        ;	If get data, goto STATUART 	
    GOTO    RECEIV		        ;	If no data, return receive
    
UARTRX
    MOVLW  b'0000111'		        ;	Error mask
    ANDWF   RCSTA,W,0		        ;	looking for Error
    BTFSS   STATUS,Z,0		      ;	check STATUS Zero
    GOTO    ERRO		            ;	If Error, goto ERRO
    MOVF    RCREG,W		          ;	Receive data -> W
    CPFSEQ  MOJILF,0		        ;	RX data = LF ?
    GOTO    SLPD		            ;	YES ... SLEEP Dicision 
    INCF    MOJICOUNT		        ;	NO  ... Increment MOJICOUNT
    GOTO    RECEIV		          ;	return Recive

SLPD

UARTTX
    MOVWF   TXREG		            ;	W -> Transfar data
    GOTO    RECEIV		          ;	Loop Receive
    
ERRO
    GOTO    $
    END

    
RESTART 
    
    GOTO RESTART
    
    END
    
    ORG	    H'0004'
    GOTO    RESTART
