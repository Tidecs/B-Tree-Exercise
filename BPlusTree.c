#include "BPlusTree.h"

static KEYTYPE unavailable = INT_MIN;
static void copyData(DATATYPE *dest, DATATYPE const *source);
static void moveKeyFromAToB(PBTreeNode x, int a, int b);

static PBTreeNode createNode(int isLeaf)
{
  PBTreeNode new_node;
  new_node = (PBTreeNode)calloc(1,sizeof(struct BPlusTreeNode));

  new_node->leaf = isLeaf;
  new_node->num = 0;
  new_node->next = NULL;
  for(int i = 0; i<DEGREE_2 ; i++)
  {
    new_node->data[i] = NULL;
  }

  for (int i=0; i < DEGREE_2 ; i++)
  {
    new_node->key[i] = unavailable;
    new_node->children[i] = NULL;
  }

  return new_node; 
} 

extern BPTree initialize()
{
  BPTree btree;

  btree = (BPTree)calloc(1,sizeof(struct BPlusTree));
  btree->root = createNode(true);
  btree->first = btree->root;
  btree->total_key_num = 0;

  //DATATYPE data = { "head" };
  //insert(btree, unavailable, data);
  //insert(btree, 15, data);

  return btree;
}


static PBTreeNode findMostLeft(PBTreeNode x)
{
  PBTreeNode temp = x;
  while(temp != NULL && !temp->leaf)
  {
    temp = temp->children[0];
  }

  return temp;
}

static PBTreeNode findMostRight(PBTreeNode x)
{
  PBTreeNode temp = x;
  while(temp != NULL && !temp->leaf)
  {
    temp = temp->children[temp->num-1];
  }

  return temp;
}

/* 
**功能：找到结点x中键值小于等于targetKey的最后一个值的下标
**输入：结点指针，键值
**返回：下标
*/
static int findUpperPosition(PBTreeNode x, KEYTYPE targetKey)
{
  int low = 0;
  int high = x->num-1;
  int mid;

  while(low < high)
  {
    mid = (low + high + 1) / 2;
    if(targetKey < x->key[mid])
    {
      high = mid-1;
    }
    else
    {
      low = mid;
    }
  }

  return high;
}

/* 
**功能：找到结点x中键值大于等于targetKey的第一个值的下标
**输入：结点指针，键值
**返回：下标
*/
static int findLowerPosition(PBTreeNode x, KEYTYPE targetKey)
{
  int low = 0;
  int high = x->num-1;
  int mid;
  
  while(low < high)
  {
    mid = (low + high) / 2;
    if(x->key[mid] < targetKey)
    {
      low = mid + 1;
    }
    else
    {
      high = mid;
    }
  }

  return low;
}

/* 
**功能：找到结点x中键值小于targetKey的最后一个值的下标
**输入：结点指针，键值
**返回：下标
*/
static int findLastLTPosition(PBTreeNode x, KEYTYPE targetKey)
{
  int low = 0, high = x->num-1;
  int mid;

  while(low < high)
  {
    mid = (low + high +1) / 2;
    if(targetKey <= x->key[mid])
    {
      high = mid - 1;
    }
    else
    {
      low = mid;
    }
  }

  return high;
}


extern DATATYPE* search(BPTree T, KEYTYPE key)
{
  PBTreeNode pnode = T->root;
  KEYTYPE findKey = unavailable;
  int position = findUpperPosition(pnode, key);

  while(!pnode->leaf)
  {
    pnode = pnode->children[position];
    position = findUpperPosition(pnode, key);
  }

  if(pnode->key[position] == key){
    return pnode->data[position];
  }

  return NULL;
}

