
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;
EXTERN	u8		gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	u32		k_reenter;

EXTERN	TSS		tss;
EXTERN	PROCESS*	p_proc_ready;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern  TASK            task_table[];
extern	irq_handler	irq_table[];

// reader-writer problem declaration
EXTERN  LOCK    read_lock;
EXTERN  LOCK    write_lock;
EXTERN  LOCK    mutex_lock;
EXTERN  LOCK    mutex_lock1;
EXTERN  LOCK    mutex_lock2;
EXTERN  int     read_count;
EXTERN  int     write_count;
EXTERN  int     mode; // 0:reader first, 1:writer first
EXTERN  int     print2; // 0: don't print2, 1: print2

// flag to indicate read or write 0:read, 1:write
EXTERN  int     read_or_write_flag;
