#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>

#include <iostream>
using namespace std;

typedef unsigned int uint;


// Merkle Tree 结构体定义 
typedef struct MerkleTreeNode {
	struct MerkleTreeNode* left;
	struct MerkleTreeNode* right;
	struct MerkleTreeNode* parent;
	uint hash_num;		// 哈希值 ，如果创建树过程中，该值为当前节点的树深度 
	uint data;
	char hash[65];
}MerkleTree;

// 定义宏函数，创建一个 Merkle Tree节点,hash_num 暂时保存其树深度，0代表是叶子节点

#define New_Merkle_Node(mt, tree_depth) {	\
	mt = (MerkleTree *)malloc(sizeof(MerkleTree)); \
	mt->left = NULL; \
	mt->right = NULL; \
	mt->parent = NULL; \
	mt->hash_num = (uint)tree_depth; \
	mt->data = 0;	\
	mt->hash;    \
	}
void tDigest(char msg[], MerkleTree* mt)
{
	unsigned char sm3_value[EVP_MAX_MD_SIZE];   //保存输出的摘要值的数组
	unsigned int sm3_len, i;
	EVP_MD_CTX* sm3ctx;                         //EVP消息摘要结构体
	sm3ctx = EVP_MD_CTX_new();//调用函数初始化
	
	EVP_MD_CTX_init(sm3ctx);                    //初始化摘要结构体
	EVP_DigestInit_ex(sm3ctx, EVP_sm3(), NULL); //设置摘要算法和密码算法引擎，这里密码算法使用sm3，算法引擎使用OpenSSL默认引擎即软算法
	EVP_DigestUpdate(sm3ctx, msg, strlen(msg));//调用摘要UpDate计算msg的摘要

	EVP_DigestFinal_ex(sm3ctx, sm3_value, &sm3_len);//摘要结束，输出摘要值   
	EVP_MD_CTX_reset(sm3ctx);                       //释放内存
	char hash_1[65];
	sprintf_s(hash_1, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
		sm3_value[0], sm3_value[1], sm3_value[2], sm3_value[3], sm3_value[4], sm3_value[5], sm3_value[6], sm3_value[7],
		sm3_value[8], sm3_value[9], sm3_value[10], sm3_value[11], sm3_value[12], sm3_value[13], sm3_value[14], sm3_value[15],
		sm3_value[16], sm3_value[17], sm3_value[18], sm3_value[19], sm3_value[20], sm3_value[21], sm3_value[22], sm3_value[23],
		sm3_value[24], sm3_value[25], sm3_value[26], sm3_value[27], sm3_value[28], sm3_value[29], sm3_value[30], sm3_value[31]);
	for (int i = 0; i < sizeof(mt->hash); i++)
	{
		mt->hash[i] = hash_1[i];
	}
}


// 打印 Merkle tree 
int first = 0;
void Print_Merkle_Tree(MerkleTree* mt, int high)
{
	MerkleTree* p = mt;
	int i;

	if (p == NULL) {
		return;
	}
	if (p->left == NULL && p->right == NULL) {
		printf("\n");
		for (i = 0; i < high - p->hash_num; i++)
			printf("      ");

		cout << p->hash[0] << p->hash[1] << p->hash[2] << p->hash[3];
		printf("--->%2d\n", p->data);
		first = 1;

		return;
	}
	else {
		Print_Merkle_Tree(mt->left, high);

		if (first == 1) {
			for (i = 0; i < high - p->hash_num; i++)
				printf("      ");

			printf("--->");
		}
		else
			printf("--->");

		cout << p->hash[0] << p->hash[1] << p->hash[2] << p->hash[3];
		first = 0;

		Print_Merkle_Tree(mt->right, high);
		//printf("\n");
	}
}


// 计算叶子节点的hash值—— hash（0x00||m)
void hash_int(int num, MerkleTree* mt) {
	char temp_1[2];
	sprintf_s(temp_1, "%x", num);
	char temp_2[sizeof(temp_1)+2] = "0";
	strcpy_s(temp_2+1, sizeof(temp_1)+1,temp_1);
	tDigest(temp_2,  mt);

}

//计算内部节点的hash值—— hash（0x01||h1||h2)
void hash_uint(MerkleTree* Left,MerkleTree* Right, MerkleTree* mt) {
	if (Right == NULL)
	{
		for (int i = 0; i < sizeof(mt->hash); i++)
		{
			mt->hash[i] = Left->hash[i];
		}
	}
	else
	{
		char* temp_1 = Left->hash;
		char* temp_2 = Right->hash;
		strcpy_s(temp_1+strlen(temp_1),strlen(temp_2)+1, temp_2);
		char temp_3[130] = "1";
		strcpy_s(temp_3+1, strlen(temp_1) + 1, temp_1);
		tDigest(temp_3, mt);

	}
}