extern RangeDataes* searchRange(BPTree T, KEYTYPE begin, KEYTYPE end)
{
  PBTreeNode pnode = T->root;
  KEYTYPE findKey = unavailable;
  int position = INT_MIN;
  RangeDataes *dataes = (RangeDataes*)malloc(sizeof(RangeDataes));
  dataes->data = NULL;
  dataes->key = NULL;
  dataes->num = 0;

  while(!pnode->leaf)
  {
    position = findLastLTPosition(pnode, begin);
    pnode = pnode->children[position];
  }
  position = findLowerPosition(pnode, begin);

  //如果当前结点内都是比begin小的值，那么就移动到下一结点 
  if(pnode->key[position] < begin)
  {
    pnode = pnode->next;
    if(pnode == NULL)
    {
      return dataes;
    }
    position = 0;
  }

  //偏移量是进入新结点后的开始索引,num记录当前已存储key的数量
  int offset = 0, num = 0;
  int size = DEGREE_2;
  DATATYPE *search_data = (DATATYPE*)malloc(size * sizeof(DATATYPE));
  KEYTYPE *search_key = (KEYTYPE*)malloc(size * sizeof(KEYTYPE));

  //处理当前结点,position 是当前结点大于等于begin的第一个key的位置
  for(int i = position; i<pnode->num && pnode->key[i] <= end ; i++)
  {
    copyData(&search_data[offset + i-position], pnode->data[i]);
    search_key[offset + i - position] = pnode->key[i];
    num++;
  }

  //处理接下来一部分结点，需要增加分配空间
  offset += num;
  while(pnode->key[pnode->num - 1] <= end && pnode->next != NULL )
  {
    pnode = pnode->next;
    search_data = (DATATYPE*)realloc(search_data, (size + num) * sizeof(DATATYPE));
    search_key = (KEYTYPE*)realloc(search_key, (size + num) * sizeof(KEYTYPE));

    int i;    //i是当前结点中小于等于end的key 的数量
    for(i = 0; i<pnode->num && pnode->key[i] <= end ; i++)
    {
      copyData(&search_data[offset + i], pnode->data[i]);
      search_key[offset + i] = pnode->key[i];
      num++;      
    }
    offset += i;    //这里从for循环退出后，i 已经是 pnode->num 的值了
  }

  //收缩分配空间，将数据返回
  search_data = (DATATYPE*)realloc(search_data, num * sizeof(DATATYPE));
  search_key = (KEYTYPE*)realloc(search_key, num * sizeof(KEYTYPE));

  dataes->data = search_data;
  dataes->key = search_key;
  dataes->num = num;

  return dataes;
}

extern BPTree update(BPTree T, KEYTYPE key, DATATYPE const *newData)
{
  DATATYPE *data = search(T, key);
  if(data == NULL)
  {
    printf("update error!\n");
    return NULL;
  }
  copyData(data, newData);

  return T;
}


/* 
**功能：分裂满结点y = x->children[i]，此时y->num = 2t
**输入：需要分裂结点的父结点，在父结点的孩子数组中的下标i
**具体描述：
**1.新建结点z，将x->children[i]的后t个键值及孩子或数据复制到z
**2.调整z->num = y->num = t
**3.将x中下标为 i+1 到 x->num-1 的<键值，数据>右移，将z插入到i+1位置
*/
static void btreeSplitChild(PBTreeNode x, int i) 
{ 
  PBTreeNode y = x->children[i];
  PBTreeNode z = createNode(y->leaf);

  z->num = DEGREE;
 
  int j = 0;
  for (j = 0; j < DEGREE ; j++) 
  {
    z->key[j] = y->key[j+DEGREE];
  }
  if (!y->leaf) 
  {
    for (j = 0; j < DEGREE ; j++) 
    {
      z->children[j] = y->children[j+DEGREE];
    }
  }
  else
  {
    z->next = y->next;
    y->next = z;
    for(j = 0; j<DEGREE ; j++)
    {
      z->data[j] = y->data[j+DEGREE];
    }
  }
  y->num = DEGREE;

  for (j = x->num; j > i+1 ; j--) 
  {
    x->key[j] = x->key[j-1];
    x->children[j] = x->children[j-1];
  }
  x->key[i+1] = z->key[0];
  x->children[i+1] = z;
  x->num += 1;
}

static void copyData(DATATYPE *dest, DATATYPE const *source)
{
  strcpy(dest->idcard, source->idcard);
}

