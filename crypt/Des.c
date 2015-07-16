/******************************************************************************************
* @file Des.c
* @anthor:Croky.Zheng
* @CreateTime:2011/8/5
* @descirption: Des,3Des,IDes加密算法
* Des算法使用56位密钥，这里将会自动添加奇偶校验位
* 而不是直接使用64位的密钥，这样会彻底忽略其中的8为
* 容易造成奇偶校验位相同的KEY导致加密串相同的问题
*****************************************************************************************/
#include "Des.h"
#include "define.h"
#include <string.h>

#if defined(INT64)
#undef INT64
#endif

#if defined(UINT64)
#undef UINT64
#endif

#if defined(BYTE)
#undef BYTE
#endif

#if WIN32
	typedef __int64 INT64;
	typedef unsigned __int64 UINT64;
#else
	typedef long long INT64;
	typedef unsigned long long UINT64;
#endif
typedef INT64* PINT64;
typedef UINT64* PUINT64;
typedef unsigned char BYTE;

//计算数组的长度
#define ARRAY_LEN(s) ((sizeof(s))/(sizeof(s[0])))

#define TRUE 1
#define FALSE 0

#define KEEPZERO32 0x0ffffffff

//字节转为二进制
static int ByteToBit(char c, char bit[8]);

//位置换,取数v，按照pSwapTable中的位描述重组数字,len表示pSwapTable的长度
static INT64 BitSwap(INT64 v,const int* pSwapTable,int len);

//生成16个48位子密钥
static PINT64 BuildSubKey(INT64 key,INT64 pSubKey[16]);

//F函数
static int F(int v,INT64 key);

//加密
static INT64 EnCrypt(INT64 num,INT64 k);

//解密
static INT64 DeCrypt(INT64 num,INT64 k);

//打印出64位整数的2进制和16进制表示方式
static void PrintHexStr(INT64 num);

//初始置换
static INT64 InitSwap(INT64 num);

//最后一次置换
static INT64 LastSwap(INT64 num);

//初始逆置换
static INT64 InvInitSwap(INT64 num);

//最后一次逆置换
static INT64 InvLastSwap(INT64 num);

//判断一个数是否有双1个位
static int IsDoubleBit_Int(int n);

static int IsDoubleBit_Byte(BYTE n);

//加密运算
static INT64 Cliper(INT64 num,INT64 pSubKey[16]);

//解密运算
static INT64 DeCliper(INT64 num,INT64 pSubKey[16]);
//初始置换表 64
static const int sp_init[64] = {
	58,50,42,34,26,18,10,2,
	60,52,44,36,28,20,12,4,
	62,54,46,38,30,22,14,6,
	64,56,48,40,32,24,16,8,
	57,49,41,33,25,17,9,1,
	59,51,43,35,27,19,11,3,
	61,53,45,37,29,21,13,5,
	63,55,47,39,31,23,15,7,
};

//密钥置换选择表1,用于密钥压缩换位1 64->56
static const int sp_key_pc1[56] = {
	57,49,41,33,25,17,9,1,
	58,50,42,34,26,18,10,2,
	59,51,43,35,27,19,11,3,
	60,52,44,36,63,55,47,39,
	31,23,15,7,62,54,46,38,
	30,22,14,6,61,53,45,37,
	29,21,13,5,28,20,12,4,
};

//密钥置换选择表2,用于密钥压缩换位2 56->48
static const int sp_key_pc2[48] = {
	14,17,11,24,1,5,3,28,
	15,6,21,10,23,19,12,4,
	26,8,16,7,27,20,13,2,
	41,52,31,37,47,55,30,40,
	51,45,33,48,44,49,39,56,
	34,53,46,42,50,36,29,32,
};

//密钥每次移位的位数，总计还是28
static const int sp_key_shifts[16] = {
	1,1,2,2,2,2,2,2,
	1,2,2,2,2,2,2,1
};

