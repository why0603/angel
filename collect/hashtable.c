/******************************************************************************************
* @file hashtable.c
* @anthor:Croky.Zheng
* @CreateTime:2013-5-24
* @descirption:��ϣ��
* ��ʱ�������C����������Щȷʵ�Ǽ����ѵ��£���Ϊ��Ȼ��һ������
* ȴ���漰���ڴ�����������ʹ���ȵȷ���
* ��Ҫ����KEY������Ҫ����VALUE���������߶�������
* ��Ȼ������Ҳ����ΪʲôJAVA��һЩ���������ٶȻ���C���Ե�
* �㷨�ϴ�ҿ��Ż��ķ�Χ��ĺ�С���ܶ�ʱ��C���滨��̫���ʱ����
* �����ݷ����ڴ棬�������ݣ��ͷ��ڴ��ϣ���JAVA����Щ������Ȼ���ŵ��˴�������
* ��JAVA�����������߳���
*****************************************************************************************/

#include "hashtable.h"
#include "hash.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

//ʹ�ó˷�������ϣ,����Ҫ(hash * 0.618) - (long)(hash * 0.618)
//������ȡ��С��������ȥ����HASH���Ͱ�����ܵõ����õ�ɢ�зֲ�
#define KNUTH	0.6180339887 //HASH�˷�ȡֵ�еĹؼ�ֵ
//��С�����С
#define MINICAPACITY	64
#define DEFAULT_RAISE_RATE 0.75f

#define HASHSUB(x) (((x) ^ ((x) >> 20) ^ ((x) >> 12))
#define HASH(x) HASHSUB(x) ^ (HASHSUB(x) >> 7) ^ (HASHSUB(x) >> 4)

//Ϊ��ϣ������
static void EnsureCapacity(HashTablePtr table);

//����һ���µĹ�ϣ��ڵ�
static HashNodePtr CreateHashNode(HashTablePtr table,void* key,void* value);

//�ƶ��ڵ�
static void MoveNode(HashTablePtr table,void* newAddress,unsigned long newCapacity);

static unsigned long Hash(unsigned long h) ;

static unsigned long IndexFor(unsigned long h, unsigned long length) ;


unsigned long Hash(unsigned long h) 
{
	h ^= (h >> 20) ^ (h >> 12);
	return h ^ (h >> 7) ^ (h >> 4);
}

unsigned long IndexFor(unsigned long h, unsigned long length) 
{
	return h & (length-1);
}

HashNodePtr CreateHashNode(HashTablePtr table,void* key,void* value)
{

	char* valuePtr;
	HashNodePtr node = NULL;
	//unsigned long key_byte_size,value_byte_size;
	//key_byte_size = GetByteSizeOfData(table->keyType,key);
	//value_byte_size = GetByteSizeOfData(table->valueType,value);
	node = (HashNodePtr)malloc(sizeof(HashNode) + table->keyByteSize + table->valueByteSize);

	//��4�ı���
	switch (table->keyByteSize)
	{
	case Char_Type:
		*((char*)node->keyValue) = *((char*)key);
		break;
	case Short_Type:
		*((short*)node->keyValue) = *((short*)key);
		break;
	case Long_Type:
		*((long*)node->keyValue) = *((long*)key);
		break;
	case LongLong_Type:
		*((long long*)node->keyValue) = *((long long*)key);
		break;
	default:
		memcpy(node->keyValue,key,table->keyByteSize);
		break;
	}

	if ((NULL != value) && (table->valueByteSize > 0))
	{
		valuePtr = (char*)node->keyValue + table->keyByteSize;
		//��4�ı���
		switch (table->valueByteSize)
		{
			//value Ӧ��֧��û�����ݵķ�ʽ
		case Unknow_Type:
			break;
		case Char_Type:
			*valuePtr = *((char*)value);
			break;
		case Short_Type:
			*((short*)valuePtr) = *((short*)value);
			break;
		case Long_Type:
			*((long*)valuePtr) = *((long*)value);
			break;
		case LongLong_Type:
			*((long long*)valuePtr) = *((long long*)value);
			break;
		default:
			memcpy(valuePtr,value,table->valueByteSize);
			break;
		}
	}
	node->next = NULL;
	return node;
}

