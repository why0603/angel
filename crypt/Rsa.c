/******************************************************************************************
* @file Rsa.c
* @anthor:Croky.Zheng
* @CreateTime:2011/8/10
* @descirption: RSA公钥加密算法
* 费马小定律:
* 假如p是质数，且(a,p)=1，那么 a^(p-1) ≡1（mod p） 假如p是质数，且a,p互质，那么 a的(p-1)次方除以p的余数恒等于1
* 这个加密算法的难点在于两个大素数的乘积很好算，而乘积的因式分解却不好算这一点
* 主要利用费马小定律，费马欧拉定理等数论中的法则
* 取两个大素数 p,q
* 取p*q = N （N为整数，p,q的乘积）
* 取(p-1)*(q-1) = M (根据欧拉函数，不大于N且与N互质的整数个数为M个)
* 则根据费马小定律 a^n = a(mod n) (a与N互质--除了1和自身没有公约数)
* 当(n mod a) != 0时 a^(n-1) = 1(mod n) 即 a的(n-1)次方除以n必然余1
* 寻找一个整数E，E与M互质 （E mod M != 0) (E是否能为质数?)
* 寻找一个整数D，(E*D) Mod M = 1
* 则：(N,e)是公钥，(N,d)是私钥
* 则对数据X进行加密
* 使用 X^E Mod N = Y (Y就是加密之后的数据)
* 使用 Y^D Mod N = X (X是解密之后的数据)
* RSA的实现难题主要是大数的运算
* 这里将使用 miracl 大数据运算库
*****************************************************************************************/
#ifdef RSA
#include "Rsa.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Primes.c"

#define MAXBITCOUNT	8192   //1024*8

#define MAXBYTECOUNT 1024

#define SLIPERCHAR	'\n'

static unsigned int GetN(unsigned int p,unsigned int q);

static unsigned int GetE(unsigned int p,unsigned int q);

static unsigned int GetD(unsigned int p,unsigned int q);


//计算N的值
static void Cacle_N(PRsaContext pContext);

//计算M的值
static void Cacle_M(PRsaContext pContext);

//计算E的值
static void Cacle_E(PRsaContext pContext);

//计算D的值
static void Cacle_D(PRsaContext pContext);

static miracl *mip;

unsigned int RsaInit(PRsaContext pContext,const char* pszFileName,int nMode,RsaFormat format)
{
	//默认10进制
	if ((format <= RSA_MIN)  || (format >= RSA_MAX))
	{
		format = RSA_DEC;
	}
	//最大长度都是5000位，输入、输出、运算用的进制都是10进制
	mip = mirsys(MAXBITCOUNT, format);
	if (NULL != pContext)
	{
		pContext->d = mirvar(0);
		pContext->e = mirvar(0);
		pContext->m = mirvar(0);
		pContext->n = mirvar(0);
		pContext->p = mirvar(0);
		pContext->q = mirvar(0);
	}

	//文件名称
	if (NULL != pszFileName)
	{
		RsaReadContextFromFile(pszFileName,pContext,nMode,format);
	}
	else
	{
		//没有输入文件且模式为F_ALL则会自动生成生成P,Q,E,D,M,N
		if ((F_ALL) == nMode)
		{
			RsaGeneratePQ(pContext);
		}
	}

	//有P,Q
	if ((pContext->p->len > 0) && 
		(pContext->q->len > 0))
	{
		if (pContext->n->len == 0)
		{
			Cacle_N(pContext);
		}

		if (pContext->m->len == 0)
		{
			Cacle_M(pContext);
		}

		//没有E则需要计算一个E
		if (pContext->e->len == 0)
		{
			Cacle_E(pContext);
		}

		//没有D则需要计算一个D
		if (pContext->d->len == 0)
		{
			Cacle_D(pContext);
		}
	}

	return (unsigned int)mip;
}

void RsaFinal(PRsaContext pContext)
{
	if (NULL != pContext)
	{
		mirkill(pContext->q);
		mirkill(pContext->q);
		mirkill(pContext->e);
		mirkill(pContext->d);
		mirkill(pContext->m);
		mirkill(pContext->n);
	}
	mirexit();
}

void Cacle_N(PRsaContext pContext)
{
	//fmul(pContext->p,pContext->q,pContext->n);//n = p * q;
	multiply(pContext->p,pContext->q,pContext->n);
}