//最后置换表
static const int sp_last[64] = {
	40,8,48,16,56,24,64,32, 
	39,7,47,15,55,23,63,31,
	38,6,46,14,54,22,62,30, 
	37,5,45,13,53,21,61,29,
	36,4,44,12,52,20,60,28,
	35,3,43,11,51,19,59,27,
	34,2,42,10,50,18,58,26,
	33,1,41,9,49,17,57,25,
};

//扩展换位表(32-48)从32为R到48位    
//F函数使用的换位表
static const int sp_box_ext[48] = {
	32,1,2,3,4,5,
	4,5,6,7,8,9,
	8,9,10,11,12,13,
	12,13,14,15,16,17,
	16,17,18,19,20,21,
	20,21,22,23,24,25,
	24,25,26,27,28,29,
	28,29,30,31,32,31,
};

//f函数尾置换表（单纯换位表）
static const int sp_box_end[32] = {
	16,7,20,21,29,12,28,17,
	1,15,23,26,5,18,31,10,
	2,8,24,14,32,27,3,9,
	19,13,30,6,22,11,4,25,
};

//S盒  1，6位组为行，2,3,4,5为列
//比如101101 则行为11=3 列位0110=6
//对应S值为S[3][6]
static const int sbox[8][64] = {
	//S1
	{
		14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
			0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
			4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
			15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
	},
	//S2
	{
		15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
			3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
			0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
			13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
		},
		//S3
		{
			10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
				13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
				13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
				1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
		},
		//S4
		{
			7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
				13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
				10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
				3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
			},
			//S5
			{
				2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
					14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
					4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
					11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
			},
			//S6
			{
				12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
					10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
					9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
					4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
				},
				//S7
				{
					4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
						13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
						1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
						6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
				},
				//S8
				{
					13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
						1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
						7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
						2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
					}
};

//位掩码表
static const unsigned char sp_key_loop[8] = {
	0x1,0x2,0x4,0x8,
	0x1,0x2,0x4,0x8,
};

//0表示序号对应的1的个数为奇数，这里数字0表示为偶数
//1表示序号对应的1的个数为偶数
//11001110,有5个1，则奇偶校验为0，偶数个1则为1
static const int double_bit_byte[256] = {
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1
};

//static INT64 subkey[16] = {0};//子密钥

//字节转换成二进制(倒置)
int ByteToBit(char c, char bit[8])
{   
	int cnt;   
	for(cnt = 0;cnt < 8; cnt++){   
		*(bit+cnt) = (c>>cnt)&0x1;   
	}   
	return 0;   
}

//判断一个数是否有双1个位
int IsDoubleBit_Int(int n)
{
	int i;
	int cnt = 0;
	for (i=0; i<32; i++)
	{
		if ((n & 0x1) == 1)
		{
			cnt++;
			n >>= 1;
		}
	}
	if ((cnt % 2) == 0)
	{
		return	TRUE;
	}
	return FALSE;
}

//判断一个数是否有双1个位
int IsDoubleBit_Byte(BYTE n)
{
	int i;
	int cnt = 0;
	for (i=0; i<8; i++)
	{
		if ((n & 0x1) == 1)
		{
			cnt++;
		}
		n >>= 1;
	}
	if ((cnt % 2) == 0)
	{
		return	TRUE;
	}
	return FALSE;
}

BOOL IsDoubleBitByByte(BYTE b)
{
	return double_bit_byte[b];
}

BOOL IsDoubleBitByInt(INT n)
{
	int* pNum = &n;
	PBYTE pByteNum = (PBYTE)pNum;
	int cnt = 0;
	//偶数+2   奇数+1
	cnt += (IsDoubleBitByByte(pByteNum[0]) ? 2 : 1);
	cnt += (IsDoubleBitByByte(pByteNum[1]) ? 2 : 1);
	cnt += (IsDoubleBitByByte(pByteNum[2]) ? 2 : 1);
	cnt += (IsDoubleBitByByte(pByteNum[3]) ? 2 : 1);
	return (BOOL)(cnt % 2);
}

