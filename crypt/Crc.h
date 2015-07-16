/******************************************************************************************
 * @file Crc.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011-5-18
 * @descirption:crc32产生表和查表算法
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_CRC32_H
#define BELIAL_CRYPT_CRC32_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//兼容C++
#endif
/*
 *@description 生成CRC32表
 *@param plTable 存储表的指针，为256个long
 *@return
 */
EXTERN	void GenCrc32Table(unsigned int* pTable);

/*
 *@description 计算pbDataBuf的crc值
 *@param pbDataBuf 需要计算的缓冲区
 *@param len 缓冲区大小
 *@return 
 */
EXTERN	unsigned int GenCrc32(unsigned char*  pDataBuf, unsigned int nLen);

/**
  * 生成数据的CRC64
  */
EXTERN	unsigned long long GenCrc64(unsigned char*  pDataBuf, unsigned int nLen);
#ifdef __cplusplus
}//兼容C++
#endif
#endif