/* 
**功能：将<键值,数据>插入到一个非满结点x
**输入：结点x，键值x，数据data
**返回：插入后结点的第一个Key值，也就是最小key值
**具体描述：
**1.如果是叶子结点，将大于插入键值的key右移一个位置，然后插入数据即可
**2.如果是内部结点，找到应该插入位置对应的孩子结点。若孩子结点已满，分裂该
**结点，调整插入孩子结点的位置（y 还是 z）继续递归插入;若孩子结点未满，直
**接递归插入数据。
**3.x->key[i] = btreeInsertNonfull(x->children[i], k, data);
**  ...
**  return x->key[0];//此处key[0]是已经插入新数据后的key[0]
**  这两行代码保证了若孩子结点key[0]被修改后，可以在递归回溯后修改父亲结
** 点的key值
*/
static KEYTYPE btreeInsertNonfull(PBTreeNode x, KEYTYPE k, DATATYPE const *data) 
{
  int i = x->num -1;
 
  if (x->leaf) 
  {
    //x->data[i+1] = (DATATYPE*)calloc(1,sizeof(DATATYPE));
    while (i >= 0 && x->key[i] > k) 
    {
      x->key[i+1] = x->key[i];
      //copyData(x->data[i+1], x->data[i]);
      x->data[i+1] = x->data[i];
      i--;
		}

    x->data[i+1] = (DATATYPE*)calloc(1,sizeof(DATATYPE));
    copyData(x->data[i+1], data);    
    x->key[i+1] = k;
    x->num += 1;

    return x->key[0];
	} 
  else 
  {
    while (i >= 0 && x->key[i] > k) i--;    //这行代码，无法处理在key[i]中比插入比key[i]的值
    if(i == -1) i+=1;    //用这行代码作为补充

    if (x->children[i]->num == DEGREE_2) {
      btreeSplitChild(x, i);
      if (k >= x->key[i+1]) i++;    //将 k > x->key[i+1] 条件修改了 为 >= 
    }
    //这行代码保证了若孩子结点key[0]被修改后，可以在递归回溯后修改父亲结点
    x->key[i] = btreeInsertNonfull(x->children[i], k, data);    
    return x->key[0];
	}
}

/* 
**功能：插入<key，data>，这是生成新结点并为之分配空间的唯一函数。
**输入：b+树，键值，数据
**返回：b+树
**具体描述：
**1.令b+树T的总键值数+1
**2.若根结点未满，调用btreeInsertNonfull直接插入数据。若根结点r满，令新
**结点node为根结点，node的第一个孩子指向r,node的第一个键值为r->key[0]
**然后再分裂r，并调用btreeInsertNonfull插入数据
*/
extern BPTree insert(BPTree T, KEYTYPE key, DATATYPE const *data) 
{
  T->total_key_num += 1;
  PBTreeNode r = T->root;
  if (r->num == DEGREE_2) {
		
    PBTreeNode pnode = createNode(false);
    T->root = pnode;
 
    pnode->children[0] = r;
    pnode->key[0] = r->key[0];
    pnode->num = 1;
 
    btreeSplitChild(pnode, 0);  //分裂原根结点r
 
    int i = 0;
    if (pnode->key[i+1] <= key) i++;    //将条件 pnode->key[i+1] < key 修改为 <=
    btreeInsertNonfull(pnode->children[i], key, data);
		
  } else {
    btreeInsertNonfull(r, key, data);
  }

  return T;
}


static void showBPNode(PBTreeNode pnode, int layer)
{
  if(pnode == NULL)
    printf("Node is empty!\n");
  else
  {
    PBTreeNode node = pnode;

    printf("\nlayer is %d, key_num is %d, is_leaf is %d\n", layer++, node->num, node->leaf);
    printf("keys are: ");
    for(int k = 0; k<node->num ; k++)
    {
      printf("%d ",node->key[k]);
    }
    printf("\n");

    if(node->leaf)
    {
      printf("dataes are: \n");
      for(int k = 0; k<node->num ; k++)
      {
        printf("%d: %s\n", node->key[k], node->data[k]->idcard);
      }
    }
    else
    {
      for(int k = 0; k<node->num ; k++)
      {
        showBPNode(node->children[k], layer);
      }
    }
  }
}

extern void showBPlusTree(BPTree T)
{
  if(T->root == NULL)
  {
    printf("Tree is empty!");
  }
  else
  {
    showBPNode(T->root, 0);
  }
}

extern void showRange(RangeDataes const *dataes)
{
  for(int i = 0; i<dataes->num ; i++)
  {
    printf("key %d is %s\n",dataes->key[i], dataes->data[i].idcard);
  }
}


