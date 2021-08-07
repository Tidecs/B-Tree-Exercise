#ifndef _B_PLUS_TREE_H
#define _B_PLUS_TREE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<limits.h>
#include<time.h>

#define DEGREE 8
#define DEGREE_2 (DEGREE * 2)
#define true 1
#define false 0

#define STRINGLENGTH 32
#define LEAF_LENGTH 4
#define NUM_LENGTH 4
#define KEY_LENGTH 4

typedef struct BPlusTreeNode *PBTreeNode;
typedef struct BPlusTree *BPTree;
typedef int KEYTYPE;
typedef struct MyData DATATYPE;
typedef struct RangeData RangeDataes;

struct MyData
{
  char idcard[STRINGLENGTH];    /* 自定义的卫星数据结构 */
};

struct BPlusTreeNode  
{
  int leaf;    /* 标识是否为叶子结点 */
  int num;     /* 结点中键值或孩子或数据的数目 */
  KEYTYPE key[DEGREE_2];
  PBTreeNode children[DEGREE_2];    /* 叶子节点中，children无效 */
  PBTreeNode next;    /* 在叶子结点中有效，指向下一个叶子结点 */
  DATATYPE *data[DEGREE_2];    /* 只有在叶节点中，data才有效; 如果是非叶节点，且data有效，说明这是根节点（只有一层） */
};

struct BPlusTree
{
  int total_key_num;    /* 整个b+树中键值或数据的数目 */
  PBTreeNode root;    /* 指向b+树的根结点 */
  PBTreeNode first;    /* 指向b+树的第一个叶子结点 */
};

struct RangeData
{
  DATATYPE *data;    /* 指向范围查询得到的数据集合 */
  KEYTYPE *key;    /* 指向范围查询得到的键值集合 */
  int num;    /* 数据或键值集合的数目 */
};

/* b+树初始化 */
extern BPTree initialize();

/* 
**功能：查找数据
**输入：b+树指针，键值
**返回：指向查找键值对应数据的指针 
*/
extern DATATYPE* search(BPTree T, KEYTYPE key);

/*
**功能：更新数据
**输入：b+树指针，键值，键值对应的需要更新的数据
**返回：b+树指针
*/
extern BPTree update(BPTree T, KEYTYPE KEY, DATATYPE const *newData);

/*
**功能：插入键值及数据
**输入：b+树指针，要插入的键值，插入键值对应的数据
**返回：b+树指针
*/
extern BPTree insert(BPTree T, KEYTYPE key, DATATYPE const *data);

/*
**功能：删除键值及对应的数据
**输入：b+树指针，键值
**返回：b+树指针
*/
extern BPTree removeKey(BPTree T, KEYTYPE key);

/*
**功能：查询键值在[begin,end]范围内，对应的<键值，数据>的集合
**输入：b+树指针，下限键值，上限键值
**返回：指向所查询的<键值，数据>集合的指针
*/
extern RangeDataes* searchRange(BPTree T, KEYTYPE begin, KEYTYPE end);

/*
**功能：销毁b+树，释放空间
**输入：b+树指针
**返回：若b+树销毁成功，返回空指针。
*/
extern BPTree destroy(BPTree T);

extern void destroyRangedata(RangeDataes *data);


/*
**功能：遍历b+树键值
**输入：b+树指针
**返回：指向键值集合的指针
*/
extern KEYTYPE* travel(BPTree T);

/*
**功能：遍历b+树键值前N个
**输入：b+树指针
**返回：指向N个键值集合的指针
*/
extern KEYTYPE* travelN(BPTree T,int count);

/*
**功能：遍历b+树卫星数据
**输入：b+树指针
**返回：指向卫星数据集合的指针
*/
extern DATATYPE* travelData(BPTree T);

/*
**功能：遍历b+树卫星数据前N个
**输入：b+树指针
**返回：指向N个卫星数据集合的指针
*/
extern DATATYPE* travelDataN(BPTree T,int count);


extern void showRange(RangeDataes const *dataes);
extern void showBPlusTree(BPTree T);

/*
**功能：序列化b+树到文件
**输入：文件路径，b+树指针
**返回：整型，1标识成功，0标识失败
*/
extern int serialize(const char *filePath, BPTree tree);

/*
**功能：反序列化文件到b+树
**输入：文件路径
**返回：序列化成功的b+树，失败返回NULL
*/
extern BPTree deserialize(const char *filePath);


#endif /* BPlusTree.h */