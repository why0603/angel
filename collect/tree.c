/******************************************************************************************
* @file tree.c
* @anthor:Croky.Zheng
* @CreateTime:2013-6-3
* @descirption:红黑树
* 其实红黑树在实际的应用中已经没什么价值
* 虽然有很稳定的插入,删除,查询速度 (logN),这个速度对于分布不均的哈希表来说
* 可能会有稍许的效率提高,但是HASH表只要设计合理，则这种优势是不存在的
* 而在空间上的优势，由于构造树节点需要大量额外的内存(左右孩子节点，父节点(可省))，也很不明显，还有可能比
* 紧凑的HASH表更加消耗内存,现在看来红黑树唯一优点就是均匀（有些HASH表会导致某个桶有大量的堆积）
* 事实上在多年的应用开发中，已经不再使用这种结构
* 虽然在C#上偶尔有应用，不过那都是因为字典集合底层使用的结构
* 
* 红黑树特征
* 1.任何一个节点都被着色――红色或是黑色。
* 2.根节点是黑色的。
* 3.所有的NIL节点都看成黑色（NIL节点是就是一个假想的或是无实在意义的节点，所有应该指向NULL的指针，都看成指向了NIL节点。包括叶节点的子节点指针或是根节点的父指针）。
* 4.如果一个节点是红色的，那么它的子节点一定是黑色的。
* 5.对于任何一个节点而言，从该节点到它的子孙节点中的NIL节点路径中，所包含的黑节点个数相同
*****************************************************************************************/
#include "tree.h"
#include "common.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

//插入节点时，返回节点的位置
//0表示覆盖式插入
//1表示作为左节点
//2表示作为右节点
//3根节点
typedef enum
{
	CoverNode = 0,
	LeftNode = 1,
	RightNode = 2,
	RootNode = 3
} NodePosition;

typedef void (*NodeCallBack)(TreePtr tree,TreeNodePtr node);

//创建一个树节点
static TreeNodePtr CreateTreeNode(TreePtr tree,void* key,void* value);

//查找key所在的节点
static TreeNodePtr FindTreeNode(TreePtr tree,void* key);

//设置child为parent的左孩子，返回parent原来的左孩子
static TreeNodePtr SetLeftChild(TreeNodePtr parent,TreeNodePtr child);

//设置child为parent的右孩子，返回parent原来的右孩子
static TreeNodePtr SetRightChild(TreeNodePtr parent,TreeNodePtr child);

//插入一个节点,并返回插入的方式
static NodePosition InsertTreeNode(TreePtr tree,TreeNodePtr node);

//检查红色节点
//要是节点自身和父节点，叔父节点都是红色,则需要调整颜色
static void CheckTree(TreePtr tree,TreeNodePtr node);

//重新查找根节点
static void FindRootNode(TreePtr tree);

//左旋转节点
static void LeftRotate(TreePtr tree,TreeNodePtr node);

//右旋转节点
static void RightRotate(TreePtr tree,TreeNodePtr node);


//设置树的根节点
static TreeNodePtr SetRootNode(TreePtr tree,TreeNodePtr node);

//递归清理节点以及子节点
static void RecursiveProcessNode(TreePtr tree,TreeNodePtr node,NodeCallBack fnNode);

//迭代清理节点以及子节点
static void IterationProcessNode(TreePtr tree,TreeNodePtr node,NodeCallBack fnNode);

//获取一个节点的左孩子中的最大值
static TreeNodePtr GetLeftMaxNode(TreeNodePtr node);

//获取一个节点右孩子中的最小值
static TreeNodePtr GetRightMinNode(TreeNodePtr node);

//删除一个节点
static void DeleteTreeNode(TreePtr tree,TreeNodePtr node);


//删除节点之后，对红黑树特性做修复
static void DeleteNodeFix(TreePtr tree,TreeNodePtr node);

//释放一个节点的内存
static void FreeNode(TreePtr tree,TreeNodePtr node);

//枚举处理所有的树节点
static void EnumTreeNode(TreePtr tree,NodeCallBack fnNode);

static void RecursiveProcessKeyValue(TreePtr tree,TreeNodePtr node,TreeCallBack fnTree);

