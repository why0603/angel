/******************************************************************************************
* @file StringHash.c
* @anthor:Croky.Zheng
* @CreateTime:2013-6-22
* @descirption:
* 与StringHash不同之处在于，内部格式发生了变化，哈希表的结构也发生变化
* StringHash使用链表哈希，多个节点使用一个桶，这种结构对于插入，删除，查找操作
* 的速率几乎一样。而StringHash使用全表HASH，插入方便，而查找（根据KEY）和删除则
* 可能需要遍历整个表（如果KEY在表中不存在）,但是能保证每一个节点都有一个独立的桶
* 这样每个元素都有一个独立的索引，可以根据索引查询获取数据
*****************************************************************************************/

#include "StringHash.h"
#include "hash.h"

//使用乘法来做哈希,则需要(hash * 0.618) - (long)(hash * 0.618)
//这样获取的小数部分再去乘以HASH表的桶数，能得到更好的散列分布
#define KNUTH	0.6180339887 //HASH乘法取值中的关键值

//字符串数量超过这个比例则要给池扩容
#define DEFAULT_RAISE_RATE 0.75f

//最小区域大小
#define MINICAPACITY	64

//为哈希表扩容
static void EnsureCapacity(StringHashPtr hash);


//移动节点
static void MoveNode(StringHashPtr stringHash,void* newAddress,unsigned long newCapacity);

//\0结尾字符串的比较方式
static Boolean StringKeyEqual(void* key1,void* key2);

//计算哈希值
//static unsigned long Hash(unsigned long h) ;

static unsigned long IndexFor(unsigned long h, unsigned long length) ;

static void FreeHashNode(StringHashNodePtr node);

static StringHashNodePtr GetStringHashNode(StringHashPtr stringHash,void* key,unsigned long len);

//将节点插入到指定的内存地址的第INDEX个序号
static unsigned long InsertNodeToIndex(StringHashNodePtr node,void* address,unsigned long capacity,unsigned long index);

unsigned long InsertNodeToIndex(StringHashNodePtr node,void* address,unsigned long capacity,unsigned long index)
{
	unsigned long idx;
	StringHashNodePtr curNode;
	for (idx = index; idx<capacity; idx++)
	{
		curNode = *((StringHashNodePtr*)address + idx);
		if (NULL == curNode)
		{
			*((StringHashNodePtr*)address + idx) = node;
			break;
		}
	}

	//没有找到合适的空桶
	if (idx == capacity)
	{
		for (idx=0; idx<index; idx++)
		{
			curNode = *((StringHashNodePtr*)address + idx);
			if (NULL == curNode)
			{
				*((StringHashNodePtr*)address + idx) = node;
				break;
			}
		}
	}
	return idx;
}

unsigned long IndexFor(unsigned long h, unsigned long length) 
{
	long double s = h* KNUTH;
	double s1 = (double)(unsigned long)s;//否则s1可能变为负数
	long double d = s - s1;
	return (unsigned long)(d * length);
}

void MoveNode(StringHashPtr stringHash,void* newAddress,unsigned long newCapacity)
{
	StringHashNodePtr curNode;
	unsigned long i = 0,index = 0;
	for (i=0; i<stringHash->modCount; i++)
	{
		curNode = *((StringHashNodePtr*)stringHash->address + i);
		if (NULL != curNode)
		{
			index = IndexFor(curNode->hash2,newCapacity);
			InsertNodeToIndex(curNode,newAddress,newCapacity,index);
		}
	}
}

void EnsureCapacity(StringHashPtr stringHash)
{
	//每次扩大一倍
	unsigned newCapacity = stringHash->modCount << 1;
	unsigned long i = 0,index = 0;
	void* newAddress = calloc(newCapacity,sizeof(StringHashNodePtr));
	if (NULL != newAddress)
	{
		//calloc会将内存清0,不需要调用memset方法
		//memset(newAddress,0,sizeof(StringHashNodePtr) * newCapacity);
		if ((NULL != stringHash->address) && (stringHash->size > 0))
		{
			MoveNode(stringHash,newAddress,newCapacity);
			free(stringHash->address);
		}
		//释放老内存
		stringHash->address = newAddress;
		stringHash->modCount = newCapacity;
		//桶数量
	}
}

