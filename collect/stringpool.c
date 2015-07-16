/******************************************************************************************
* @file stringpool.c
* @anthor:Croky.Zheng
* @CreateTime:2013-5-24
* @descirption:字符串池,用于保存所有的字符串，保证一个相同的字符串只有一个实例
* 这里所有的字符串只是用一种格式,用于兼容所有的字符串
* 当然也可以保存字节数组
* 2:以\0\0\0\0结尾的字符串
*****************************************************************************************/

#include "stringpool.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>

//使用乘法来做哈希,则需要(hash * 0.618) - (long)(hash * 0.618)
//这样获取的小数部分再去乘以HASH表的桶数，能得到更好的散列分布
#define KNUTH	0.6180339887 //HASH乘法取值中的关键值

//字符串数量超过这个比例则要给池扩容
#define DEFAULT_RAISE_RATE 0.75f

//最小区域大小
#define MINICAPACITY	64

#define HASHSUB(x) (((x) ^ ((x) >> 20) ^ ((x) >> 12))

#define HASH(x) HASHSUB(x) ^ (HASHSUB(x) >> 7) ^ (HASHSUB(x) >> 4)

typedef struct __pool_node__
{
	//hash1 + hash2 + hash3 用于区别出两个不同的字符串
	//三个字符串产出的哈希值一样的可能性是如此的小
	unsigned long hash1;
	unsigned long hash2;
	//这个哈希值额外使用于
	unsigned long hash3;
	//
	struct __pool_node__* next;

	unsigned long length;
	//字符串的值
	char value[];
} PoolNode,*PoolNodePtr;

//为哈希表扩容
static void EnsureCapacity(StringPoolPtr pool);


//移动节点
static void MoveNode(StringPoolPtr pool,void* newAddress,unsigned long newCapacity);

//\0结尾字符串的哈希值计算方式
static unsigned long StringHashCode(char* pStr,unsigned long nLen);

//\0结尾字符串的比较方式
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
			//获取节点应该在的索引位置
			index = IndexFor(Hash(curNode->hash3),newCapacity);

			//获取新表的index节点
			newNodePtr = (PoolNodePtr*)newAddress + index;
			
			//保存当前节点的下一个节点
			nextNode = curNode->next;

			//将节点指针指向位置的头部作为下一个节点，这样就节点就好比一个list head，总是指向列表的表头
			curNode->next = *newNodePtr;

			//将当前节点定为表头
			*newNodePtr = curNode;

			//*((HashNodePtr)newAddress + index) = *curNode;
			//旧的list的下一个节点
			curNode = nextNode;
		}
	}
}

void EnsureCapacity(StringPoolPtr pool)
{
	//每次扩大一倍
	unsigned newCapacity = pool->modCount << 1;
	unsigned long i = 0,index = 0;
	//void* newAddress = malloc((sizeof(HashNode) + table->keyByteSize) * newCapacity);
	void* newAddress = calloc(newCapacity,sizeof(PoolNodePtr));
	if (NULL != newAddress)
	{
		//calloc会将内存清0,不需要调用memset方法
		//memset(newAddress,0,sizeof(HashNodePtr) * newCapacity);
		if (NULL != pool->address)
		{
			MoveNode(pool,newAddress,newCapacity);
			free(pool->address);
		}
		//释放老内存
		pool->address = newAddress;
		pool->modCount = newCapacity;
		//桶数量
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

		//桶使用量超过75%，则需要扩容
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
		if((curNode->length == len) && //KEY长度
			(curNode->hash1 == hash1) && //hash值相同
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
		//前面4个字节为长度
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
		//有一些unicode字符有4个字节,这显然是有问题的
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
			if((curNode->length == len) && //KEY长度
				(curNode->hash1 == hash1) && //hash值相同
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
			curNode = (PoolNodePtr)malloc(sizeof(PoolNode) + len + 4);//2字节结束符
			if (NULL != curNode)
			{
				curNode->hash1 = hash1;
				curNode->hash2 = hash2;
				curNode->hash3 = hash3;
				//memcpy(curNode->value,(char*)&len,sizeof(unsigned long));
				//前面四个字节为字符串长度
				//*((unsigned long*)&curNode->value) = len;
				curNode->length = len;
				//最后两个个字节为0
				*((long*)&curNode->value[len]) = 0;
				memcpy(&curNode->value,key,len);
				//新节点为元素链表的头节点
				curNode->next = *oldNodePtr;
				//增加一个桶使用量
				if (NULL == *oldNodePtr)
				{
					pool->modUseCount++;
				}
				*oldNodePtr = curNode;
				//增加一个元素
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
			if((oldNode->length == len) && //KEY长度
				(oldNode->hash1 == hash1) && //hash值相同
				(oldNode->hash2 == hash2) &&
				(oldNode->hash3 == hash3) &&
				(memcmp(oldNode->value,key,len) == 0))
			{
				//删除的是头节点
				if (oldNode == *oldNodePtr)
				{
					*oldNodePtr = oldNode->next;
				}
				else
				{
					preNode->next = oldNode->next;
				}
				//变成了空指针
				if (NULL == *oldNodePtr)
				{
					//空出一个桶
					pool->modUseCount--;
				}
				//释放空间
				free(oldNode);
				//元素-1
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