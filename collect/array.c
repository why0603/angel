/******************************************************************************************
* @file array.c
* @anthor:Croky.Zheng
* @CreateTime:2013-5-15
* @descirption:动态数组
* 可以自动添加长度的数组
* 元素长度固定
* 可以根据索引访问，每次容量满了之后，需要分配新的空间
* 新的内存空间必须大于之前的内存空间，并且复制原数组的所有数据
* 使用这种数组，最好初始化的时候就知道数据的最大值，并且都是顺序操作
* 很不利于数据的删除，如果有很多删除操作，这会导致效率十分的低
* 为了实现方便，将不做太多的内存分配和超越索引检测
*****************************************************************************************/

#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <common.h>

//默认列表的元素个数
#define MIN_COUNT	20

//递归排序数，超过这个SIZE的数组，不适用递归排序
#define RECURSIVE_SORT_NUM 10000

//当数组元素超过这个比例，则扩大数组
#define DEFAULT_RAISE_RATE	0.5f

//static ArrayPtr CreateSelfArray(unsigned long capacity,unsigned long size,float raiseRate);

//检查索引是否超出了数组所能表示的最大范围
//如果超过则重新分配数组的内存
static void EnsureCapacity(ArrayPtr array,unsigned count,unsigned index);

//为数组扩容，capacity为新的大小,在index的地方空余出count条数据
static void ExpandArray(ArrayPtr array,unsigned long capacity,unsigned count,unsigned index);

//设置数组值 
//数组，数据，索引
static void SetData(ArrayPtr array,void* data,unsigned long count,unsigned long index);

//在动态数组内部移动数据
static void MoveData(void* dest,unsigned long destIdx,void* src,unsigned long srcIdx,unsigned long count,unsigned long byteSize);

//比较两个数的值，index1==index2=0，index1>index2=1,index1<index=-1
static CompareResult CompareIndex(ArrayPtr array,unsigned long index1,unsigned long index2);

static CompareResult CompareData(ArrayPtr array,void* data1,void* data2);

static void Swap(ArrayPtr array,unsigned long index1,unsigned long index2);

static void RecursiveSort(ArrayPtr array,unsigned long low,unsigned long high);

static void IterationSort(ArrayPtr array,unsigned long low,unsigned long high);

void Swap(ArrayPtr array,unsigned long index1,unsigned long index2)
{
	void* tmp;
	unsigned long offset1 = index1 * array->elementByteSize;
	unsigned long offset2 = index2 * array->elementByteSize;
	if (index1 != index2)
	{
		if (array->size < array->capacity)
		{
			tmp = (char*)array->address + (array->capacity - 1) * array->elementByteSize;
		}
		else
		{
			tmp = malloc(array->elementByteSize);
		}
		memcpy(tmp,(char*)array->address + offset1,array->elementByteSize);
		memcpy((char*)array->address + offset1,(char*)array->address + offset2,array->elementByteSize);
		memcpy((char*)array->address + offset2,tmp,array->elementByteSize);
		if (array->size < array->capacity)
		{
			memset((char*)array->address + array->capacity - 1,0,array->elementByteSize);
		}
		else
		{
			free(tmp);
		}
	}
}

CompareResult CompareData(ArrayPtr array,void* data1,void* data2)
{
	//这样表示是字符串,而且是以\0结束的字符串
	/*
	if ((array->dataType == Chars_Type) && (array->elementByteSize == sizeof(void*)))
	{
		//return Compare(*(char**)data1,*(char**)data2,Chars_Type,array->isSigned);
		return (CompareResult)strcmp(*(char**)data1,*(char**)data2);
	}
	else if ((array->dataType == Unicode_Type) && (array->elementByteSize == sizeof(void*)))
	{
		return (CompareResult)wcscmp(*(wchar_t**)data1,*(wchar_t**)data2);
	}
	else
	{
		return Compare(data1,data2,array->elementByteSize,array->isSigned);
	}
	*/
	return Compare(data1,data2,array->dataType,array->elementByteSize,array->isSigned);
}

CompareResult CompareIndex(ArrayPtr array,unsigned long index1,unsigned long index2)
{
	CompareResult ret = Compare_Equal;
	void* data1 = (char*)array->address + index1;
	void* data2 = (char*)array->address + index2;
	if (index1 != index2)
	{
		ret = CompareData(array,data1,data2);
	}
	return ret;
}

