/******************************************************************************************
* @file tree.c
* @anthor:Croky.Zheng
* @CreateTime:2013-6-3
* @descirption:�����
* ��ʵ�������ʵ�ʵ�Ӧ�����Ѿ�ûʲô��ֵ
* ��Ȼ�к��ȶ��Ĳ���,ɾ��,��ѯ�ٶ� (logN),����ٶȶ��ڷֲ������Ĺ�ϣ����˵
* ���ܻ��������Ч�����,����HASH��ֻҪ��ƺ��������������ǲ����ڵ�
* ���ڿռ��ϵ����ƣ����ڹ������ڵ���Ҫ����������ڴ�(���Һ��ӽڵ㣬���ڵ�(��ʡ))��Ҳ�ܲ����ԣ����п��ܱ�
* ���յ�HASH����������ڴ�,���ڿ��������Ψһ�ŵ���Ǿ��ȣ���ЩHASH��ᵼ��ĳ��Ͱ�д����Ķѻ���
* ��ʵ���ڶ����Ӧ�ÿ����У��Ѿ�����ʹ�����ֽṹ
* ��Ȼ��C#��ż����Ӧ�ã������Ƕ�����Ϊ�ֵ伯�ϵײ�ʹ�õĽṹ
* 
* ���������
* 1.�κ�һ���ڵ㶼����ɫ�D�D��ɫ���Ǻ�ɫ��
* 2.���ڵ��Ǻ�ɫ�ġ�
* 3.���е�NIL�ڵ㶼���ɺ�ɫ��NIL�ڵ��Ǿ���һ������Ļ�����ʵ������Ľڵ㣬����Ӧ��ָ��NULL��ָ�룬������ָ����NIL�ڵ㡣����Ҷ�ڵ���ӽڵ�ָ����Ǹ��ڵ�ĸ�ָ�룩��
* 4.���һ���ڵ��Ǻ�ɫ�ģ���ô�����ӽڵ�һ���Ǻ�ɫ�ġ�
* 5.�����κ�һ���ڵ���ԣ��Ӹýڵ㵽��������ڵ��е�NIL�ڵ�·���У��������ĺڽڵ������ͬ
*****************************************************************************************/
#include "tree.h"
#include "common.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

//����ڵ�ʱ�����ؽڵ��λ��
//0��ʾ����ʽ����
//1��ʾ��Ϊ��ڵ�
//2��ʾ��Ϊ�ҽڵ�
//3���ڵ�
typedef enum
{
	CoverNode = 0,
	LeftNode = 1,
	RightNode = 2,
	RootNode = 3
} NodePosition;

typedef void (*NodeCallBack)(TreePtr tree,TreeNodePtr node);

//����һ�����ڵ�
static TreeNodePtr CreateTreeNode(TreePtr tree,void* key,void* value);

//����key���ڵĽڵ�
static TreeNodePtr FindTreeNode(TreePtr tree,void* key);

//����childΪparent�����ӣ�����parentԭ��������
static TreeNodePtr SetLeftChild(TreeNodePtr parent,TreeNodePtr child);

//����childΪparent���Һ��ӣ�����parentԭ�����Һ���
static TreeNodePtr SetRightChild(TreeNodePtr parent,TreeNodePtr child);

//����һ���ڵ�,�����ز���ķ�ʽ
static NodePosition InsertTreeNode(TreePtr tree,TreeNodePtr node);

//����ɫ�ڵ�
//Ҫ�ǽڵ�����͸��ڵ㣬�常�ڵ㶼�Ǻ�ɫ,����Ҫ������ɫ
static void CheckTree(TreePtr tree,TreeNodePtr node);

//���²��Ҹ��ڵ�
static void FindRootNode(TreePtr tree);

//����ת�ڵ�
static void LeftRotate(TreePtr tree,TreeNodePtr node);

//����ת�ڵ�
static void RightRotate(TreePtr tree,TreeNodePtr node);


//�������ĸ��ڵ�
static TreeNodePtr SetRootNode(TreePtr tree,TreeNodePtr node);

