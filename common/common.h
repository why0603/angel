/******************************************************************************************
 * @file common.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013/6/6
 * @descirption: 一些公共函数
 *****************************************************************************************/
#ifndef BELIAL_COMMON_COMMON_H
#define BELIAL_COMMON_COMMON_H
#include "typedef.h"
#include "define.h"



#ifdef __cplusplus
extern "C" {	//兼容C++
#endif


/**
 * 获取数据的字节数
 */
unsigned long GetByteSizeOfData(DataType dataType,void* data);

/**
 * 比较两个数据的大小,将根据len的长度做强制类型转化(在原始拷贝中存在字节序列倒序的问题)
 * 1个字节将根据char类型比较
 * 2个字节将根据short类型比较
 * 4个字节将根据long类型比较
 * 8个字节将根据__int64类型比较
 * 0个字节将根据以\0结束的字符串方式比较strcmp
 * 其他字节数将直接使用内存比较memcmp
 * @param data1 第一个比较数
 * @param data2 第二个比较数
 * @param len 比较双方的长度，如果是\0结尾的字符,则使用0
 * @param isSigned True 有符号  False 无符号
 * @return data1>data2=1 data1=data2=0 data1<data2=-1
 */
EXTERN CompareResult Compare(void* data1,void* data2,DataType dataType,unsigned long len,Boolean isSigned);

/**
 * 将src的数据拷贝到dest中
 * @param dest 目标内存地址
 * @param src 来源内存地址
 * @len 数据长度
 * 拷贝的类型规则同上
 */
EXTERN void CopyData(void* dest,void* src,DataType dataType,unsigned long len);
#ifdef __cplusplus
}
#endif
#endif