void MoveData(void* dest,unsigned long destIdx,void* src,unsigned long srcIdx,unsigned long count,unsigned long byteSize)
{
	//cpu字宽
	//unsigned long w = GetCpuWordSize() / 4;
	//最多需要执行多少次CPU字宽
	//unsigned long loop = count / w;
	char* dest_ptr = (char*)dest + destIdx * byteSize;
	char* src_ptr = (char*)src + srcIdx * byteSize;
	unsigned long offsetSize = count * byteSize;
	char* dest_last = dest_ptr + offsetSize;
	char* src_last = src_ptr + offsetSize;
	//目标地址和源地址不同,可能来自不同的数组
	if (dest != src)
	{
		//直接复制,memcpy为内联方式,并且有自身的优化
		memcpy(dest_ptr,src_ptr,offsetSize);
	}
	else //同一个数组
	{
		if (dest_ptr < src_ptr)
		{
			//dest 与 src 不相交
			if (dest_last < src_ptr)
			{
				memcpy(dest_ptr,src_ptr,offsetSize);
			}
			else //dest与src相交
			{
				//这里可以根据CPU字长优化,只能在insert操作中来避免这种操作,而delete时,则无法躲避
				while (dest_ptr < dest_last)
				{
					*dest_ptr++ = *src_ptr++;
				}
			}
		}
		else
		{
			//src 与 dest不相交
			if(src_last < dest_ptr)
			{
				memcpy(dest_ptr,src_ptr,offsetSize);
			}
			else //src与dest相交
			{
				while (dest_ptr < dest_last)
				{
					*(--dest_last) = *(--src_last);
				}
			}
		}
	}
}

void SetData(ArrayPtr array,void* data,unsigned long count,unsigned long index)
{
	unsigned long loop = 0;
	if (index < array->capacity)
	{
		//是4的倍数
		switch (array->elementByteSize)
		{
		case Char_Type:
			for (loop = 0;loop < count; loop ++)
			{
				*((char*)array->address + index + loop) = *((char*)data + loop);
			}
			break;
		case Short_Type:
			for (loop = 0;loop < count; loop ++)
			{
				*((short*)array->address + index + loop) = *((short*)data + loop);
			}
			break;
		case Long_Type:
			for (loop = 0;loop < count; loop ++)
			{
				*((long*)array->address + index + loop) = *((long*)data + loop);
			}
			break;
		case LongLong_Type:
			for (loop = 0;loop < count; loop ++)
			{
				*((long long*)array->address + index + loop) = *((long long*)data + loop);
			}
			break;
		default:
			memcpy((char*)array->address + (index * array->elementByteSize),data,(array->elementByteSize * count));
			break;
		}
	}
}

void EnsureCapacity(ArrayPtr array,unsigned count,unsigned index)
{
	//新的数组长度
	unsigned long newCapacity = 0;
	//老的数组长度
	unsigned long oldCapacity = array->capacity;
	unsigned long size = array->size + count;
	if (size > oldCapacity)
	{
		newCapacity = (oldCapacity * 3) / 2 + 1;

		if (newCapacity < size)
		{
			newCapacity = size;
		}
		ExpandArray(array,newCapacity,count,index);
	}
}

void ExpandArray(ArrayPtr array,unsigned long capacity,unsigned count,unsigned index)
{
	void* newAddress = NULL;
	if (capacity > array->capacity)
	{
		//这里分配新地址
		newAddress = calloc(array->elementByteSize,capacity);
		if ((NULL != newAddress))
		{
			array->capacity = capacity;
			if (index == array->size)
			{
				memcpy(newAddress,array->address,array->elementByteSize * array->size);
			}
			else
			{
				//拷贝index之前的数据
				MoveData(newAddress,
					0,
					array->address,
					0,
					index,
					array->elementByteSize
					);
				//拷贝index之后的数据
				MoveData(newAddress,
					index + count,
					array->address,
					index,
					array->size - index,
					array->elementByteSize
					);
			}
			//这里释放老地址
			free(array->address);
			array->address = newAddress;
			array->raiseCount++;
		}
	}
}


