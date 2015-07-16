/******************************************************************************************
 * @file Rsa.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011-8-10
 * @descirption:RSA��Կ�����㷨
 *****************************************************************************************/
#ifndef BELIAL_CRYPT_RSA_H
#define BELIAL_CRYPT_RSA_H
#ifdef RSA
#include "typedef.h"
#include "define.h"
#include "miracl.h"
#define F_P		0x00000001
#define F_Q		0x00000002
#define F_E		0x00000004
#define F_D		0x00000008
#define F_N		0x00000010
#define F_M		0x00000020
#define F_ALL	F_P | F_Q | F_E | F_D | F_N | F_M



//û�д���
#define RSA_ERR_NOERR	0

//������������ϢΪ��
#define RSA_ERR_NULL	-1
//p��������
#define RSA_ERR_P		-2
//q��������
#define RSA_ERR_Q		-3
//e��m���ǻ��� 
#define RSA_ERR_E		-4
//e * d mod m != 1
#define RSA_ERR_D		-5
//(q - 1) * (p - 1) != m
#define RSA_ERR_M		-6
//q * p != n
#define RSA_ERR_N		-7


//��ͨ��
typedef struct {
	unsigned int e;
	unsigned int d;
	unsigned int n;
	unsigned int m;
	unsigned int q;
	unsigned int p;
}RsaElement,*PRsaElement;


//����
typedef struct {
	big e;
	big d;
	big n;
	big m;
	big q;
	big p;
} RsaContext,*PRsaContext;

typedef struct {
	//Կ�׵��ܳ��ȣ���������ṹ��
	unsigned int keyLen;

	//e,d�ĳ���
	unsigned int baseLen;
	
	//n�ĳ���
	unsigned int modLen;
	
	//e,d���ַ�����ʽ
	char* pBase;
	
	//n���ַ�����ʽ
	char* pMod;
}RsaKey,*PRsaKey;

typedef RsaKey RsaPrivateKey;
typedef RsaKey RsaPublicKey;

typedef PRsaKey PRsaPrivateKey;
typedef PRsaKey PRsaPublicKey;

typedef enum {
	RSA_MIN = -1,
	RSA_BIN = 2,
	RSA_OCT = 8,
	RSA_DEC = 10,
	RSA_HEX = 16,
	RSA_MAX
} RsaFormat;

#ifdef __cplusplus
extern "C" {	//����C++
#endif


/**************************************************************
 * RSA�����㷨��ʼ������Ҫʹ�ü�����Կ
 * @param pszFileName ����������P,Q����E,D,M,N���ڵ��ļ�·��
 * @param pContext ��ʼ��ʹ�õĹ�����������Ϣ
 * @param nMode ������������F_Q,F_P,F_D�ȣ�ʹ������ţ�����һһ��ȡ
 * @param format ����������ݵĸ�ʽ2���� 10���� 16����
 * ���ļ��Ķ�ȡ˳��Ϊp,q,e,d,n,m
 *************************************************************/
EXTERN unsigned int RsaInit(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format);

/**************************************************************
 * @param pContext ������������Ϣ
 * ����RSA�㷨��Ҫʹ�õ�������Դ
 *************************************************************/
EXTERN void RsaFinal(PRsaContext pContext);


/**************************************************************
 * ���ļ��ж�ȡ������������Ϣ������p,q,e,d,m,n��ֵ
 * @param pszFileName ����������P,Q����E,D,M,N���ڵ��ļ�·��
 * @param pContext ������������Ϣ
 * @param nMode ������������F_Q,F_P,F_D�ȣ�ʹ������ţ�����һһ��ȡ
 * @param format ����������ݵĸ�ʽ2���� 10���� 16����
 * ����RSA�㷨��Ҫʹ�õ�������Դ
 *************************************************************/
EXTERN void RsaReadContextFromFile(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format);


/**************************************************************
 * ��������������Ϣд�뵽�ļ��У�����p,q,e,d,m,n��ֵ
 * @param pszFileName ����������P,Q����E,D,M,N���ڵ��ļ�·��
 * @param pContext ������������Ϣ
 * @param nMode ������������F_Q,F_P,F_D�ȣ�ʹ������ţ�����һһ��ȡ
 * @param format ����������ݵĸ�ʽ2���� 10���� 16����
 * ����RSA�㷨��Ҫʹ�õ�������Դ
 *************************************************************/
void RsaWriteContextToFile(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format);

//��ȡP��ֵ
EXTERN void RsaReadP(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);

//��ȡQ��ֵ
EXTERN void RsaReadQ(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);

//��ȡE��ֵ
EXTERN void RsaReadE(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);

//��ȡD��ֵ
EXTERN void RsaReadD(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);


/****************************************************
 * ��������������P,Q,�����������2��1024�η�����
 * �������������������ڵ�
 * @param pContext ������������Ϣ
 ***************************************************/
EXTERN void RsaGeneratePQ(PRsaContext pContext);

//���ع�Կ
void RsaReadPublicKey(PRsaContext pContext,PRsaPublicKey pKey);

//����˽Կ
void RsaReadPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey);

