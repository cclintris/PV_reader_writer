
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "proc.h"

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);
PUBLIC char * itoa(char * str, int num);

/* kernel.asm */
void restart();

/* main.c */
void ProcessA();
void ProcessB();
void ProcessC();
void ProcessD();
void ProcessE();
void ProcessF();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);


/* 以下是系统调用相关 */

/* proc.c */
/* sys_call */
PUBLIC int  sys_get_ticks();
PUBLIC void sys_print_str(char* str);
PUBLIC void sys_sleep(int milli_second);
PUBLIC void sys_print_color_str(char* str, int color);
PUBLIC void sys_p_lock(LOCK *lock);
PUBLIC void sys_v_lock(LOCK *lock);
PUBLIC void sys_set_task_time(int task_time);
PUBLIC int  sys_get_task_time();
PUBLIC int  sys_set_read_or_write_flag(int read_or_write_flag);
PUBLIC int  sys_get_read_or_write_flag();

/* syscall.asm */
PUBLIC void sys_call();             /* int_handler */
PUBLIC int  get_ticks();
PUBLIC void print_str(char* str);
PUBLIC void sleep(int milli_second);
PUBLIC void print_color_str(char* str, int color);
PUBLIC void p_lock(LOCK *lock);
PUBLIC void v_lock(LOCK *lock);
PUBLIC void set_task_time(int task_time);
PUBLIC int  get_task_time();
PUBLIC int  set_read_or_write_flag(int read_or_write_flag);
PUBLIC int  get_read_or_write_flag();
