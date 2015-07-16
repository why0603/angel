/******************************************************************************************
* @file stringpool.c
* @anthor:Croky.Zheng
* @CreateTime:2013-5-24
* @descirption:�ַ�����,���ڱ������е��ַ�������֤һ����ͬ���ַ���ֻ��һ��ʵ��
* �������е��ַ���ֻ����һ�ָ�ʽ,���ڼ������е��ַ���
* ��ȻҲ���Ա����ֽ�����
* 2:��\0\0\0\0��β���ַ���
*****************************************************************************************/

#include "stringpool.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>

//ʹ�ó˷�������ϣ,����Ҫ(hash * 0.618) - (long)(hash * 0.618)
//������ȡ��С��������ȥ����HASH���Ͱ�����ܵõ����õ�ɢ�зֲ�
#define KNUTH	0.6180339887 //HASH�˷�ȡֵ�еĹؼ�ֵ

//�ַ��������������������Ҫ��������
#define DEFAULT_RAISE_RATE 0.75f

//��С�����С
#define MINICAPACITY	64

#define HASHSUB(x) (((x) ^ ((x) >> 20) ^ ((x) >> 12))

#define HASH(x) HASHSUB(x) ^ (HASHSUB(x) >> 7) ^ (HASHSUB(x) >> 4)

typedef struct __pool_node__
{
	//hash1 + hash2 + hash3 ���������������ͬ���ַ���
	//�����ַ��������Ĺ�ϣֵһ���Ŀ���������˵�С
	unsigned long hash1;
	unsigned long hash2;
	//�����ϣֵ����ʹ����
	unsigned long hash3;
	//
	struct __pool_node__* next;

	unsigned long length;
	//�ַ�����ֵ
	char value[];
} PoolNode,*PoolNodePtr;

//Ϊ��ϣ������
static void EnsureCapacity(StringPoolPtr pool);


//�ƶ��ڵ�
static void MoveNode(StringPoolPtr pool,void* newAddress,unsigned long newCapacity);

//\0��β�ַ����Ĺ�ϣֵ���㷽ʽ
static unsigned long StringHashCode(char* pStr,unsigned long nLen);

//\0��β�ַ����ıȽϷ�ʽ
static Boolean StringKeyEqual(void* key1,void* key2);

static unsigned long Hash(unsigned long h) ;

static unsigned long IndexFor(unsigned long h, unsigned long length) ;

static void FreeHashNode(PoolNodePtr node);

static PoolNodePtr GetStringPoolNode(StringPoolPtr pool,void* key,unsigned long len);

unsigned long Hash(unsigned long h) 
{
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}

unsigned long IndexFor(unsigned long h, unsigned long length) 
{
	return h & (length-1);
}

void MoveNode(StringPoolPtr pool,void* newAddress,unsigned long newCapacity)
{
	PoolNodePtr curNode,nextNode;
	PoolNodePtr* newNodePtr;
	unsigned long i = 0,index = 0;

	for (i=0; i<pool->modCount; i++)
	{
		//oldNodePtr = (PoolNodePtr*)pool->address + i;
		curNode = *((PoolNodePtr*)pool->address + i);//*oldNodePtr;
		while(NULL != curNode)
		{
			//��ȡ�ڵ�Ӧ���ڵ�����λ��
			index = IndexFor(Hash(curNode->hash3),newCapacity);

			//��ȡ�±��index�ڵ�
			newNodePtr = (PoolNodePtr*)newAddress + index;
			
			//���浱ǰ�ڵ����һ���ڵ�
			nextNode = curNode->next;

			//���ڵ�ָ��ָ��λ�õ�ͷ����Ϊ��һ���ڵ㣬�����ͽڵ�ͺñ�һ��list head������ָ���б�ı�ͷ
			curNode->next = *newNodePtr;

			//����ǰ�ڵ㶨Ϊ��ͷ
			*newNodePtr = curNode;

			//*((HashNodePtr)newAddress + index) = *curNode;
			//�ɵ�list����һ���ڵ�
			curNode = nextNode;
		}
	}
}

void EnsureCapacity(StringPoolPtr pool)
{
	//ÿ������һ��
	unsigned newCapacity = pool->modCount << 1;
	unsigned long i = 0,index = 0;
	//void* newAddress = malloc((sizeof(HashNode) + table->keyByteSize) * newCapacity);
	void* newAddress = calloc(newCapacity,sizeof(PoolNodePtr));
	if (NULL != newAddress)
	{
		//calloc�Ὣ�ڴ���0,����Ҫ����memset����
		//memset(newAddress,0,sizeof(HashNodePtr) * newCapacity);
		if (NULL != pool->address)
		{
			MoveNode(pool,newAddress,newCapacity);
			free(pool->address);
		}
		//�ͷ����ڴ�
		pool->address = newAddress;
		pool->modCount = newCapacity;
		//Ͱ����
	}
}

