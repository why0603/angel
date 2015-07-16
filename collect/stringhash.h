/******************************************************************************************
 * @file StringHash.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-29
 * @descirption:字符串哈希表
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_STRINGHASH_H
#define BELIAL_COLLECT_STRINGHASH_H
#include "typedef.h"
#include "define.h"
#include <stdlib.h>
#include <string.h>


//哈希算法函数指针
typedef unsigned long (*StringHashCode)(char*,unsigned long);

typedef struct __string_hash_node__
{
	//hash1 + hash2 用于区别出两个不同的字符串
	//三个字符串产出的哈希值一样的可能性是如此的小
	unsigned long hash1;

	//这个哈希值额外使用于index
	unsigned long hash2;

	//字符串长度
	unsigned long length;

	//字符串的值,以\0\0结束的字符串
	char value[];
} StringHashNode,*StringHashNodePtr;

typedef struct
{
	//当前池中的元素数量
	unsigned long size;

	//池容量
	unsigned long modCount;

	StringHashCode hashCode1;

	StringHashCode hashCode2;
	//格式
	//StringFormat fomrat;

	//数组的起始地址
	void* address;
} StringHash,*StringHashPtr;


//枚举字符串池的时候的回调函数
//获取的数据格式为：
//4字节字符串长度 + 字符串 + 4字节\0结束符
typedef void (*StringHashCallBack)(StringHashPtr stringHash,char* key);

//像这种POOL最好一个应用中只有一个实例
//StringHash StringHash;
#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 创建一个字符串池，并指定初始化池的大小
 * @param capacity 字符串池的容量
 * @return 返回字符串池指针
 */
EXTERN StringHashPtr CreateStringHash(unsigned long capacity);

/**
 * 创建一个总容量为16的默认字符串池
 * @return 返回字符串池指针
 */
EXTERN StringHashPtr CreateDefaultStringHash();

/**
 * 向字符串池中写入数据
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 */
EXTERN long PutStringHash(StringHashPtr stringHash,char* key,unsigned long len);

/**
 * 向字符串池中写入字符串，字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 */
EXTERN long PutConstStringHash(StringHashPtr stringHash,const char* key);


EXTERN long PutConstUnicodeStringHash(StringHashPtr stringHash,const wchar_t* key);


/**
 * 获取字符串在字符串池中的指针,要是池中不存在此字符串则需要额外调用put方法
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 */
EXTERN char* GetStringHash(StringHashPtr stringHash,char* key,unsigned long len);


/**
 * 向字符串池中写入字符串，字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @return 获取字符串在池中的指针
 */
EXTERN char* GetConstStringHash(StringHashPtr stringHash,char* key);

/**
 * 检查字符串是否已经在字符串池中
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 * @return 包含则返回True,否则返回False
 */
EXTERN Boolean ContairStringHash(StringHashPtr stringHash,char* key,unsigned long len);

/**
 * 检查字符串是否已经在字符串池中 字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @return 包含则返回True,否则返回False
 */
EXTERN Boolean ContairConstStringHash(StringHashPtr stringHash,char* key);

/**
 * 删除字符串池中包含的字符串
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 */
EXTERN void RemoveStringHash(StringHashPtr stringHash,char* key,unsigned long len);

/**
 * 删除字符串池中包含的字符串 字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 */
EXTERN void RemoveConstStringHash(StringHashPtr stringHash,char* key);

/**
 * 清理所有的字符串池中的数据
 * @param pool 被操作的字符串池
 */
EXTERN void CleanStringHash(StringHashPtr stringHash);

/**
 * 销毁字符串池，并释放所有空间
 */
EXTERN void DestroyStringHash(StringHashPtr stringHash);

/** 
 * 使用回调函数处理所有的节点数据
 * 数据的格式：前面4个字节表示字符串长度 +　字符串　＋四个字节\0
 */
EXTERN void EnumStringHash(StringHashPtr stringHash,StringHashCallBack fnStringHash);
#ifdef __cplusplus
}//兼容C++
#endif
#endif