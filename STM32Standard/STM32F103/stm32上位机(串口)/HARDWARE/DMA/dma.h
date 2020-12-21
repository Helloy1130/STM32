#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
							    					    
void DMA2_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//≈‰÷√DMA2_CHx
void DMA1_Config(void);
	
#endif
