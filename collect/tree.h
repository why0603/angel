/******************************************************************************************
 * @file tree.h
 * @anthor:Croky.Zheng
 * @CreateTime:2013-6-3
 * @descirption:树形结构
 * 基于红黑树实现的一个树形结构
 * 可以用于Set Map之类的操作
 *****************************************************************************************/
#ifndef BELIAL_COLLECT_TREE_H
#define BELIAL_COLLECT_TREE_H
#include "typedef.h"
#include "define.h"

//节点颜色
typedef enum
{
	//红色节点
	RedNode,
	//黑色节点
	BlackNode,
}TreeColor;

//树节点
typedef struct __tree_node__
{
	//左节点
	struct __tree_node__* left;

	//右节点
	struct __tree_node__* right;

	//父节点
	struct __tree_node__* parent;

	//
	TreeColor color;

	//数据
	char keyValue[];
} TreeNode,*TreeNodePtr;

//
typedef struct
{
	//是否变长类型
	//变长类型则只能保存其指针
	DataType keyType;

	DataType valueType;
	//数据是否有符号,默认为有符号数值
	Boolean isSigned;

	//当前节点个数
	unsigned long size;

	//关键词字节数
	unsigned long keyByteSize;

	//值的字节数
	unsigned long valueByteSize;

	//树形结构的根节点
	TreeNodePtr root;
}Tree,*TreePtr;

typedef void (*TreeCallBack)(TreePtr tree,char* keyValue);

#ifdef __cplusplus
extern "C" {	//兼容C++
#endif

/**
 * 创建一棵红黑树
 * @param keyByteSize key所占用的字节数
 * @param valueByteSize value占用字节数
 */
EXTERN TreePtr CreateTree(unsigned long keyByteSize,unsigned long valueByteSize);

/**
 * 创建一棵默认红黑树
 * KEY VALUE的大小为指针的长度
 */
EXTERN TreePtr CreateDefaultTree();

/**
 * 将数据插入数中
 * @param tree 要插入的树
 * @param key 关键词
 * @param value 插入的值
 */
EXTERN void InsertTree(TreePtr tree,void* key,void* value);

/**
 * 获取一个关键词所在节点的深度
 */
EXTERN unsigned long GetTreeNodeLayer(TreePtr tree,void* key);

/**
 * 获取一个关键词的值
 */
EXTERN void* GetTree(TreePtr tree,void* key);

/**
 * 获取一个关键词包含的词
 * 结果拷贝到value指向的内存地址
 */
EXTERN void GetFromTree(TreePtr tree,void* key,void* value);

/**
 * 检查树中是否包含key
 */
EXTERN Boolean ContairTree(TreePtr tree,void* key);

/**
 * 将KEY移除出树
 */
EXTERN void RemoveTree(TreePtr tree,void* key);

/**
 * 清理所有数据
 */
EXTERN void CleanTree(TreePtr tree);

/**
 * 清理所有数据并销毁整棵树
 */
EXTERN void DestroyTree(TreePtr tree);

/**
 * 枚举所有的树节点
 */
EXTERN void EnumTree(TreePtr tree,TreeCallBack fnTree);
#ifdef __cplusplus
}//兼容C++
#endif
#endif