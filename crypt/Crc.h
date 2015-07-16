/******************************************************************************************
 * @file Crc.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011-5-18
 * @descirption:crc32������Ͳ���㷨
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_CRC32_H
#define BELIAL_CRYPT_CRC32_H
#include "typedef.h"
#include "define.h"


#ifdef __cplusplus
extern "C" {	//����C++
#endif
/*
 *@description ����CRC32��
 *@param plTable �洢���ָ�룬Ϊ256��long
 *@return
 */
EXTERN	void GenCrc32Table(unsigned int* pTable);

/*
 *@description ����pbDataBuf��crcֵ
 *@param pbDataBuf ��Ҫ����Ļ�����
 *@param len ��������С
 *@return 
 */
EXTERN	unsigned int GenCrc32(unsigned char*  pDataBuf, unsigned int nLen);

/**
  * �������ݵ�CRC64
  */
EXTERN	unsigned long long GenCrc64(unsigned char*  pDataBuf, unsigned int nLen);
#ifdef __cplusplus
}//����C++
#endif
#endif