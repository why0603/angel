/******************************************************************************************
 * @file hashtable.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-24
 * @descirption:哈希表
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_HASHTABLE_H
#define BELIAL_COLLECT_HASHTABLE_H
#include "typedef.h"
#include "define.h"

//哈希算法函数指针
typedef unsigned long (*HashCode)(char*,unsigned long);

//KEY比较函数指针
typedef Boolean (*KeyEqual)(void*,void*);

//数据存储方式
typedef enum
{
	//不存储
	NotSaveMode,
	
	//直接赋值
	DirectMode,

	//拷贝赋值
	CopyMode,
}DataSaveMode;

typedef struct __HashNode__
{

	//求出的HASH值
	unsigned long hashValue;

	//下一个节点
	struct __HashNode__* next;
	
	//我想value为整型的应用面要更大一些
	//这样除了兼容所有的整型类型，还可以在64位系统中充当指针的作用
	//而仅仅是在小数据量时，浪费一点点空间
	//__int64 value;

	//一直以来，迷惑于保存KEY值
	//现在觉得如果有两个不同HASH算法产出的值应该基本够了
	//大不了三个，网上找了找，居然真有人这样做了
	//这样还是有缺点的， 因为无法枚举KEY
	//__int64 key;
	//value指针,value不使用拷贝的形式来保存数据
	//void* value;
	//void* key;
	//
	//key-value  ,KEY和VALUE放一起
	char keyValue[];
} HashNode,*HashNodePtr;

typedef struct
{
	//当前哈希表中的元素数量
	unsigned long size;

	//桶数量
	unsigned long bucketCount;

	//桶使用量
	unsigned long bucketUseCount;

	//临界值->扩展比例,如果桶使用量超过这个比例，则扩容
	float raiseRate;

	//是否使用链表的方式,使用链表方式将额外造成一些空间上的负担
	//桶的使用率也会变低,这将要求桶的分布更加均匀，需要更好的哈希算法
	unsigned long useList;

	//key的字节长度,这个值为0表示为不定长字符串
	unsigned long keyByteSize;

	DataType keyType;

	//value的字节长度,这个值大于0则会在哈希表中创建内存，拷贝VALUE
	unsigned long valueByteSize;

	DataType valueType;

	//哈希值的调用，如果不指定，则使用默认哈希值算法
	HashCode hashCode;

	//key比较的算法,如果不提供此函数，则实际上类似字符串的值
	//与其他复杂结构体的比较，则无法验证
	KeyEqual keyEqual;
	
	//数组的起始地址
	void* address;
} HashTable,*HashTablePtr;

typedef void (*HashTableCallBack)(HashTablePtr table,char* keyValue);
#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 创建哈希表
 * @param capacity 哈希表的初始容量
 * @param keyByteSize key的字节数
 * @param valueByteSize value的字节数
 */
EXTERN HashTablePtr CreateHashTable(unsigned long capacity,unsigned long keyByteSize,unsigned long valueByteSize);

/**
 * 创建默认哈希表
 * 容量为64个桶，key,value的字节数都为计算机位数
 */
EXTERN HashTablePtr CreateDefaultHashTable();

/**
 * 哈希表中是否包含key
 * @param table 要查询的表
 * @param key 要查询的KEY
 */
EXTERN Boolean ContairHashTable(HashTablePtr table,void* key);


/**
 * 在哈希表中查询值,将值作为指针返回
 * @param table 要查询的表
 * @param key 要查询的KEY
 * @return 返回值所在的内存地址
 */
EXTERN void* GetHashTable(HashTablePtr table,void* key);

/** 
 * 在哈希表中查询值 值拷贝到VALUE中
 * @param table 要查询的表
 * @param key 要查询的KEY
 * @param value 保存值的内存地址
 */
EXTERN void GetFromHashTable(HashTablePtr table,void* key,void* value);

/**
 * 将数据添加到哈希表中
 * @param table 要查询的表
 * @param key 要查询的KEY
 * @param value 添加值的内存地址
 */
EXTERN void PutHashTable(HashTablePtr table,void* key,void* value);


/**
 * 将某个关键词从哈希表中删除
 * @param table 要查询的表
 * @param key 要删除的KEY
 */
EXTERN void RemoveHashTable(HashTablePtr table,void* key);

/**
 * 清理哈希表，将删除所有的桶
 * @param table 要查询的表
 */
EXTERN void CleanHashTable(HashTablePtr table);

/**
 * 销毁哈希表并释放所有内存
 * @param table 要查询的表
 */
EXTERN void DestroyHashTable(HashTablePtr table);

/**
 * 枚举哈希表，调用回调函数处理所有的数据
 */
EXTERN void EnumHashTable(HashTablePtr table,HashTableCallBack fnHashTable);
#ifdef __cplusplus
}//兼容C++
#endif
#endif