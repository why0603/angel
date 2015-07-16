/******************************************************************************************
 * @file trie.c
 * @anthor:Croky.Zheng
 * @CreateTime:2013-6-15
 * @descirption:trie�ṹ
 * ����TRIE�����νṹ,ʹ��˫�����TRIE�ṹ
 * ��ʵ���������TRIE��һֱ�ǱȽ����ѵ����⣬��ΪTRIE��ԵĿռ���������˵Ĵ�
 * һֱ��DOUBLE-ARRAY-TRIE���֣�������Ϊ���ٿռ����ģ����ǹ����ʱ��Ҳ�Ƚ�����
 * �������DAT���ܶȣ�һֱ���������⣨û���������������Ƶ����ӿռ��ܶȵ���Ч������
 * ����ԵĴʵ�Ҳ��һ�����⣬���ȫ��ʹ��UNICODE���룬�����ɻ����ӿռ�����
 * ��Ϊÿ�������µķ�������Ȼ�����65535���ڵ�
 * ��UNICODE�����У���һ����Ϊ4�ֽڱ���
 * ��GB18030�����У���һ����Ϊ4�ֽڱ���
 * ��ʵ��Ӧ�Ķ�������CJK1��CJK2�ĺ���
 * ���ﵱȻҪ�������е�UNICODE�ַ�����������ȷ���ַ����룬��Ȼ���Խ�ʡ�ڴ�ռ�
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