//�ݹ�����ڵ��Լ��ӽڵ�
static void RecursiveProcessNode(TreePtr tree,TreeNodePtr node,NodeCallBack fnNode);

//��������ڵ��Լ��ӽڵ�
static void IterationProcessNode(TreePtr tree,TreeNodePtr node,NodeCallBack fnNode);

//��ȡһ���ڵ�������е����ֵ
static TreeNodePtr GetLeftMaxNode(TreeNodePtr node);

//��ȡһ���ڵ��Һ����е���Сֵ
static TreeNodePtr GetRightMinNode(TreeNodePtr node);

//ɾ��һ���ڵ�
static void DeleteTreeNode(TreePtr tree,TreeNodePtr node);


//ɾ���ڵ�֮�󣬶Ժ�����������޸�
static void DeleteNodeFix(TreePtr tree,TreeNodePtr node);

//�ͷ�һ���ڵ���ڴ�
static void FreeNode(TreePtr tree,TreeNodePtr node);

//ö�ٴ������е����ڵ�
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
	//��ǰ�ڵ��Ǹ��ڵ�����ӻ����Һ���
	position = (node == parent->left) ? LeftNode : RightNode;
	brother = (node == parent->left) ? parent->right : parent->left;
	brother_color = (NULL == brother) ? BlackNode : brother->color;
	parent_color = parent->color;
	color = node->color;
	//�ֵܽڵ�ʱ��ɫ
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
		//�����ֵܽڵ��븸�ڵ����ɫ
		brother->color = BlackNode;
		parent->color = RedNode;
		//���������ת��,brother��Ϊ��ɫ,�ֵܽڵ��Ϊ��ɫ
		//����Ҫ����������߼�����
		DeleteNodeFix(tree,node);
	}
	else if ((NULL != brother) && (brother_color == BlackNode))
	{
		brother_left_color = (NULL == brother->left) ? BlackNode : brother->left->color;
		brother_right_color = (NULL == brother->right) ? BlackNode : brother->right->color;
		//ȫ��Ϊ��ɫ case 1
		if ((parent_color == BlackNode) && 
			//(brother_color == BlackNode) && 
			(brother_left_color == BlackNode) && 
			(brother_right_color == BlackNode))
		{
			//������Υ����������ԣ������ֵܽڵ�����һ����ɫ�ڵ㣬�պ�ƽ��
			brother->color = RedNode;
		}//���ڵ�Ϊ��ɫ���������Ǻ�ɫ case 2
		else if ((parent_color == RedNode) && 
			//(brother_color == BlackNode) && 
			(brother_left_color == BlackNode) && 
			(brother_right_color == BlackNode))
		{
			//�����ֵܽڵ��븸�ڵ����ɫ
			//�������ڽ�ɾ���ĺ�ɫ�ڵ㲹�룬�ֵܽڵ�û�б仯
			brother->color = BlackNode;
			parent->color = RedNode;
		}//case 3,תΪ����case 5������
		else if ((position == LeftNode) && 
			(brother_left_color == RedNode) && //�����Ǻ�ɫ
			(brother_right_color == BlackNode))
		{//�����ͱ������һ���Һ����Ǻ�ɫ���ֵܽڵ�
			RightRotate(tree,brother);
			//�ֵܽڵ����������ӽڵ���ɫ
			brother->color = RedNode;
			brother->left->color = BlackNode;
			DeleteNodeFix(tree,node);
		}//case 4,תΪ����case 6������
		else if ((position == RightNode) && //����Ŀǰ���㷨����������ǲ����ܻ��е�
			(brother_left_color == RedNode) && 
			(brother_right_color == BlackNode))//�Һ����Ǻ�ɫ
		{//���һ����ɫ�ֵ�
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
			//��N�����һ����ɫ�ڵ�
			LeftRotate(tree,parent);
		}//case 6
		else if ((position == RightNode) &&
			(brother_left_color == RedNode))
		{
			brother->color = parent->color;
			parent->color = BlackNode;
			brother->left->color = BlackNode;
			//��N�����һ����ɫ�ڵ�
			RightRotate(tree,parent);
		}
		else
		{
			//printf("\r\n �ѵ������������ \r\n");
		}
	}
	else
	{
		//printf("\r\n brother node = null \r\n");
	}
}