//���ɹ�Կ
void RsaGenPublicKey(PRsaContext pContext,PRsaPublicKey pKey);

//����˽Կ
void RsaGenPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey);

/**************************************************************
 * ��ȡ����nBitCount������nCount����ת�����ַ����浽pszStr��
 * ÿ������ʹ��\n����
 * @param nBitCount ���ٶ���λ������
 * @param nCount ��������nCount������
 * @param pszStr ���浽���ڴ�ָ��
 * @return ��������ʹ�õ��ڴ泤�� �����ָ�����
 ************************************************************/
EXTERN unsigned int RsaCaclePrimes(unsigned int nBitCount, 
	unsigned int nCount, 
	char* pszStr);


/**************************************************************
 * ��ȡ����nBitCount������nCount����ת�����ַ����浽�ļ���
 * ÿ������ʹ��\n����
 * @param nBitCount ���ٶ���λ������
 * @param nCount ��������nCount������
 * @param pszFileName ���浽���ļ�·��
 * @return ��������ʹ�õ��ڴ泤�� �����ָ�����
 ************************************************************/
EXTERN unsigned int RsaCaclePrimesToFile(unsigned int nBitCount,
	unsigned int nCount,
	const char* pszFileName);


/**************************************************************
 * ��ȡ����nBitCount��������ת�����ַ����浽pszStr��
 * ÿ������ʹ��\n����
 * @param nBitCount ���ٶ���λ������
 * @param pszStr ���浽���ڴ�ָ��
 * @return �������������ָ���
 ************************************************************/
EXTERN unsigned int RsaCaclePrime(unsigned int nBitCount, 
	char* pszStr);


//��ӡ��RSA CONTEXT ������̨
EXTERN void RsaPrintContext(PRsaContext pContext);

/*********************************************************
 * ��������RSA����
 * @param pContext ������������Ϣ
 * @param pInput ��������(����)
 * @param nLen �������ݵĳ��ȣ��ֽ�)
 * @param pOutput �������(����)
 * @return ������ݵĳ���
 * ��tea,des,blowfish��ͬ�ľ���RSA���ܵĽ���ǲ��ȳ���
 *********************************************************/
EXTERN unsigned int RsaEncrypt(PRsaContext pContext,
	void* pInput,
	unsigned int nLen,
	void* pOutput);


/*********************************************************
 * ��������RSA����
 * @param pContext ������������Ϣ
 * @param pInput ��������(����)
 * @param nLen �������ݵĳ��ȣ��ֽ�)
 * @param pOutput �������(����)
 * @return ������ݵĳ���
 *********************************************************/
EXTERN unsigned int RsaDecrypt(PRsaContext pContext,
	void* pInput,
	unsigned int nLen,
	void* pOutput);



/*********************************************************
 * ���RSA�㷨�й������������ݵĺϷ���
 * @param pContext ������������Ϣ
 * @param nMode Ҫ��������F_P,F_Q,F_E,F_D,F_M,F_N
 * @return �Ϸ�����0�����Ϸ����ش�����(errcode < 0)
 *********************************************************/
EXTERN int RsaCheckContext(PRsaContext pContext,int nMode);

#ifdef __cplusplus
}//����C++
#endif //c
#endif //RSA
#endif