StringPoolPtr CreateStringPool(unsigned long capacity)
{
	unsigned initCapacity = 1;
	StringPoolPtr pool = (StringPoolPtr)malloc(sizeof(StringPoolPtr));
	if (NULL != pool)
	{
		memset(pool,0,sizeof(PoolNodePtr));
		while (initCapacity < capacity)
		{
			initCapacity <<= 1;
		}

		if (initCapacity == 0)
		{
			initCapacity = MINICAPACITY;
		}
		pool->hashCode1 = Hash2;
		pool->hashCode2 = Hash3;
		pool->hashCode3 = Hash4;

		//Ͱʹ��������75%������Ҫ����
		pool->modUseCount = 0;
		pool->size = 0;
		pool->modCount = initCapacity >> 1;
		EnsureCapacity(pool);
	}
	return pool;
}

StringPoolPtr CreateDefaultStringPool()
{
	return CreateStringPool(16);
}

PoolNodePtr GetStringPoolNode(StringPoolPtr pool,char* key,unsigned long len)
{
	unsigned long index,hash1,hash2,hash3;
	PoolNodePtr* oldNodePtr;
	PoolNodePtr curNode;
	hash1 = pool->hashCode1((char*)key,len);
	hash2 = pool->hashCode2((char*)key,len);
	hash3 = pool->hashCode3((char*)key,len);
	index = IndexFor(Hash(hash3),pool->modCount);
	oldNodePtr = (PoolNodePtr*)pool->address + index;
	curNode = *oldNodePtr;
	while (NULL != curNode)
	{
		if((curNode->length == len) && //KEY����
			(curNode->hash1 == hash1) && //hashֵ��ͬ
			(curNode->hash2 == hash2) &&
			(curNode->hash3 == hash3) &&
			(memcmp(curNode->value,key,len) == 0))
		{
			break;
		}
		curNode = curNode->next;
	}
	return curNode;
}

Boolean ContairStringPool(StringPoolPtr pool,char* key,unsigned long len)
{
	PoolNodePtr node = NULL;
	Boolean ret = False;
	if ((NULL != pool) && (NULL != key) && (len > 0))
	{
		node = GetStringPoolNode(pool,key,len);
	}
	if (NULL != node)
	{
		ret = True;
	}
	return ret;
}

Boolean ContairConstStringPool(StringPoolPtr pool,char* key)
{
	PoolNodePtr node = NULL;
	Boolean ret = False;
	if ((NULL != pool) && (NULL != key))
	{
		node = GetStringPoolNode(pool,key,(unsigned long)strlen(key));
	}
	if (NULL != node)
	{
		ret = True;
	}
	return ret;
}
char* GetStringPool(StringPoolPtr pool,char* key,unsigned long len)
{
	PoolNodePtr node = GetStringPoolNode(pool,key,len);
	if (NULL != node)
	{
		//ǰ��4���ֽ�Ϊ����
		return node->value;
	}
	return NULL;
}

char* GetConstStringPool(StringPoolPtr pool,char* key)
{
	if ((NULL != pool) && (NULL != key))
	{
		return GetStringPool(pool,key,(unsigned long)strlen(key));
	}
	return NULL;
}

char* PutConstStringPool(StringPoolPtr pool,const char* key)
{
	if ((NULL != pool) && (NULL != key))
	{
		PutStringPool(pool,(char*)key,(unsigned long)strlen(key));
	}
	return NULL;
}

char* PutUnicodeToStringPool(StringPoolPtr pool,const wchar_t* key)
{
	if ((NULL != pool) && (NULL != key))
	{
		//��һЩunicode�ַ���4���ֽ�,����Ȼ���������
		PutStringPool(pool,(char*)key,(unsigned long)wcslen(key) * 2);
	}
	return NULL;
}

