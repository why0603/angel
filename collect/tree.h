/******************************************************************************************
 * @file tree.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-6-3
 * @descirption:���νṹ
 * ���ں����ʵ�ֵ�һ�����νṹ
 * ��������Set Map֮��Ĳ���
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_TREE_H
#define BELIAL_COLLECT_TREE_H
#include "typedef.h"
#include "define.h"

//�ڵ���ɫ
typedef enum
{
	//��ɫ�ڵ�
	RedNode,
	//��ɫ�ڵ�
	BlackNode,
}TreeColor;

//���ڵ�
typedef struct __tree_node__
{
	//��ڵ�
	struct __tree_node__* left;

	//�ҽڵ�
	struct __tree_node__* right;

	//���ڵ�
	struct __tree_node__* parent;

	//
	TreeColor color;

	//����
	char keyValue[];
} TreeNode,*TreeNodePtr;

//
typedef struct
{
	//�Ƿ�䳤����
	//�䳤������ֻ�ܱ�����ָ��
	DataType keyType;

	DataType valueType;
	//�����Ƿ��з���,Ĭ��Ϊ�з�����ֵ
	Boolean isSigned;

	//��ǰ�ڵ����
	unsigned long size;

	//�ؼ����ֽ���
	unsigned long keyByteSize;

	//ֵ���ֽ���
	unsigned long valueByteSize;

	//���νṹ�ĸ��ڵ�
	TreeNodePtr root;
}Tree,*TreePtr;

typedef void (*TreeCallBack)(TreePtr tree,char* keyValue);

#ifdef __cplusplus
extern "C" {	//����C++
#endif

/**
 * ����һ�ú����
 * @param keyByteSize key��ռ�õ��ֽ���
 * @param valueByteSize valueռ���ֽ���
 */
EXTERN TreePtr CreateTree(unsigned long keyByteSize,unsigned long valueByteSize);

/**
 * ����һ��Ĭ�Ϻ����
 * KEY VALUE�Ĵ�СΪָ��ĳ���
 */
EXTERN TreePtr CreateDefaultTree();

/**
 * �����ݲ�������
 * @param tree Ҫ�������
 * @param key �ؼ���
 * @param value �����ֵ
 */
EXTERN void InsertTree(TreePtr tree,void* key,void* value);

/**
 * ��ȡһ���ؼ������ڽڵ�����
 */
EXTERN unsigned long GetTreeNodeLayer(TreePtr tree,void* key);

/**
 * ��ȡһ���ؼ��ʵ�ֵ
 */
EXTERN void* GetTree(TreePtr tree,void* key);

/**
 * ��ȡһ���ؼ��ʰ����Ĵ�
 * ���������valueָ����ڴ��ַ
 */
EXTERN void GetFromTree(TreePtr tree,void* key,void* value);

/**
 * ��������Ƿ����key
 */
EXTERN Boolean ContairTree(TreePtr tree,void* key);

/**
 * ��KEY�Ƴ�����
 */
EXTERN void RemoveTree(TreePtr tree,void* key);

/**
 * ������������
 */
EXTERN void CleanTree(TreePtr tree);

/**
 * �����������ݲ�����������
 */
EXTERN void DestroyTree(TreePtr tree);

/**
 * ö�����е����ڵ�
 */
EXTERN void EnumTree(TreePtr tree,TreeCallBack fnTree);
#ifdef __cplusplus
}//����C++
#endif
#endif