void RecursiveProcessKeyValue(TreePtr tree,TreeNodePtr node,TreeCallBack fnTree)
{
	fnTree(tree,node->keyValue);
	if (NULL != node->left)
	{
		RecursiveProcessKeyValue(tree,node->left,fnTree);
	}
	if (NULL != node->right)
	{
		RecursiveProcessKeyValue(tree,node->right,fnTree);
	}
}

void EnumTreeNode(TreePtr tree,NodeCallBack fnNode)
{	
	if ((NULL != tree) && (tree->size > 0))
	{
		if (tree->size > 10000)
		{
			RecursiveProcessNode(tree,tree->root,fnNode);
		}
		else
		{
			IterationProcessNode(tree,tree->root,fnNode);
		}
	}
}

void FreeNode(TreePtr tree,TreeNodePtr node)
{
	free(node);
	tree->size--;
}

void DeleteNodeFix(TreePtr tree,TreeNodePtr node)
{
	TreeNodePtr brother = NULL;
	TreeNodePtr parent = NULL;
	NodePosition position;
	TreeColor brother_color,parent_color,color,brother_left_color,brother_right_color;
	parent = node->parent;
	//当前节点是父节点的左孩子还是右孩子
	position = (node == parent->left) ? LeftNode : RightNode;
	brother = (node == parent->left) ? parent->right : parent->left;
	brother_color = (NULL == brother) ? BlackNode : brother->color;
	parent_color = parent->color;
	color = node->color;
	//兄弟节点时红色
	if (brother_color == RedNode)
	{
		if (position == LeftNode)
		{
			LeftRotate(tree,parent);
		}
		else
		{
			RightRotate(tree,parent);
		}
		//交互兄弟节点与父节点的颜色
		brother->color = BlackNode;
		parent->color = RedNode;
		//经过上面的转换,brother成为红色,兄弟节点成为黑色
		//则需要按照下面的逻辑处理
		DeleteNodeFix(tree,node);
	}
	else if ((NULL != brother) && (brother_color == BlackNode))
	{
		brother_left_color = (NULL == brother->left) ? BlackNode : brother->left->color;
		brother_right_color = (NULL == brother->right) ? BlackNode : brother->right->color;
		//全部为黑色 case 1
		if ((parent_color == BlackNode) && 
			//(brother_color == BlackNode) && 
			(brother_left_color == BlackNode) && 
			(brother_right_color == BlackNode))
		{
			//这样不违反红黑树特性，又让兄弟节点少了一个黑色节点，刚好平衡
			brother->color = RedNode;
		}//父节点为红色，其他都是黑色 case 2
		else if ((parent_color == RedNode) && 
			//(brother_color == BlackNode) && 
			(brother_left_color == BlackNode) && 
			(brother_right_color == BlackNode))
		{
			//交互兄弟节点与父节点的颜色
			//这样等于将删除的黑色节点补齐，兄弟节点没有变化
			brother->color = BlackNode;
			parent->color = RedNode;
		}//case 3,转为符合case 5的情形
		else if ((position == LeftNode) && 
			(brother_left_color == RedNode) && //左孩子是红色
			(brother_right_color == BlackNode))
		{//这样就变成了有一个右孩子是红色的兄弟节点
			RightRotate(tree,brother);
			//兄弟节点与他的左孩子节点颜色
			brother->color = RedNode;
			brother->left->color = BlackNode;
			DeleteNodeFix(tree,node);
		}//case 4,转为符合case 6的情形
		else if ((position == RightNode) && //按照目前的算法，这种情况是不可能会有的
			(brother_left_color == RedNode) && 
			(brother_right_color == BlackNode))//右孩子是红色
		{//获得一个红色兄弟
			LeftRotate(tree,brother);
			brother->color = RedNode;
			brother->right->color = BlackNode;
			DeleteNodeFix(tree,node);
		}//case 5
		else if ((position == LeftNode) &&
			(brother_right_color == RedNode))
		{
			brother->color = parent->color;
			parent->color = BlackNode;
			brother->right->color = BlackNode;
			//给N添加了一个黑色节点
			LeftRotate(tree,parent);
		}//case 6
		else if ((position == RightNode) &&
			(brother_left_color == RedNode))
		{
			brother->color = parent->color;
			parent->color = BlackNode;
			brother->left->color = BlackNode;
			//给N添加了一个黑色节点
			RightRotate(tree,parent);
		}
		else
		{
			//printf("\r\n 难道还有其他情况 \r\n");
		}
	}
	else
	{
		//printf("\r\n brother node = null \r\n");
	}
}

