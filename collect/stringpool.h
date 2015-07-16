/******************************************************************************************
 * @file stringpool.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-29
 * @descirption:字符串池,用于保存所有的字符串，保证一个相同的字符串只有一个实例
 * 字符串池会将输入的字符串或者字节数据组全部拷贝到堆上留一个备份，方便以后使用
 * 支持四种字符串格式
 * 1:以\0结尾的普通字符串
 * 2:以\0\0结尾的字符串
 * 3:前两个字节为字符串长度的字符串
 * 4:前四个字节为字符串长度的字符串
 * 5:无格式表示的字符串
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_STRINGPOOL_H
#define BELIAL_COLLECT_STRINGPOOL_H
#include "typedef.h"
#include "define.h"


//哈希算法函数指针
typedef unsigned long (*HashCode)(char*,unsigned long);

//输出格式
typedef enum
{
	//以\0结束字符串
	Char_Format = 1,
	//以\0\0结束字符串
	Wchar_Format = 2,
	//以两字节短整型表示长度开始的字符串
	Short_Head_Format = 3,
	//以四字节整型表示长度开始的字符串
	Int_Head_Format = 4
}StringFormat;

typedef struct
{
	//当前池中的元素数量
	unsigned long size;

	//池容量
	unsigned long modCount;

	//桶使用量
	unsigned long modUseCount;

	HashCode hashCode1;

	HashCode hashCode2;

	HashCode hashCode3;
	//格式
	//StringFormat fomrat;

	//数组的起始地址
	void* address;
} StringPool,*StringPoolPtr;


//枚举字符串池的时候的回调函数
//获取的数据格式为：
//4字节字符串长度 + 字符串 + 4字节\0结束符
typedef void (*StringPoolCallBack)(StringPoolPtr pool,char* key);

//像这种POOL最好一个应用中只有一个实例
//StringPool stringPool;
#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 创建一个字符串池，并指定初始化池的大小
 * @param capacity 字符串池的容量
 * @return 返回字符串池指针
 */
EXTERN StringPoolPtr CreateStringPool(unsigned long capacity);

/**
 * 创建一个总容量为16的默认字符串池
 * @return 返回字符串池指针
 */
EXTERN StringPoolPtr CreateDefaultStringPool();

/**
 * 向字符串池中写入数据
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 */
EXTERN char* PutStringPool(StringPoolPtr pool,char* key,unsigned long len);

/**
 * 向字符串池中写入字符串，字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 */
EXTERN char* PutConstStringPool(StringPoolPtr pool,const char* key);

//EXTERN void PutUnicodeToStringPool(StringPoolPtr pool,const wchar_t* key);


/**
 * 获取字符串在字符串池中的指针,要是池中不存在此字符串则需要额外调用put方法
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 */
EXTERN char* GetStringPool(StringPoolPtr pool,char* key,unsigned long len);


/**
 * 向字符串池中写入字符串，字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @return 获取字符串在池中的指针
 */
EXTERN char* GetConstStringPool(StringPoolPtr pool,char* key);

/**
 * 检查字符串是否已经在字符串池中
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 * @return 包含则返回True,否则返回False
 */
EXTERN Boolean ContairStringPool(StringPoolPtr pool,char* key,unsigned long len);

/**
 * 检查字符串是否已经在字符串池中 字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @return 包含则返回True,否则返回False
 */
EXTERN Boolean ContairConstStringPool(StringPoolPtr pool,char* key);

/**
 * 删除字符串池中包含的字符串
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 * @param len 字符串长度
 */
EXTERN void RemoveStringPool(StringPoolPtr pool,char* key,unsigned long len);

/**
 * 删除字符串池中包含的字符串 字符串以\0结尾
 * @param pool 被操作的字符串池
 * @param key 操作的字符串
 */
EXTERN void RemoveConstStringPool(StringPoolPtr pool,char* key);

/**
 * 清理所有的字符串池中的数据
 * @param pool 被操作的字符串池
 */
EXTERN void CleanStringPool(StringPoolPtr pool);

/**
 * 销毁字符串池，并释放所有空间
 */
EXTERN void DestroyStringPool(StringPoolPtr pool);

/** 
 * 使用回调函数处理所有的节点数据
 * 数据的格式：前面4个字节表示字符串长度 +　字符串　＋四个字节\0
 */
EXTERN void EnumStringPool(StringPoolPtr pool,StringPoolCallBack fnStringPool);
#ifdef __cplusplus
}//兼容C++
#endif
#endif