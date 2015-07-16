/*********************************************************************
 * @file Blowfish.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011/8/3
 * @descirption:ʹ��BLOWFISH�㷨���ܣ���������
 **********************************************************************/
#ifndef BELIAL_CRYPT_BLOWFISH_H
#define BELIAL_CRYPT_BLOWFISH_H
#include "define.h"
#include "typedef.h"

typedef enum __enum_BF_mode
{
	BF_Mode_Min = -1,
	BF_Mode_ECB = 0,//ֱ�Ӽ���
	BF_Mode_BCB = 1,//���ϸ����������֮���ټ���
	BF_Mode_Max
} BlowFishMode;

typedef struct {
	char* pKey;
	unsigned int nLen;
	unsigned int pbox[18];
	unsigned int sbox[4][256];
	BlowFishMode mode;
} BlowFishContext,*PBlowFishContext;

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**************************************************************
 * �����㷨��ʼ������Ҫʹ�ü�����Կ
 * @param pKey ��Կ
 * @param nSize ��Կ����
 *************************************************************/
/*
EXTERN void BlowFishInit(char* pKey,unsigned int nLen);
*/
/**************************************************************
 * ʹ��BLOWFISH��������
 * @param pInput ����
 * @param nSize ���ĳ���
 * @param pOutput ����
 * @param pSize ���ĵĳ��� �����أ�
 * @param mode �����㷨��ģʽ 0,1
 *************************************************************/
/*
EXTERN void BlowFishEncrypt(void* pInput,
	unsigned int nSize,
	void* pOutput,
	BlowFishMode mode);
*/
/**************************************************************
 * ʹ��BLOWFISH��������
 * @param pInput ����
 * @param nSize ���ĳ���
 * @param pOutput ����
 * @param pSize ���ĵĳ��� �����أ�
 * @param mode �����㷨��ģʽ 0,1
 *************************************************************/
/*
EXTERN void BlowFishDecrypt(void* pInput,
	unsigned int nSize,
	void* pOutput,
	BlowFishMode mode);
*/


/*
 * BlowFishInit�Ķ��̰߳汾
 */
EXTERN void BlowFishInit(PBlowFishContext pContext);

/*
 * BlowFishEncrypt�Ķ��̰߳汾
 */
EXTERN void BlowFishEncrypt(PBlowFishContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);

/*
 * BlowFishDecrypt�Ķ��̰߳汾
 */
EXTERN void BlowFishDecrypt(PBlowFishContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);
#ifdef __cplusplus
}//����C++
#endif
#endif