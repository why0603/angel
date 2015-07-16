/******************************************************************************************
 * @file trie.c
 * @anthor:Croky.Zheng
 * @CreateTime:2013-6-15
 * @descirption:trie结构
 * 基于TRIE的树形结构,使用双数组的TRIE结构
 * 其实如何来构造TRIE树一直是比较困难的问题，因为TRIE面对的空间消耗是如此的大
 * 一直到DOUBLE-ARRAY-TRIE出现，可以略为减少空间消耗，但是构造的时候也比较困难
 * 如何增加DAT的密度，一直是困难问题（没有人提出红黑树类似的增加空间密度的有效方法）
 * 而针对的词典也是一大问题，如果全部使用UNICODE编码，则无疑会增加空间消耗
 * 因为每次增加新的分区，必然会产生65535个节点
 * 在UNICODE编码中，有一部分为4字节编码
 * 在GB18030编码中，有一部分为4字节编码
 * 其实对应的都是中文CJK1，CJK2的汉子
 * 这里当然要包含所有的UNICODE字符，可是有正确的字符编码，仍然可以节省内存空间
 *****************************************************************************************/
#include "trie.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

TriePtr CreateTrie(unsigned long segSize)
{
	TriePtr trie = (TriePtr)malloc(sizeof(Trie));
	if (NULL != trie)
	{
		trie->capacity = segSize;
		trie->nodeCount = 0;
		trie->size = 0;
		trie->segSize = segSize;
		trie->base = (long*)calloc(segSize,sizeof(long));
		trie->check = (long*)calloc(segSize,sizeof(long));
	}
	return trie;
}

void InsertTrie(TriePtr trie,unsigned short* str,unsigned long len)
{
	unsigned long i;
	long position;
	if ((NULL != trie) && (NULL != str) && (len > 1))
	{
		for (i=0; i<len; i++)
		{
			position = trie->base[str[i]];
		}
	}
}