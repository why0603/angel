/******************************************************************************************
 * @file array.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-15
 * @descirption:动态数组
 * 可以自动添加长度的数组
 * 元素长度固定
 * 可以根据索引访问，每次容量满了之后，需要分配新的空间
 * 新的内存空间必须大于之前的内存空间，并且复制原数组的所有数据
 * 使用这种数组，最好初始化的时候就知道数据的最大值，并且都是顺序操作
 * 很不利于数据的删除，如果有很多删除操作，这会导致效率十分的低
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_ARRAY_H
#define BELIAL_COLLECT_ARRAY_H
#include "typedef.h"
#include "define.h"

//
typedef struct
{
	//初始化大小,这个容量是指所能装载的元素个数
	//unsigned long initCapacity;

	//数组的当前大小，表示所能装载的元素个数
	unsigned long capacity;

	//当前的元素个数
	unsigned long size;

	//单个元素的字节数
	unsigned long elementByteSize;

	//扩容次数
	unsigned long raiseCount;

	//是否变长类型
	//变长类型则只能保存其指针
	DataType dataType;

	//数据是否有符号,默认为有符号数值
	Boolean isSigned;

	//数组的起始地址
	void* address;
}Array,*ArrayPtr;


typedef enum
{
	//索引超出范围
	Error_Index_Range = -1,
	//内存分配失败
	Error_No_Memory = -2,
}ArrayError;

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 创建一个动态数组
 * @param capacity 数组的初始化容量
 * @param elementByteSize 每个数组元素占用的字节量
 * @param raiseRate 扩容因子
 * @return 返回动态数组指针
 */
EXTERN	ArrayPtr CreateArray(unsigned long capacity,unsigned long elementByteSize);

/**
 * 创建一个数组，使用默认参数，数组大小为20，增长因子为0.66
 * @param elementByteSize 每个数组元素占用的字节量
 * @return 返回动态数组指针
 */
EXTERN	ArrayPtr CreateDefaultArray();

/**
 * 将数据添加到动态数组末尾
 * @param array 数组指针
 * @param data 数据指针
 */
EXTERN	void InsertArray(ArrayPtr array,void* data);

/**
 * 将数据添加到动态数组，并且指定索引，如果索引超过当前数组大小，则不会添加成功
 * @param array 数组指针
 * @param data 数据指针
 * @param index 指定的数组索引
 * @return 添加成功返回True,否则返回False
 */
EXTERN	Boolean InsertArrayIndex(ArrayPtr array,void* data,unsigned long index);

/**
 * 将多条连续的数据添加到动态数组，并且指定索引，如果索引超过当前数组大小，则不会添加成功
 * @param array 数组指针
 * @param data 数据指针
 * @param count 数据个数
 * @param index 指定的数组索引
 * @return 添加成功返回True,否则返回False
 */
EXTERN	Boolean InsertMoreArrayIndex(ArrayPtr array,void* data,unsigned long count,unsigned long index);

/**
 * 将一个动态数组的数据全部添加到另一个动态数组，并且指定索引，如果索引超过当前数组大小，则不会添加成功
 * @param dest 目标数组
 * @param destIdx 需要插入数据的目标数组位置
 * @param src 来源数组
 * @param count 来源数组需要被插入懂啊目标数组的数据个数
 * @param srcIdx 来源数组索引
 * @return 添加成功返回True,否则返回False
 */
EXTERN	Boolean InsertArrayToArray(ArrayPtr dest,unsigned long destIdx,ArrayPtr src,unsigned long count,unsigned long srcIdx);

/**
 * 获取动态数组指定索引的数据
 * @param array 动态数组
 * @param data 存放数据的指针
 * @param index 指定索引
 * @return 成功返回True,否则返回False
 */
EXTERN	Boolean GetDataFromArray(ArrayPtr array,void* data,unsigned long index);

/**
 * 获取动态数组指定索引的数据
 * @param array 动态数组
 * @param index 指定索引
 * @return 直接返回数据在内存中的地址
 */
EXTERN	void* GetDataArray(ArrayPtr array,unsigned long index);
/**
 * 获取动态数组指定数据的索引，将从0索引开始查找，要是没有发现数据，返回-1
 * @param array 动态数组
 * @param data 要查找的数据
 * @return 成功返回数据的索引,否则返回-1
 */
EXTERN	int IndexOfArray(ArrayPtr array,void* data);

/**
 * 删除动态数组指定索引的数据，并将数据保存在data中，data如果为NULL，则不做处理
 * @param array 动态数组
 * @param data 被删除的数据
 * @param index 被删除数据的索引
 * @return 成功返回数据的索引,否则返回-1
 */
EXTERN	Boolean RemoveArrayIndex(ArrayPtr array,void* data,unsigned long index);


/**
 * 删除动态数组中指定索引的多条数据，index = 3,删除count=5条，则索引为3,4,5,6,7的5条数据将被删除
 * @param array 动态数组
 * @param data 被删除的数据
 * @param count 被删除数据条数
 * @param index 被删除数据的索引
 * @return 成功返回数据的索引,否则返回-1
 */
EXTERN	Boolean RemoveArrayMoreIndex(ArrayPtr array,unsigned long count,unsigned long index);


/**
 * 删除动态数组中指定的数据,如果没有找到此数据则会删除失败
 * @param array 动态数组
 * @param data 需要删除的数据
 * @return 成功返回数据的索引,否则返回-1
 */
EXTERN	Boolean RemoveDataArray(ArrayPtr array,void* data);


/**
 * 删除动态数组中指定的多条数据,如果没有找到此数据则会删除失败
 * @param array 动态数组
 * @param data 需要删除的数据
 * @return 成功返回数据的索引,否则返回-1
 */
EXTERN	void RemoveMoreDataArray(ArrayPtr array,void* data,unsigned long count);

/**
 * 获取动态数组的子数组
 * @param array 动态数组
 * @param count 要获取的子数组数据条数
 * @param index 子数组在父数组中指定的索引
 * @return 返回子数组的指针
 */
EXTERN	ArrayPtr SubArray(ArrayPtr array,unsigned long count,unsigned long index);

/**
 * 清理动态数组的所有数据
 * @param array 被清理的数组
 */
EXTERN	void CleanArray(ArrayPtr array);

/**
 * 销毁数组并释放所有内存
 * 如果数组中包含有堆上分配的指针，不会释放
 */
EXTERN	void DestroyArray(ArrayPtr array);

EXTERN	void SortArray(ArrayPtr array);

#ifdef __cplusplus
}//兼容C++
#endif
#endif