//ɾ�����ڵ�,����ʱ�����ڵ㣬���ֻ��һ������,����������
void DeleteTreeNode(TreePtr tree,TreeNodePtr node)
{
	TreeNodePtr brother = NULL;
	TreeNodePtr child = NULL;
	TreeNodePtr parent = NULL;

	parent = node->parent;
	child = (NULL == node->left) ? node->right : node->left;
	//child = node->left;

	//û�и��ڵ����Ů�ڵ�,��Ψһһ���ڵ�,case 1
	if ((NULL == child) && (NULL == parent))
	{
		SetRootNode(tree,NULL);
	}
	else if ((NULL != child) && (NULL == parent))
	{
		//ֻ��һ�����ӽڵ�,û�и��ڵ�,case 2
		SetRootNode(tree,child);
	}
	else if ((NULL == child) && (NULL != parent))
	{
		//û�к��ӽڵ�,�и��ڵ�
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
		//�к��ӽڵ�,�и��ڵ�
		if (node == parent->left)
		{
			SetLeftChild(parent,child);//child = NULL
		}
		else
		{
			SetRightChild(parent,child);//child = NULL
		}
		//N=B����P=R,C=Rʱ����Ҫ�����ӽڵ�����Ϊ��ɫ
		child->color = BlackNode;
		if (BlackNode == node->color)
		{
			//����ڵ��Ѿ�������node��λ��
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


//�ݹ�ɾ���ڵ��Լ���Ӧ�������ӽڵ�
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
	//Ѱ�Ҹ��ڵ�,����ת�ͽ�����,���ڵ�����׻�λ
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
	//���ڵ㣬�游�ڵ㣬�常�ڵ�
	TreeNodePtr parent,grand,uncle;//great

	//�븸�ڵ�Ĺ�ϵ(���ӻ����Һ���),���ڵ����游�ڵ�Ĺ�ϵ
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
	//���ڵ�ΪNULL�����ʾ�Ǹ��ڵ�
	if (NULL == parent)
	{
		SetRootNode(tree,node);
	}
	else if ((node->color == RedNode) && (parent->color == RedNode))
	{
		//����ڵ� ���ڵ�Ϊ��ɫ
		//���ʱ��һ������һ���游�ڵ�
		grand = parent->parent;
		uncle = (grand->left == parent) ? grand->right : grand->left;
		//����ڵ� ���ڵ� �常�ڵ㶼Ϊ��ɫ
		if ((NULL != uncle) && (uncle->color == RedNode))
		{
			grand->color = RedNode;
			parent->color = BlackNode;
			uncle->color = BlackNode;
			CheckTree(tree,grand);
			//���游�ڵ���Ϊ�²���Ľڵ㿪ʼ������ĺϷ���
		}
		else  //�常�ڵ��Ǻ�ɫ���߿սڵ�
		{
			//����ڵ� ���ڵ�Ϊ��ɫ �常�ڵ�Ϊ��ɫ��NULL��ʾ��ɫ��
			selfPosition = (node == parent->left) ? LeftNode : RightNode;
			parentPosition = (parent == grand->left) ? LeftNode : RightNode;
			//��ת��Ϊ ���� ���� ���� ����
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
				//����������Ҫ�����ڵ���ɫ,����֪�������游�ڵ�϶��Ǻ�ɫ
				parent->color = BlackNode;
				grand->color = RedNode;
			}
			else if ((parentPosition == RightNode) && (selfPosition == LeftNode))
			{
				//����
				//�Ը��ڵ���ת�����ʱ�򽫱�����ҵĹ�ϵ
				RightRotate(tree,parent);
				//Ҫ�Ǽ��ٵݹ飬������תһ��
				//CheckTree(tree,node);
				LeftRotate(tree,grand);
				parent->color = BlackNode;
				grand->color = RedNode;
			}
			else if ((parentPosition == LeftNode) && (selfPosition == RightNode))
			{
				//����
				//�Ը��ڵ���ת�����ʱ�򽫱������Ĺ�ϵ
				LeftRotate(tree,parent);
				//�´μ����ᷢ���������ϵ
				//Ҫ�Ǽ��ٵݹ飬������תһ��
				//CheckTree(tree,node);
				RightRotate(tree,grand);
				parent->color = BlackNode;
				grand->color = RedNode;
			}
			else
			{
				//����
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
		//���﷢���游�ڵ�϶���Ⱦ���˺�ɫ�������ڵ㶼��Ⱦ�ɺ�ɫ
		//��Ϊ������Ǻ�ɫ�ڵ㣬��һ����ڵ�������������ɫ�ڵ�
		//��
	}
	else
	{
		//������ʾ�����������
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
		//������ͬKEY�Ľڵ�
		if (compare == Compare_Equal)
		{
			//����VALUE
			memcpy(parent->keyValue + tree->keyByteSize,node->keyValue + tree->keyByteSize,tree->keyByteSize);
			ret = True;
			nodePosition = CoverNode;
			break;
		}
		else if (compare == Compare_Less)
		{
			//���ڵ�С�ڵ�ǰ�ڵ�,��Ϊ���ڵ���Һ���
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
			//���ڵ���ڵ�ǰ�ڵ�,��Ϊ���ڵ������
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

	//һֱ��NULL��û�в���,�Ǿ��Ǹ��ڵ���
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

//����childΪparent������
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

//����ת
void LeftRotate(TreePtr tree,TreeNodePtr node)
{
	//��ȡ�ڵ�ĸ��ڵ�
	TreeNodePtr parent = node->parent;

	//��Ϊ�����Һ��ӵ�����
	//����ԭ���Һ��ӵ����ӣ����ӣ�,�������Һ���û��
	TreeNodePtr left = SetLeftChild(node->right,node);

	//���Һ�����Ϊ���ڵ��(��,��)����,�Һ��ӽڵ�ճ�
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

	//��֮ǰ�Һ��ӵ���������Ϊ�Һ���
	SetRightChild(node,left);
}

//����ת
void RightRotate(TreePtr tree,TreeNodePtr node)
{
	//��ȡ�ڵ�ĸ��ڵ�
	TreeNodePtr parent = node->parent;

	//��Ϊ�������ӵ��Һ���
	//����ԭ�����ӵ��Һ��ӣ����ӣ�,�������Һ���û��
	TreeNodePtr right = SetRightChild(node->left,node);

	//��������Ϊ���ڵ���Һ���,���ӽڵ�ճ�

	//���Һ�����Ϊ���ڵ��(��,��)����,�Һ��ӽڵ�ճ�
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
		//��ʾ�Ǹ��ڵ�,���ڵ�û�и��ڵ�
		SetRootNode(tree,node->left);
	}

	//��֮ǰ���ӵ��Һ�������Ϊ���������
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
		//����Ľڵ�Ϊ���ڵ�����ӻ����Һ���ʱ����Ҫ������ĺϷ���
		//����Ǹ���ʽ����ֱ�Ӳ�����ڵ��������ٿ���
		if (position == RootNode)
		{
			//���ڵ����Ϊ��ɫ
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
			//����ʽ����,ʲôҲ������ԭ�е�value�����÷���
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
			//���׻����޺��ӽڵ�
			child = node;
		}
		else
		{
			//��ȡ���ӽڵ��µ����ӽڵ�������ֵ
			//����ڵ��Ȼû���Һ���
			child = GetLeftMaxNode(node);


			//�滻KEY��VALUE
			memcpy(node->keyValue,child->keyValue,tree->keyByteSize + tree->valueByteSize);
		}
		DeleteTreeNode(tree,child);
	}
	else
	{
		//printf("\r\nnode is null!\r\n");
	}
}