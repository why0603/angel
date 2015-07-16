/******************************************************************************************
 * @file list.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-5-20
 * @descirption:链表，每加入一个元素，则需要重新开辟一次元素空间
 * 加入的时候需要malloc一段内存，实际上已经偏慢
 * 而因为没有使用十字链表，其实删除的速度是更加的慢
 * 因为每次都需要从第一个节点开始往后走，如果每次都是删除最后一个节点，则相当遍历整个链表
 * 他的好处就是扩容的时候不需要去复制表内容,而INSERT，DELETE,SEARCH都是相当的慢
 * 如果是要随机读取表中内容，并且需要频繁根据序号来删除数据，那还是用array 
 * 只是对删除有特殊要求，则可以稍微改改为双向（十字）链表
 * 作为stack,queue则单向链表足够了
 * 实践证明，这种链表的效率不是一般的低，特别是大量的malloc后的回收，简直就是在扯淡
 * 所以，还是少用的好
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_LIST_H
#define BELIAL_COLLECT_LIST_H
#include "typedef.h"
#include "define.h"

typedef struct __list_node
{
	//下一个节点的指针
	struct __list_node* next;
	//包含的数据，这种数组并不占用结构体的空间,0 bytes
	//如果要保存的只是字符串的指针，则需要将insert的数据参数定为指针的指针
	char data[];
}ListNode,*ListNodePtr;

//
typedef struct
{
	//当前的元素个数
	unsigned long size;

	//单个元素的字节数
	unsigned long elementByteSize;

	//数据是否有符号,默认为有符号数值
	Boolean isSigned;

	//数据类型
	DataType dataType;

	//指向链表第一个元素的节点
	ListNodePtr head;

	//指向链表最后一个元素的节点
	ListNodePtr tail;
}List,*ListPtr;

//由于链表从索引获取数据的方式效率十分低下
//所以提供一个回调方法，用于每次枚举所有的链表
//调用方可以编写一个回调函数用来处理每个节点
typedef void (*ListCallBack)(ListPtr list,ListNodePtr node);


#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 创建一个默认链表，链表将存储CPU字长的数据
 * @param capacity 数组的初始化容量
 * @return 返回链表指针
 */
EXTERN	ListPtr CreateDefaultList();

/**
 * 创建一个指定数据长度链表
 * @param elementByteSize 链表存储的数据长度
 * @return 返回链表指针
 */
EXTERN	ListPtr CreateList(DataType dataType);
/**
 * 插入数据到链表头部
 * @param list 数据要插入的链表
 * @data 数据
 */
EXTERN	void InsertListHead(ListPtr list,void* data);

/**
 * 插入数据到链表尾部
 * @param list 数据要插入的链表
 * @data 数据
 */
EXTERN	void InsertList(ListPtr list,void* data);

//排序插入,从小到大排序插入数据
EXTERN void InsertListSort(ListPtr list,void* data);
/**
 * 插入连续的批量数据到链表头部
 * @param list 数据要插入的链表
 * @data 数据
 * @count 数据的条数
 */
EXTERN	void InsertMoreListHead(ListPtr list,void* data,unsigned long count);


/**
 * 插入连续的批量数据到链表尾部
 * @param list 数据要插入的链表
 * @data 数据
 * @count 数据的条数
 */
EXTERN	void InsertMoreList(ListPtr list,void* data,unsigned long count);

/**
 * 将两个链表链接起来
 * @param dest 目标链表
 * @src 将会失效的链表，不能对此链表做任何内存释放的操作，否则会影响dest
 */
EXTERN	void InsertListToList(ListPtr dest,ListPtr src);

/**
 * 将两个链表链接起来,src链表将在dest链表的前面
 * @param dest 目标链表
 * @src 将会失效的链表，不能对此链表做任何内存释放的操作，否则会影响dest
 */
EXTERN	void InsertListToListHead(ListPtr dest,ListPtr src);

/**
 * 获取数据在链表中的位置,这种查找之后其实并没有多大意义，
 * 链表最好不要有序号相关的操作
 * @param list 链表
 * @param data 要查找的数据
 * @return 返回此数据的序号，如果不存在，则返回-1 
 */
EXTERN	int IndexOfList(ListPtr list,void* data);

/**
 * 获取指定序号的数据，这种方法的效率那是相当的低
 * 链表获取数据的方式，最好还是从list->head开始往下查找next = NULL的节点
 * @param list 链表
 * @param data 数据存放位置
 * @param index 取指定序号
 */
EXTERN	void GetDataFromList(ListPtr list,void* data,unsigned long index);

/** 
 * 删除头节点，并返回数据
 * @param list 链表
 * @param data 数据存放位置
 */
EXTERN	void RemoveListHead(ListPtr list,void* data);

/** 
 * 删除尾节点，并返回数据，需要重新定位尾部节点
 * @param list 链表
 * @param data 数据存放位置
 */
EXTERN	void RemoveListTail(ListPtr list,void* data);


/** 
 * 删除包含指定数据的节点
 * @param list 链表
 * @param data 要被删除的数据
 */
EXTERN	void RemoveDataList(ListPtr list,void* data);

/**
 * 清除所有数据和节点,并释放节点占用的内存
 * @param list 链表
 */
EXTERN	void CleanList(ListPtr list);

/**
 * 销毁链表，释放所有内存
 */
EXTERN	void DestroyList(ListPtr list);

/**
 * 枚举链表所有节点并执行回调函数处理单个节点
 */
EXTERN void EnumListNode(ListPtr list,ListCallBack fnList);

//--------------------------------------------------------
/**
 * 将数据压入栈
 * @param list 数据要压入的链表
 * @data 数据
 */
EXTERN	void PushStack(ListPtr list,void* data);


/**
 * 将数据压入栈
 * @param list 数据要压入的链表
 * @data 数据
 * @count 数据的条数
 */
EXTERN	void PushAllStack(ListPtr list,void* data,unsigned long count);

/** 
 * 数据出栈
 * @param list 链表
 * @data 数据存储的位置
 */
EXTERN	void PopStack(ListPtr list,void* data);


/** 
 * 所有数据出栈
 * @param list 链表
 */
EXTERN	void PopAllStack(ListPtr list);

//---------------------------------------------------栈操作
//---------------------------------------------------队列操作
/**
 * 将数据推进队列
 * @param list 链表
 * @data 数据
 */
EXTERN	void OfferQueue(ListPtr list,void* data);

/**
 * 获取和移除数据
 * @param list 链表
 * @data 数据存放位置
 */
EXTERN	void PollQueue(ListPtr list,void* data);

/**
 * 获取但是不移除数据
 * @param list 链表
 * @data 数据存放位置
 */
EXTERN	void PeekQueue(ListPtr list,void* data);

//-----------------------------------------------队列操作结束




#ifdef __cplusplus
}//兼容C++
#endif
#endif