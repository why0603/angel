/******************************************************************************************
 * @file charUtils.h
 * @anthor:Croky.Zheng
 * @CreateTime:
 * @descirption:�ַ��������һЩ����
 *****************************************************************************************/
#ifndef BELIAL_COMMON_CHARUTILS_H
#define BELIAL_COMMON_CHARUTILS_H
#include "typedef.h"
#include "define.h"

#define ISDIGIT(c) ((((c)>='0') && ((c)<='9')) ? TRUE:FALSE)
#define ISUPPER(c) ((((c)>='A') && ((c)<='Z')) ? TRUE:FALSE)
#define ISLOWER(c) ((((c)>='a') && ((c)<='z')) ? TRUE:FALSE)
#define ISLETTER(c) (ISUPPER(c) | ISLOWER(c))
#define ISLETTERORDIGHT(c) (ISLETTER(c) | ISDIGIT(c))

#define ISOCTLETTER(c) ((((c)>='0') && ((c)<='7')) ? TRUE:FALSE)
#define ISBINLETTER(c) ((((c)=='1') || ((c)=='0')) ? TRUE:FALSE)
#ifdef __cplusplus
extern "C" {	//����C++
#endif
//������ת��Ϊ16�����ַ���ʽ
EXTERN	UINT8 Int2HexStr(UINT64 number,PCHAR pStr);

//������ת��Ϊ2�����ַ���ʽ
EXTERN	UINT8	Int2BinStr(UINT64 number,PCHAR pStr);

//������ת��Ϊ8�����ַ���ʽ
EXTERN	UINT8	Int2OctStr(UINT64 number,PCHAR pStr);

//������ת��Ϊ10�����ַ���ʽ
EXTERN	UINT8	Int2DecStr(INT32 number,PCHAR pStr);

//����ָ���Ľ��ƽ�����ת��Ϊ�ַ�
EXTERN	UINT8	Int2Str(UINT64 number,INT32 shift,PCHAR pStr);

/********************************************************
 * ���ֽ���ת��Ϊ16���Ƶ��ַ���
 * @param pInput ��Ҫת�����ֽ���
 * @param pOutput ת����Ľ�����
 * @param nLen pInput�ĳ���
 * @param cDelimiter �ֽڷָ���
 * @return ת����ĳ���
 *******************************************************/
EXTERN UINT Byte2Hex(PBYTE pInput,
	PCHAR pOutput,
	UINT nLen,
	CHAR cDelimiter);
/*
EXTERN BOOL IsDigit(CHAR c);//�ַ��Ƿ���������
EXTERN BOOL IsUpper(CHAR c);//�Ƿ��д��ĸ
EXTERN BOOL IsLower(CHAR c);//�Ƿ�Сд��ĸ
EXTERN BOOL IsLetter(CHAR c); //�Ƿ�����ĸ
EXTERN BOOL IsLetterOrDigit(CHAR c); //�Ƿ�����ĸ��������
*/
//�����ַ����ĳ��ȣ���0Ϊ����
EXTERN	UINT32 StrLen(PCHAR pStr);

//��pStr ���� �ַ����� pDest
EXTERN	PCHAR StrCopy(PCHAR pSrc,PCHAR pDest,UINT32 srcLen);

//�Ƚ������ַ����Ƿ����
EXTERN	BOOL StrEqual(PCHAR pSrc,PCHAR pDest);

//�Ƚ������ַ����Ƿ����
EXTERN	INT32 StrComp(PCHAR pSrc,PCHAR pDest);

//���һ���ַ�����HASHֵ
EXTERN	UINT32 Hash(PCHAR key);//,UINT32 length) ;

/**
 * ��16���Ƶ��ַ���תΪ��������
 * @param pStr Ҫת�����ַ���
 * @return ת���������
 */
EXTERN	INT32 HexStr2Int(PCHAR pStr);
/**
 * ��8���Ƶ��ַ���תΪ��������
 * @param pStr Ҫת�����ַ���
 * @return ת���������
 */
EXTERN	INT32 OctStr2Int(PCHAR pStr);
/**
 * ��2���Ƶ��ַ���תΪ��������
 * @param pStr Ҫת�����ַ���
 * @return ת���������
 */
EXTERN	INT32 BinStr2Int(PCHAR pStr);

/**
 * һ���������֣����ַ�����
 * ���磺124334 �ĳ���Ϊ6
 * 7809879098 �ĳ���Ϊ10
 * @param n ����
 * @return ����n���ַ�����
 */
EXTERN	UINT8	StrSizeOfInt(INT32 n);

/**
 * ���ַ���ת������ָ������תΪ��������
 * @param pStr Ҫת�����ַ���
 * @param shift ���Ƴ��ȣ�16Ϊ4��8Ϊ3��2Ϊ1
 * @return ת���������
 */
EXTERN	INT32 Str2Int(PCHAR pStr,INT32 shift);

/**
 * ���ַ�תΪ8���ֽ�������2���Ʒ�ʽ
 * @param ch Ҫת�����ַ�
 * @param bit[8] 8���ֽڴ��0.1
 * @return �ɹ�����0��ֻ�᷵��0
 */
EXTERN	INT ByteToBit(BYTE ch, BYTE bit[8]);

/**
 * ���ַ���������0.1תΪһ���ַ�
 * @param pCh ת����Ľ��
 * @param bit[8] 8���ֽڴ��0.1
 * @return �ɹ�����0��ֻ�᷵��0
 */
EXTERN	INT BitToByte(BYTE bit[8],PBYTE pCh);

EXTERN void CopyString(unsigned char dest[],char* src,unsigned int srcLen);
#ifdef __cplusplus
}
#endif
#endif