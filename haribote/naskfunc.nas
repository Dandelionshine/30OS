; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 僆僽僕僃僋僩僼傽僀儖傪嶌傞儌乕僪	
[INSTRSET "i486p"]				; 486偺柦椷傑偱巊偄偨偄偲偄偆婰弎
[BITS 32]						; 32價僢僩儌乕僪梡偺婡夿岅傪嶌傜偣傞
[FILE "naskfunc.nas"]			; 僜乕僗僼傽僀儖柤忣曬

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_asm_inthandler20, _asm_inthandler21
		GLOBAL	_asm_inthandler2c, _asm_inthandler0c
		GLOBAL	_asm_inthandler0d, _asm_end_app
		GLOBAL	_memtest_sub
		GLOBAL	_farjmp, _farcall
		GLOBAL	_asm_hrb_api, _start_app
		GLOBAL	_shutdown, _reboot
		EXTERN	_inthandler20, _inthandler21
		EXTERN	_inthandler2c, _inthandler0d
		EXTERN	_inthandler0c
		EXTERN	_hrb_api

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS 偲偄偆堄枴
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS 偲偄偆堄枴
		RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler0c:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0c
		CMP		EAX,0
		JNE		_asm_end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; INT 0x0c 偱傕丄偙傟偑昁梫
		IRETD

_asm_inthandler0d:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0d
		CMP		EAX,0			; 偙偙偩偗堘偆
		JNE		_asm_end_app	; 偙偙偩偗堘偆
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; INT 0x0d 偱偼丄偙傟偑昁梫
		IRETD

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; 乮EBX, ESI, EDI 傕巊偄偨偄偺偱乯
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_farjmp:		; void farjmp(int eip, int cs);
		JMP		FAR	[ESP+4]				; eip, cs
		RET

_farcall:		; void farcall(int eip, int cs);
		CALL	FAR	[ESP+4]				; eip, cs
		RET

_asm_hrb_api:
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD		; 曐懚偺偨傔偺PUSH
		PUSHAD		; hrb_api偵傢偨偡偨傔偺PUSH
		MOV		AX,SS
		MOV		DS,AX		; OS梡偺僙僌儊儞僩傪DS偲ES偵傕擖傟傞
		MOV		ES,AX
		CALL	_hrb_api
		CMP		EAX,0		; EAX偑0偱側偗傟偽傾僾儕廔椆張棟
		JNE		_asm_end_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD
_asm_end_app:
;	EAX偼tss.esp0偺斣抧
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET					; cmd_app傊婣傞

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD		; 32價僢僩儗僕僗僞傪慡晹曐懚偟偰偍偔
		MOV		EAX,[ESP+36]	; 傾僾儕梡偺EIP
		MOV		ECX,[ESP+40]	; 傾僾儕梡偺CS
		MOV		EDX,[ESP+44]	; 傾僾儕梡偺ESP
		MOV		EBX,[ESP+48]	; 傾僾儕梡偺DS/SS
		MOV		EBP,[ESP+52]	; tss.esp0偺斣抧
		MOV		[EBP  ],ESP		; OS梡偺ESP傪曐懚
		MOV		[EBP+4],SS		; OS梡偺SS傪曐懚
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	埲壓偼RETF偱傾僾儕偵峴偐偣傞偨傔偺僗僞僢僋挷惍
		OR		ECX,3			; 傾僾儕梡偺僙僌儊儞僩斣崋偵3傪OR偡傞
		OR		EBX,3			; 傾僾儕梡偺僙僌儊儞僩斣崋偵3傪OR偡傞
		PUSH	EBX				; 傾僾儕偺SS
		PUSH	EDX				; 傾僾儕偺ESP
		PUSH	ECX				; 傾僾儕偺CS
		PUSH	EAX				; 傾僾儕偺EIP
		RETF
;	傾僾儕偑廔椆偟偰傕偙偙偵偼棃側偄

_shutdown:
	JMP start
	db 0x00, 0x00
	protect16:
	db 0xb8, 0x08, 0x00, 0x8e, 0xd8, 0x8e, 0xc0, 0x8e, 0xd0
	db 0x0f, 0x20, 0xc0, 0x66, 0x25, 0xfe,0xff,0xff, 0x7f
	db 0x0f, 0x22, 0xc0
	db 0xea
	dw 0x0650,0x0000
	ALIGNB 16
	protect16_len EQU $ - protect16
	
	;上面的代码为16位保护模式跳入实模式功能代码
	;保护模式代码传送到内存0x0630处，为它保留0x20 B

realmod:
	db 0x8c, 0xc8
	db 0x8e, 0xd8
	db 0x8e, 0xc0
	db 0x8e, 0xd0
	db 0xbc, 0x00, 0x08
	db 0xe4, 0x92
	db 0x24, 0xfd
	db 0xe6, 0x92
	db 0x90, 0x90, 0x90
	db 0xfb, 0x90
	db 0xb8, 0x03, 0x00
	db 0xcd, 0x10
	;关机
	db 0xb8, 0x07, 0x53
	db 0xbb, 0x01, 0x00
	db 0xb9, 0x03, 0x00
	db 0xcd, 0x15
	ALIGNB 16
	realmod_len EQU $ - realmod
	; 以上代码段为实模式下设置字符显示模式及关机代码
	; 实模式功能代码传送到0x0650处。