void MoveNode(HashTablePtr table,void* newAddress,unsigned long newCapacity)
{
	HashNodePtr curNode,nextNode;
	HashNodePtr* newNodePtr;
	unsigned long i = 0,index = 0;

	for (i=0; i<table->bucketCount; i++)
	{
		curNode = *((HashNodePtr*)table->address + i);
		while(NULL != curNode)
		{
			//��ȡ�ڵ�Ӧ���ڵ�����λ��
			index = IndexFor(Hash(curNode->hashValue),newCapacity);

			//��ȡ�±��index�ڵ�
			newNodePtr = (HashNodePtr*)newAddress + index;

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

void EnsureCapacity(HashTablePtr table)
{
	//ÿ������һ��
	unsigned newCapacity = table->bucketCount << 1;
	unsigned long i = 0,index = 0;
	//void* newAddress = malloc((sizeof(HashNode) + table->keyByteSize) * newCapacity);
	void* newAddress = calloc(newCapacity,sizeof(HashNodePtr));
	if (NULL != newAddress)
	{
		//calloc�Ὣ�ڴ���0,����Ҫ����memset����
		//memset(newAddress,0,sizeof(HashNodePtr) * newCapacity);
		if (NULL != table->address)
		{
			MoveNode(table,newAddress,newCapacity);
			free(table->address);
		}
		table->address = newAddress;
		table->bucketCount = newCapacity;
		//Ͱ����
	}
}

HashTablePtr CreateHashTable(unsigned long capacity,unsigned long keyByteSize,unsigned long valueByteSize)
{
	unsigned initCapacity = 1;
	HashTablePtr table = (HashTablePtr)malloc(sizeof(HashTable));
	if (NULL != table)
	{
		memset(table,0,sizeof(HashTable));
		while (initCapacity < capacity)
		{
			initCapacity <<= 1;
		}

		if (initCapacity == 0)
		{
			initCapacity = MINICAPACITY;
		}

		table->hashCode = Hash7;
		///*
		if (keyByteSize == 0)
		{
			table->keyByteSize = sizeof(void*);
		}
		else
		{
			table->keyByteSize = keyByteSize;
		}
		
		table->valueByteSize = valueByteSize;
		//*/

		//table->keyByteSize = keyDataType;


		//Ͱʹ��������75%������Ҫ����
		table->raiseRate = DEFAULT_RAISE_RATE;

		table->bucketCount = initCapacity >> 1;
		//�����ַ�ռ�
		EnsureCapacity(table);
	}
	return table;
}

HashTablePtr CreateDefaultHashTable()
{
	return CreateHashTable(16,sizeof(void*),sizeof(void*));
}

HashNodePtr GetHashTableNode(HashTablePtr table,void* key)
{
	unsigned long index,hash;
	HashNodePtr* oldNodePtr;
	HashNodePtr curNode;
	hash = table->hashCode((char*)key,table->keyByteSize);
	index = IndexFor(Hash(hash),table->bucketCount);
	oldNodePtr = (HashNodePtr*)table->address + index;
	curNode = *oldNodePtr;
	while (NULL != curNode)
	{
		if((curNode->hashValue == hash) && //hashֵ��ͬ
			(memcmp(key,curNode->keyValue,table->keyByteSize) == 0))
		{
			break;
		}
		curNode = curNode->next;
	}
	return curNode;
}

Boolean ContairHashTable(HashTablePtr table,void* key)
{
	HashNodePtr node = NULL;
	Boolean ret = False;
	if ((NULL != table) && (NULL != key))
	{
		node = GetHashTableNode(table,key);
	}
	if (NULL != node)
	{
		ret = True;
	}
	return ret;
}

void* GetHashTable(HashTablePtr table,void* key)
{
	HashNodePtr node = GetHashTableNode(table,key);
	if ((NULL != node) && (table->valueByteSize > 0))
	{
		return node->keyValue + table->keyByteSize;
	}
	return NULL;
}


void GetFromHashTable(HashTablePtr table,void* key,void* value)
{
	HashNodePtr node = GetHashTableNode(table,key);
	if ((NULL != node) && (table->valueByteSize > 0))
	{
		CopyData(value,node->keyValue + table->keyByteSize,(DataType)table->valueByteSize,table->valueByteSize);
	}
}

void PutHashTable(HashTablePtr table,void* key,void* value)
{
	unsigned long index,hash;
	HashNodePtr newNode,oldNode;
	HashNodePtr* oldNodePtr;
	void* tmpValue = NULL;
	//newNode = CreateHashNode(table,key,value);
	if ((NULL != table) && (NULL != key))
	{
		hash = table->hashCode((char*)key,table->keyByteSize);
		index = IndexFor(Hash(hash),table->bucketCount);
		oldNodePtr = (HashNodePtr*)table->address + index;
		oldNode = *oldNodePtr;
		while(NULL != oldNode)
		{
			if((oldNode->hashValue == hash) && //hashֵ��ͬ
				(memcmp(key,oldNode->keyValue,table->keyByteSize) == 0) && //ֵ��ͬ
				(table->valueByteSize > 0) && //ֵ���Ͳ����
				(NULL != value))
			{
				memcpy(oldNode->keyValue + table->keyByteSize,value,table->valueByteSize);
				break;
			}
			oldNode = oldNode->next;
		}
		if (NULL == oldNode)
		{
			newNode = CreateHashNode(table,key,value);
			newNode->hashValue = hash;
			//�½ڵ�ΪԪ�������ͷ�ڵ�
			newNode->next = *oldNodePtr;
			//����һ��Ͱʹ����
			if (NULL == *oldNodePtr)
			{
				table->bucketUseCount += 1;
			}
			*oldNodePtr = newNode;
			//����һ��Ԫ��
			table->size += 1;
			if (table->size > DEFAULT_RAISE_RATE * table->bucketCount)
			{
				EnsureCapacity(table);
			}
		}
	}
}

void RemoveHashTable(HashTablePtr table,void* key)
{
	unsigned long index,hash;
	HashNodePtr oldNode,preNode;
	HashNodePtr* oldNodePtr;
	void* tmpValue = NULL;
	//newNode = CreateHashNode(table,key,value);
	hash = table->hashCode((char*)key,table->keyByteSize);
	index = IndexFor(Hash(hash),table->bucketCount);
	oldNodePtr = (HashNodePtr*)table->address + index;
	preNode = oldNode = *oldNodePtr;
	while(NULL != oldNode)
	{
		if((oldNode->hashValue == hash) && //hashֵ��ͬ
			(memcmp(key,oldNode->keyValue,table->keyByteSize) == 0))
		{
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
				table->bucketUseCount--;
			}
			free(oldNode);
			//Ԫ��-1
			table->size--;
			break;
		}
		preNode = oldNode;
		oldNode = oldNode->next;
	}
}

void CleanHashTable(HashTablePtr table)
{
	HashNodePtr curNode,preNode;
	unsigned long i = 0,index = 0;
	if (NULL != table)
	{
		for (i=0; i<table->bucketCount; i++)
		{
			//oldNodePtr = (PoolNodePtr*)pool->address + i;
			curNode = *((HashNodePtr*)table->address + i);//*oldNodePtr;
			while(NULL != curNode)
			{
				preNode = curNode->next;
				free(curNode);
				curNode = preNode;
			}
		}
		table->size = 0;
		table->bucketUseCount = 0;
		table->bucketCount = 0;
	}
	if (NULL != table->address)
	{
		memset(table->address,0,table->bucketCount * sizeof(HashNodePtr));
	}
}
void EnumHashTable(HashTablePtr table,HashTableCallBack fnHashTable)
{
	
	HashNodePtr curNode;
	unsigned long i = 0;
	if (NULL != table)
	{
		for (i=0; i<table->bucketCount; i++)
		{
			//oldNodePtr = (PoolNodePtr*)pool->address + i;
			curNode = *((HashNodePtr*)table->address + i);//*oldNodePtr;
			while(NULL != curNode)
			{
				if (NULL != fnHashTable)
				{
					fnHashTable(table,curNode->keyValue);
				}
				curNode = curNode->next;
			}
		}
	}
}
void DestroyHashTable(HashTablePtr table)
{
	CleanHashTable(table);
	if (NULL != table)
	{
		if (NULL != table->address)
		{
			free(table->address);
		}
		free(table);
	}
}