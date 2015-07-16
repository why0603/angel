#include "hash.h"
#include "charUtils.h"
#include "base64.h"
#include "blowfish.h"
#include "Crc.h"
#include "Des.h"
#include "Md5.h"
#include "Rc.h"
#include "Rsa.h"
#include "Sha.h"
#include "tea.h"
#include "Rijndael.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*

void PrintHexStr(INT64 v)
{
	char words[65] = {0};
	char* pszword = NULL;
	//pszword = _i64toa(v,words,2);//_ui64toa  _ui64tow
	_i64toa_s(v,words,65,2);
	printf("%s,%I64X,%I64d\r\n",pszword,v,v);
}

void PrintDoubuleByte()
{
	unsigned char n;
	int cnt = 0;
	for (n=0; n<=255; n++)
	{
		if (IsDoubleBit_Byte(n))
		{
			printf("%d,",n);
			cnt++;
		}
		if (n == 255) break;
	}
	printf("\r\n%d",cnt);
}
*/

//原始BASE64编码表
static unsigned char pem_array[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', //09
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', //19
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', //29
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //39
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', //49
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', //59
        '8', '9', '+', '/'
 };

//RSA加密后的数据居然需要256个字符来保存
//则转为16进制则需要512个字符，加一个结束符，为513
//这里特意使用1K作为保存的容量
#define CLIPERLEN 1024
void main()
{
	/*
	__int64 ov = 0xFFFFEEEEEFFFFFFF;//0x0123456789ABCDEF;
	__int64 key = 0x70726F6772316D;//0x133457799BBCDFF1;
	printf("%s\r\n","start...");
	PrintHexStr(ov);
	ov = EnCrypt(ov,key);
	printf("加密后:");
	PrintHexStr(ov);
	ov = DeCrypt(ov,key);
	printf("解密后:");
	PrintHexStr(ov);
	*/
	
	MD5Context md5_ctx = {0};
	ShaContext sha_ctx = {0};
	RCContext rc_ctx = {0};
	RijndaelContext rd_ctx = {0};
	BlowFishContext bf_ctx = {0};
	DesContext des_ctx = {0};
	TeaContext tea_ctx = {0};
#ifdef RSA
	RsaContext rsa_ctx;
	RsaPublicKey publicKey;
	RsaPrivateKey privateKey;
	RsaElement rsaElement;
#endif
	unsigned int sum=0;
	char cliper[CLIPERLEN] = {0};
	char pliper[CLIPERLEN] = {0};
	char hexer[CLIPERLEN] = {0};
	char cArr[4] = {0x12,0x34,0x56,0x78};
	int base64[256] = {0};
	char* verb = "abc我喊你呢123你怎么就没有听到呢!@#$";
	unsigned int verb_len = 36;
	unsigned int cliper_len = CHECKDIV8(verb_len);//RC6不是使用这个算法
	//DesInit("why0603",7);
	//TeaInit("why0603",7);
	//BlowFishInit("why0603",7);

	rc_ctx.pKey = (unsigned char*)"why0603";
	rc_ctx.nKeyLen = 7;
	rc_ctx.nRound = 20;
	strcpy((char*)rd_ctx.cryptKey,"why0603");

	//DES
	strcpy((char*)des_ctx.key,"WELCOME");//why0603
	des_ctx.mode = Des_Mode_BCB;

	DesInit(&des_ctx);
	DesEncrypt(&des_ctx,verb,verb_len,cliper);//Des_Mode_BCB
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	DesDecrypt(&des_ctx,cliper,cliper_len,pliper);
	printf("Des BCB:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//DES
	des_ctx.mode = Des_Mode_ECB;
	DesEncrypt(&des_ctx,verb,verb_len,cliper);//Des_Mode_BCB
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	DesDecrypt(&des_ctx,cliper,cliper_len,pliper);
	printf("Des ECB:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//TEA
	TeaInit(&tea_ctx);
	tea_ctx.mode = Tea_Mode;
	TeaSetKey(&tea_ctx,"why0603",7);
	TeaEncrypt(&tea_ctx,verb,verb_len,cliper);//Tea_Mode_Tea
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	TeaDecrypt(&tea_ctx,cliper,cliper_len,pliper);
	printf("Tea:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//XTEA
	tea_ctx.mode = XTea_Mode;
	TeaEncrypt(&tea_ctx,verb,verb_len,cliper);//Tea_Mode_Tea
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	TeaDecrypt(&tea_ctx,cliper,cliper_len,pliper);
	printf("XTea:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//XXTEA
	tea_ctx.mode = XXTea_Mode;
	TeaEncrypt(&tea_ctx,verb,verb_len,cliper);//Tea_Mode_Tea
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	TeaDecrypt(&tea_ctx,cliper,cliper_len,pliper);
	printf("XXTea:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	/*
	//BLOWFISH
	BlowFishEncrypt(verb,verb_len,cliper,BF_Mode_BCB);//BF_Mode_ECB
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	BlowFishDecrypt(cliper,cliper_len,pliper,BF_Mode_BCB);
	printf("Blowfish ECB:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//BLOWFISH
	BlowFishEncrypt(verb,verb_len,cliper,BF_Mode_ECB);//BF_Mode_ECB
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	BlowFishDecrypt(cliper,cliper_len,pliper,BF_Mode_ECB);
	printf("Blowfish BCB:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);
	*/
	bf_ctx.pKey = (char*)"why0603";
	bf_ctx.nLen = 7;
	bf_ctx.mode = BF_Mode_BCB;
	BlowFishInit(&bf_ctx);
	BlowFishEncrypt(&bf_ctx,verb,verb_len,cliper);
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	BlowFishDecrypt(&bf_ctx,cliper,cliper_len,pliper);
	printf("Blowfish BCB:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//BASE64
	sum = Base64Encrypt((unsigned char*)verb,verb_len,(unsigned char*)pliper,Base64_Mode_PEM);
	printf("Base64:%s\r\n",pliper);
	sum = Base64Decrypt((unsigned char*)pliper,sum,(unsigned char*)cliper,Base64_Mode_PEM);
	printf("Base64:%s\r\n",cliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);

	//MD5算法
	MD5Init(&md5_ctx);
	MD5Update(&md5_ctx,(unsigned char*)verb,verb_len);
	MD5Final(&md5_ctx,(unsigned char*)cliper);
	Byte2Hex((unsigned char*)cliper,hexer,16,0);
	printf("MD5-abc:%s\r\n",hexer);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);


	//SHA1-160算法
	ShaInit(&sha_ctx);
	ShaUpdate(&sha_ctx,(unsigned char*)verb,verb_len);
	ShaFinal(&sha_ctx,(unsigned char*)cliper);
	Byte2Hex((unsigned char*)cliper,hexer,20,0);
	printf("Sha1-abc:%s\r\n",hexer);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//CRC32-CRC64
	;
	Int2HexStr(GenCrc32((unsigned char*)verb,verb_len),hexer);
	Int2HexStr(GenCrc64((unsigned char*)verb,verb_len),cliper);
	printf("CRC32-abc:%s\r\n",hexer);
	printf("CRC64-abc:%s\r\n",cliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	///*
	//RC5
	rc_ctx.mode = RC_Mode_RC5;
	RCInit(&rc_ctx);
	RCEncrypt(&rc_ctx,verb,verb_len,cliper);
	Byte2Hex((unsigned char*)cliper,hexer,verb_len,0);
	RCDecrypt(&rc_ctx,cliper,cliper_len,pliper);
	printf("RC5:%s,%s\r\n",hexer,pliper);
	RCDestroy(&rc_ctx);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);
	//*/
	//RC6
	rc_ctx.mode = RC_Mode_RC6;
	RCInit(&rc_ctx);
	//verb_len = 3;
	RCEncrypt(&rc_ctx,verb,verb_len,cliper);
	Byte2Hex((unsigned char*)cliper,hexer,ENRC6SIZE(verb_len),0);
	RCDecrypt(&rc_ctx,cliper,ENRC6SIZE(verb_len),pliper);
	printf("RC6:%s,%s\r\n",hexer,pliper);
	RCDestroy(&rc_ctx);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);

	//rijndael
	RijndaelInit(&rd_ctx);
	RijndaelEncrypt(&rd_ctx,verb,verb_len,cliper);
	Byte2Hex((unsigned char*)cliper,hexer,ENRIJNDAELSIZE(verb_len),0);
	RijndaelDecrypt(&rd_ctx,cliper,ENRIJNDAELSIZE(verb_len),pliper);
	printf("RIJNDAEL:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);
	//RC6SetKey(&rc6_ctx,verb,verb_len);
	/*
	RsaInit("d:\\rsa.txt",&rsa_ctx,F_ALL,RSA_DEC);//F_ALL);
	if (RSA_ERR_NOERR != RsaCheckContext(&rsa_ctx,F_ALL))//;//检测
	{
		return;
	}
	//-----------------------------------------
	publicKey.pBase = (char*)malloc(1024);
	publicKey.pMod = (char*)malloc(1024);
	privateKey.pBase = (char*)malloc(1024);
	privateKey.pMod = (char*)malloc(1024);

	RsaGenPrivateKey(&rsa_ctx,&privateKey);
	RsaGenPublicKey(&rsa_ctx,&publicKey);
	RsaReadPrivateKey(&rsa_ctx,&privateKey);
	RsaReadPublicKey(&rsa_ctx,&publicKey);
	free(publicKey.pBase);
	free(publicKey.pMod);
	free(privateKey.pBase);
	free(privateKey.pMod);
	//-----------------------------------------
	e = RsaEncrypt(&rsa_ctx,verb,verb_len,cliper);//900
	CYByte2Hex((unsigned char*)cliper,hexer,e,0);
	d = RsaDecrypt(&rsa_ctx,cliper,e,pliper);
	printf("RSA:%s,%s\r\n",hexer,pliper);
	memset(cliper,0,CLIPERLEN);
	memset(pliper,0,CLIPERLEN);
	memset(hexer,0,CLIPERLEN);
	*/
	//PrintContext(&rsa_ctx);
	//RsaInit(NULL,&rsa_ctx,F_P|F_Q,RSA_HEX);
	//CaclePrimesToFile(1024,10000,"d:\\primes_16_2.txt");
	//GetED(&rsaElement);
	//printf("e=%u,d=%u\r\n",rsaElement.e,rsaElement.d);
	//printf("cp=%u,cc=%u\r\n",e,d);
	//printf("%x,%x,%x,%x\r\n",(unsigned int)(-60),-60,((unsigned int)(-60))>>13,(-60)>>13);
	//printf("%x",*((int*)cArr));
	getchar();
}