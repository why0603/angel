/******************************************************************************************
* @file array.c
* @anthor:Croky.Zheng
* @CreateTime:2013-5-15
* @descirption:��̬����
* �����Զ���ӳ��ȵ�����
* Ԫ�س��ȹ̶�
* ���Ը����������ʣ�ÿ����������֮����Ҫ�����µĿռ�
* �µ��ڴ�ռ�������֮ǰ���ڴ�ռ䣬���Ҹ���ԭ�������������
* ʹ���������飬��ó�ʼ����ʱ���֪�����ݵ����ֵ�����Ҷ���˳�����
* �ܲ��������ݵ�ɾ��������кܶ�ɾ����������ᵼ��Ч��ʮ�ֵĵ�
* Ϊ��ʵ�ַ��㣬������̫����ڴ����ͳ�Խ�������
*****************************************************************************************/

#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <common.h>

//Ĭ���б��Ԫ�ظ���
#define MIN_COUNT	20

//�ݹ����������������SIZE�����飬�����õݹ�����
#define RECURSIVE_SORT_NUM 10000

//������Ԫ�س����������������������
#define DEFAULT_RAISE_RATE	0.5f

//static ArrayPtr CreateSelfArray(unsigned long capacity,unsigned long size,float raiseRate);

//��������Ƿ񳬳����������ܱ�ʾ�����Χ
//������������·���������ڴ�
static void EnsureCapacity(ArrayPtr array,unsigned count,unsigned index);

//Ϊ�������ݣ�capacityΪ�µĴ�С,��index�ĵط������count������
static void ExpandArray(ArrayPtr array,unsigned long capacity,unsigned count,unsigned index);

//��������ֵ 
//���飬���ݣ�����
static void SetData(ArrayPtr array,void* data,unsigned long count,unsigned long index);

//�ڶ�̬�����ڲ��ƶ�����
static void MoveData(void* dest,unsigned long destIdx,void* src,unsigned long srcIdx,unsigned long count,unsigned long byteSize);

//�Ƚ���������ֵ��index1==index2=0��index1>index2=1,index1<index=-1
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
	//������ʾ���ַ���,��������\0�������ַ���
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
	//cpu�ֿ�
	//unsigned long w = GetCpuWordSize() / 4;
	//�����Ҫִ�ж��ٴ�CPU�ֿ�
	//unsigned long loop = count / w;
	char* dest_ptr = (char*)dest + destIdx * byteSize;
	char* src_ptr = (char*)src + srcIdx * byteSize;
	unsigned long offsetSize = count * byteSize;
	char* dest_last = dest_ptr + offsetSize;
	char* src_last = src_ptr + offsetSize;
	//Ŀ���ַ��Դ��ַ��ͬ,�������Բ�ͬ������
	if (dest != src)
	{
		//ֱ�Ӹ���,memcpyΪ������ʽ,������������Ż�
		memcpy(dest_ptr,src_ptr,offsetSize);
	}
	else //ͬһ������
	{
		if (dest_ptr < src_ptr)
		{
			//dest �� src ���ཻ
			if (dest_last < src_ptr)
			{
				memcpy(dest_ptr,src_ptr,offsetSize);
			}
			else //dest��src�ཻ
			{
				//������Ը���CPU�ֳ��Ż�,ֻ����insert���������������ֲ���,��deleteʱ,���޷����
				while (dest_ptr < dest_last)
				{
					*dest_ptr++ = *src_ptr++;
				}
			}
		}
		else
		{
			//src �� dest���ཻ
			if(src_last < dest_ptr)
			{
				memcpy(dest_ptr,src_ptr,offsetSize);
			}
			else //src��dest�ཻ
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
		//��4�ı���
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
	//�µ����鳤��
	unsigned long newCapacity = 0;
	//�ϵ����鳤��
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
		//��������µ�ַ
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
				//����index֮ǰ������
				MoveData(newAddress,
					0,
					array->address,
					0,
					index,
					array->elementByteSize
					);
				//����index֮�������
				MoveData(newAddress,
					index + count,
					array->address,
					index,
					array->size - index,
					array->elementByteSize
					);
			}
			//�����ͷ��ϵ�ַ
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
		if (dataType == Unknow_Type || dataType == Pointer_Type || dataType == Chars_Type) //ָ�������ر���
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
		//=0.0f��ʾ�Զ�����
		array->size = 0;
		array->raiseCount = 0;
		//Ĭ������¶�̬���鱣����ǵȳ�����
		array->dataType = (DataType)array->elementByteSize;
		array->isSigned = True;
		array->address = calloc(array->elementByteSize,array->capacity);
	}
	return array;
}

//������̬����
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
		//��0��β�ı䳤����
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
		//��ֹindex �ڵ�ǰ���������ֵ
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

//��ȡ���ݵ�������
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

//ɾ������������
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
//ɾ��ָ������
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

//��ȡ��̬�����������
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