//删除树节点,到此时的树节点，最多只有一个孩子,而且是左孩子
void DeleteTreeNode(TreePtr tree,TreeNodePtr node)
{
	TreeNodePtr brother = NULL;
	TreeNodePtr child = NULL;
	TreeNodePtr parent = NULL;

	parent = node->parent;
	child = (NULL == node->left) ? node->right : node->left;
	//child = node->left;

	//没有父节点和子女节点,是唯一一个节点,case 1
	if ((NULL == child) && (NULL == parent))
	{
		SetRootNode(tree,NULL);
	}
	else if ((NULL != child) && (NULL == parent))
	{
		//只有一个孩子节点,没有父节点,case 2
		SetRootNode(tree,child);
	}
	else if ((NULL == child) && (NULL != parent))
	{
		//没有孩子节点,有父节点
		if (node == parent->left)
		{
			SetLeftChild(parent,NULL);//child = NULL
		}
		else
		{
			SetRightChild(parent,NULL);//child = NULL
		}
		/*
		if (BlackNode == node->color)
		{
		DeleteNodeFix(tree,node);
		}
		*/
	}
	else
	{
		//有孩子节点,有父节点
		if (node == parent->left)
		{
			SetLeftChild(parent,child);//child = NULL
		}
		else
		{
			SetRightChild(parent,child);//child = NULL
		}
		//N=B但是P=R,C=R时，需要将孩子节点设置为黑色
		child->color = BlackNode;
		if (BlackNode == node->color)
		{
			//这个节点已经代替了node的位置
			DeleteNodeFix(tree,child);
		}
	}

	FreeNode(tree,node);
}

TreeNodePtr GetRightMinNode(TreeNodePtr node)
{
	TreeNodePtr child = NULL;
	if (NULL != node)
	{
		child = node->right;
		while(NULL != child)
		{
			if (NULL == child->left)
			{
				break;
			}
			child = child->left;
		}
	}
	return child;
}

TreeNodePtr GetLeftMaxNode(TreeNodePtr node)
{
	TreeNodePtr child = NULL;
	if (NULL != node)
	{
		child = node->left;
		while(NULL != child)
		{
			if (NULL == child->right)
			{
				break;
			}
			child = child->right;
		}
	}
	return child;
}


//递归删除节点以及对应的所有子节点
void RecursiveProcessNode(TreePtr tree,TreeNodePtr node,NodeCallBack fnNode)
{
	TreeNodePtr left = NULL;
	TreeNodePtr right = NULL;
	left = node->left;
	right = node->right;
	if (NULL != fnNode)
	{
		fnNode(tree,node);
	}
	if (NULL != left)
	{
		RecursiveProcessNode(tree,left,fnNode);
	}
	if (NULL != right)
	{
		RecursiveProcessNode(tree,right,fnNode);
	}
}

void IterationProcessNode(TreePtr tree,TreeNodePtr node,NodeCallBack fnNode)
{
	unsigned long i;
	TreeNodePtr left = NULL;
	TreeNodePtr right = NULL;
	ArrayPtr nodeList = CreateArray(tree->size,sizeof(void*));
	InsertArray(nodeList,&node);
	for (i=0; i<nodeList->size; i++)
	{
		node = *(TreeNodePtr*)GetDataArray(nodeList,i);
		left = node->left;
		right = node->right;
		if (NULL != fnNode)
		{
			fnNode(tree,node);
		}
		if (NULL != left)
		{
			InsertArray(nodeList,&left);
		}
		if (NULL != right)
		{
			InsertArray(nodeList,&right);
		}
	}
	DestroyArray(nodeList);
}

//
void FindRootNode(TreePtr tree)
{
	TreeNodePtr root = tree->root;
	//寻找根节点,在旋转和交互中,根节点很容易换位
	if (NULL != root)
	{
		while(NULL != root->parent)
		{
			root = root->parent;
		}
	}
	tree->root = root;
}

