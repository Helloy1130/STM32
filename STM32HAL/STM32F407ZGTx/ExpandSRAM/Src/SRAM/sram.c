//
// Created by Y on 2020/9/30.
//

#include "sram.h"

SRAM_HandleTypeDef SRAM_Handler;    //SRAM���

//��ָ����ַ(WriteAddr+Bank1_SRAM3_ADDR)��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n)
{
    for(;n!=0;n--)
    {
        *(__IO uint8_t*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

//��ָ����ַ((WriteAddr+Bank1_SRAM3_ADDR))��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void FSMC_SRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n)
{
    for(;n!=0;n--)
    {
        *pBuffer++=*(__IO uint8_t*)(Bank1_SRAM3_ADDR+ReadAddr);
        ReadAddr++;
    }
}
