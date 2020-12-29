
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
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

#define READ 0
#define WRITE 1

/*======================================================================*
                            init_all_process
 *======================================================================*/
PRIVATE void init_all_process() {
    proc_table[0].block_until = 0;
    proc_table[1].block_until = 0;
    proc_table[2].block_until = 0;
    proc_table[3].block_until = 0;
    proc_table[4].block_until = 0;
    proc_table[5].block_until = 0;

    proc_table[0].ticks = proc_table[0].priority = 1;
    proc_table[1].ticks = proc_table[1].priority = 1;
    proc_table[2].ticks = proc_table[2].priority = 1;
    proc_table[3].ticks = proc_table[3].priority = 1;
    proc_table[4].ticks = proc_table[4].priority = 1;
    proc_table[5].ticks = proc_table[5].priority = 1;

    proc_table[0].pv_state = PV_PROCESS_READY;
    proc_table[1].pv_state = PV_PROCESS_READY;
    proc_table[2].pv_state = PV_PROCESS_READY;
    proc_table[3].pv_state = PV_PROCESS_READY;
    proc_table[4].pv_state = PV_PROCESS_READY;
    proc_table[5].pv_state = PV_PROCESS_READY;
}

/*======================================================================*
                            init_lock
 *======================================================================*/
PRIVATE void init_lock() {
    read_lock.src = 3;
    read_lock.head = read_lock.tail = 0;
    write_lock.src = 1;
    write_lock.head = write_lock.tail = 0;
    mutex_lock.src = 1;
    mutex_lock.head = mutex_lock.tail = 0;
    mutex_lock1.src = 1;
    mutex_lock1.head = mutex_lock1.tail = 0;
    mutex_lock2.src = 1;
    mutex_lock2.head = mutex_lock2.tail = 0;
    read_count = 0;
    write_count = 0;
    mode = 0;
    print2 = 0;
}

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main() {
    disp_str("-----\"kernel_main\" begins-----\n");

    TASK *p_task = task_table;
    PROCESS *p_proc = proc_table;
    char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    int i;
    for (i = 0; i < NR_TASKS; i++) {
        strcpy(p_proc->p_name, p_task->name);    // name of the process
        p_proc->pid = i;            // pid

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
               sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
               sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
        p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
                          | SA_TIL | RPL_TASK;
        p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
                          | SA_TIL | RPL_TASK;
        p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
                          | SA_TIL | RPL_TASK;
        p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
                          | SA_TIL | RPL_TASK;
        p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
                          | SA_TIL | RPL_TASK;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK)
                          | RPL_TASK;

        p_proc->regs.eip = (u32) p_task->initial_eip;
        p_proc->regs.esp = (u32) p_task_stack;
        p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    // init
    init_all_process();
    init_lock();

    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;

    /* 初始化 8253 PIT */
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));

    put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
    enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */

    // clear screen whenever screen full
    if (disp_pos > 80 * 25 * 2) {
        disp_pos = 0;
        for (int i = 0; i < 80 * 25; i++) {
            disp_str(" ");
        }
        disp_pos = 0;
    }

    restart();

    // while(1){}
}

/*
 * ----------------------
 * |  r1  |    读开始    |
 * |  r2  |      读     |
 * |  r3  |    读完成    |
 * |  w1  |    写开始    |
 * |  w2  |      写     |
 * |  w3  |    写完成    |*/

/*======================================================================*
                            reader_first
 *======================================================================*/
void reader_first_strategy_reader(char *process, int color, int task_time) {
    print_color_str(process, color);
    print_color_str("r1.", color);

    p_lock(&mutex_lock);
    read_count += 1;
    if (read_count == 1) {
        p_lock(&write_lock);
    }
    p_lock(&read_lock);
    set_read_or_write_flag(READ);
    v_lock(&mutex_lock);

    // {read ops} 臨界區
    set_task_time(task_time);
    while(get_task_time() > 0) {
        if (print2) {
            print_color_str(process, color);
            print_color_str("r2.", color);
        }
    }
    // sleep(500);

    p_lock(&mutex_lock);
    read_count -= 1;
    if (read_count == 0) {
        v_lock(&write_lock);
    }
    v_lock(&read_lock);
    v_lock(&mutex_lock);

    print_color_str(process, color);
    print_color_str("r3.", color);
}

void reader_first_strategy_writer(char *process, int color, int task_time) {
    print_color_str(process, color);
    print_color_str("w1.", color);

    p_lock(&write_lock);
    set_read_or_write_flag(WRITE);

    // {write ops} 臨界區
    set_task_time(task_time);
    while(get_task_time() > 0) {
        if(print2) {
            print_color_str(process, color);
            print_color_str("w2.", color);
        }
    }
    // sleep(500);

    v_lock(&write_lock);

    print_color_str(process, color);
    print_color_str("w3.", color);
}

