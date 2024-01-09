#ifndef __MEM_RW_CHG_H__
#define __MEM_RW_CHG_H__


#ifdef __aarch64__

void set_init_mm_ptr(void* ptr);

void* get_init_mm_ptr();

int set_page_rw(unsigned long addr);

int set_page_ro(unsigned long addr);

#endif

#endif//__MEM_RW_CHG_H__