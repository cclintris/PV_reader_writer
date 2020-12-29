
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK	task_table[NR_TASKS] = {
        {ProcessA, STACK_SIZE_ProcessA, "ProcessA"},
        {ProcessB, STACK_SIZE_ProcessB, "ProcessB"},
        {ProcessC, STACK_SIZE_ProcessC, "ProcessC"},
        {ProcessD, STACK_SIZE_ProcessD, "ProcessD"},
        {ProcessE, STACK_SIZE_ProcessE, "ProcessE"},
        {ProcessF, STACK_SIZE_ProcessF, "ProcessF"},
};

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {
        sys_get_ticks,
        sys_print_str,
        sys_sleep,
        sys_print_color_str,
        sys_p_lock,
        sys_v_lock,
        sys_set_task_time,
        sys_get_task_time,
        sys_set_read_or_write_flag,
        sys_get_read_or_write_flag
};