ArrayPtr CreateArray(unsigned long capacity,unsigned long elementByteSize)
{
	ArrayPtr array = (ArrayPtr)calloc(1,sizeof(Array));
	if (NULL != array)
	{
		if (capacity == 0)
		{
			array->capacity = MIN_COUNT;
		}
		else
		{
			array->capacity = capacity;
		}
		/*
		if (dataType == Unknow_Type || dataType == Pointer_Type || dataType == Chars_Type) //指针类型特别处理
		{
			array->elementByteSize = sizeof(void*);
			//elementByteSize = sizeof(void*);//GetCpuWordSize(NULL,NULL) / 4;
		}
		else
		{
			array->elementByteSize = dataType;
		}
		*/
		if (elementByteSize == 0)
		{
			array->elementByteSize = sizeof(void*);
		}
		else
		{
			array->elementByteSize = elementByteSize;
		}
		//=0.0f表示自动扩容
		array->size = 0;
		array->raiseCount = 0;
		//默认情况下动态数组保存的是等长数据
		array->dataType = (DataType)array->elementByteSize;
		array->isSigned = True;
		array->address = calloc(array->elementByteSize,array->capacity);
	}
	return array;
}

//创建动态数组
ArrayPtr CreateDefaultArray()
{
	return CreateArray(MIN_COUNT,sizeof(void*));
}

void InsertArray(ArrayPtr array,void* data)
{
	if ((NULL != array) && (NULL != data))
	{
		if (array->capacity <= array->size)
		{
			EnsureCapacity(array,1,array->size);
		}
		//以0结尾的变长类型
		/*
		if (array->dataType == Chars_Type)
		{
		*((char**)array->address + array->size) = (char*)malloc(strlen((char*)data));
		if (NULL != *((char**)array->address + array->size))
		{
		memcpy(*((char**)array->address + array->size),data,strlen((char*)data));
		}
		}
		else
		{
		memcpy((char*)array->address + array->elementByteSize * array->size,data,array->elementByteSize);
		}
		*/
		//memcpy((char*)array->address + array->elementByteSize * array->size,data,array->elementByteSize);
		SetData(array,data,1,array->size);
		array->size++;
	}
}

Boolean InsertArrayIndex(ArrayPtr array,void* data,unsigned long index)
{
	return InsertMoreArrayIndex(array,data,1,array->size);
}

Boolean InsertMoreArrayIndex(ArrayPtr array,void* data,unsigned long count,unsigned long index)
{
	Boolean ret = False;
	unsigned long maxIndex = array->size;
	if ((NULL != array) && (NULL != data) && (index <= array->size) && (count > 0))
	{
		//防止index 在当前数组最大数值
		EnsureCapacity(array,count,index);
		SetData(array,data,count,index);
		array->size += count;
		ret = True;
	}
	return ret;
}

Boolean InsertArrayToArray(ArrayPtr dest,unsigned long destIdx,ArrayPtr src,unsigned long count,unsigned long srcIdx)
{
	Boolean ret = False;
	if ((NULL != dest) && (NULL != dest->address) && (destIdx <= dest->size) && 
		(NULL != src) && (NULL != src->address) && (count > 0) && (srcIdx + count <= src->size) && 
		(dest->elementByteSize == src->elementByteSize))
	{
		ret = InsertMoreArrayIndex(dest,(char*)src->address + srcIdx * src->elementByteSize,count,destIdx);
	}
	return ret;
}

Boolean GetDataFromArray(ArrayPtr array,void* data,unsigned long index)
{
	unsigned long offset = 0;
	Boolean ret = False;
	if ((NULL != array) && (NULL != array->address) && (index < array->size) && (NULL != data))
	{
		offset = index * array->elementByteSize;
		memcpy(data,(char*)array->address + offset,array->elementByteSize);
		ret = True;
	}
	return ret;
}

void* GetDataArray(ArrayPtr array,unsigned long index)
{
	if (NULL != array)
	{
		return (char*)array->address + (index * array->elementByteSize);
	}
	return NULL;
}