extern KEYTYPE* travelN(BPTree T, int num)
{
  PBTreeNode pnode = T->first;
  KEYTYPE *keys = (KEYTYPE*)calloc(num,sizeof(KEYTYPE));    //初始化的 -1,head 不计入total_key_num中 
  int offset = 0;

  while(pnode != NULL)
  {
    for(int i = 0; i+offset<num+1 && i<pnode->num ; i++)
    {
      keys[i+offset] = pnode->key[i];
    }
    offset += pnode->num;
    pnode = pnode->next;
  }

  return keys;
}

extern KEYTYPE* travel(BPTree T)
{
  return travelN(T,T->total_key_num);
}

extern DATATYPE* travelDataN(BPTree T, int num)
{
  PBTreeNode pnode = T->first;
  DATATYPE* dataes = (DATATYPE*)calloc(num, sizeof(DATATYPE));
  int offset = 0;

  while(pnode != NULL)
  {
    for(int i = 0; i<pnode->num ; i++)
    {
      copyData(dataes+(i+offset), pnode->data[i]); 
    }

    offset += pnode->num;
    pnode = pnode->next;
  }

  return dataes;
}

extern DATATYPE* travelData(BPTree T)
{
  return travelDataN(T, T->total_key_num);
}


static void deleteButNotDestroyBPlusNode(PBTreeNode x)
{
  free(x);
  x = NULL;
}

/* 
**功能：合并两个num == t 的相邻结点，children[i+1]合并到children[i]
**输入：父结点x指针, 相邻结点中下标较小的结点的下标
**具体描述：
**1.将结点z的0～t-1的key以及data或children复制回y的t~2t-1
**2.调整y->num = 2t ,如果是叶子结点，调整next指针，y->next = z->next
**3.调整父亲结点，下标 index+2～num-1 的键值及孩子结点指针左移。
*/
static void btreeMergeChild(PBTreeNode x, int index)
{
  PBTreeNode y = x->children[index];
  PBTreeNode z = x->children[index+1];
  if(y->num > DEGREE || z->num > DEGREE)
  {
    printf("Merge error!");
    return;
  }

  y->num = DEGREE_2;
  if(y->leaf)
  {
    for(int j = 0; j<DEGREE ; j++)
    {
      y->key[j+DEGREE] = z->key[j];
      //copyData(y->data[j+DEGREE], z->data[j]);    //此处有错误2
      y->data[j+DEGREE] = z->data[j];    //修正错误2
    }
    y->next = z->next;
  }
  else
  {
    for(int i = 0; i<DEGREE ; i++)
    {
      y->key[i+DEGREE] = z->key[i];
      y->children[i+DEGREE] = z->children[i];
    }
  }

  deleteButNotDestroyBPlusNode(z);

  for(int j = index+1; j<x->num-1 ; j++)
  {
    x->key[j] = x->key[j+1]; 
    x->children[j] = x->children[j+1];
  }
  x->num -= 1;
}