//��������
void CleanArray(ArrayPtr array)
{
	if ((NULL != array) && (NULL != array->address))
	{
		memset(array->address,0,array->capacity * array->elementByteSize);
		array->size = 0;
	}
}

//���ٶ�̬����
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

//ʹ�ÿ��������㷨
//���㷨���õĵط�����ʹ�õݹ�ķ�ʽ��Ԫ������̫��ᵼ��ջ���
void SortArray(ArrayPtr array)
{
	unsigned low = 0;
	unsigned high = array->size - 1;
	if ((NULL != array) && (array->size > 0))
	{
		//����Ԫ��������1��������ʹ�õݹ�
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

//��������ĵݹ鷽��
void RecursiveSort(ArrayPtr array,unsigned long low,unsigned long high)
{

	void* sentry = NULL;//һ���ڱ�
	unsigned long sentryIndex = low; //�ڱ����ڵ�����λ��
	unsigned long hi = high;
	unsigned long lo = low;
	//ʹ�����һ���ڵ���Ϊ��ʱ����
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
		//���������ҵ�һ��С��sentry����
		while((low < high) && (CompareData(array,GetDataArray(array,high),sentry) >= Compare_Equal)) 
		{
			--high;
		}
		if (low < high)//����ע�͵��İ�
		{
			//Swap(array,low,high);//a[low] = a[high],����a[low]������ҳ�����Ϊ�µ��ڱ�
			SetData(array,GetDataArray(array,high),1,low);
			low++;//�ڱ��Ѿ����滻,��ֱ�Ӹ�ǰһ���ȽϾ���
		}
		while((low < high) && (CompareData(array,GetDataArray(array,low),sentry) <= Compare_Equal))
		{
			++low;
		}
		if (low < high)//���ֵһ�㶼�ǿ϶���
		{
			SetData(array,GetDataArray(array,low),1,high);//high֮ǰ����Ϊ�µ��ڱ�������ʹ�õ�ǰlow��ֵ���
			high--;//�µ�highλ�õ�ֵ�Ѿ����滻,��������һ��
		}
	}
	//��ʱlow�����ڱ���Ӧ���ڵ�λ��
	SetData(array,sentry,1,low);
	if (array->size < array->capacity)
	{
		memset((char*)array->address + (array->capacity - 1) * array->elementByteSize,0,array->elementByteSize);
	}
	else
	{
		free(sentry);
	}
	//if ((low - 1) > lo)//��low==0ʱ,������׾ʹ���
	if (low > (lo + 1))
	{
		//low���޷�������,����-1֮����Ȼ�Ǻܴ������
		RecursiveSort(array,lo,low - 1);
	}
	if ((low + 1) < hi)
	{
		RecursiveSort(array,low + 1,hi);
	}
}

//��������ĵ�������,�������ܱ�����Ϊ������̫�󣬵�����Ϊ�ݹ������ջ�������
//����ͬʱ��������Ҫ������ڴ�ռ�������ÿ�ηָ�������ϱ���±�
void IterationSort(ArrayPtr array,unsigned long low,unsigned long high)
{

	unsigned long i;
	void* sentry = NULL;//һ���ڱ�
	unsigned long sentryIndex = low; //�ڱ����ڵ�����λ��
	unsigned long hi = high;
	unsigned long lo = low;
	ArrayPtr patitionList = NULL;
	//��TMD LONG��������32λ��������
	unsigned long long patition = ((unsigned long long)high << 32) | low;
	if (low < high)
	{
		//ʹ��һ��64λ��������ʾÿ�ηָ��low��high
		//high��64λ�ĸ�32λ,low�ڵ�32λ
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
			//��ȡ�ڱ�ֵ
			GetDataFromArray(array,sentry,low);
			while(low < high)
			{
				//���������ҵ�һ��С��sentry����
				while((low < high) && (CompareData(array,GetDataArray(array,high),sentry) >= Compare_Equal)) 
				{
					--high;
				}
				if (low < high)//����ע�͵��İ�
				{
					//Swap(array,low,high);//a[low] = a[high],����a[low]������ҳ�����Ϊ�µ��ڱ�
					SetData(array,GetDataArray(array,high),1,low);
					low++;//�ڱ��Ѿ����滻,��ֱ�Ӹ�ǰһ���ȽϾ���
				}
				while((low < high) && (CompareData(array,GetDataArray(array,low),sentry) <= Compare_Equal))
				{
					++low;
				}
				if (low < high)//���ֵһ�㶼�ǿ϶���
				{
					SetData(array,GetDataArray(array,low),1,high);//high֮ǰ����Ϊ�µ��ڱ�������ʹ�õ�ǰlow��ֵ���
					high--;//�µ�highλ�õ�ֵ�Ѿ����滻,��������һ��
				}
			}
			//��ʱlow�����ڱ���Ӧ���ڵ�λ��
			SetData(array,sentry,1,low);
			//if ((low - 1) > lo)//��low==0ʱ,������׾ʹ���
			//�����µķָ�����
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