//获取数据的索引号
int IndexOfArray(ArrayPtr array,void* data)
{
	unsigned long loop = 0;
	unsigned long count = 0;
	unsigned long byteSize = 0;
	int index = -1;
	if ((NULL != array) && (NULL != array->address) && (NULL != data) && (array->size > 0))
	{
		count = array->size;
		byteSize = array->elementByteSize;
		for (loop=0; loop < count; loop++)
		{
			if (Compare_Equal == CompareData(array,(char*)array->address + loop * byteSize,data))
			{
				index = loop;
				break;
			}
		}
	}
	return index;
}


Boolean RemoveArrayIndex(ArrayPtr array,void* data,unsigned long index)
{
	Boolean ret = False;
	if ((NULL != array) && (NULL != array->address) && (index < array->size))
	{
		if (NULL != data)
		{
			ret = GetDataFromArray(array,data,index);
		}
		MoveData(array->address,
			index,
			array->address,
			index + 1,
			array->size - index - 1,
			array->elementByteSize
			);
		array->size--;
		ret = True;
	}
	return ret;
}

//删除连续的数据
Boolean RemoveArrayMoreIndex(ArrayPtr array,unsigned long count,unsigned long index)
{
	Boolean ret = False;
	if ((NULL != array) && (NULL != array->address) && (index < array->size) && (count > 0) && (count + index <= array->size))
	{
		MoveData(array->address,
			index,
			array->address,
			index + count,
			array->size - index - count,
			array->elementByteSize
			);
		array->size -= count;
	}
	return ret;
}
//删除指定数据
Boolean RemoveDataArray(ArrayPtr array,void* data)
{
	int index = IndexOfArray(array,data);
	Boolean ret = False;
	if (-1 != index)
	{
		ret = RemoveArrayIndex(array,NULL,index);
	}
	return ret;
}

void RemoveMoreDataArray(ArrayPtr array,void* data,unsigned long count)
{
	unsigned long i;
	if ((NULL != array) && (NULL != data) && (count > 0) && (count <= array->size))
	{
		for (i=0; i<count; i++)
		{
			RemoveDataArray(array,(char*)data + i * array->elementByteSize);
		}
	}
}

//获取动态数组的子数组
ArrayPtr SubArray(ArrayPtr array,unsigned long count,unsigned long index)
{
	ArrayPtr sub_array = NULL;
	if ((NULL != array) && (count > 0) && (index + count <= array->size))
	{
		sub_array = CreateArray(count,array->elementByteSize);
		if (NULL != sub_array)
		{
			sub_array->raiseCount = 0;
			sub_array->dataType = sub_array->dataType;
			MoveData(sub_array->address,
				0,
				array->address,
				index,
				count,
				array->elementByteSize
				);
			sub_array->size = count;
		}
	}
	return sub_array;
}

//清理数组
void CleanArray(ArrayPtr array)
{
	if ((NULL != array) && (NULL != array->address))
	{
		memset(array->address,0,array->capacity * array->elementByteSize);
		array->size = 0;
	}
}

//销毁动态数组
void DestroyArray(ArrayPtr array)
{
	if (NULL != array)
	{
		if (NULL != array->address)
		{
			free(array->address);
		}
		free(array);
	}
}

//使用快速排序算法
//这算法不好的地方就是使用递归的方式则元素数量太多会导致栈溢出
void SortArray(ArrayPtr array)
{
	unsigned low = 0;
	unsigned high = array->size - 1;
	if ((NULL != array) && (array->size > 0))
	{
		//数组元素数量在1万以内则使用递归
		if (array->size < RECURSIVE_SORT_NUM)
		{
			RecursiveSort(array,low,high);
		}
		else
		{
			IterationSort(array,low,high);
		}
	}
}