static void CheckTree(TreePtr tree,TreeNodePtr node)
{
	//父节点，祖父节点，叔父节点
	TreeNodePtr parent,grand,uncle;//great

	//与父节点的关系(左孩子还是右孩子),父节点与祖父节点的关系
	NodePosition selfPosition,parentPosition;
	parent = node->parent;
	/*
	great = (NULL != parent) ? parent->parent : NULL;
	uncle = (NULL != great) ? ((great->left == parent) ? great->right : great->left) : NULL;
	TreeColor selfColor = node->color;
	TreeColor parentColor = (NULL != parent) ? parent->color : BlackNode;
	TreeColor uncleColor = (NULL != uncle) ? uncle->color : BlackNode;
	TreeColor greatColor = (NULL != great) ? great->color : BlackNode;
	*/
	//父节点为NULL，这表示是根节点
	if (NULL == parent)
	{
		SetRootNode(tree,node);
	}
	else if ((node->color == RedNode) && (parent->color == RedNode))
	{
		//自身节点 父节点为红色
		//这个时候一定会有一个祖父节点
		grand = parent->parent;
		uncle = (grand->left == parent) ? grand->right : grand->left;
		//自身节点 父节点 叔父节点都为红色
		if ((NULL != uncle) && (uncle->color == RedNode))
		{
			grand->color = RedNode;
			parent->color = BlackNode;
			uncle->color = BlackNode;
			CheckTree(tree,grand);
			//将祖父节点作为新插入的节点开始检测树的合法性
		}
		else  //叔父节点是黑色或者空节点
		{
			//自身节点 父节点为红色 叔父节点为黑色（NULL表示黑色）
			selfPosition = (node == parent->left) ? LeftNode : RightNode;
			parentPosition = (parent == grand->left) ? LeftNode : RightNode;
			//旋转分为 左左 左右 右左 右右
			if ((selfPosition == LeftNode) && (parentPosition == LeftNode))
			{
				/*
				G(b)                    P(b)
				/      \                 /     \
				P(r)     U(b)   -->      N(r)    G(r)
				/   \                            /    \
				N(r)    M                          M     U(b)
				*/
				RightRotate(tree,grand);
				//左左的情况需要更换节点颜色,可以知道的是祖父节点肯定是黑色
				parent->color = BlackNode;
				grand->color = RedNode;
			}
			else if ((parentPosition == RightNode) && (selfPosition == LeftNode))
			{
				//右左
				//以父节点右转，这个时候将变成右右的关系
				RightRotate(tree,parent);
				//要是减少递归，可以再转一次
				//CheckTree(tree,node);
				LeftRotate(tree,grand);
				parent->color = BlackNode;
				grand->color = RedNode;
			}
			else if ((parentPosition == LeftNode) && (selfPosition == RightNode))
			{
				//左右
				//以父节点左转，这个时候将变成左左的关系
				LeftRotate(tree,parent);
				//下次检测则会发现是左左关系
				//要是减少递归，可以再转一次
				//CheckTree(tree,node);
				RightRotate(tree,grand);
				parent->color = BlackNode;
				grand->color = RedNode;
			}
			else
			{
				//右右
				/*
				G(b)                    P(b)
				/      \                 /    \
				U(b)     P(r)   -->      G(r)   N(r)
				/     \          /   \
				M      N(r)     U(b)   M
				*/
				LeftRotate(tree,grand);
				parent->color = BlackNode;
				grand->color = RedNode;
			}
		}
		//这里发现祖父节点肯定被染成了红色，而父节点都被染成黑色
		//因为插入的是红色节点，而一个红节点必须包含两个黑色节点
		//·
	}
	else
	{
		//这样表示调整基本完毕
	}
}

NodePosition InsertTreeNode(TreePtr tree,TreeNodePtr node)
{
	NodePosition nodePosition = CoverNode;
	Boolean ret = False;
	TreeNodePtr parent = tree->root;
	int compare = 0;
	while(NULL != parent)
	{
		compare = Compare(parent->keyValue,node->keyValue,tree->keyType,tree->keyByteSize,tree->isSigned);
		//遇到相同KEY的节点
		if (compare == Compare_Equal)
		{
			//覆盖VALUE
			memcpy(parent->keyValue + tree->keyByteSize,node->keyValue + tree->keyByteSize,tree->keyByteSize);
			ret = True;
			nodePosition = CoverNode;
			break;
		}
		else if (compare == Compare_Less)
		{
			//父节点小于当前节点,则为父节点的右孩子
			if (NULL == parent->right)
			{
				SetRightChild(parent,node);
				ret = True;
				nodePosition = RightNode;
				break;
			}
			else
			{
				parent = parent->right;
			}
		}
		else
		{
			//父节点大于当前节点,则为父节点的左孩子
			if (NULL == parent->left)
			{
				SetLeftChild(parent,node);
				ret = True;
				nodePosition = LeftNode;
				break;
			}
			else
			{
				parent = parent->left;
			}
		}
	}

	//一直到NULL都没有插入,那就是根节点了
	if (False == ret)
	{
		//tree->root = node;
		//node->color = BlackNode;
		SetRootNode(tree,node);
		nodePosition = RootNode;
	}
	return nodePosition;
}

