
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_get_ticks       equ 0
_NR_print_str 		equ 1
_NR_sleep           equ 2
_NR_print_color_str equ 3
_NR_p_lock          equ 4
_NR_v_lock          equ 5
_NR_set_task_time   equ 6
_NR_get_task_time   equ 7
_NR_set_read_or_write_flag   equ 8
_NR_get_read_or_write_flag   equ 9

INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global  print_str
global  sleep
global  print_color_str
global  p_lock
global  v_lock
global  set_task_time
global  get_task_time
global  set_read_or_write_flag
global  get_read_or_write_flag

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              print_str
; ====================================================================
print_str:
	mov eax, _NR_print_str
	mov	ebx, [esp + 4]
	mov ecx, 0
	int INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              sleep
; ====================================================================
sleep:
	mov eax, _NR_sleep
	mov	ebx, [esp + 4]
	int INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              print_color_str
; ====================================================================
print_color_str:
	mov eax, _NR_print_color_str
	mov	ebx, [esp + 4]
	mov ecx, [esp + 8]
	int INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              p_lock
; ====================================================================
p_lock:
	mov eax, _NR_p_lock
	mov	ebx, [esp + 4]
	int INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              v_lock
; ====================================================================
v_lock:
	mov eax, _NR_v_lock
	mov	ebx, [esp + 4]
	int INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                             set_task_time
; ====================================================================
set_task_time:
	mov eax, _NR_set_task_time
	mov	ebx, [esp + 4]
	int INT_VECTOR_SYS_CALL
	ret


; ====================================================================
;                             get_task_time
; ====================================================================
get_task_time:
	mov	eax, _NR_get_task_time
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                             set_read_or_write_flag
; ====================================================================
set_read_or_write_flag:
	mov eax, _NR_set_read_or_write_flag
	mov	ebx, [esp + 4]
	int INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                             get_read_or_write_flag
; ====================================================================
get_read_or_write_flag:
	mov	eax, _NR_get_read_or_write_flag
	int	INT_VECTOR_SYS_CALL
	ret
