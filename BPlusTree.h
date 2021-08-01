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

#define DEGREE 2
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
  char idcard[STRINGLENGTH];
};

struct BPlusTreeNode
{
  int leaf;
  int num;
  KEYTYPE key[DEGREE_2];
  PBTreeNode children[DEGREE_2];    /* 叶子节点中，children无效 */
  PBTreeNode next;
  DATATYPE *data[DEGREE_2];    /* 只有在叶节点中，data才有效; 如果是非叶节点，且data有效，说明这是根节点 */
};

struct BPlusTree
{
  int total_key_num;
  PBTreeNode root;
  PBTreeNode first;
};

struct RangeData
{
  DATATYPE *data;
  KEYTYPE *key;
  int num;
};

extern BPTree initialize();
extern DATATYPE* search(BPTree T, KEYTYPE key);
extern BPTree update(BPTree T, KEYTYPE KEY, DATATYPE newData);
extern BPTree insert(BPTree T, KEYTYPE key, DATATYPE data);
extern BPTree removeKey(BPTree T, KEYTYPE key);
extern RangeDataes searchRange(BPTree T, KEYTYPE begin, KEYTYPE end);
extern BPTree destroy(BPTree T);

extern KEYTYPE* travel(BPTree T);
extern KEYTYPE* travelN(BPTree T,int count);
extern DATATYPE* travelData(BPTree T);
extern DATATYPE* travelDataN(BPTree T,int count);

extern void showRange(RangeDataes dataes);
extern void showBPlusTree(BPTree T);

extern int serialize(const char *filePath, BPTree tree);
extern BPTree deserialize(const char *filePath);


#endif /* BPlusTree.h */