TreeNodePtr SetRootNode(TreePtr tree,TreeNodePtr node)
{
	TreeNodePtr root = tree->root;
	tree->root = node;
	if (NULL != node)
	{
		node->parent = NULL;
		node->color = BlackNode;
	}
	return root;
}

TreeNodePtr SetRightChild(TreeNodePtr parent,TreeNodePtr child)
{
	TreeNodePtr right = parent->right;
	parent->right = child;
	if (NULL != child)
	{
		child->parent = parent;
	}
	return right;
}

//设置child为parent的左孩子
TreeNodePtr SetLeftChild(TreeNodePtr parent,TreeNodePtr child)
{
	TreeNodePtr left = parent->left;
	parent->left = child;
	if (NULL != child)
	{
		child->parent = parent;
	}
	return left;
}

//左旋转
void LeftRotate(TreePtr tree,TreeNodePtr node)
{
	//获取节点的父节点
	TreeNodePtr parent = node->parent;

	//变为自身右孩子的左孩子
	//返回原来右孩子的左孩子（孙子）,自身左右孩子没变
	TreeNodePtr left = SetLeftChild(node->right,node);

	//将右孩子作为父节点的(左,右)孩子,右孩子节点空出
	if (NULL != parent)
	{
		if (parent->left == node)
		{
			SetLeftChild(parent,node->right);
		}
		else
		{
			SetRightChild(parent,node->right);
		}
	}
	else
	{
		SetRootNode(tree,node->right);
	}

	//将之前右孩子的左孩子设置为右孩子
	SetRightChild(node,left);
}

//右旋转
void RightRotate(TreePtr tree,TreeNodePtr node)
{
	//获取节点的父节点
	TreeNodePtr parent = node->parent;

	//变为自身左孩子的右孩子
	//返回原来左孩子的右孩子（孙子）,自身左右孩子没变
	TreeNodePtr right = SetRightChild(node->left,node);

	//将左孩子作为父节点的右孩子,左孩子节点空出

	//将右孩子作为父节点的(左,右)孩子,右孩子节点空出
	if (NULL != parent)
	{
		if (parent->left == node)
		{
			SetLeftChild(parent,node->left);
		}
		else
		{
			SetRightChild(parent,node->left);
		}
	}
	else
	{
		//表示是根节点,根节点没有父节点
		SetRootNode(tree,node->left);
	}

	//将之前左孩子的右孩子设置为自身的左孩子
	SetLeftChild(node,right);
}

TreeNodePtr CreateTreeNode(TreePtr tree,void* key,void* value)
{
	TreeNodePtr node = (TreeNodePtr)malloc(sizeof(TreeNode) + tree->keyByteSize + tree->valueByteSize);
	if (NULL != node)
	{
		memcpy(node->keyValue,key,tree->keyByteSize);
		if ((NULL != value) && (tree->valueByteSize > 0))
		{
			memcpy((char*)node->keyValue + tree->keyByteSize,value,tree->valueByteSize);
		}
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;
		node->color = RedNode;
	}
	return node;
}

TreeNodePtr FindTreeNode(TreePtr tree,void* key)
{
	TreeNodePtr node = NULL;
	int compare = 0;
	if (NULL != tree)
	{
		node = tree->root;
		while(NULL != node)
		{
			compare = Compare(node->keyValue,key,tree->keyType,tree->keyByteSize,tree->isSigned);
			if (compare == Compare_Equal)
			{
				break;
			}
			else if (compare == Compare_Less)
			{
				node = node->right;
			}
			else
			{
				node = node->left;
			}
		}
	}
	return node;
}