//64位置换,大部分网络代码都是字节置换
//实际上DES中是位置换
INT64 BitSwap(INT64 num,const int* pSwapTable,int len)
{
	int i;
	INT64 nv = 0;
	for (i = 0; i < len; i++)
	{
		nv <<= 1;
		nv |= ((num >> (64 - *(pSwapTable+i))) & 0x1);//位数1-64是从左到右
	}
	return nv;
}

//逆置换
INT64 InvBitSwap(INT64 num,const int* pSwapTable,int len)
{
	int i;
	INT64 tmp;
	INT64 nv = 0;
	for (i = 0; i < len; i++)
	{
		tmp = num >> (64 - 1 - i);
		tmp &= 0x1;
		tmp <<= (64 - *(pSwapTable+i));
		nv |= tmp;
	}
	return nv;
}

INT64 InitSwap(INT64 num)
{
	return BitSwap(num,sp_init,ARRAY_LEN(sp_init));
}

INT64 LastSwap(INT64 num)
{
	return BitSwap(num,sp_last,ARRAY_LEN(sp_last));
}

INT64 InvInitSwap(INT64 num)
{
	return InvBitSwap(num,sp_last,ARRAY_LEN(sp_last));
}

INT64 InvLastSwap(INT64 num)
{
	return InvBitSwap(num,sp_init,ARRAY_LEN(sp_init));
}

/************************************************************
* 生成子密钥
* 子密钥生成过程
* 1:将64位子密钥压缩（去掉奇偶校验位8,16,32...每个字节最后一位)
* 2:通过sp_pc1压缩换位 获得56个字节，c0=左28位  d0=右28位
* 3:将c0,d0按照表sp_key_shifts循环移位
* 4:将移位后的合并成一个新的56字节数字
* 5:根据表sp_pc2压缩换位，获得48个字节的数字
***********************************************************/

PINT64 BuildSubKey(INT64 key,INT64 pSubKey[16])
{
	int c,d,tmp_bit;
	int i;
	INT64 ret;//保存结果
	//将64位转为56位密钥，去除奇偶校验位
	INT64 nkey = BitSwap(key,sp_key_pc1,ARRAY_LEN(sp_key_pc1));
	d = nkey &0x0fffffff;
	c = (nkey>>28)&0x0fffffff;
	for (i=0; i<16; i++)
	{
		if (sp_key_shifts[i] == 1)
		{
			/*不能使用汇编的循环左移，TMD，只有28位有效位
			__asm
			{
			rol c,1;
			rol d,1;
			}
			*/
			tmp_bit =  c & 0x08000000;//第28位
			c <<= 1;//左移1位
			c &= 0x0fffffff;//将前面4位清0
			c |= (tmp_bit >> 27);//将第28位放置最后

			tmp_bit =  d & 0x08000000;//第28位
			d <<= 1;//左移1位
			d &= 0x0fffffff;//将前面4位清0
			d |= (tmp_bit >> 27);//将第28位放置最后
		} 
		else if (sp_key_shifts[i] == 2)
		{
			/*不能使用汇编的循环左移，TMD，只有28位有效位
			__asm
			{
			rol c,2;
			rol d,2;
			}
			*/
			tmp_bit =  c & 0x0C000000;//第28,27位
			c <<= 2;//左移2位
			c &= 0x0fffffff;//将前面4位清0
			c |= (tmp_bit >> 26);//将第28位放置最后

			tmp_bit =  d & 0x0C000000;//第28,27位
			d <<= 2;//左移2位
			d &= 0x0fffffff;//将前面4位清0
			d |= (tmp_bit >> 26);//将第28位放置最后
		}
		ret = 0;
		ret = c & 0x0fffffff;
		ret <<= 28;
		ret |= d;
		ret <<= 8;//左移8位，使得位数左对齐
		*(pSubKey + i) = BitSwap(ret,sp_key_pc2,ARRAY_LEN(sp_key_pc2));
	}
	return pSubKey;
}

