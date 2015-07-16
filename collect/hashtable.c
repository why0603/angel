/******************************************************************************************
* @file hashtable.c
* @anthor:Croky.Zheng
* @CreateTime:2013-5-24
* @descirption:哈希表
* 有时候觉得在C环境下做这些确实是件困难的事，因为虽然是一个容器
* 却还涉及到内存管理，数据类型处理等等方面
* 是要拷贝KEY，还是要拷贝VALUE，还是两者都不拷贝
* 当然，现在也发现为什么JAVA的一些容器操作速度会快过C语言的
* 算法上大家可优化的范围真的很小，很多时候C上面花了太多的时间在
* 给数据分配内存，拷贝数据，释放内存上，而JAVA，这些开销显然都放到了创建对象
* 和JAVA的垃圾回收线程上
*****************************************************************************************/

#include "hashtable.h"
#include "hash.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

//使用乘法来做哈希,则需要(hash * 0.618) - (long)(hash * 0.618)
//这样获取的小数部分再去乘以HASH表的桶数，能得到更好的散列分布
#define KNUTH	0.6180339887 //HASH乘法取值中的关键值
//最小区域大小
#define MINICAPACITY	64
#define DEFAULT_RAISE_RATE 0.75f

#define HASHSUB(x) (((x) ^ ((x) >> 20) ^ ((x) >> 12))
#define HASH(x) HASHSUB(x) ^ (HASHSUB(x) >> 7) ^ (HASHSUB(x) >> 4)

//为哈希表扩容
static void EnsureCapacity(HashTablePtr table);

//创建一个新的哈希表节点
static HashNodePtr CreateHashNode(HashTablePtr table,void* key,void* value);

//移动节点
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

	//是4的倍数
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
		//是4的倍数
		switch (table->valueByteSize)
		{
			//value 应该支持没有数据的方式
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
			//获取节点应该在的索引位置
			index = IndexFor(Hash(curNode->hashValue),newCapacity);

			//获取新表的index节点
			newNodePtr = (HashNodePtr*)newAddress + index;

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

void EnsureCapacity(HashTablePtr table)
{
	//每次扩大一倍
	unsigned newCapacity = table->bucketCount << 1;
	unsigned long i = 0,index = 0;
	//void* newAddress = malloc((sizeof(HashNode) + table->keyByteSize) * newCapacity);
	void* newAddress = calloc(newCapacity,sizeof(HashNodePtr));
	if (NULL != newAddress)
	{
		//calloc会将内存清0,不需要调用memset方法
		//memset(newAddress,0,sizeof(HashNodePtr) * newCapacity);
		if (NULL != table->address)
		{
			MoveNode(table,newAddress,newCapacity);
			free(table->address);
		}
		table->address = newAddress;
		table->bucketCount = newCapacity;
		//桶数量
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


		//桶使用量超过75%，则需要扩容
		table->raiseRate = DEFAULT_RAISE_RATE;

		table->bucketCount = initCapacity >> 1;
		//分配地址空间
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
		if((curNode->hashValue == hash) && //hash值相同
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
			if((oldNode->hashValue == hash) && //hash值相同
				(memcmp(key,oldNode->keyValue,table->keyByteSize) == 0) && //值相同
				(table->valueByteSize > 0) && //值类型不会空
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
			//新节点为元素链表的头节点
			newNode->next = *oldNodePtr;
			//增加一个桶使用量
			if (NULL == *oldNodePtr)
			{
				table->bucketUseCount += 1;
			}
			*oldNodePtr = newNode;
			//增加一个元素
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
		if((oldNode->hashValue == hash) && //hash值相同
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
			//变成了空指针
			if (NULL == *oldNodePtr)
			{
				//空出一个桶
				table->bucketUseCount--;
			}
			free(oldNode);
			//元素-1
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