/* 
**功能：从一个 num>t 的结点中删除<key,data>。
**输入：结点指针x,键值key
**返回：结点在删除操作完成后的key[0]
**具体描述：
**1.加强的限制条件：删除路径中，沿途结点在进入前,通过借或合并，保证num>t
**2.若是叶子结点，直接调整data和key中的元素位置，删除目标<key，data>
**3.若是内部结点，找到要递归删除的孩子结点的下标position，如果孩子结点
**num>t,直接递归删除；如果num == t，则需要借或者合并。
**4.若兄弟结点有任一个结点num>t，则向该兄弟结点借一个前驱或后继，否则与兄**弟结点合并。
**5.x->key[position] = btreeRemoveGTDegree(x->children[position], key);
**  ...
**  return x->key[0];//此处key[0]是已经删除数据后的key[0]
**  这两行代码保证了若孩子结点key[0]被修改后，可以在递归回溯后修改父亲结
** 点的key值
*/
static KEYTYPE btreeRemoveGTDegree(PBTreeNode x, KEYTYPE key)
{
  int position = findUpperPosition(x, key);

  if(x->leaf)
  {
    if(x->key[position] != key)
    {
      printf("remove leaf error!\n");
    }

    //free(x->data[position]);    //这里存在错误1  .1
    free(x->data[position]);    //搭配下面的更改2
    for(int i = position; i<x->num-1 ; i++)
    {
      x->key[i] = x->key[i+1];
      //copyData(x->data[i], x->data[i+1]);    //这里是导致错误1的原因  .3
      x->data[i] = x->data[i+1];    //此处为更改2第一步
    }
    x->data[x->num-1] = NULL;    //此处为更改2第二步
    //free(x->data[x->num-1]);    //修正错误1  .2

    x->num -= 1;
    return x->key[0];
  }
  else
  {
    if(x->children[position]->num > DEGREE)
    {
      x->key[position] = btreeRemoveGTDegree(x->children[position], key); 
    }
    else if(x->children[position]->num == DEGREE)
    {
      if(position - 1 >= 0 && x->children[position-1]->num > DEGREE)
      {
        moveKeyFromAToB(x, position-1, position);
      }
      else if(position+1 < x->num && x->children[position+1]->num >DEGREE)
      {
        moveKeyFromAToB(x, position+1, position);
      }
      else if(position-1 >= 0 && x->children[position-1]->num == DEGREE)
      {
        btreeMergeChild(x, position-1); position -= 1;
      }
      else if(position+1 <x->num && x->children[position+1]->num == DEGREE)
      {
        btreeMergeChild(x, position);
      }
      
      x->key[position] = btreeRemoveGTDegree(x->children[position], key);
    }

    return x->key[0];
  }
}

/* 
**功能：B结点向A结点借一个前驱或后继<key,data>
**输入：父亲结点x， A下标a, B下标b
**具体描述：
**1.若a < b，则B向A借的是前驱<key,data>；否则借的是后继<key,data>
**2.从A中删除这个前驱或后继<key,data>
**3.将前驱或后继<key,data>插入B
*/
static void moveKeyFromAToB(PBTreeNode x, int a, int b)
{
  PBTreeNode p;
  KEYTYPE tmpKey;
  DATATYPE tmpData;
  if(a < b)
  {
    p = findMostRight(x->children[a]); /* 找到前驱<key,data> */
    tmpKey = p->key[p->num-1];
    copyData(&tmpData, p->data[p->num-1]);
    btreeRemoveGTDegree(x->children[a], tmpKey);
    btreeInsertNonfull(x->children[b], tmpKey, &tmpData);
  }
  else if(a > b)
  {
    p = findMostLeft(x->children[a]); /* 找到后继<key,data> */
    tmpKey = p->key[0];
    copyData(&tmpData, p->data[0]);
    btreeRemoveGTDegree(x->children[a], tmpKey);
    btreeInsertNonfull(x->children[b], tmpKey, &tmpData);    
  }
}

/* 
**功能：在b+树T中，删除<key,data>
**输入：b+树指针，键值
**返回：操作结束后的b+树
**具体描述：若要删除的键值存在，令总键值数目减1,调用btreeRemoveGTDegree
*/
extern BPTree removeKey(BPTree T, KEYTYPE key)
{
  DATATYPE * data = search(T, key);
  if(data == NULL)
  {
    printf("Remove error! These isn't the key!");
    return NULL;
  }

  printf("remove:%d %s\n", key, data->idcard);
  T->total_key_num -= 1; 
  PBTreeNode r = T->root;
  btreeRemoveGTDegree(r, key);

  return T;
}


extern void destroyRangedata(RangeDataes *data)
{
  free(data->data);
  free(data->key);
  free(data);
}

static void destroyNodeRecursively(PBTreeNode x)
{
  if(x->leaf)
  {
    for(int i = 0; i<x->num ; i++)
    {
      free(x->data[i]);
    }
    free(x);
  }
  else
  {
    for(int i = 0; i<x->num ; i++)
    {
      destroyNodeRecursively(x->children[i]);
    }
    free(x);
  }
}

extern BPTree destroy(BPTree T)
{
  PBTreeNode x = T->root;
  destroyNodeRecursively(x);
  free(T);

  return T;
}


