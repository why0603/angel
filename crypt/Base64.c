/******************************************************************************************
 * @file Base64.c
 * @anthor:Croky.Zheng
 * @CreateTime:2012/2/27
 * @descirption: Base64编码算法
 * 规则：
 * Base64编码将3个字节按6位分组分为4个字节，例如：
 * 转换前 10101101 10111010 01110110
 * 转换后 00101011 00011011 00101001 00110110
 * 然后根据每个字节的值去字典中查找代替值
 * 填充：
 * 由于每次都是3个字节转4个字节，因此不足3个字节时则需要补0
 * 补0产生的值不按编码表转换，而是补一个字节加一个(=)，补二个字节加两个(=)
 * base64编码后的字符串长度必然是4的倍数
 *****************************************************************************************/
#include "Base64.h"
#include <string.h>

//=
#define ZEROCHAR	61 

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

static unsigned char re_pem_array[] = {
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3e,0x0,0x0,0x0,0x3f,
	0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,
	0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x0,0x0,0x0,0x0,0x0,
	0x0,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
	0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x0,0x0,0x0,0x0,0x0,
};

//URL变种
static unsigned char url_array[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', //09
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', //19
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', //29
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //39
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', //49
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', //59
        '8', '9', '*', '_'
 };

static unsigned char re_url_array[] = {
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3e,0x0,0x0,0x0,0x0,0x0,
	0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,
	0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x0,0x0,0x0,0x0,0x3f,
	0x0,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
	0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x0,0x0,0x0,0x0,0x0,
};

//正则表达式变种
static unsigned char regular_array[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', //09
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', //19
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', //29
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //39
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', //49
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', //59
        '8', '9', '!', '-'
 };

static unsigned char re_regular_array[] = {
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x3e,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3f,0x0,0x0,
	0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,
	0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x0,0x0,0x0,0x0,0x0,
	0x0,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
	0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x0,0x0,0x0,0x0,0x0,
};

//根据模式选择编码字典
static unsigned char* SelectDict(unsigned char** pOutput,
	Base64Mode mode);

//根据模式选择解码字典
static unsigned char* ReSelectDict(unsigned char** pOutput,
	Base64Mode mode);

//选择字典
unsigned char* SelectDict(unsigned char** pOutput,
	Base64Mode mode)
{
	unsigned char* pDict = NULL;//ictionaries
	/**确定字典*/
	if (mode == Base64_Mode_REG)
	{
		pDict = regular_array;
	}
	else if (mode == Base64_Mode_URL)
	{
		pDict = url_array;
	}
	else if (mode == Base64_Mode_PEM)
	{
		pDict = pem_array;
	}
	else
	{
		pDict = pem_array;
	}
	*pOutput = pDict;
	return *pOutput;
}


static unsigned char* ReSelectDict(unsigned char** pOutput,
	Base64Mode mode)
{
	unsigned char* pDict = NULL;//ictionaries
	/**确定字典*/
	if (mode == Base64_Mode_REG)
	{
		pDict = re_regular_array;
	}
	else if (mode == Base64_Mode_URL)
	{
		pDict = re_url_array;
	}
	else if (mode == Base64_Mode_PEM)
	{
		pDict = re_pem_array;
	}
	else
	{
		pDict = re_pem_array;
	}
	*pOutput = pDict;
	return *pOutput;
}