/*======================================================================*
                            writer_first
 *======================================================================*/
void writer_first_strategy_reader(char *process, int color, int task_time) {
    print_color_str(process, color);
    print_color_str("r1.", color);

    p_lock(&mutex_lock2);
    p_lock(&read_lock);
    p_lock(&mutex_lock);
    read_count += 1;
    if(read_count == 1) {
        p_lock(&write_lock);
    }
    v_lock(&mutex_lock);
    v_lock(&read_lock);
    set_read_or_write_flag(READ);
    v_lock(&mutex_lock2);

    // {read ops} 臨界區
    set_task_time(task_time);
    while(get_task_time() > 0) {
        if(print2) {
            print_color_str(process, color);
            print_color_str("r2.", color);
        }
    }
    // sleep(500);

    p_lock(&mutex_lock);
    read_count--;
    if(read_count == 0) {
        v_lock(&write_lock);
    }
    v_lock(&mutex_lock);
}

void writer_first_strategy_writer(char *process, int color, int task_time) {
    p_lock(&mutex_lock1);
    write_count += 1;
    if(write_count == 1) {
        p_lock(&read_lock);
    }
    v_lock(&mutex_lock1);

    p_lock(&write_lock);
    set_read_or_write_flag(WRITE);

    // {write ops} 臨界區
    set_task_time(task_time);
    while(get_task_time() > 0) {
        if(print2) {
            print_color_str(process, color);
            print_color_str("w2.", color);
        }
    }
    v_lock(&write_lock);
    // sleep(500);

    p_lock(&mutex_lock1);
    write_count--;
    if(write_count == 0) {
        v_lock(&read_lock);
    }
    v_lock(&mutex_lock1);
}

/*======================================================================*
                            int_2_string
 *======================================================================*/


/*======================================================================*
                            regular_ProcessF
 *======================================================================*/
//void regular_ProcessF(char *process, int color, int task_time) {
//    print_color_str(process, color);
//    set_task_time(task_time);
//    while(get_task_time() > 0) {
//        sleep(1);
//    }
//}


/*======================================================================*
                               ProcessA
 *======================================================================*/
void ProcessA() {
    while (1) {
        if(mode) {
            // writer first
            writer_first_strategy_reader("A", COLOR_BLUE, 2);
            sleep(500);
        }else {
            // reader first
            reader_first_strategy_reader("A", COLOR_BLUE, 2);
            sleep(500);
        }
    }
}

/*======================================================================*
                               ProcessB
 *======================================================================*/
void ProcessB() {
    while (1) {
        if (mode) {
            // writer first
            writer_first_strategy_reader("B", COLOR_GREEN, 3);
            sleep(500);
        } else {
            // reader first
            reader_first_strategy_reader("B", COLOR_GREEN, 3);
            sleep(500);
        }
    }
}

/*======================================================================*
                               ProcessC
 *======================================================================*/
void ProcessC() {
    while (1) {
        if(mode) {
            // writer first
            writer_first_strategy_reader("C", COLOR_CYAN, 3);
            sleep(500);
        }else {
            // reader first
            reader_first_strategy_reader("C", COLOR_CYAN, 3);
            sleep(500);
        }
    }
}

/*======================================================================*
                               ProcessD
 *======================================================================*/
void ProcessD() {
    while (1) {
        if(mode) {
            // writer first
            writer_first_strategy_writer("D", COLOR_RED, 3);
            sleep(500);
        }else {
            // reader first
            reader_first_strategy_writer("D", COLOR_RED, 3);
            sleep(500);
        }
    }
}

/*======================================================================*
                               ProcessE
 *======================================================================*/
void ProcessE() {
    while (1) {
        if(mode) {
            // writer first
            writer_first_strategy_writer("E", COLOR_WHITE, 4);
            sleep(500);
        }else {
            // reader first
            reader_first_strategy_writer("E", COLOR_WHITE, 4);
            sleep(500);
        }
    }
}

/*======================================================================*
                               ProcessF
 *======================================================================*/
void ProcessF() {
    while (1) {
//        regular_ProcessF("F", COLOR_YELLOW, 1);
        print_color_str("F", COLOR_YELLOW);
        if(get_read_or_write_flag()) {
            // writing
            print_color_str("w.", COLOR_YELLOW);
        }else {
            // reading
            print_color_str("r.", COLOR_YELLOW);
            char read_count_str[100];
            print_color_str(itoa(read_count_str, read_count), COLOR_YELLOW);
        }
        milli_delay(500);
    }
}
