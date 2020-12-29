
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;

	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table+NR_TASKS; p++) {
			if ((p->ticks > greatest_ticks) && (p->block_until <= 0) && (p->pv_state == PV_PROCESS_READY)) {
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}else if(p->block_until > 0){
			    p->block_until--;
            }
		}

		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
				p->ticks = p->priority;
			}
		}
	}
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

/*======================================================================*
                           sys_print_str
 *======================================================================*/
PUBLIC void sys_print_str(char* str) {
    if (disp_pos > 80 * 25 * 2) {
        disp_pos = 0;
        for (int i=0 ; i<80*25 ; i++) {
            disp_str(" ");
        }
        disp_pos = 0;
    }
    disp_str(str);
}

/*======================================================================*
                           sys_sleep
 *======================================================================*/
PUBLIC void sys_sleep(int milli_seconds) {
    // disp_str("sleep");
    // disp_int(milli_second);

    // turn A in BLOCK state
    // p_proc_ready->block_until = milli_seconds * 10;
    p_proc_ready->block_until = milli_seconds;

    // give control to next proc
    schedule();
}

/*======================================================================*
                           sys_print_color_str
 *======================================================================*/
PUBLIC void sys_print_color_str(char* str, int color) {
    if (disp_pos > 80 * 25 * 2) {
        disp_pos = 0;
        for (int i=0 ; i<80*25 ; i++) {
            disp_str(" ");
        }
        disp_pos = 0;
    }
    disp_color_str(str, color);
}

/*======================================================================*
                           sys_p_lock
 *======================================================================*/
PUBLIC void sys_p_lock(LOCK *lock) {
    lock->src--;

    if (lock->src < 0) {
        // block p_proc_ready: state
        p_proc_ready->pv_state = PV_PROCESS_BLOCK;
        lock->block_list[lock->head] = p_proc_ready;
        lock->head++;
        if(lock->head == PV_MAX_LIST_SIZE) {
            lock->head = 0;
        }
        // give control to next proc
        schedule();
    }
}

/*======================================================================*
                           sys_v_lock
 *======================================================================*/
PUBLIC void sys_v_lock(LOCK *lock) {
    lock->src++;

    if (lock->src <= 0) {
        // wakeup first proc in list
        lock->block_list[lock->tail]->pv_state = PV_PROCESS_READY;
        lock->tail++;
        if(lock->tail == PV_MAX_LIST_SIZE) {
            lock->tail = 0;
        }
    }
}

/*======================================================================*
                           sys_set_task_time
 *======================================================================*/
PUBLIC void sys_set_task_time(int task_time) {
    p_proc_ready->task_time = task_time;
}

/*======================================================================*
                           sys_get_task_time
 *======================================================================*/
PUBLIC int sys_get_task_time() {
    return p_proc_ready->task_time;
}

/*======================================================================*
                           sys_set_read_or_write_flag
 *======================================================================*/
PUBLIC int sys_set_read_or_write_flag(int read_or_write_flag) {
    read_or_write_flag = read_or_write_flag;
}

/*======================================================================*
                           sys_get_read_or_write_flag
 *======================================================================*/
PUBLIC int sys_get_read_or_write_flag() {
    return read_or_write_flag;
}
