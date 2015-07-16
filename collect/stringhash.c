/******************************************************************************************
* @file StringHash.c
* @anthor:Croky.Zheng
* @CreateTime:2013-6-22
* @descirption:
* ��StringHash��֮ͬ�����ڣ��ڲ���ʽ�����˱仯����ϣ��ĽṹҲ�����仯
* StringHashʹ�������ϣ������ڵ�ʹ��һ��Ͱ�����ֽṹ���ڲ��룬ɾ�������Ҳ���
* �����ʼ���һ������StringHashʹ��ȫ��HASH�����뷽�㣬�����ң�����KEY����ɾ����
* ������Ҫ�������������KEY�ڱ��в����ڣ�,�����ܱ�֤ÿһ���ڵ㶼��һ��������Ͱ
* ����ÿ��Ԫ�ض���һ�����������������Ը���������ѯ��ȡ����
*****************************************************************************************/

#include "StringHash.h"
#include "hash.h"

//ʹ�ó˷�������ϣ,����Ҫ(hash * 0.618) - (long)(hash * 0.618)
//������ȡ��С��������ȥ����HASH���Ͱ�����ܵõ����õ�ɢ�зֲ�
#define KNUTH	0.6180339887 //HASH�˷�ȡֵ�еĹؼ�ֵ

//�ַ��������������������Ҫ��������
#define DEFAULT_RAISE_RATE 0.75f

//��С�����С
#define MINICAPACITY	64

//Ϊ��ϣ������
static void EnsureCapacity(StringHashPtr hash);


//�ƶ��ڵ�
static void MoveNode(StringHashPtr stringHash,void* newAddress,unsigned long newCapacity);

//\0��β�ַ����ıȽϷ�ʽ
static Boolean StringKeyEqual(void* key1,void* key2);

//�����ϣֵ
//static unsigned long Hash(unsigned long h) ;

static unsigned long IndexFor(unsigned long h, unsigned long length) ;

static void FreeHashNode(StringHashNodePtr node);

static StringHashNodePtr GetStringHashNode(StringHashPtr stringHash,void* key,unsigned long len);

//���ڵ���뵽ָ�����ڴ��ַ�ĵ�INDEX�����
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

	//û���ҵ����ʵĿ�Ͱ
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
	double s1 = (double)(unsigned long)s;//����s1���ܱ�Ϊ����
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
	//ÿ������һ��
	unsigned newCapacity = stringHash->modCount << 1;
	unsigned long i = 0,index = 0;
	void* newAddress = calloc(newCapacity,sizeof(StringHashNodePtr));
	if (NULL != newAddress)
	{
		//calloc�Ὣ�ڴ���0,����Ҫ����memset����
		//memset(newAddress,0,sizeof(StringHashNodePtr) * newCapacity);
		if ((NULL != stringHash->address) && (stringHash->size > 0))
		{
			MoveNode(stringHash,newAddress,newCapacity);
			free(stringHash->address);
		}
		//�ͷ����ڴ�
		stringHash->address = newAddress;
		stringHash->modCount = newCapacity;
		//Ͱ����
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
		if ((NULL == curNode) ||		 //���Ͱû�нڵ�
			((len == curNode->length) &&  //�ַ�������һ��
			(hash1 == curNode->hash1) && //�ַ���HASHֵһ��
			(memcmp(curNode->value,key,len) == 0)))//�ַ���һ��
		{
			break;
		}
	}

	//û���ҵ����ʵĿ�Ͱ
	if (idx == capacity)
	{
		for (idx=0; idx<index; idx++)
		{
			curNode = *((StringHashNodePtr*)stringHash->address + idx);
			if ((NULL == curNode) ||		 //���Ͱû�нڵ�
				((len == curNode->length) &&  //�ַ�������һ��
				(hash1 == curNode->hash1) && //�ַ���HASHֵһ��
				(memcmp(curNode->value,key,len) == 0)))//�ַ���һ��
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
		//��һЩunicode�ַ���4���ֽ�,����Ȼ���������
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
		//�պ��Ǹ���Ͱ
		if (NULL == curNode)
		{
			/*
			*/
			allow_insert = True;
		}
		else
		{
			//�����Ƿ��д�key�ڵ�
			curNode = GetStringHashNode(stringHash,key,len);
			if (NULL == curNode)
			{
				allow_insert = True;
			}
		}

		if (allow_insert == True)
		{
			curNode = (StringHashNodePtr)malloc(sizeof(StringHashNode) + len + 4);//4�ֽ�\0\0\0\0
			curNode->hash1 = hash1;
			curNode->hash2 = hash2;
			curNode->length = len;
			memcpy(curNode->value,key,len);
			*((long*)&curNode->value[len]) = 0;
			ret_index = InsertNodeToIndex(curNode,stringHash->address,stringHash->modCount,index);
			stringHash->size++;
			//����75%������������
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
		if (NULL == curNode)		 //���Ͱû�нڵ�
		{
			break;
		}
		*/
		if((len == curNode->length) &&  //�ַ�������һ��
			(hash1 == curNode->hash1) && //�ַ���HASHֵһ��
			(memcmp(curNode->value,key,len) == 0))//�ַ���һ��
		{
			allow_delete = True;
			break;
		}
	}

	//û���ҵ����ʵĿ�Ͱ
	if ((idx == capacity) && (allow_delete == False))
	{
		for (idx=0; idx<index; idx++)
		{
			curNode = *((StringHashNodePtr*)stringHash->address + idx);
			/*
			if (NULL == curNode)		 //���Ͱû�нڵ�
			{
				break;
			}
			*/
			if((len == curNode->length) &&  //�ַ�������һ��
				(hash1 == curNode->hash1) && //�ַ���HASHֵһ��
				(memcmp(curNode->value,key,len) == 0))//�ַ���һ��
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