// 遍历二叉树，如果找到空的叶子节点返回叶子结点指针
// 如果没有找到，即为满二叉树，则返回 NULL 
// 先序遍 
// 找到最后一个节点 
MerkleTree* Find_Last_Node(MerkleTree* mt) {
	MerkleTree* p = mt, * tmp;

	if (p->left == NULL && p->right == NULL)	// 叶子节点 
		return p;
	else if (p->right == NULL && p->left != NULL)
		return Find_Last_Node(p->left);
	else if (p->right != NULL)
		return Find_Last_Node(p->right);
}

// 根据最后一个节点，找到插入的位置 
MerkleTree* Find_Empty_Node(MerkleTree* mt) {
	MerkleTree* p = mt->parent;

	while (p->left != NULL && p->right != NULL && p->parent != NULL) {
		p = p->parent;
	}
	if (p->parent == NULL && p->left != NULL && p->right != NULL) {		// 到顶了
		//printf("当前节点位置，p->hash_num=%d, 到顶了！！！ \n", p->hash_num); 
		return NULL;
	}
	else {
		//printf("当前节点位置，p->hash_num=%d \n", p->hash_num); 
		return p;
	}
}

// 更新树的哈希值 
void update_hash_Merkle(MerkleTree* mt, int tree_depth)
{
	if (mt != NULL)
	{
		if (mt->hash_num == 0) {
			hash_int(mt->data,mt);
		}
		else {
			update_hash_Merkle(mt->left, tree_depth - 1);
			update_hash_Merkle(mt->right, tree_depth - 1);
			hash_uint(mt->left, mt->right, mt);
		}
	}
}


// Merkle tree 初始化 (递归实现)
MerkleTree* Creat_Merkle_Tree(MerkleTree* mt, int* arr, int nums, int tree_depth)
{
	MerkleTree* node, * tmp, * p;
	int i;
	if (nums == 0) {
		// nums 等于0时，数据添加完毕，此时返回merkle tree头结点
		// 更新节点的哈希值  
		update_hash_Merkle(mt, tree_depth);
		printf("创建完毕\n");

		if (mt != NULL) {
			first = 0;
			printf("\n开始打印当前 Merkle 树:\n");
			Print_Merkle_Tree(mt, mt->hash_num);
			printf("\n");
		}
		return mt;
	}
	else {
		
		// 每次添加一个叶子节点，优先左树，其次右树
		// 创建一个树结点 
		New_Merkle_Node(node, 0);
		node->data = *arr;

		// 如果 mt 为空，说明当前没有树	
		if (mt == NULL) {
			// 创建头结点
			New_Merkle_Node(mt, 1);
			mt->left = node; 	// 结头节点赋值 
			node->parent = mt;
			// 当前树高度 +1 
			tree_depth++;

			// 递归
			
			mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
		}
		// 如果 mt 不为空,mt为头结点 
		else
		{
			p = Find_Empty_Node(Find_Last_Node(mt));	// 遍历当前树，找到一个空的叶子节点，满二叉树时返回NULL 

			// 如果flag 为1 说明存在空的 右叶子节点 
			if (p != NULL) {
				// 如果最底下就是叶子节点，就直接赋值 
				if (p->left->hash_num == 0 && p->right == NULL)
				{
					p->right = node;
					node->parent = p;
				}
				else
				{
					i = p->hash_num - 1;
					// 创建一个新的头结点
					New_Merkle_Node(tmp, i);
					p->right = tmp;
					tmp->parent = p;

					p = p->right;
					

					i--;
					// 根据树的深度创建同样深度的左树 
					while (i > 0) {
						// 创建结点
						New_Merkle_Node(tmp, i);
						p->left = tmp;
						tmp->parent = p;

						p = p->left;
						i--;
					}

					// 叶子节点赋值 
					p->left = node;
					node->parent = p;
				}
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
			}
			//如果没有空的叶子节点，则新建一个头结点 
			else
			{
				tmp = mt;	// 保存当前头结点
				tree_depth++; 		// 树高度 +1 

				// 创建一个新的头结点
				New_Merkle_Node(mt, tree_depth);
				mt->left = tmp; 	// 结头节点赋值 
				tmp->parent = mt;

				// 创建头结点 -  叶子节点 之间的所有节点 
				i = tree_depth - 1;	// 第二层节点 

				// 头结点 right 赋值  
				New_Merkle_Node(tmp, i);
				mt->right = tmp;
				tmp->parent = mt;

				i--;
				p = mt->right;
				

				// 根据树的深度创建同样深度的左树 
				while (i > 0) {
					// 创建结点
					New_Merkle_Node(tmp, i);
					p->left = tmp;
					tmp->parent = p;

					p = p->left;
					i--;
				}
				// 叶子节点赋值 
				p->left = node;
				node->parent = p;

				// 递归调用 
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
			}
		}
	}
}

int main(void)
{
	OpenSSL_add_all_algorithms();
	int array[] = { 0, 1, 2, 3 ,4 ,5 ,6, 7};
	MerkleTree* mt = NULL;
	mt = Creat_Merkle_Tree(mt, array, sizeof(array) / sizeof(int), 0);
  	return 0;
}