//快速排序的递归方法
void RecursiveSort(ArrayPtr array,unsigned long low,unsigned long high)
{

	void* sentry = NULL;//一个哨兵
	unsigned long sentryIndex = low; //哨兵所在的数组位置
	unsigned long hi = high;
	unsigned long lo = low;
	//使用最后一个节点作为临时变量
	if (array->size < array->capacity)
	{
		sentry = (char*)array->address + (array->capacity - 1) * array->elementByteSize;
	}
	else
	{
		sentry = malloc(array->elementByteSize);
	}
	GetDataFromArray(array,sentry,low);
	while(low < high)
	{
		//从右向左找第一个小于sentry的数
		while((low < high) && (CompareData(array,GetDataArray(array,high),sentry) >= Compare_Equal)) 
		{
			--high;
		}
		if (low < high)//可以注释掉的吧
		{
			//Swap(array,low,high);//a[low] = a[high],将比a[low]大的数找出来作为新的哨兵
			SetData(array,GetDataArray(array,high),1,low);
			low++;//哨兵已经被替换,则直接跟前一个比较就行
		}
		while((low < high) && (CompareData(array,GetDataArray(array,low),sentry) <= Compare_Equal))
		{
			++low;
		}
		if (low < high)//这个值一般都是肯定的
		{
			SetData(array,GetDataArray(array,low),1,high);//high之前被作为新的哨兵，这里使用当前low的值填充
			high--;//新的high位置的值已经被替换,可以往后一步
		}
	}
	//此时low就是哨兵所应该在的位置
	SetData(array,sentry,1,low);
	if (array->size < array->capacity)
	{
		memset((char*)array->address + (array->capacity - 1) * array->elementByteSize,0,array->elementByteSize);
	}
	else
	{
		free(sentry);
	}
	//if ((low - 1) > lo)//当low==0时,则很容易就大于
	if (low > (lo + 1))
	{
		//low是无符号类型,所以-1之后仍然是很大的正数
		RecursiveSort(array,lo,low - 1);
	}
	if ((low + 1) < hi)
	{
		RecursiveSort(array,low + 1,hi);
	}
}

//快速排序的迭代方法,迭代法能避免因为数据量太大，导致因为递归产生的栈溢出问题
//但是同时迭代法需要额外的内存空间来保存每次分割区域的上标和下标
void IterationSort(ArrayPtr array,unsigned long low,unsigned long high)
{

	unsigned long i;
	void* sentry = NULL;//一个哨兵
	unsigned long sentryIndex = low; //哨兵所在的数组位置
	unsigned long hi = high;
	unsigned long lo = low;
	ArrayPtr patitionList = NULL;
	//这TMD LONG类型左移32位还犯法了
	unsigned long long patition = ((unsigned long long)high << 32) | low;
	if (low < high)
	{
		//使用一个64位整型来表示每次分割的low和high
		//high在64位的高32位,low在低32位
		patitionList = CreateArray((hi - lo),LongLong_Type);
		InsertArray(patitionList,&patition);
		sentry = malloc(array->elementByteSize);
		for (i=0; i<patitionList->size; i++)
		{
			patition = *(unsigned long long*)GetDataArray(patitionList,i);
			high = patition >> 32;
			low = patition & 0x0ffffffffL;
			lo = low;
			hi = high;
			memset(sentry,0,array->elementByteSize);
			//获取哨兵值
			GetDataFromArray(array,sentry,low);
			while(low < high)
			{
				//从右向左找第一个小于sentry的数
				while((low < high) && (CompareData(array,GetDataArray(array,high),sentry) >= Compare_Equal)) 
				{
					--high;
				}
				if (low < high)//可以注释掉的吧
				{
					//Swap(array,low,high);//a[low] = a[high],将比a[low]大的数找出来作为新的哨兵
					SetData(array,GetDataArray(array,high),1,low);
					low++;//哨兵已经被替换,则直接跟前一个比较就行
				}
				while((low < high) && (CompareData(array,GetDataArray(array,low),sentry) <= Compare_Equal))
				{
					++low;
				}
				if (low < high)//这个值一般都是肯定的
				{
					SetData(array,GetDataArray(array,low),1,high);//high之前被作为新的哨兵，这里使用当前low的值填充
					high--;//新的high位置的值已经被替换,可以往后一步
				}
			}
			//此时low就是哨兵所应该在的位置
			SetData(array,sentry,1,low);
			//if ((low - 1) > lo)//当low==0时,则很容易就大于
			//产生新的分割区域
			if (low > (lo + 1))
			{
				//RecursiveSort(array,lo,low - 1);
				patition = ((unsigned long long)low - 1) << 32 | lo;
				InsertArray(patitionList,&patition);
			}
			if ((low + 1) < hi)
			{
				//RecursiveSort(array,low + 1,hi);
				patition = ((unsigned long long)hi << 32) | (low + 1);
				InsertArray(patitionList,&patition);
			}
		}
		DestroyArray(patitionList);
		free(sentry);
	}
}