/**************************************************************************
* DES 算法的F函数
* 运算过程:
* 将32位参数R 扩展到48位 
* 将48位的R与K做异或运算
* 将48位数字按照每6位分成8组，分别对应8个S盒
* 取组中数字第6位和第1位取S盒的行，5,4,3,2位取S盒的列 101010 则行10 列0101
* ----将S盒中取到的数据4位代替输入数字的中间4位----可能不准
* 将输出的32位按照置换表sp_box 做位置换
* 返回置换后的结果
*************************************************************************/
int F(int r,INT64 k)
{
	INT64 nr,num,ret,s_ret;
	int i,row,col;
	BYTE box[8] = {0};//8个字节的BOX
	BYTE r_box[8] = {0};//8个字节的S盒替换后的数据，可能不用
	nr = BitSwap(((INT64)r) << 32,sp_box_ext,ARRAY_LEN(sp_box_ext));
	num = nr ^ k;//异或运算
	s_ret = 0;
	//48位分8组
	for (i=7; i>=0; i--)
	{
		box[i] = (BYTE)(num & 0x3f);
		num >>= 6;
	}

	//S盒替换
	for (i=0; i<8; i++)
	{
		row = ((box[i] >> 4) & 0x2) | (box[i] & 0x1);//取第1位和6位
		col = (box[i] >> 1) & 0x0f;//取2,3,4,5位
		s_ret <<= 4;
		s_ret |= (sbox[i][row * 16 + col]);
	}
	ret = BitSwap(s_ret << 32,sp_box_end,ARRAY_LEN(sp_box_end));
	return (int)(ret & KEEPZERO32);
}

INT64 Cliper(INT64 num,INT64 pSubKey[16])
{
	int i,f,r,l,tmp;
	INT64 nv = 0;
	l = (int)((num >> 32) & KEEPZERO32);
	r = (int)(num & KEEPZERO32);
	for (i=0; i<16; i++)
	{
		tmp = r;
		f = F(r,pSubKey[i]);
		r = l ^ f;
		l = tmp;
	}
	nv |= r;
	nv <<= 32;
	nv |= (l & KEEPZERO32);
	return nv;
}

INT64 DeCliper(INT64 num,INT64 pSubKey[16])
{
	int i,f,r,l,tmp;
	INT64 nv = 0;
	l = (int)(num & KEEPZERO32);//加密时的l
	r = (int)((num >> 32) & KEEPZERO32);//加密时的r
	for (i=15; i>=0; i--)
	{
		tmp = l;
		f = F(l,pSubKey[i]);
		l = r ^ f;
		r = tmp;
	}
	nv |= l;
	nv <<= 32;
	nv |= (r & KEEPZERO32);
	return nv;
}

INT64 EnCrypt(INT64 num,INT64 k)
{
	INT64 nv;
	INT64 pSubKey[16];//子密钥
	BuildSubKey(k,pSubKey);
	nv = InitSwap(num);
	nv = Cliper(nv,pSubKey);
	nv = LastSwap(nv);
	return nv;
}

INT64 DeCrypt(INT64 num,INT64 k)
{
	INT64 nv;
	INT64 pSubKey[16];//子密钥
	BuildSubKey(k,pSubKey);
	nv = InvInitSwap(num);
	nv = DeCliper(nv,pSubKey);
	nv = InvLastSwap(nv);
	return nv;
}

//转化key,将7个字节的key转为一个64位的整型数据
//添加奇偶校验位
INT64 ChooseKey1(char cKey[7])
{
	int i;
	INT64 key = 0;
	unsigned char c = 0;
	INT64 tmp = *((INT64*)cKey);
	for (i=0; i<7; i++)
	{
		key <<= 8;
		//这表示是偶数位，应该加1
		c = (tmp >> (7- i - 1) * 7) & 0x7f;//移位
		c <<= 1;
		if (double_bit_byte[c])
		{
			c |= 0x1;
		}
		key |= c;
	}
	return key;
}

