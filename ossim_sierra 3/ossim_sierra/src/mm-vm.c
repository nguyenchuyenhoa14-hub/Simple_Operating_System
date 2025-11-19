// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Virtual memory module mm/mm-vm.c
 */

#include "string.h"
#include "mm.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*get_vma_by_num - get vm area by numID
 *@mm: memory region
 *@vmaid: ID vm area to alloc memory region
 *
 */

//Hàm tìm và trả về vùng nhớ ảo (vm_area_struct) có vm_id tương ứng với vmaid trong tiến trình.
struct vm_area_struct *get_vma_by_num(struct mm_struct *mm, int vmaid) {
    struct vm_area_struct *pvma = mm->mmap;

    if (mm->mmap == NULL)
      return NULL;

    int vmait = pvma->vm_id;

    while (vmait < vmaid)
    {
      if (pvma == NULL)
        return NULL;

      pvma = pvma->vm_next;
      vmait = pvma->vm_id;
    }
    return pvma;
}

//Hàm thực hiện hoán trang (swap-out) từ RAM sang SWAP.
int __mm_swap_page(struct pcb_t *caller, int vicfpn , int swpfpn) {
    __swap_cp_page(caller->mram, vicfpn, caller->active_mswp, swpfpn);
    return 0;
}

/*get_vm_area_node - get vm area for a number of pages
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@incpgnum: number of page
 *@vmastart: vma end
 *@vmaend: vma end
 *
 */

//Hàm tạo vùng nhớ mới cho tiến trình tại vị trí hiện tại của sbrk (tức là vùng heap mở rộng thêm).
struct vm_rg_struct *get_vm_area_node_at_brk(struct pcb_t *caller, int vmaid, int size, int alignedsz) {
    struct vm_rg_struct * newrg;
    newrg = malloc(sizeof(struct vm_rg_struct));

    /* TODO retrive current vma to obtain newrg, current comment out due to compiler redundant warning*/
    //struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);
    struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

    /* TODO: update the newrg boundary
    // newrg->rg_start = ...
    // newrg->rg_end = ...
    */
    newrg->rg_start = cur_vma->sbrk;
    newrg->rg_end = cur_vma->sbrk + alignedsz;

    return newrg;
}

/*validate_overlap_vm_area
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@vmastart: vma end
 *@vmaend: vma end
 *
 */

//Hàm này được dùng để kiểm tra xem vùng nhớ mới cần cấp phát [vmastart, vmaend) có bị trùng lặp (overlap) với các vùng nhớ trống hiện có trong vùng nhớ ảo (VMA) hay không.
int validate_overlap_vm_area(struct pcb_t *caller, int vmaid, int vmastart, int vmaend) {
    //struct vm_area_struct *vma = caller->mm->mmap;

    /* TODO validate the planned memory area is not overlapped */

    struct vm_area_struct *vma = caller->mm->mmap;

    while (vma != NULL) {
        if (vma->vm_start == vma->vm_end) {
            vma = vma->vm_next;
            continue;
        }
        if (vma->vm_id == vmaid) {
            if (!(vmaend < vma->vm_start || vmastart > vma->vm_end)) {
                return -1;
            }
            struct vm_rg_struct *tmp = vma->vm_freerg_list;
            while(tmp != NULL) {
                if (!(vmaend < vma->vm_start || vmastart > vma->vm_end)) {
                    return -1;
                }
                tmp = tmp->rg_next;
            }
        }
        vma = vma->vm_next;
    }
  return 0;
}

/*inc_vma_limit - increase vm area limits to reserve space for new variable
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@inc_sz: increment size
 *
 */
int inc_vma_limit(struct pcb_t *caller, int vmaid, int inc_sz) {
    struct vm_rg_struct * newrg = malloc(sizeof(struct vm_rg_struct));
    int inc_amt = PAGING_PAGE_ALIGNSZ(inc_sz);
    int incnumpage =  inc_amt / PAGING_PAGESZ;
    struct vm_rg_struct *area = get_vm_area_node_at_brk(caller, vmaid, inc_sz, inc_amt);
    struct vm_area_struct *cur_vma = get_vma_by_num(caller->mm, vmaid);

    int old_end = cur_vma->vm_end;

    /*Validate overlap of obtained region */
    if (validate_overlap_vm_area(caller, vmaid, area->rg_start, area->rg_end) < 0)
        return -1; /*Overlap and failed allocation */

    /* TODO: Obtain the new vm area based on vmaid */
    //cur_vma->vm_end... 
    // inc_limit_ret...

    cur_vma->vm_end = cur_vma->vm_end + inc_sz;

    if (vm_map_ram(caller, area->rg_start, area->rg_end, old_end, incnumpage , newrg) < 0)
        return -1; /* Map the memory to MEMRAM */

    return 0;
}

// #endifz