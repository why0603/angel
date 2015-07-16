/******************************************************************************************
 * @file RC.h
 * @anthor:Croky.Zheng
 * @CreateTime:2012-3-20
 * @descirption: RC5,6�㷨
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_RC_H
#define BELIAL_CRYPT_RC_H
#include "typedef.h"
#include "define.h"

//rc5�����ĳ���
#define ENRC5SIZE(x)	CHECKDIV8(x)
//rc6�����ĳ���
#define ENRC6SIZE(x)	CHECKDIV16(x)

//RC5�����ĳ���
#define DERC5SIZE(x)	(x)
//RC6�����ĳ���
#define DERC6SIZE(x)	(x)


typedef enum __enum_Rc_MODE{
	Rc_Mode_Min = -1,
	RC_Mode_RC5 = 0,//tea
	RC_Mode_RC6 = 1,//xtea
	Rc_Mode_Max
} RcMode;

typedef struct {
	//�ֳ� λ
	unsigned int nWordSizeBit;

	//��������
	unsigned int nRound;

	//�ֳ� �ֽ�
	unsigned int nWordSizeByte;
	
	//��Կ
	unsigned char *pKey;

	//��Կ����
	unsigned int nKeyLen;

	//��λ��=log2(nWordSizeBit)
	unsigned int nShiftCount;

	//��Կ����
	unsigned long* pSubKey;

	//��Կ���鳤��
	unsigned int nSubKeyLen;

	//����ģʽ
	RcMode mode;
} RCContext,*PRCContext;


#ifdef __cplusplus
extern "C" {	//����C++
#endif
/********************************************
 * ��ʼ��RC6�㷨
 * param pContext MD5Context�ṹ��
 *******************************************/
EXTERN void RCInit(PRCContext pContext);

/***********************************************************
 * ���ַ���ʹ��rc5�㷨����
 * @param pContext �㷨��ʼ��ʱ���������
 * @param pInput  ��Ҫ���ܵ�����
 * @param pOutput	���ܺ�����ı����ָ��
 * @param nSize	���ĵĳ���,���Ȳ���8�ı���ʱ���������ֽڣ�ֵΪ0
 * @return 
 ***********************************************************/
EXTERN	void RCEncrypt(PRCContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);

/***********************************************************
 * ���ַ���ʹ��TEA�㷨����
 * @param pContext �㷨��ʼ��ʱ���������
 * @param pInput  ����
 * @param pOutput ���ܺ������
 * @param nSize	���ĵĳ���
 * @return 
 ***********************************************************/
EXTERN	void RCDecrypt(PRCContext pContext,
	void* pInput,
	unsigned int nSize,
	void* pOutput);

EXTERN void RCDestroy(PRCContext pContext);

//CYEXTERN void RC6SetKey(PRC6Context pContext,char* pKey,unsigned int nLen);
#ifdef __cplusplus
}
#endif
#endif