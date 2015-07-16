/******************************************************************************************
* @file Rsa.c
* @anthor:Croky.Zheng
* @CreateTime:2011/8/10
* @descirption: RSA��Կ�����㷨
* ����С����:
* ����p����������(a,p)=1����ô a^(p-1) ��1��mod p�� ����p����������a,p���ʣ���ô a��(p-1)�η�����p�����������1
* ��������㷨���ѵ����������������ĳ˻��ܺ��㣬���˻�����ʽ�ֽ�ȴ��������һ��
* ��Ҫ���÷���С���ɣ�����ŷ������������еķ���
* ȡ���������� p,q
* ȡp*q = N ��NΪ������p,q�ĳ˻���
* ȡ(p-1)*(q-1) = M (����ŷ��������������N����N���ʵ���������ΪM��)
* ����ݷ���С���� a^n = a(mod n) (a��N����--����1������û�й�Լ��)
* ��(n mod a) != 0ʱ a^(n-1) = 1(mod n) �� a��(n-1)�η�����n��Ȼ��1
* Ѱ��һ������E��E��M���� ��E mod M != 0) (E�Ƿ���Ϊ����?)
* Ѱ��һ������D��(E*D) Mod M = 1
* ��(N,e)�ǹ�Կ��(N,d)��˽Կ
* �������X���м���
* ʹ�� X^E Mod N = Y (Y���Ǽ���֮�������)
* ʹ�� Y^D Mod N = X (X�ǽ���֮�������)
* RSA��ʵ��������Ҫ�Ǵ���������
* ���ｫʹ�� miracl �����������
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


//����N��ֵ
static void Cacle_N(PRsaContext pContext);

//����M��ֵ
static void Cacle_M(PRsaContext pContext);

//����E��ֵ
static void Cacle_E(PRsaContext pContext);

//����D��ֵ
static void Cacle_D(PRsaContext pContext);

static miracl *mip;

unsigned int RsaInit(PRsaContext pContext,const char* pszFileName,int nMode,RsaFormat format)
{
	//Ĭ��10����
	if ((format <= RSA_MIN)  || (format >= RSA_MAX))
	{
		format = RSA_DEC;
	}
	//��󳤶ȶ���5000λ�����롢����������õĽ��ƶ���10����
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

	//�ļ�����
	if (NULL != pszFileName)
	{
		RsaReadContextFromFile(pszFileName,pContext,nMode,format);
	}
	else
	{
		//û�������ļ���ģʽΪF_ALL����Զ���������P,Q,E,D,M,N
		if ((F_ALL) == nMode)
		{
			RsaGeneratePQ(pContext);
		}
	}

	//��P,Q
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

		//û��E����Ҫ����һ��E
		if (pContext->e->len == 0)
		{
			Cacle_E(pContext);
		}

		//û��D����Ҫ����һ��D
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
	//e��Ҫ��m����
	unsigned int int_e = 0;
	big x = mirvar(0);
	big y = mirvar(0);
	big z = mirvar(0);
	big max_div = mirvar(1);//���Լ��
	big min_e = mirvar(2);//e��С��ֵΪ2
	//��n,m�����Լ��
	//egcd(pContext->n,pContext->m,z);
	copy(pContext->n,x);//
	copy(pContext->m,y);//
	divide(x,y,z);//x=x mod y  : z = x / y;
	//divide(y,x,z);//y=y mod x : z = y / x;
	/*
	egcd(y,x,z);
	int_e = compare(z,max_div);//z > 1 �򷵻�1������1�򷵻�0��С��1�򷵻�-1
	while (int_e != 0)//���Լ��Ϊ1�����ʾ�ǻ��ʵ�
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
	//divisible(x,y);//��ȡx�Ƿ�������y;
	while(divisible(y,x) == FALSE)
	{
		decr(x,1,x);
		//x < 2���˳�
		if (compare(x,min_e) == -1)
		{
			break;
		}
	}
	//�������e<=2,��ʹ����������㷨
	int_e = compare(x,min_e);

	if ((int_e == -1) || (int_e == 0))
	{
		srand(time(NULL));
		int_e = mill_primes[rand()%mill_primes_count];//��ȡE��ֵ��ʹ���������е�һ��ֵ
		convert(int_e,pContext->e);//��ȡ���һ��������ΪE��ֵ
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

	//����һ�������rand();
	//rand()%mill_primes_count
}

void Cacle_D(PRsaContext pContext)
{
	//big d = mirvar(0);
	//big z = mirvar(0);
	//Ѱ��һ������D��(E*D) Mod M = 1
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
		//cotstr(x,s);//��һ���������������ת����һ���ַ���
		//cinstr(x,s);//�������ַ���ת���ɴ���
		//bytes_to_big(n,s,x);//��s�е�n���ַ�ת���ɴ���
		//big_to_bytes(n,x,s,b);//
	}
	mirkill(baseValue);
	mirkill(prime);
	return numdig(prime);//��ȡprime���ָ���
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
			nLen += numdig(prime);//��ȡprime���ָ���
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
			otnum(prime,pFile);//���Զ��ӻ�����Ϣ
			//cotnum(prime,pFile);//cotnum��otnum����һЩ��װ
			//fputc('\n',pFile);
			copy(prime,baseValue);
			nLen += numdig(prime);//��ȡprime���ָ���
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
	//������Сλ
	//sftbit(minBound, MAXBITCOUNT/2-1, minBound);

	//bigbits
	//����һ��ָ�����ȵĽ��Ƶ���������ú���ʹ�����õ����������������ʼ�����ӵ���irand����
	//bigdig(MAXBITCOUNT/8,10,pContext->p);
	//bigdig(MAXBITCOUNT/8,10,pContext->q);
	//����2��512�η�
	expb2(MAXBITCOUNT/8, minPrime);//����2��1024�η�, 2^1024 ~= 1.8 * 10^308
	nxprime(minPrime,pContext->p);
#ifdef DEBUG
	printf("p num = %d\r\n",numdig(pContext->p));
#endif	
	//expb2(MAXBITCOUNT/16, minPrime);//����2��512�η�, 2^1024 ~= 1.8 * 10^308

	//nxprime(minPrime,pContext->q); 

	nxprime(pContext->p,pContext->q); //��֤q > p;��ʵû�������Ҫ
#ifdef DEBUG	
	printf("q num = %d\r\n",numdig(pContext->p));
#endif	
	/*
	//Ѱ�Ҵ�����P
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

	//Ѱ�Ҵ�����Q
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

//����˽Կ
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

//���ɹ�Կ
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

//����˽Կ
void RsaReadPrivateKey(PRsaContext pContext,PRsaPrivateKey pKey)
{
	bytes_to_big(pKey->baseLen,pKey->pBase,pContext->d);
	bytes_to_big(pKey->modLen,pKey->pMod,pContext->n);
}

//���ع�Կ
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
	//��16���Ʒ�ʽд��
	//mip->IOBASE = 16;
	int oldformat = mip->IOBASE;
	FILE* pFile = fopen(pszFileName,"w+");
	//Ĭ��10����
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
	//��16���Ʒ�ʽ��ȡ
	//mip->IOBASE = 16;
	int oldformat = mip->IOBASE;
	FILE* pFile = fopen(pszFileName,"r");
	//Ĭ��10����
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

		//m,e���Լ != 1
		if ((nMode & F_E) > 0)
		{
			//e��m��Ҫ����
			egcd(pContext->e,pContext->m,e1);
			//if (divisible(pContext->m,pContext->e) == TRUE)
			//���Լ��Ϊ1
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
			//����ҪΪ1
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
	big p = mirvar(0);//����
	big c = mirvar(0);//����
	bytes_to_big(nLen,(const char*)pInput,p);//������תΪһ��������,��������
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
	big c = mirvar(0);//����
	big p = mirvar(0);
	bytes_to_big(nLen,(const char*)pInput,c);//������תΪһ��������,��������
	powmod(c,pContext->d,pContext->n,p);
	//len = numdig(p);
	len = big_to_bytes(MAXBYTECOUNT,p,(char*)pOutput,FALSE);
	mirkill(p);
	mirkill(c);
	return len;
}
#endif

/*
//����P,Q��ȡE,D������
void GetED(PRsaElement pElement)
{
	//������E���� M=(p-1)(q-1)
	//��ȡM/2......(M/2)����
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
//* ȡE�㷨
//* ȡn % m = k
//* ö�� m % k--
//* ����������Ϊ0 �򷵻�
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
	//ָ��������big,flash���������ߴ磬����ߴ�Խ���ܱ�ʾ�����־�Խ��
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