GDTIDT:
	dw 0x0000, 0x0000, 0x0000, 0x0000
	dw 0xffff, 0x0000, 0x9200, 0x0000
	dw 0xffff, 0x0000, 0x9800, 0x0000
	dw 0x0000
	dw 0x0017
	dw 0x0600, 0x0000
	dw 0x03ff
	dw 0x0000, 0x0000
	ALIGNB 16
	GDTIDT_lenth EQU $ - GDTIDT
	;以上为GDT及ITD表项数据
	;以上数据传送到0x0600处，保留0x30 B的空间。
	
start:
	MOV EBX, GDTIDT
	MOV EDX, 0x600
	MOV CX, GDTIDT_lenth
	.loop1:
	MOV AL, [CS:EBX]
	MOV [EDX], AL
	INC EBX
	INC EDX
	loop .loop1

	MOV EBX, protect16
	MOV EDX, 0x630
	MOV CX, protect16_len
	.loop2:
	MOV AL, [CS:EBX]
	MOV [EDX], AL
	INC EBX
	INC EDX
	loop .loop2

	MOV EBX, realmod
	MOV EDX, 0x650
	MOV CX, realmod_len
	.loop3:
	MOV AL, [CS:EBX]
	MOV [EDX], AL
	INC EBX
	INC EDX
	loop .loop3

	LGDT [0x061A]
	LIDT [0x0620]
	JMP 2*8:0x0630
;--------------------------------------------------------------------------------------------------
;1.将以上代码复制追加到naskfunc.nas文件即可。还需要在前面导出shutdown2 这个符号。
;2.在bootpack.h 加入如下 void shutdown(void);
;3 在cons_runcmd 加一个命令处理分支：
; }else if (strcmp(cmdline, "shutdown")== 0) {
; shutdown();
; }

_reboot:
	JMP start_reboot
	db 0x00, 0x00
	protect16_reboot:
	db 0xb8, 0x08, 0x00, 0x8e, 0xd8, 0x8e, 0xc0, 0x8e, 0xd0
	db 0x0f, 0x20, 0xc0, 0x66, 0x25, 0xfe,0xff,0xff, 0x7f
	db 0x0f, 0x22, 0xc0
	db 0xea
	dw 0x0650,0x0000
	ALIGNB 16
	protect16_reboot_len EQU $ - protect16_reboot
	
	;上面的代码为16位保护模式跳入实模式功能代码
	;保护模式代码传送到内存0x0630处，为它保留0x20 B

realmod_reboot:
	db 0x8c, 0xc8
	db 0x8e, 0xd8
	db 0x8e, 0xc0
	db 0x8e, 0xd0
	db 0xbc, 0x00, 0x08
	db 0xe4, 0x92
	db 0x24, 0xfd
	db 0xe6, 0x92
	db 0x90, 0x90, 0x90
	db 0xfb, 0x90
	db 0xb8, 0x03, 0x00
	db 0xcd, 0x10
	
	;重启
	mov eax,0xffff
    push eax
    mov eax,0
    push eax
    retf
	realmod_reboot_len EQU $ - realmod_reboot
	; 以上代码段为实模式下设置字符显示模式及关机代码
	; 实模式功能代码传送到0x0650处。

GDTIDT_reboot:
	dw 0x0000, 0x0000, 0x0000, 0x0000
	dw 0xffff, 0x0000, 0x9200, 0x0000
	dw 0xffff, 0x0000, 0x9800, 0x0000
	dw 0x0000
	dw 0x0017
	dw 0x0600, 0x0000
	dw 0x03ff
	dw 0x0000, 0x0000
	ALIGNB 16
	GDTIDT_reboot_lenth EQU $ - GDTIDT_reboot
	;以上为GDT及ITD表项数据
	;以上数据传送到0x0600处，保留0x30 B的空间。
	
start_reboot:
	MOV EBX, GDTIDT_reboot
	MOV EDX, 0x600
	MOV CX, GDTIDT_reboot_lenth
	.loop1_reboot:
	MOV AL, [CS:EBX]
	MOV [EDX], AL
	INC EBX
	INC EDX
	loop .loop1_reboot

	MOV EBX, protect16_reboot
	MOV EDX, 0x630
	MOV CX, protect16_reboot_len
	.loop2_reboot:
	MOV AL, [CS:EBX]
	MOV [EDX], AL
	INC EBX
	INC EDX
	loop .loop2_reboot

	MOV EBX, realmod_reboot
	MOV EDX, 0x650
	MOV CX, realmod_reboot_len
	.loop3_reboot:
	MOV AL, [CS:EBX]
	MOV [EDX], AL
	INC EBX
	INC EDX
	loop .loop3_reboot

	LGDT [0x061A]
	LIDT [0x0620]
	JMP 2*8:0x0630