/******************************************************************************************
 * @file Base64.c
 * @anthor:Croky.Zheng
 * @CreateTime:2012/2/27
 * @descirption: Base64�����㷨
 * ����
 * Base64���뽫3���ֽڰ�6λ�����Ϊ4���ֽڣ����磺
 * ת��ǰ 10101101 10111010 01110110
 * ת���� 00101011 00011011 00101001 00110110
 * Ȼ�����ÿ���ֽڵ�ֵȥ�ֵ��в��Ҵ���ֵ
 * ��䣺
 * ����ÿ�ζ���3���ֽ�ת4���ֽڣ���˲���3���ֽ�ʱ����Ҫ��0
 * ��0������ֵ���������ת�������ǲ�һ���ֽڼ�һ��(=)���������ֽڼ�����(=)
 * base64�������ַ������ȱ�Ȼ��4�ı���
 *****************************************************************************************/
#include "Base64.h"
#include <string.h>

//=
#define ZEROCHAR	61 

//ԭʼBASE64�����
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

//URL����
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

//������ʽ����
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

//����ģʽѡ������ֵ�
static unsigned char* SelectDict(unsigned char** pOutput,
	Base64Mode mode);

//����ģʽѡ������ֵ�
static unsigned char* ReSelectDict(unsigned char** pOutput,
	Base64Mode mode);

//ѡ���ֵ�
unsigned char* SelectDict(unsigned char** pOutput,
	Base64Mode mode)
{
	unsigned char* pDict = NULL;//ictionaries
	/**ȷ���ֵ�*/
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
	/**ȷ���ֵ�*/
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
	unsigned int nSize,//������ֽ���
	unsigned char* pOutput,
	Base64Mode mode)
{
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned int i,j,k = 0;
	unsigned char* pDict = NULL;//ictionaries

	SelectDict(&pDict,mode);

	for (i=0; i<nSize; i+=3)//ÿ�δ��������ֽ�
	{
		j = 0;//��Ҫ������ֽ���
		if ((i + 3) > nSize)
		{
			j = i + 3 - nSize;
		}
		//1���ֽڱ�ת��Ϊ
		//��:11001101
		//11001101 00000000 00000000 (��0)
		//00110011 00010000 00000000 00000000 (תΪ4���ֽ�)
		if (j == 1)
		{
			byte0 = *(pInput + i);
			//��һ��ǰ��6λ                              00111111
			*(pOutput + k++) = *(pDict + ((byte0 >> 2) & 0x3f));
			//��һ���ֽں���2λ                          00110000
			*(pOutput + k++) = *(pDict + ((byte0 << 4) & 0x30));
			 //����=
			*(pOutput + k++) = ZEROCHAR;
			 //����=
			*(pOutput + k++) = ZEROCHAR;
		}
		else if (j == 2)
		{
			byte0 = *(pInput + i);
			byte1 = *(pInput + i + 1);
			//��һ��ǰ��6λ                              00111111
			*(pOutput + k++) = *(pDict + ((byte0 >> 2) & 0x3f));
			//��һ���ֽں���2λ + ��һ���ֽڵ�ǰ��4λ     00110000                 00001111
			*(pOutput + k++) = *(pDict + (((byte0 << 4) & 0x30) | (((byte1 >> 4) & 0xf))));
			//�ڶ����ֽڵĺ���4λ                        00111100
			*(pOutput + k++) = *(pDict + ((byte1 << 2) & 0x3C));
			 //����=
			*(pOutput + k++) = ZEROCHAR;
		}
		else
		{
			byte0 = *(pInput + i);
			byte1 = *(pInput + i + 1);
			byte2 = *(pInput + i + 2);

			//��һ��ǰ��6λ                              00111111
			*(pOutput + k++) = *(pDict + ((byte0 >> 2) & 0x3f));

			//��һ���ֽں���2λ + ��һ���ֽڵ�ǰ��4λ     00110000                 00001111
			*(pOutput + k++) = *(pDict + (((byte0 << 4) & 0x30) | (((byte1 >> 4) & 0xf))));

			//�ڶ����ֽڵĺ���4λ+�������ֽڵ�ǰ��2λ     00111100                00000011
			*(pOutput + k++) = *(pDict + (((byte1 << 2) & 0x3C) | ((byte2 >> 6) & 0x3)));

			//�������ֽڵĺ���6λ
			*(pOutput + k++) = *(pDict + (byte2 & 0x3f));
		}
	}
	return k;
}


unsigned int Base64Decrypt(unsigned char* pInput,
	unsigned int nSize,//������ֽ���
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

	for (i=0; i<nSize; i+=4)//ÿ�δ���4���ֽ�
	{
		//����4���ֽ���ֹͣ����
		if ((i + 4) > nSize)
		{
			break;
		}
		//����Щֵ��ʾ�����Ϲ淶
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
		//Ϊ�Ⱥ���Ϊ0
		if (*(pInput + i + 2) == ZEROCHAR)
		{
			byte2 = 0;
		}
		else
		{
			byte2 = *(pDict + *(pInput + i + 2));
		}

		//Ϊ�Ⱥ���Ϊ0
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
			//��һ���ֽں�6λ���ڶ����ֽ�5,6λ
			*(pOutput + k++) = (byte0 << 2) | (byte1 >> 4);
		} 
		else if (byte3 == 0) 
		{
			//��һ���ֽں�6λ���ڶ����ֽ�5,6λ
			*(pOutput + k++) = (byte0 << 2) | (byte1 >> 4);

			//�ڶ����ֽں�4λ���������ֽ�3,4,5,6λ
			*(pOutput + k++) = (byte1 << 4) | (byte2 >> 2);
		}
		else
		{
			//��һ���ֽں�6λ���ڶ����ֽ�5,6λ
			*(pOutput + k++) = (byte0 << 2) | (byte1 >> 4);

			//�ڶ����ֽں�4λ���������ֽ�3,4,5,6λ
			*(pOutput + k++) = (byte1 << 4) | (byte2 >> 2);

			//�������ֽں�2λ�����ĸ��ֽں�6λ
			*(pOutput + k++) = (byte2 << 6) | byte3;
		}
	}
	return k;
}