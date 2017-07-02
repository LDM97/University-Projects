; ======================================================
; ===== Seven Segment Displays Port 02 =================

	JMP Start

	DB	10	; Org of hardware timer.
	DB	FA	; 0
	DB	A	; 1
	DB	B6	; 2
	DB	9E	; 3
	DB	4E	; 4
	DB	DC	; 5
	DB	FC	; 6
	DB	8A	; 7
	DB	FE	; 8
	DB	DE	; 9

		; ----- Hardware timer -----

	org	10
	CLI		; Clear flag so can run the update code.

	INC	BL	; Point to next entry.

	iret	; return.	

		;----- MAIN PROGRAM -----

Start: ; Down to UpdateLoop, sets the initial values up and sets the clock to display 00.
	
	MOV	BL,03	; Start address of table.

	MOV 	AL,[BL]	
	OUT	02	; Set left side of display to 0.
	
	ADD	AL,1
	OUT	02	; Set right side of display to 0.

	MOV	CL,03	; Use to track the tens.

UpdateLoop:	; loop but keep updating the clock with current values.
	STI
	
	MOV	AL,[CL]
	OUT	02	; Output the updated 10s unit (left hand display).
	MOV	AL,[BL]
	ADD	AL,1	; Increment so points to right hand display.
	OUT	02	; Output the updated unit (right hand display).

	CMP	BL,0D	; Have reached 9 on right hand display?
	JZ	CheckEnd
	
	jmp 	UpdateLoop	; Continue to loop.

CheckEnd:	; Check if reached 99, if not just go to next ten (increment left display).

	CMP 	CL,0C	; Reached end of table and ten is 9 as well, (AKA reached 99).
	JZ	Reset	; Then reset to 0.
	
			; Otherwise, increment the 10 and reset the units.
	INC	CL	; Increment for next tens unit.
	MOV	BL,03	; Reset the unit (BL) to 0.

	jmp	UpdateLoop		; Return.

Reset:	; Reset to 00 if reached 99.

	MOV	BL,03	; Reset to 0
	MOV	CL,03	; Reset to 0
	jmp	UpdateLoop	; Return

	END
; ======================================================