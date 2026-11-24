/*
 * Copyright (C) 2026 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Caitoa release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

//#ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Virtual memory module mm/mm-vm.c
 */

#include "string.h"
#include "mm64.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*get_vma_by_num - get vm area by numID
 *@mm: memory region
 *@vmaid: ID vm area to alloc memory region
 *
 */
struct vm_area_struct *get_vma_by_num(struct mm_struct *mm, int vmaid)
{
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

int __mm_swap_page(struct pcb_t *caller, addr_t vicfpn , addr_t swpfpn)
{
    __swap_cp_page(caller->krnl->mram, vicfpn, caller->krnl->active_mswp, swpfpn);
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
struct vm_rg_struct *get_vm_area_node_at_brk(struct pcb_t *caller, int vmaid, addr_t alignedsz)
{
  struct vm_rg_struct * newrg;
  /* TODO retrive current vma to obtain newrg */

  struct vm_area_struct *cur_vma = get_vma_by_num(caller->krnl->mm, vmaid);
  if (cur_vma == NULL){
    return NULL;
  }

  newrg = malloc(sizeof(struct vm_rg_struct));
  if(newrg == NULL){
    return newrg;
  }
  newrg->vmaid = vmaid;
  newrg->rg_next = NULL;

  /* TODO: update the newrg boundary */
  newrg->rg_start = cur_vma->sbrk;
  newrg->rg_end = cur_vma->sbrk + alignedsz;
  
  /* END TODO */

  return newrg;
}

/*validate_overlap_vm_area
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@vmastart: vma end
 *@vmaend: vma end
 *
 */
int validate_overlap_vm_area(struct pcb_t *caller, int vmaid, addr_t vmastart, addr_t vmaend)
{
  struct vm_area_struct *vma = caller->krnl->mm->mmap;

  /* TODO validate the planned memory area is not overlapped */
  if (vmastart >= vmaend)
  {
    return -1;
  }

  if (vma == NULL)
  {
    return -1;
  }

  /* TODO validate the planned memory area is not overlapped */

  struct vm_area_struct *cur_area = get_vma_by_num(caller->krnl->mm, vmaid);
  if (cur_area == NULL)
  {
    return -1;
  }

  while (vma != NULL)
  {
    if (vma != cur_area && OVERLAP(vmastart, vmaend, vma->vm_start, vma->vm_end))
    {
      return -1;
    }
    vma = vma->vm_next;
  }
  /* End TODO*/

  return 0;
}

/*inc_vma_limit - increase vm area limits to reserve space for new variable
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@inc_sz: increment size
 *
 */
int inc_vma_limit(struct pcb_t *caller, int vmaid, addr_t inc_sz)
{
  /* TODO with new address scheme, the size need tobe aligned 
   *      the raw inc_sz maybe not fit pagesize
   */ 
  if(inc_sz == 0)
    return -1;

  addr_t aligned_inc_sz = PAGING64_PAGE_ALIGNSZ(inc_sz);
  int inc_num_page = aligned_inc_sz / PAGING64_PAGESZ; 

  struct vm_area_struct *cur_vma = get_vma_by_num(caller->krnl->mm, vmaid);
  if(cur_vma == NULL)
    return -1;

  struct vm_rg_struct *area = get_vm_area_node_at_brk(caller, vmaid, aligned_inc_sz);
  if (area == NULL)
    return -1;

  /* 
   * TODO Validate overlap of obtained region 
   */
  if (validate_overlap_vm_area(caller, vmaid, area->rg_start, area->rg_end) < 0)
  {
    free(area);  
    return -1;  //Overlap and failed allocation 
  }

  /* TODO: Obtain the new vm area based on vmaid */
  addr_t old_end = cur_vma->vm_end;
  
  struct vm_rg_struct* newrg = malloc(sizeof(struct vm_rg_struct));
  if(newrg == NULL){
    free(area);
    return -1;
  }
  
  struct framephy_struct *frames = NULL;
  if(vmap_page_range(caller, area->rg_start, inc_num_page, frames, newrg) < 0){
    free(newrg);
    free(area);
    return -1;
  }


  cur_vma->vm_end = old_end + aligned_inc_sz;
  free(newrg);
  free(area);
  return 0;
}

// #endif
