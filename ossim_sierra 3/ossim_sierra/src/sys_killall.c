/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"
#include "queue.h"
#include "string.h"
#include "stdlib.h"

//Hàm có công dụng kết thúc tất cả tiến trình có tiên trùng với tên của tiến trình được truyền vào từ bộ nhớ
int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];
    uint32_t data;

    //hardcode for demo only
    uint32_t memrg = regs->a1;
    
    /* TODO: Get name of the target proc */
    //proc_name = libread..
    
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        proc_name[i]= data;
        if(data == -1) proc_name[i]='\0';
        i++;
    }

    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    //caller->running_list
    //caller->mlq_ready_queu
    struct queue_t *que1 = caller->running_list;
    struct queue_t *que2 = caller->mlq_ready_queue;

    if (que1 == NULL) {
        return -1;
    }

    if (que2 == NULL) {
        return -1;
    }

    struct pcb_t *proc_tmp;
    /* TODO Maching and terminating 
     *       all processes with given
     *        name in var proc_name
     */
    
    for (int i = 0; i < que1->size; i++) {
        proc_tmp = que1->proc[i + 1];
        if (strcmp(proc_tmp->path, proc_name) == 0 && proc_tmp != caller) {
            if (proc_tmp->mm && proc_tmp->mm->pgd) {
                free_pcb_memph(proc_tmp);
            }
            else {
                free(proc_tmp);
            }

            syscall(proc_tmp, 101, regs);
        }
    }

    for (int i = 0; i < que2->size; i++) {
        proc_tmp = que2->proc[i + 1];
        if (strcmp(proc_tmp->path, proc_name) == 0) {
            if (proc_tmp->mm && proc_tmp->mm->pgd) {
                free_pcb_memph(proc_tmp);
            }
            else {
                free(proc_tmp);
            }
            
            syscall(proc_tmp, 101, regs);
        }
    }

    return 0; 
}