unsigned long GetTreeNodeLayer(TreePtr tree,void* key)
{
	TreeNodePtr node = tree->root;
	int compare = 0;
	unsigned long layer = 1;
	while(NULL != node)
	{
		compare = Compare(node->keyValue,key,(DataType)tree->keyType,tree->keyByteSize,tree->isSigned);
		if (compare == Compare_Equal)
		{
			return layer;
		}
		else if (compare == Compare_Less)
		{
			node = node->right;
		}
		else
		{
			node = node->left;
		}
		layer++;
	}
	return 0;
}

void* GetTree(TreePtr tree,void* key)
{
	TreeNodePtr node = FindTreeNode(tree,key);
	if (NULL != node)
	{
		return (char*)node->keyValue + tree->keyByteSize;
	}
	return NULL;
}

void GetFromTree(TreePtr tree,void* key,void* value)
{
	TreeNodePtr node = FindTreeNode(tree,key);
	if ((NULL != node) && (tree->valueByteSize > 0) && (NULL != value))
	{
		CopyData(value,(char*)node->keyValue + tree->keyByteSize,(DataType)tree->valueByteSize,tree->valueByteSize);
	}
}

Boolean ContairTree(TreePtr tree,void* key)
{
	Boolean ret = False;
	TreeNodePtr node = FindTreeNode(tree,key);
	if (NULL != node)
	{
		ret = True;
	}
	return ret;
}

TreePtr CreateTree(unsigned long keyByteSize,unsigned long valueByteSize)
{
	TreePtr tree = (TreePtr)calloc(1,sizeof(Tree));
	if (NULL != tree)
	{
		tree->size = 0;
		if (keyByteSize == 0)
		{
			tree->keyByteSize = sizeof(void*);
		}
		else
		{
			tree->keyByteSize = keyByteSize;
		}
		tree->valueByteSize = valueByteSize;
		tree->keyType = (DataType)tree->keyByteSize;
		tree->valueType = (DataType)tree->valueByteSize;
		tree->size = 0;
		tree->root = NULL;
	}
	return tree;
}

TreePtr CreateDefaultTree()
{
	return CreateTree(sizeof(void*),sizeof(void*));
}

void InsertTree(TreePtr tree,void* key,void* value)
{
	NodePosition position;
	TreeNodePtr node;
	if ((NULL != tree) && (NULL != key))
	{
		node = CreateTreeNode(tree,key,value);
		position = InsertTreeNode(tree,node);
		//插入的节点为父节点的左孩子或者右孩子时，需要检查树的合法性
		//如果是覆盖式或者直接插入根节点则无需再考虑
		if (position == RootNode)
		{
			//根节点必须为黑色
			//node->color = BlackNode;
			tree->size++;
		}
		else if ((position == LeftNode) || (position == RightNode))
		{
			tree->size++;
			CheckTree(tree,node);
		}
		else
		{
			//覆盖式插入,什么也不做，原有的value都懒得返回
		}
	}
	return;
}

void CleanTree(TreePtr tree)
{
	EnumTreeNode(tree,FreeNode);
	if (NULL != tree)
	{
		tree->size = 0;
		tree->root = NULL;
	}
}

void EnumTree(TreePtr tree,TreeCallBack fnTree)
{
	if ((NULL != tree) && (tree->size > 0) && (NULL != fnTree))
	{
		RecursiveProcessKeyValue(tree,tree->root,fnTree);
	}
}

void DestroyTree(TreePtr tree)
{
	CleanTree(tree);
	if (NULL != tree)
	{
		free(tree);
	}
}


void RemoveTree(TreePtr tree,void* key)
{
	TreeNodePtr child;
	TreeNodePtr node = FindTreeNode(tree,key);
	if (NULL != node)
	{
		if ((NULL == node->left) || (NULL == node->right))
		{
			//单亲或者无孩子节点
			child = node;
		}
		else
		{
			//获取左孩子节点下的孙子节点中最大的值
			//这个节点必然没有右孩子
			child = GetLeftMaxNode(node);


			//替换KEY和VALUE
			memcpy(node->keyValue,child->keyValue,tree->keyByteSize + tree->valueByteSize);
		}
		DeleteTreeNode(tree,child);
	}
	else
	{
		//printf("\r\nnode is null!\r\n");
	}
}