void Cacle_M(PRsaContext pContext)
{
	big p2;
	big q2;
	//one = mirvar(1);
	p2 = mirvar(0);
	q2 = mirvar(0);
	//fsub(pContext->p,one,p2);
	//subtract(pContext->p,one,p2);

	decr(pContext->p,1,p2);//p2 = p - 1;
	//fsub(pContext->q,one,q2);
	//subtract(pContext->q,one,q2);
	decr(pContext->q,1,q2);//q2 = q - 1
	//fmul(p2,q2,pContext->m);// m = (p - 1) * (q - 1);
	multiply(p2,q2,pContext->m);
	//mirkill(one);
	mirkill(p2);
	mirkill(q2);
}

void Cacle_E(PRsaContext pContext)
{
	//e需要与m互质
	unsigned int int_e = 0;
	big x = mirvar(0);
	big y = mirvar(0);
	big z = mirvar(0);
	big max_div = mirvar(1);//最大公约数
	big min_e = mirvar(2);//e最小的值为2
	//求n,m的最大公约数
	//egcd(pContext->n,pContext->m,z);
	copy(pContext->n,x);//
	copy(pContext->m,y);//
	divide(x,y,z);//x=x mod y  : z = x / y;
	//divide(y,x,z);//y=y mod x : z = y / x;
	/*
	egcd(y,x,z);
	int_e = compare(z,max_div);//z > 1 则返回1，等于1则返回0，小于1则返回-1
	while (int_e != 0)//最大公约数为1，则表示是互质的
	{
	//incr(z,1,pContext->e);//e = z + 1;
	decr(x,1,x);
	if (compare(x,min_e) == -1)
	{
	break;
	}
	egcd(y,x,z);
	}
	*/
	//divisible(x,y);//获取x是否能整除y;
	while(divisible(y,x) == FALSE)
	{
		decr(x,1,x);
		//x < 2则退出
		if (compare(x,min_e) == -1)
		{
			break;
		}
	}
	//计算出的e<=2,则使用随机数的算法
	int_e = compare(x,min_e);

	if ((int_e == -1) || (int_e == 0))
	{
		srand(time(NULL));
		int_e = mill_primes[rand()%mill_primes_count];//获取E的值，使用素数表中的一个值
		convert(int_e,pContext->e);//获取随机一个素数作为E的值
	}
	else
	{
		//e>2;
		copy(x,pContext->e);
	}
	mirkill(x);
	mirkill(y);
	mirkill(z);
	mirkill(max_div);
	mirkill(min_e);
	//copy(pContext->n,n);
	//copy(pContext->m,m);
	//big ret;
	//modmult2(pContext->n,pContext->m,pContext->e);
	//divide(x,y,z);//x=x mod y  : z = x / y;

	//产生一个随机数rand();
	//rand()%mill_primes_count
}

void Cacle_D(PRsaContext pContext)
{
	//big d = mirvar(0);
	//big z = mirvar(0);
	//寻找一个整数D，(E*D) Mod M = 1
	//multi_inverse(1,&pContext->e, pContext->m, &pContext->d);
	//mirkill(d);
	//mirkill(z);
	xgcd(pContext->e, pContext->m, pContext->d, pContext->d, pContext->d);
}

unsigned int RsaCaclePrime(unsigned int nBitCount,char* pszStr)
{
	big baseValue = mirvar(0);
	big prime = mirvar(0);
	expb2(nBitCount,baseValue);
	if (TRUE == nxprime(baseValue,prime))
	{
		cotstr(prime,pszStr);
		//cotstr(x,s);//将一个大数根据其进制转换成一个字符串
		//cinstr(x,s);//将大数字符串转换成大数
		//bytes_to_big(n,s,x);//将s中的n个字符转换成大数
		//big_to_bytes(n,x,s,b);//
	}
	mirkill(baseValue);
	mirkill(prime);
	return numdig(prime);//获取prime数字个数
}

unsigned int RsaCaclePrimes(unsigned int nBitCount,unsigned int nCount,char* pszStr)
{
	unsigned int nLen = 0;
	big baseValue = mirvar(0);
	big prime = mirvar(0);
	expb2(nBitCount,baseValue);
	while(nCount > 0)
	{
		if (TRUE == nxprime(baseValue,prime))
		{
			cotstr(prime,pszStr);
			copy(prime,baseValue);
			nLen += numdig(prime);//获取prime数字个数
			pszStr[nLen] = SLIPERCHAR;
			pszStr += (nLen + 1);
			nCount--;
		}
	}
	mirkill(baseValue);
	mirkill(prime);
	return nLen;
}