StringHashPtr CreateStringHash(unsigned long capacity)
{
	unsigned initCapacity = 1;
	StringHashPtr stringHash = (StringHashPtr)malloc(sizeof(StringHash));
	if (NULL != stringHash)
	{
		memset(stringHash,0,sizeof(StringHashNodePtr));
		while (initCapacity < capacity)
		{
			initCapacity <<= 1;
		}

		if (initCapacity == 0)
		{
			initCapacity = MINICAPACITY;
		}
		stringHash->hashCode1 = Hash2;
		stringHash->hashCode2 = Hash3;
		//hash->hashCode3 = Hash4;
		stringHash->size = 0;
		stringHash->modCount = initCapacity >> 1;
		EnsureCapacity(stringHash);
	}
	return stringHash;
}

StringHashPtr CreateDefaultStringHash()
{
	return CreateStringHash(16);
}

StringHashNodePtr GetStringHashNode(StringHashPtr stringHash,char* key,unsigned long len)
{
	unsigned long idx,index,hash1,hash2;
	StringHashNodePtr curNode = NULL;
	unsigned long capacity = stringHash->modCount;
	hash1 = stringHash->hashCode1((char*)key,len);
	hash2 = stringHash->hashCode2((char*)key,len);
	index = IndexFor(hash2,stringHash->modCount);
	for (idx = index; idx<capacity; idx++)
	{
		curNode = *((StringHashNodePtr*)stringHash->address + idx);
		if ((NULL == curNode) ||		 //这个桶没有节点
			((len == curNode->length) &&  //字符串长度一致
			(hash1 == curNode->hash1) && //字符串HASH值一致
			(memcmp(curNode->value,key,len) == 0)))//字符串一致
		{
			break;
		}
	}

	//没有找到合适的空桶
	if (idx == capacity)
	{
		for (idx=0; idx<index; idx++)
		{
			curNode = *((StringHashNodePtr*)stringHash->address + idx);
			if ((NULL == curNode) ||		 //这个桶没有节点
				((len == curNode->length) &&  //字符串长度一致
				(hash1 == curNode->hash1) && //字符串HASH值一致
				(memcmp(curNode->value,key,len) == 0)))//字符串一致
			{
				break;
			}
		}
	}
	return curNode;
}

Boolean ContairStringHash(StringHashPtr stringHash,char* key,unsigned long len)
{
	StringHashNodePtr node = NULL;
	Boolean ret = False;
	if ((NULL != stringHash) && (NULL != key) && (len > 0))
	{
		node = GetStringHashNode(stringHash,key,len);
	}
	if (NULL != node)
	{
		ret = True;
	}
	return ret;
}

Boolean ContairConstStringHash(StringHashPtr stringHash,char* key)
{
	StringHashNodePtr node = NULL;
	Boolean ret = False;
	if ((NULL != stringHash) && (NULL != key))
	{
		node = GetStringHashNode(stringHash,key,(unsigned long)strlen(key));
	}
	if (NULL != node)
	{
		ret = True;
	}
	return ret;
}

char* GetStringHash(StringHashPtr stringHash,char* key,unsigned long len)
{
	StringHashNodePtr node = GetStringHashNode(stringHash,key,len);
	if (NULL != node)
	{
		return node->value;
	}
	return NULL;
}

char* GetStringHashByIndex(StringHashPtr stringHash,unsigned long index)
{
	StringHashNodePtr node = *(StringHashNodePtr*)stringHash->address + index;
	if (NULL != node)
	{
		return node->value;
	}
	return NULL;
}

char* GetConstStringHash(StringHashPtr stringHash,char* key)
{
	if ((NULL != stringHash) && (NULL != key))
	{
		return GetStringHash(stringHash,key,(unsigned long)strlen(key));
	}
	return NULL;
}

long PutConstStringHash(StringHashPtr stringHash,const char* key)
{
	long ret_index = -1;
	if ((NULL != stringHash) && (NULL != key))
	{
		ret_index = PutStringHash(stringHash,(char*)key,(unsigned long)strlen(key));
	}
	return ret_index;
}

long PutConstUnicodeStringHash(StringHashPtr stringHash,const wchar_t* key)
{
	long ret_index = -1;
	if ((NULL != stringHash) && (NULL != key))
	{
		//有一些unicode字符有4个字节,这显然是有问题的
		ret_index = PutStringHash(stringHash,(char*)key,(unsigned long)wcslen(key) * 2);
	}
	return ret_index;
}

