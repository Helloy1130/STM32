//
// Created by Y on 2020/9/30.
//

#include "sram.h"

SRAM_HandleTypeDef SRAM_Handler;    //SRAM句柄

//在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FSMC_SRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n)
{
    for(;n!=0;n--)
    {
        *(__IO uint8_t*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

//在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FSMC_SRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n)
{
    for(;n!=0;n--)
    {
        *pBuffer++=*(__IO uint8_t*)(Bank1_SRAM3_ADDR+ReadAddr);
        ReadAddr++;
    }
}