unsigned int RsaCaclePrimesToFile(unsigned int nBitCount,unsigned int nCount,const char* pszFileName)
{
	FILE* pFile = NULL;
	unsigned int nLen = 0;
	big baseValue = mirvar(0);
	big prime = mirvar(0);
	if (NULL == pszFileName)
	{
		return 0;
	}
	pFile = fopen(pszFileName,"w+");
	expb2(nBitCount,baseValue);
	while(nCount > 0)
	{
		if (nxprime(baseValue,prime) == TRUE)
		{
			otnum(prime,pFile);//会自动加换行信息
			//cotnum(prime,pFile);//cotnum对otnum做了一些封装
			//fputc('\n',pFile);
			copy(prime,baseValue);
			nLen += numdig(prime);//获取prime数字个数
			nCount--;
#if defined(DEBUG)
			printf("%d\r\n",nCount);
#endif
		}
	}
	mirkill(baseValue);
	mirkill(prime);
	fclose(pFile);
	return nLen;
}

void RsaGeneratePQ(PRsaContext pContext)
{
	big minPrime = mirvar(0);
	//big minBound = mirvar(1);
	//irand((unsigned int)time(0));//clock();
	//设置最小位
	//sftbit(minBound, MAXBITCOUNT/2-1, minBound);

	//bigbits
	//产生一个指定长度的进制的随机数，该函数使用内置的随机数发生器，初始化种子调用irand函数
	//bigdig(MAXBITCOUNT/8,10,pContext->p);
	//bigdig(MAXBITCOUNT/8,10,pContext->q);
	//计算2的512次方
	expb2(MAXBITCOUNT/8, minPrime);//计算2的1024次方, 2^1024 ~= 1.8 * 10^308
	nxprime(minPrime,pContext->p);
#ifdef DEBUG
	printf("p num = %d\r\n",numdig(pContext->p));
#endif	
	//expb2(MAXBITCOUNT/16, minPrime);//计算2的512次方, 2^1024 ~= 1.8 * 10^308

	//nxprime(minPrime,pContext->q); 

	nxprime(pContext->p,pContext->q); //保证q > p;其实没有这个必要
#ifdef DEBUG	
	printf("q num = %d\r\n",numdig(pContext->p));
#endif	
	/*
	//寻找大素数P
	while(1)
	{
	if (compare(minBound, pContext->p) == 1)
	{
	add (pContext->p,minBound,pContext->p);
	}
	if (isprime(pContext->p))
	{
	break ;
	}
	}

	//寻找大素数Q
	while(1)
	{
	if (compare(minBound,pContext->q) == 1)
	{
	add (pContext->q,minBound,pContext->q);
	}
	if (isprime(pContext->q))
	{
	break ;
	}
	}
	*/

	//Cacl_M(pContext);
	//Cacl_N(pContext);
	//Cacl_E(pContext);
	//Cacl_D(pContext);
	mirkill(minPrime);
}

//生成私钥
void RsaGenPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey)
{
	/*
	if (pContext->d->len == 0)
	{
		Cacle_D(pContext);
	}
	*/
	pKey->baseLen = big_to_bytes(MAXBYTECOUNT,pContext->d,pKey->pBase,FALSE);
	pKey->modLen = big_to_bytes(MAXBYTECOUNT,pContext->n,pKey->pMod,FALSE);
	pKey->keyLen = pKey->baseLen + pKey->modLen + sizeof(RsaKey);
}

//生成公钥
void RsaGenPublicKey(PRsaContext pContext,PRsaPublicKey pKey)
{
	/*
	if (pContext->e->len == 0)
	{
		Cacle_e(pContext);
	}
	*/
	pKey->baseLen = big_to_bytes(MAXBYTECOUNT,pContext->e,pKey->pBase,FALSE);
	pKey->modLen = big_to_bytes(MAXBYTECOUNT,pContext->n,pKey->pMod,FALSE);
	pKey->keyLen = pKey->baseLen + pKey->modLen + sizeof(RsaKey);
}

//加载私钥
void RsaReadPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey)
{
	bytes_to_big(pKey->baseLen,pKey->pBase,pContext->d);
	bytes_to_big(pKey->modLen,pKey->pMod,pContext->n);
}

//加载公钥
void RsaReadPublicKey(PRsaContext pContext,PRsaPublicKey pKey)
{
	bytes_to_big(pKey->baseLen,pKey->pBase,pContext->e);
	bytes_to_big(pKey->modLen,pKey->pMod,pContext->n);
}

