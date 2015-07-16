/******************************************************************************************
 * @file Rsa.h
 * @anthor:Croky.Zheng
 * @CreateTime:2011-8-10
 * @descirption:RSA公钥加密算法
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



//没有错误
#define RSA_ERR_NOERR	0

//关联上下文信息为空
#define RSA_ERR_NULL	-1
//p不是素数
#define RSA_ERR_P		-2
//q不是素数
#define RSA_ERR_Q		-3
//e与m不是互质 
#define RSA_ERR_E		-4
//e * d mod m != 1
#define RSA_ERR_D		-5
//(q - 1) * (p - 1) != m
#define RSA_ERR_M		-6
//q * p != n
#define RSA_ERR_N		-7


//普通数
typedef struct {
	unsigned int e;
	unsigned int d;
	unsigned int n;
	unsigned int m;
	unsigned int q;
	unsigned int p;
}RsaElement,*PRsaElement;


//大数
typedef struct {
	big e;
	big d;
	big n;
	big m;
	big q;
	big p;
} RsaContext,*PRsaContext;

typedef struct {
	//钥匙的总长度，包括这个结构体
	unsigned int keyLen;

	//e,d的长度
	unsigned int baseLen;
	
	//n的长度
	unsigned int modLen;
	
	//e,d的字符串形式
	char* pBase;
	
	//n的字符串形式
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
extern "C" {	//兼容C++
#endif


/**************************************************************
 * RSA加密算法初始化，需要使用加密密钥
 * @param pszFileName 两个大素数P,Q还有E,D,M,N所在的文件路径
 * @param pContext 初始化使用的关联上下文信息
 * @param nMode 所包含的数据F_Q,F_P,F_D等，使用与符号，可以一一读取
 * @param format 输入输出数据的格式2进制 10进制 16进制
 * 对文件的读取顺序为p,q,e,d,n,m
 *************************************************************/
EXTERN unsigned int RsaInit(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format);

/**************************************************************
 * @param pContext 关联上下文信息
 * 回收RSA算法所要使用的所有资源
 *************************************************************/
EXTERN void RsaFinal(PRsaContext pContext);


/**************************************************************
 * 从文件中读取关联上下文信息，包括p,q,e,d,m,n的值
 * @param pszFileName 两个大素数P,Q还有E,D,M,N所在的文件路径
 * @param pContext 关联上下文信息
 * @param nMode 所包含的数据F_Q,F_P,F_D等，使用与符号，可以一一读取
 * @param format 输入输出数据的格式2进制 10进制 16进制
 * 回收RSA算法所要使用的所有资源
 *************************************************************/
EXTERN void RsaReadContextFromFile(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format);


/**************************************************************
 * 将关联上下文信息写入到文件中，包括p,q,e,d,m,n的值
 * @param pszFileName 两个大素数P,Q还有E,D,M,N所在的文件路径
 * @param pContext 关联上下文信息
 * @param nMode 所包含的数据F_Q,F_P,F_D等，使用与符号，可以一一读取
 * @param format 输入输出数据的格式2进制 10进制 16进制
 * 回收RSA算法所要使用的所有资源
 *************************************************************/
void RsaWriteContextToFile(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format);

//读取P的值
EXTERN void RsaReadP(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);

//读取Q的值
EXTERN void RsaReadQ(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);

//读取E的值
EXTERN void RsaReadE(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);

//读取D的值
EXTERN void RsaReadD(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format);


/****************************************************
 * 生成两个大素数P,Q,两个数都会比2的1024次方还大
 * 但是这两个素数是相邻的
 * @param pContext 关联上下文信息
 ***************************************************/
EXTERN void RsaGeneratePQ(PRsaContext pContext);

//加载公钥
void RsaReadPublicKey(PRsaContext pContext,PRsaPublicKey pKey);

//加载私钥
void RsaReadPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey);

//生成公钥
void RsaGenPublicKey(PRsaContext pContext,PRsaPublicKey pKey);

//生成私钥
void RsaGenPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey);

/**************************************************************
 * 获取最少nBitCount的素数nCount个，转换成字符保存到pszStr中
 * 每个素数使用\n隔开
 * @param nBitCount 最少多少位的素数
 * @param nCount 连续生成nCount个素数
 * @param pszStr 保存到的内存指针
 * @return 返回所有使用的内存长度 包括分隔符号
 ************************************************************/
EXTERN unsigned int RsaCaclePrimes(unsigned int nBitCount, 
	unsigned int nCount, 
	char* pszStr);


/**************************************************************
 * 获取最少nBitCount的素数nCount个，转换成字符保存到文件中
 * 每个素数使用\n隔开
 * @param nBitCount 最少多少位的素数
 * @param nCount 连续生成nCount个素数
 * @param pszFileName 保存到的文件路径
 * @return 返回所有使用的内存长度 包括分隔符号
 ************************************************************/
EXTERN unsigned int RsaCaclePrimesToFile(unsigned int nBitCount,
	unsigned int nCount,
	const char* pszFileName);


/**************************************************************
 * 获取最少nBitCount的素数，转换成字符保存到pszStr中
 * 每个素数使用\n隔开
 * @param nBitCount 最少多少位的素数
 * @param pszStr 保存到的内存指针
 * @return 返回素数的数字个数
 ************************************************************/
EXTERN unsigned int RsaCaclePrime(unsigned int nBitCount, 
	char* pszStr);


//打印出RSA CONTEXT 到控制台
EXTERN void RsaPrintContext(PRsaContext pContext);

/*********************************************************
 * 对数据做RSA加密
 * @param pContext 关联上下文信息
 * @param pInput 输入数据(明文)
 * @param nLen 输入数据的长度（字节)
 * @param pOutput 输出数据(密文)
 * @return 输出数据的长度
 * 与tea,des,blowfish不同的就是RSA加密的结果是不等长的
 *********************************************************/
EXTERN unsigned int RsaEncrypt(PRsaContext pContext,
	void* pInput,
	unsigned int nLen,
	void* pOutput);


/*********************************************************
 * 对数据做RSA解密
 * @param pContext 关联上下文信息
 * @param pInput 输入数据(密文)
 * @param nLen 输入数据的长度（字节)
 * @param pOutput 输出数据(明文)
 * @return 输出数据的长度
 *********************************************************/
EXTERN unsigned int RsaDecrypt(PRsaContext pContext,
	void* pInput,
	unsigned int nLen,
	void* pOutput);



/*********************************************************
 * 检查RSA算法中关联上下文数据的合法性
 * @param pContext 关联上下文信息
 * @param nMode 要检查的数据F_P,F_Q,F_E,F_D,F_M,F_N
 * @return 合法返回0，不合法返回错误码(errcode < 0)
 *********************************************************/
EXTERN int RsaCheckContext(PRsaContext pContext,int nMode);

#ifdef __cplusplus
}//兼容C++
#endif //c
#endif //RSA
#endif