char* PutStringPool(StringPoolPtr pool,char* key,unsigned long len)
{
	unsigned long index,hash1,hash2,hash3;
	PoolNodePtr* oldNodePtr;
	PoolNodePtr curNode;
	if ((NULL != pool) && (NULL != key) && (len > 0))
	{
		hash1 = pool->hashCode1((char*)key,len);
		hash2 = pool->hashCode2((char*)key,len);
		hash3 = pool->hashCode3((char*)key,len);
		index = IndexFor(Hash(hash3),pool->modCount);
		oldNodePtr = (PoolNodePtr*)pool->address + index;
		curNode = *oldNodePtr;
		while(NULL != curNode)
		{
			if((curNode->length == len) && //KEY����
				(curNode->hash1 == hash1) && //hashֵ��ͬ
				(curNode->hash2 == hash2) &&
				(curNode->hash3 == hash3) &&
				(memcmp(curNode->value,key,len) == 0))
			{
				break;
			}
			curNode = curNode->next;
		}
		if (NULL == curNode)
		{
			curNode = (PoolNodePtr)malloc(sizeof(PoolNode) + len + 4);//2�ֽڽ�����
			if (NULL != curNode)
			{
				curNode->hash1 = hash1;
				curNode->hash2 = hash2;
				curNode->hash3 = hash3;
				//memcpy(curNode->value,(char*)&len,sizeof(unsigned long));
				//ǰ���ĸ��ֽ�Ϊ�ַ�������
				//*((unsigned long*)&curNode->value) = len;
				curNode->length = len;
				//����������ֽ�Ϊ0
				*((long*)&curNode->value[len]) = 0;
				memcpy(&curNode->value,key,len);
				//�½ڵ�ΪԪ�������ͷ�ڵ�
				curNode->next = *oldNodePtr;
				//����һ��Ͱʹ����
				if (NULL == *oldNodePtr)
				{
					pool->modUseCount++;
				}
				*oldNodePtr = curNode;
				//����һ��Ԫ��
				pool->size++;
				//printf("hash size:%d,%d,%d,%s\r\n",pool->size,pool->modUseCount,index,key);
				if (pool->size > DEFAULT_RAISE_RATE * pool->modCount)
				{
					EnsureCapacity(pool);
				}
				return curNode->value;
			}
		}
	}
	return NULL;
}

void RemoveStringPool(StringPoolPtr pool,char* key,unsigned long len)
{
	unsigned long index,hash1,hash2,hash3;
	PoolNodePtr oldNode,preNode;
	PoolNodePtr* oldNodePtr;
	void* tmpValue = NULL;
	if ((NULL != pool) && (NULL != key) && (len > 0))
	{
		hash1 = pool->hashCode1((char*)key,len);
		hash2 = pool->hashCode2((char*)key,len);
		hash3 = pool->hashCode3((char*)key,len);
		index = IndexFor(Hash(hash3),pool->modCount);
		oldNodePtr = (PoolNodePtr*)pool->address + index;
		preNode = oldNode = *oldNodePtr;
		while(NULL != oldNode)
		{
			if((oldNode->length == len) && //KEY����
				(oldNode->hash1 == hash1) && //hashֵ��ͬ
				(oldNode->hash2 == hash2) &&
				(oldNode->hash3 == hash3) &&
				(memcmp(oldNode->value,key,len) == 0))
			{
				//ɾ������ͷ�ڵ�
				if (oldNode == *oldNodePtr)
				{
					*oldNodePtr = oldNode->next;
				}
				else
				{
					preNode->next = oldNode->next;
				}
				//����˿�ָ��
				if (NULL == *oldNodePtr)
				{
					//�ճ�һ��Ͱ
					pool->modUseCount--;
				}
				//�ͷſռ�
				free(oldNode);
				//Ԫ��-1
				pool->size--;
				break;
			}
			preNode = oldNode;
			oldNode = oldNode->next;
		}
	}
}

void RemoveConstStringPool(StringPoolPtr pool,char* key)
{
	if ((NULL != pool) && (NULL != key))
	{
		RemoveStringPool(pool,key,(unsigned long)strlen(key));
	}
}

void CleanStringPool(StringPoolPtr pool)
{
	PoolNodePtr curNode,preNode;
	unsigned long i = 0;
	if (NULL != pool)
	{
		for (i=0; i<pool->modCount; i++)
		{
			//oldNodePtr = (PoolNodePtr*)pool->address + i;
			curNode = *((PoolNodePtr*)pool->address + i);//*oldNodePtr;
			while(NULL != curNode)
			{
				preNode = curNode->next;
				free(curNode);
				curNode = preNode;
			}
		}
		pool->size = 0;
		//pool->modCount = 0;
		pool->modUseCount = 0;
	}
	if (NULL != pool->address)
	{
		memset(pool->address,0,pool->modCount * sizeof(PoolNodePtr));
	}
}


void EnumStringPool(StringPoolPtr pool,StringPoolCallBack fnStringPool)
{
	
	PoolNodePtr curNode;
	unsigned long i = 0;
	if (NULL != pool)
	{
		for (i=0; i<pool->modCount; i++)
		{
			//oldNodePtr = (PoolNodePtr*)pool->address + i;
			curNode = *((PoolNodePtr*)pool->address + i);//*oldNodePtr;
			while(NULL != curNode)
			{
				if (NULL != fnStringPool)
				{
					fnStringPool(pool,curNode->value);
				}
				curNode = curNode->next;
			}
		}
	}
}

void DestroyStringPool(StringPoolPtr pool)
{
	CleanStringPool(pool);
	if(NULL != pool)
	{
		if (NULL != pool->address)
		{
			free(pool->address);
		}
		free(pool);
	}
}