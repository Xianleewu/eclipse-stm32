/*
 * malloc.h
 *
 *  Created on: 2017年5月6日
 *      Author: xianlee
 */

#ifndef MALLOC_MALLOC_H_
#define MALLOC_MALLOC_H_

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

#define MEM_BLOCK_SIZE			32
#define MEM_MAX_SIZE			35*1024
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE

struct _m_mallco_dev
{
	void (*init)(void);
	u8 (*perused)(void);
	u8 	*membase;
	u16 *memmap;
	u8  memrdy;
};

extern struct _m_mallco_dev mallco_dev;

void mymemset(void *s,u8 c,u32 count);
void mymemcpy(void *des,void *src,u32 n);
void mem_init(void);
u32 mem_malloc(u32 size);
u8 mem_free(u32 offset);
u8 mem_perused(void);


void myfree(void *ptr);
void *mymalloc(u32 size);
void *myrealloc(void *ptr,u32 size);


#endif /* MALLOC_MALLOC_H_ */