long PutStringHash(StringHashPtr stringHash,char* key,unsigned long len)
{
	unsigned long index,hash1,hash2;
	StringHashNodePtr curNode;
	unsigned long capacity = stringHash->modCount;
	Boolean allow_insert = False;
	long ret_index = -1;
	if ((NULL != stringHash) && (NULL != key) && (len > 0))
	{
		hash1 = stringHash->hashCode1((char*)key,len);
		hash2 = stringHash->hashCode2((char*)key,len);
		index = IndexFor(hash2,stringHash->modCount);
		curNode = *((StringHashNodePtr*)stringHash->address + index);
		//刚好是个空桶
		if (NULL == curNode)
		{
			/*
			*/
			allow_insert = True;
		}
		else
		{
			//查找是否有此key节点
			curNode = GetStringHashNode(stringHash,key,len);
			if (NULL == curNode)
			{
				allow_insert = True;
			}
		}

		if (allow_insert == True)
		{
			curNode = (StringHashNodePtr)malloc(sizeof(StringHashNode) + len + 4);//4字节\0\0\0\0
			curNode->hash1 = hash1;
			curNode->hash2 = hash2;
			curNode->length = len;
			memcpy(curNode->value,key,len);
			*((long*)&curNode->value[len]) = 0;
			ret_index = InsertNodeToIndex(curNode,stringHash->address,stringHash->modCount,index);
			stringHash->size++;
			//超过75%，则扩大容器
			if (stringHash->size > DEFAULT_RAISE_RATE * stringHash->modCount)
			{
				EnsureCapacity(stringHash);
			}
		}
	}
	return ret_index;
}

void RemoveStringHash(StringHashPtr stringHash,char* key,unsigned long len)
{
	unsigned long idx,index,hash1,hash2;
	StringHashNodePtr curNode = NULL;
	Boolean allow_delete = False;
	unsigned long capacity = stringHash->modCount;
	hash1 = stringHash->hashCode1((char*)key,len);
	hash2 = stringHash->hashCode2((char*)key,len);
	index = IndexFor(hash2,stringHash->modCount);
	for (idx = index; idx<capacity; idx++)
	{
		curNode = *((StringHashNodePtr*)stringHash->address + idx);
		/*
		if (NULL == curNode)		 //这个桶没有节点
		{
			break;
		}
		*/
		if((len == curNode->length) &&  //字符串长度一致
			(hash1 == curNode->hash1) && //字符串HASH值一致
			(memcmp(curNode->value,key,len) == 0))//字符串一致
		{
			allow_delete = True;
			break;
		}
	}

	//没有找到合适的空桶
	if ((idx == capacity) && (allow_delete == False))
	{
		for (idx=0; idx<index; idx++)
		{
			curNode = *((StringHashNodePtr*)stringHash->address + idx);
			/*
			if (NULL == curNode)		 //这个桶没有节点
			{
				break;
			}
			*/
			if((len == curNode->length) &&  //字符串长度一致
				(hash1 == curNode->hash1) && //字符串HASH值一致
				(memcmp(curNode->value,key,len) == 0))//字符串一致
			{
				allow_delete = True;
				break;
			}
		}
	}

	if ((allow_delete == True) && (NULL != curNode))
	{
		free(curNode);
		*((StringHashNodePtr*)stringHash->address + idx) = NULL;
		stringHash->size--;
	}
}

void RemoveConstStringHash(StringHashPtr stringHash,char* key)
{
	if ((NULL != stringHash) && (NULL != key))
	{
		RemoveStringHash(stringHash,key,(unsigned long)strlen(key));
	}
}

void CleanStringHash(StringHashPtr stringHash)
{
	StringHashNodePtr curNode;
	unsigned long i = 0;
	if (NULL != stringHash)
	{
		for (i=0; i<stringHash->modCount; i++)
		{
			//oldNodePtr = (StringHashNodePtr*)hash->address + i;
			curNode = *((StringHashNodePtr*)stringHash->address + i);//*oldNodePtr;
			if (NULL != curNode)
			{
				free(curNode);
			}
		}
		stringHash->size = 0;
		stringHash->modCount = 0;
		//hash->modUseCount = 0;
	}
	if (NULL != stringHash->address)
	{
		memset(stringHash->address,0,stringHash->modCount * sizeof(StringHashNodePtr));
	}
}


void EnumStringHash(StringHashPtr stringHash,StringHashCallBack fnStringHash)
{
	StringHashNodePtr curNode;
	unsigned long i = 0;
	if ((NULL != stringHash) && (NULL != fnStringHash))
	{
		for (i=0; i<stringHash->modCount; i++)
		{
			curNode = *((StringHashNodePtr*)stringHash->address + i);//*oldNodePtr;
			if(NULL != curNode)
			{
				fnStringHash(stringHash,curNode->value);
			}
		}
	}
}
void DestroyStringHash(StringHashPtr stringHash)
{
	CleanStringHash(stringHash);
	if (NULL != stringHash)
	{
		if (NULL != stringHash->address)
		{
			free(stringHash->address);
		}
		free(stringHash);
	}
}