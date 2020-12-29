# 操作系统实验四 - 说明文档

[TOC]

## 系统调用

### 系统调用流程
添加调用系统所需牵涉的文件如下:
(以下假设添加系统调用名为 xxx)

- /kernel/proc.c
在此文件中，要添加系统调用的的方法名和逻辑，统一以 sys_xxx 命名<br><br/>

- /kernel/syscall.asm
在此文件中，要添加与 /kernel/global.c 中 sys_call_table 索引一一对应的系统调用向量，系统调用的汇编代码，以及对外的导出符号。统一以 xxx 命名<br><br/>

- /kernel/global.c
在此文件中，要将新添加的系统调用名(与 /kernel/proc.c)中一样添加到 sys_call_table 中<br><br/>

- /include/const.h
在此文件中，要更新 NR_SYS_CALL 常数，表示的是
当前系统调用个数<br><br/>

### /kernel/proc.c
在此文件中，我添加了以下方法:
- PUBLIC int sys_get_ticks()
该方法返回的是当前的 ticks<br><br/>

- PUBLIC void sys_print_str(char* str)
该方法实现了打印一个给定的字符串<br><br/>

- PUBLIC void sys_print_color_str(char* str, int color)
该方法实现了以给定的颜色代码打印一个给定的字符串<br><br/>

- PUBLIC void sys_sleep(int milli_seconds)
该方法实现作业要求一，调用此方法进程会在milli_seconds 毫秒内不被分配时间片<br><br/>

- PUBLIC void sys_p_lock(LOCK *lock)
该方法实现了 PV 操作的 P 操作<br><br/>

- PUBLIC void sys_v_lock(LOCK *lock)
该方法实现了 PV 操作的 V 操作<br><br/>

- PUBLIC void sys_set_task_time(int task_time)
该方法实现了根据给定的 task_time 设定当前进程分配的时间片<br><br/>

- PUBLIC int sys_get_task_time()
该方法实现了返回当前进程分配的时间片<br><br/>

- PUBLIC int sys_set_read_or_write_flag(int read_or_write_flag)
该方法实现了设定当前进程为读或是写状态(通过 /include/global.h 中的 read_or_write_flag 标记位)<br><br/>

- PUBLIC int sys_get_read_or_write_flag()
该方法实现了返回当前进程为读或是写状态(通过 /include/global.h 中的 read_or_write_flag 标记位)<br><br/>

### /kernel/syscall.asm
在此文件中，我添加了 10 个系统调用向量。用于汇编代码，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc1.png)
10 个导出符号，以供外部文件使用，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc2.png)
以及每个系统调用的汇编代码:
- get_ticks
- sleep
- print_str
- print_color_str
- p_lock
- v_lock
- set_task_time
- get_task_time
- set_read_or_write_flag
- get_read_or_write_flag

我的思路是依据 /kernel/kernel.asm 中的 sys_call 例程。将系统调用向量压入 eax 中，若该系统调用要接受参数，则将 esp + 4 压入 ebx，然后调用系统调用的中断向量(INT_VECTOR_SYS_CALL)，最后 ret。

### /kernel/global.c
在此文件中，我为每个系统调用在 sys_call_table 中都注册一次，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc3.png)

### /include/const.h
在此文件中，我将 NR_SYS_CALL 常数更改为 10，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc4.png)

## 进程结构
在 /include/proc.h 中我对 s_proc 结构体添加了一些必要的属性，最后如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc5.png)

我添加的属性为:
- block_until:代表进程剩余的阻塞时间(作业要求1)
- pv_state:代表进程在 PV 操作时为阻塞态或者就绪态
- task_time:代表进程剩余的时间片

因应作业要求3，所有进程声明如下图:
/include/proc.h
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc13.png)
/kernel/global.c
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc14.png)

## 锁
在 /include/proc.h 中我添加了锁的结构体，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc9.png)

- src:代表该锁的资源数，也就相当于同时有多少个进程可以获得访问的权利
- block_list:当锁当前无剩余资源，却又有进程想获得，就将进程加入到 block_list 队列中
- head:用来维护 block_list 的头指针
- tail:用来维护 block_list 的尾指针

因应作业要求3，所有锁声明见读者-写者问题。

## schedule()
该方法也是一个系统调用，见 /kernel/proc.c，是基础代码的一部份，但是我对其中的判断条件做了修改，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc10.png)
修改了 if 条件，我的思路是必须在进程的阻塞剩余时间小于 0(p->block_until <= 0)，且 PV 操作完的状态为就绪态(p->pv_state = PV_PROCESS_READY)，才能参与到调度的选择之中，否则若还有阻塞时间(p->block_until > 0)，就递减，因为又过了一个时钟周期，那如果是PV 操作完的状态为阻塞态，那么就需要等到调用 V 操作来转换状态。

## 初始化
对于所有进程以及锁，我们都需要进行初始化，如下图:

- 初始化进程:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc11.png)
我的思路是将每个进程的 ticks 和 priority 都设为 1，因为每个在 main() 中都会调用时钟中断处理程序 clock_handler()，在 clock_handler() 中又会调用 schedule()。(具体见 /kernel/clock.c 中 clock_handler() 逻辑。)
所以我这么做就相当于每次都会调用 schedule，比较方便思考和理解。<br><br/>

- 初始化锁:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc12.png)
因应作业要求3，见读者-写者问题。

## PV 操作
我的思路将 PV 操作包装成了两个系统调用，所以牵涉到的文件就是系统调用流程提到的四个文件。

为了表示进程调用了 PV 操作后的状态，我还在/include/proc.h 中添加了以下状态:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc6.png)
 
### P 操作
P 操作具体的逻辑在 /kernel/proc.c 中，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc7.png)

### V 操作
V 操作具体的逻辑在 /kernel/proc.c 中，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc8.png)

## 读者-写者 问题
因应作业要求3，有关读者写者问题的变量声明如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc16.png)
初始化所有锁以及相关变量:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc12.png)
因为同时可以有 3 读者，故 read_lock.src = 3。其他锁都只能同时由一个进程获得，因此 src = 1。read_count，write_count，则为实现作业要求的 F 进程，表示当前读者数或写者数。mode 是用于切换读者优先还是写者优先的开关，print2 则是为了打印方便的开关，read_or_write_flag 则是一个标志位，表示当前进程是在读还是在写。

具体代码都在 /kernel/main.c 中。
这边附上一篇我理解后整理的博客:[PV 操作 - 读者写者问题](https://blog.csdn.net/weixin_46803507/article/details/111116042)

### 读者优先
- 打印信息约定符号表:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc17.png)

- 读者进程:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc18.png)
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc19.png)

- 写者进程:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc20.png)

### 写者优先
- 打印信息约定符号表:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc17.png)

- 读者进程:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc21.png)
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc22.png)

- 写者进程:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc23.png)
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc24.png)

### 实际调用
因应作业要求3，在 /include/const.h 中我添加了六种颜色的代号常量，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc31.png)

ABC 为读者进程，实际调用如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc25.png)
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc26.png)
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc27.png)

DE 为写者进程，实际调用如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc28.png)
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc29.png)

F 实际调用如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc30.png)

## kernel_main
在 /kernel/main.c 的 kernel_main() 中，我添加了一些初始化方法调用，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc32.png)
然后，因为基础代码一开始打印了部分信息，以及为了打印上美观，每次当打印信息填满整个屏幕，我都要做一次清屏，如下图:
![](https://wtfhhh.oss-cn-beijing.aliyuncs.com/os4doc33.png)