void RsaWriteContextToFile(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format)
{
	//以16进制方式写入
	//mip->IOBASE = 16;
	int oldformat = mip->IOBASE;
	FILE* pFile = fopen(pszFileName,"w+");
	//默认10进制
	if ((format <= RSA_MIN)  || (format >= RSA_MAX))
	{
		format = RSA_DEC;
	}
	mip->IOBASE = format;
	if ((nMode & F_P) > 0)
	{
		cotnum(pContext->p,pFile);
	}
	if ((nMode & F_Q) > 0)
	{
		cotnum(pContext->q,pFile);
	}
	if ((nMode & F_E) > 0)
	{
		cotnum(pContext->e,pFile);
	}
	if ((nMode & F_D) > 0)
	{
		cotnum(pContext->d,pFile);
	}
	if ((nMode & F_M) > 0)
	{
		cotnum(pContext->m,pFile);
	}
	if ((nMode & F_N) > 0)
	{
		cotnum(pContext->n,pFile);
	}
	mip->IOBASE = oldformat;
	fclose(pFile);
}

void RsaReadContextFromFile(const char* pszFileName,
	PRsaContext pContext,
	int nMode,
	RsaFormat format)
{
	//以16进制方式读取
	//mip->IOBASE = 16;
	int oldformat = mip->IOBASE;
	FILE* pFile = fopen(pszFileName,"r");
	//默认10进制
	if ((format <= RSA_MIN)  || (format >= RSA_MAX))
	{
		format = RSA_DEC;
	}
	mip->IOBASE = format;
	if ((nMode & F_P) > 0)
	{
		cinnum(pContext->p,pFile);
	}
	if ((nMode & F_Q) > 0)
	{
		cinnum(pContext->q,pFile);
	}
	if ((nMode & F_E) > 0)
	{
		cinnum(pContext->e,pFile);
	}
	if ((nMode & F_D) > 0)
	{
		cinnum(pContext->d,pFile);
	}
	if ((nMode & F_M) > 0)
	{
		cinnum(pContext->m,pFile);
	}
	if ((nMode & F_N) > 0)
	{
		cinnum(pContext->n,pFile);
	}
	mip->IOBASE = oldformat;
	fclose(pFile);
}

void ReadP(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format)
{
	RsaReadContextFromFile(pszFileName,pContext,F_P,format);
}

void ReadQ(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format)
{
	RsaReadContextFromFile(pszFileName,pContext,F_Q,format);
}

void ReadE(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format)
{
	RsaReadContextFromFile(pszFileName,pContext,F_E,format);
}

void ReadD(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format)
{
	RsaReadContextFromFile(pszFileName,pContext,F_D,format);
}

void ReadM(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format)
{
	RsaReadContextFromFile(pszFileName,pContext,F_M,format);
}

void ReadN(const char* pszFileName,
	PRsaContext pContext,
	RsaFormat format)
{
	RsaReadContextFromFile(pszFileName,pContext,F_N,format);
}

void PrintContext(PRsaContext pContext)
{
	if (NULL != pContext)
	{
		printf("P=\r\n");
		cotnum(pContext->p,stdout);
		printf("\r\n");

		printf("Q=\r\n");
		cotnum(pContext->q,stdout);
		printf("\r\n");


		printf("E=\r\n");
		cotnum(pContext->e,stdout);
		printf("\r\n");


		printf("D=\r\n");
		cotnum(pContext->d,stdout);
		printf("\r\n");


		printf("M=\r\n");
		cotnum(pContext->m,stdout);
		printf("\r\n");


		printf("N=\r\n");
		cotnum(pContext->n,stdout);
		printf("\r\n");
	}
}

int RsaCheckContext(PRsaContext pContext,int nMode)
{
	big sum = mirvar(0);
	big tmp1 = mirvar(0);
	big tmp2 = mirvar(0);
	big p1 = mirvar(0);
	big q1 = mirvar(0);
	big e1 = mirvar(1);
	if (NULL == pContext)
	{
		return RSA_ERR_NULL;
	}
	else
	{
		if ((nMode & F_P) > 0)
		{
			if (!isprime(pContext->p))
			{
				return RSA_ERR_P;
			}
		}

		if ((nMode & F_Q) > 0)
		{
			if (!isprime(pContext->q))
			{
				return RSA_ERR_P;
			}
		}

		//m,e最大公约 != 1
		if ((nMode & F_E) > 0)
		{
			//e与m需要互质
			egcd(pContext->e,pContext->m,e1);
			//if (divisible(pContext->m,pContext->e) == TRUE)
			//最大公约数为1
			if ((e1->len != 1) || ((unsigned int)(*e1->w) != 1))
			{
				return RSA_ERR_E;
			}
		}

		//(e*d) mod m != 1
		if ((nMode & F_D) > 0)
		{
			multiply(pContext->e,pContext->d,sum);
			divide(sum,pContext->m,tmp1);//sum = sum mod p->m,tmp1 = sum / p->m;
			//余数要为1
			if ((sum->len != 1) || ((unsigned int)(*sum->w) != 1))
			{
				return RSA_ERR_N;
			}
		}

		//m != (p-1) * (q-1);
		if ((nMode & F_M) > 0)
		{
			decr(pContext->p,1,p1);
			decr(pContext->q,1,q1);
			multiply(p1,q1,tmp2);
			if (compare(tmp2,pContext->m) != 0)
			{
				return RSA_ERR_M;
			}
		}

		//n != p * q;
		if ((nMode & F_N) > 0)
		{
			multiply(pContext->p,pContext->q,sum);
			if (compare(sum,pContext->n) != 0)
			{
				return RSA_ERR_N;
			}
		}
	}
	return RSA_ERR_NOERR;
}