unsigned int Base64Encrypt(unsigned char* pInput,
	unsigned int nSize,//输入的字节数
	unsigned char* pOutput,
	Base64Mode mode)
{
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned int i,j,k = 0;
	unsigned char* pDict = NULL;//ictionaries

	SelectDict(&pDict,mode);

	for (i=0; i<nSize; i+=3)//每次处理三个字节
	{
		j = 0;//需要补充的字节数
		if ((i + 3) > nSize)
		{
			j = i + 3 - nSize;
		}
		//1个字节被转化为
		//如:11001101
		//11001101 00000000 00000000 (补0)
		//00110011 00010000 00000000 00000000 (转为4个字节)
		if (j == 1)
		{
			byte0 = *(pInput + i);
			//第一个前面6位                              00111111
			*(pOutput + k++) = *(pDict + ((byte0 >> 2) & 0x3f));
			//第一个字节后面2位                          00110000
			*(pOutput + k++) = *(pDict + ((byte0 << 4) & 0x30));
			 //加上=
			*(pOutput + k++) = ZEROCHAR;
			 //加上=
			*(pOutput + k++) = ZEROCHAR;
		}
		else if (j == 2)
		{
			byte0 = *(pInput + i);
			byte1 = *(pInput + i + 1);
			//第一个前面6位                              00111111
			*(pOutput + k++) = *(pDict + ((byte0 >> 2) & 0x3f));
			//第一个字节后面2位 + 后一个字节的前面4位     00110000                 00001111
			*(pOutput + k++) = *(pDict + (((byte0 << 4) & 0x30) | (((byte1 >> 4) & 0xf))));
			//第二个字节的后面4位                        00111100
			*(pOutput + k++) = *(pDict + ((byte1 << 2) & 0x3C));
			 //加上=
			*(pOutput + k++) = ZEROCHAR;
		}
		else
		{
			byte0 = *(pInput + i);
			byte1 = *(pInput + i + 1);
			byte2 = *(pInput + i + 2);

			//第一个前面6位                              00111111
			*(pOutput + k++) = *(pDict + ((byte0 >> 2) & 0x3f));

			//第一个字节后面2位 + 后一个字节的前面4位     00110000                 00001111
			*(pOutput + k++) = *(pDict + (((byte0 << 4) & 0x30) | (((byte1 >> 4) & 0xf))));

			//第二个字节的后面4位+第三个字节的前面2位     00111100                00000011
			*(pOutput + k++) = *(pDict + (((byte1 << 2) & 0x3C) | ((byte2 >> 6) & 0x3)));

			//第三个字节的后面6位
			*(pOutput + k++) = *(pDict + (byte2 & 0x3f));
		}
	}
	return k;
}


unsigned int Base64Decrypt(unsigned char* pInput,
	unsigned int nSize,//输入的字节数
	unsigned char* pOutput,
	Base64Mode mode)
{
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
	unsigned int i,k = 0;
	unsigned char* pDict = NULL;//ictionaries

	ReSelectDict(&pDict,mode);

	for (i=0; i<nSize; i+=4)//每次处理4个字节
	{
		//不足4个字节则停止解码
		if ((i + 4) > nSize)
		{
			break;
		}
		//有这些值表示不符合规范
		/*
		if (*(pInput + i + 0) >= 0x7f ||
			*(pInput + i + 1) >= 0x7f ||
			*(pInput + i + 2) >= 0x7f ||
			*(pInput + i + 3) >= 0x7f)
		{
			break;
		}
		*/
		byte0 = *(pDict + *(pInput + i));
		byte1 = *(pDict + *(pInput + i + 1));
		//为等号设为0
		if (*(pInput + i + 2) == ZEROCHAR)
		{
			byte2 = 0;
		}
		else
		{
			byte2 = *(pDict + *(pInput + i + 2));
		}

		//为等号设为0
		if (*(pInput + i + 3) == ZEROCHAR)
		{
			byte3 = 0;
		}
		else
		{
			byte3 = *(pDict + *(pInput + i + 3));
		}


		if ((byte2 == 0) && 
			(byte3 == 0))
		{
			//第一个字节后6位，第二个字节5,6位
			*(pOutput + k++) = (byte0 << 2) | (byte1 >> 4);
		} 
		else if (byte3 == 0) 
		{
			//第一个字节后6位，第二个字节5,6位
			*(pOutput + k++) = (byte0 << 2) | (byte1 >> 4);

			//第二个字节后4位，第三个字节3,4,5,6位
			*(pOutput + k++) = (byte1 << 4) | (byte2 >> 2);
		}
		else
		{
			//第一个字节后6位，第二个字节5,6位
			*(pOutput + k++) = (byte0 << 2) | (byte1 >> 4);

			//第二个字节后4位，第三个字节3,4,5,6位
			*(pOutput + k++) = (byte1 << 4) | (byte2 >> 2);

			//第三个字节后2位，第四个字节后6位
			*(pOutput + k++) = (byte2 << 6) | byte3;
		}
	}
	return k;
}