/*
static INT64 ChooseKey(char str[7])
{
    unsigned int i;
	char key[8];
    key[0] = str[0] >> 1;//01234567 前7位
    key[1] = (( str[0] & 0x01) << 6) | (str[1] >> 2);//上一个字节第8位，后一个字节的前6位
    key[2] = (( str[1] & 0x03) << 5) | (str[2] >> 3);
    key[3] = (( str[2] & 0x07) << 4) | (str[3] >> 4);
    key[4] = (( str[3] & 0x0F) << 3) | (str[4] >> 5);
    key[5] = (( str[4] & 0x1F) << 2) | (str[5] >> 6);
    key[6] = (( str[5] & 0x3F) << 1) | (str[6] >> 7);
    key[7] = str[6] & 0x7F;
    for ( i = 0; i < 8; i++ )
    {
        key[i] = ( key[i] << 1 );//7->8个字节，每个字节空出最后一位作为奇偶位
    }
    return *((INT64*)key);
}
*/

void DesInit(DesContextPtr context)
{
	INT64 key = 0;
	char cKey[7] = {0};
	if (NULL != context)
	{
		//最多取7个字节
		memcpy(cKey,context->key, 7);

		//获取KEY
		key = ChooseKey(cKey);

		//构建子密钥
		BuildSubKey(key,context->subkey);
	}
}

void DesEncrypt(DesContextPtr context,
	void* pInput,
	unsigned int srcLen,
	void* pOutput)
{
	unsigned int slen,loop,i;
	INT64 nv = 0,cnv=0;
	INT64* pDesOut = (INT64*)pOutput;//结果
	slen = CHECKDIV8(srcLen);
	memset(pOutput,0,slen);
	memcpy(pOutput,pInput,srcLen);//补0
	//key的处理
	//实际的key应该是56位7个字节，这里超过7个字节将被截断
	//没有7个字节也将补0
	//memcpy(cKey,pKey,(keyLen>7)?7:keyLen);//最多取7个字节
	//key = ChooseKey(cKey);//获取KEY
	loop = slen / 8;//获取循环次数
	//BuildSubKey(key,pSubKey);
	for(i=0; i<loop; i++)
	{
		nv = *(pDesOut + i);
		nv = InitSwap(nv);
		nv = Cliper(nv,context->subkey);
		nv = LastSwap(nv);
		if (Des_Mode_ECB == context->mode)
		{
			*(pDesOut + i) = nv;
		}
		else if (Des_Mode_BCB == context->mode)
		{
			*(pDesOut + i) = nv ^ cnv;
			cnv = nv;
		}
	}
	return;
}


void DesDecrypt(DesContextPtr context,
	void* pInput,
	unsigned int srcLen,
	void* pOutput)
{
	unsigned int loop,i;
	INT64 nv = 0,cnv = 0;
	INT64* pDesOut = (INT64*)pOutput;//结果
	if ((srcLen % 8) != 0)
	{
		return;
	}
	//slen = CHECKDIV8(srcLen);
	memset(pOutput,0,srcLen);
	memcpy(pOutput,pInput,srcLen);//补0
	//key的处理
	//实际的key应该是56位7个字节，这里超过7个字节将被截断
	//没有7个字节也将补0
	//memcpy(cKey,pKey,(keyLen>7)?7:keyLen);//最多取7个字节
	//key = ChooseKey(cKey);//获取KEY
	loop = srcLen / 8;//获取循环次数
	//BuildSubKey(key,pSubKey);
	for(i=0; i<loop; i++)
	{
		nv = *(pDesOut + i);
		if (Des_Mode_BCB == context->mode)
		{
			nv = nv ^ cnv;
			cnv = nv;
		}
		nv = InvInitSwap(nv);
		nv = DeCliper(nv,context->subkey);
		nv = InvLastSwap(nv);
		*(pDesOut+i) = nv;
	}
	return;
}