unsigned int RsaEncrypt(PRsaContext pContext,
	void* pInput,
	unsigned int nLen,
	void* pOutput)
{
	unsigned int len = 0;
	big p = mirvar(0);//明文
	big c = mirvar(0);//密文
	bytes_to_big(nLen,(const char*)pInput,p);//将输入转为一个大数字,这是明文
	//len = big_to_bytes(MAXBYTECOUNT,p,(char*)pOutput,FALSE);
	powmod(p,pContext->e,pContext->n,c);
	//len = numdig(c);
	//len = size(c);
	len = big_to_bytes(MAXBYTECOUNT,c,(char*)pOutput,FALSE);
	mirkill(p);
	mirkill(c);
	return len;
}

unsigned int RsaDecrypt(PRsaContext pContext,
	void* pInput,
	unsigned int nLen,
	void* pOutput)
{
	unsigned int len = 0;
	big c = mirvar(0);//密文
	big p = mirvar(0);
	bytes_to_big(nLen,(const char*)pInput,c);//将输入转为一个大数字,这是明文
	powmod(c,pContext->d,pContext->n,p);
	//len = numdig(p);
	len = big_to_bytes(MAXBYTECOUNT,p,(char*)pOutput,FALSE);
	mirkill(p);
	mirkill(c);
	return len;
}
#endif

/*
//根据P,Q获取E,D两个数
void GetED(PRsaElement pElement)
{
	//找整数E可以 M=(p-1)(q-1)
	//获取M/2......(M/2)的余
	pElement->n = pElement->p * pElement->q;
	pElement->m = (pElement->p - 1) * (pElement->q - 1);
	pElement->e = GetE(pElement->p,pElement->q);
	if (pElement->e >= 1)
	{
		pElement->d = GetD(pElement->e,pElement->m);
	}
	return;
}

unsigned int GetN(unsigned int p,unsigned int q)
{
	return p * q;
}

//
//* 取E算法
//* 取n % m = k
//* 枚举 m % k--
//* 发现余数不为0 则返回
//
unsigned int GetE(unsigned int p,unsigned int q)
{
	unsigned int n = p * q;
	unsigned int m = (p - 1) * (q - 1);
	unsigned int e = n % m;
	while (1)
	{
		if (e <= 1)
		{
			break;
		}
		if ((m % e != 0))
		{
			break;
		}
		e--;
	}
	return e;
}

unsigned int GetD(unsigned int e,unsigned int m)
{
	unsigned int d = 2;
	unsigned int sum = 0;
	for (; d<m; d++)
	{
		if ((d * e) % m == 1)
		{
			break;
		}
	}
	return d;
}


unsigned int RsaEncrypt(PRsaElement pElement,unsigned int pliper)
{
	//指定基数和big,flash变量的最大尺寸，这个尺寸越大，能表示的数字就越大
	big flash_r = mirvar(0); //initialise big variable nf=1
	big flash_e = mirvar(pElement->e);
	big flash_m = mirvar(pliper);
	big flash_n = mirvar(pElement->n);
	//premult(nf, n--, nf); // nf=n!=n*(n-1)*...2*1
	//power2(sum,pliper,flash_p);
	powmod(flash_m,flash_e,flash_n,flash_r);
#if defined(DEBUG)
	printf("rsa encrypt:");
	cotnum(flash_r, stdout);
	printf("\r\n");
#endif
	return *flash_r->w;
}

unsigned int RsaDecrypt(PRsaElement pElement,unsigned int cliper)
{
	big flash_r = mirvar(0);
	big flash_d = mirvar(pElement->d);
	big flash_c = mirvar(cliper);
	big flash_n = mirvar(pElement->n);
	powmod(flash_c,flash_d,flash_n,flash_r);
#if defined(DEBUG)
	printf("rsa decrypt:");
	cotnum(flash_r, stdout);
	printf("\r\n");
#endif
	return *flash_r->w;
}
*/