static PBTreeNode deserializeNode(int fd, PBTreeNode *preNode)
{
  int i;
  PBTreeNode pnode = (PBTreeNode)malloc(sizeof(struct BPlusTreeNode));

  if((i = read(fd, &pnode->leaf, LEAF_LENGTH)) == -1)
  {
    printf("DeserializeNode error: READ LEAF\n");
    return NULL;
  }

  if((i = read(fd, &pnode->num, NUM_LENGTH)) == -1)
  {
    printf("DeserializeNode error: READ NUM\n");
    return NULL;
  }

  for(int j = 0 ; j<pnode->num ; j++)
  {
    if((i = read(fd, &pnode->key[j], KEY_LENGTH)) == -1)
    {
      printf("DeserializeNode error: READ key[%d]\n", i);
      return NULL;
    }
  }

  //难点：在先序遍历的反序列化中，如何维护叶结点指针next
  if(!pnode->leaf)
  {
    for(int j = 0; j<pnode->num ; j++)
    {
      if((pnode->children[j] = deserializeNode(fd, preNode)) == NULL)
      {
        return NULL;
      }

      //解决难点：在参数中传递一个前叶结点指针的指针
      if(pnode->children[j]->leaf && j>0)
      {
        pnode->children[j-1]->next = pnode->children[j];
      }
      if(pnode->children[j]->leaf && j == 0)
      {
        (*preNode)->next = pnode->children[j];
      }
      if(pnode->children[j]->leaf && j == pnode->num-1)
      {
        *preNode = pnode->children[j];
      }
    }
  }
  else
  {
    pnode->next = NULL;
    for(int j = 0; j<pnode->num ; j++)
    {
      pnode->data[j] = (DATATYPE*)malloc(sizeof(DATATYPE));
      if((i = read(fd, pnode->data[j], sizeof(DATATYPE))) == -1)
      {
        printf("DeserializeNode error: READ data[%d]\n", j);
        return NULL;
      }
    }
  }

  return pnode;
}

extern BPTree deserialize(const char *filePath)
{
  int fd, tag;
  if((fd = open(filePath, O_RDWR)) == -1)
  {
    printf("Open error in Deserialize\n");
    return NULL;
  }

  BPTree btree = initialize();
  if((tag = read(fd, &btree->total_key_num, 4)) == -1)
  {
    return NULL;
  }
  
  btree->root = deserializeNode(fd, &btree->first);
  btree->first = findMostLeft(btree->root);

  if((tag = close(fd)) == -1)
  {
    printf("Close error in Deserialize\n");
    return NULL;
  }

  return btree;
}

static int serializeNode(PBTreeNode pnode, int fd)
{
  int i;

  if((i = write(fd, &pnode->leaf, LEAF_LENGTH)) == -1)
  {
    printf("SerializeNode error: WRITE LEAF\n");
    return false;
  }

  if((i = write(fd, &pnode->num, NUM_LENGTH)) == -1)
  {
    printf("SerializeNode error: WRITE NUM\n");
    return false;
  }

  for(int j = 0 ; j<pnode->num ; j++)
  {
    if((i = write(fd, &pnode->key[j], KEY_LENGTH)) == -1)
    {
      printf("SerializeNode error: WRITE key[%d]\n", j);
      return false;
    }
  }

  if(!pnode->leaf)
  {
    for(int j = 0; j<pnode->num ; j++)
    {
      if((i = serializeNode(pnode->children[j], fd)) == -1)
      {
        return false;
      }
    }
  }
  else
  {
    for(int j = 0; j<pnode->num; j++)
    {
      if((i = write(fd, pnode->data[j], sizeof(DATATYPE))) == -1)
      {
        printf("SerializeNode error: WRITE data[%d]\n", j);
        return false;
      }
    }
  }

  return true;
}

extern int serialize(const char *filePath, BPTree tree)
{
  int fd;
  int tag;
  if((fd = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 00700)) == -1)
  {
    printf("Open error in Serialize\n");
    return false;
  }

  if((tag = write(fd, &tree->total_key_num, 4)) == -1)
  {
    printf("serialize error 2\n");
    return false;
  }

  if((tag = serializeNode(tree->root, fd)) == false)
  {
    printf("serialize error 3\n");
    return false;
  }

  if((tag = close(fd)) == -1)
  {
    printf("serialize error 4\n");
    return false;
  }

  return true;
}

