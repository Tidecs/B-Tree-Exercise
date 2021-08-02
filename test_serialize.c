#include"BPlusTree.h"

int main()
{
  BPTree tree = initialize();
  DATATYPE data = { "abc" };

  srand((unsigned int)time(0));
  for(int i = 0; i<100 ; i++)
  {
    insert(tree, rand()%3000, data);
  }

  showBPlusTree(tree);
  serialize("data1", tree);
  KEYTYPE *p = travel(tree);
  for(int i = 0; i<tree->total_key_num ; i++)
  {
    printf(": %d ",p[i]);
  }

  destroy(tree);
  tree = NULL;

  BPTree newTree = deserialize("data1");
  showBPlusTree(newTree);
  p = travel(newTree);
  for(int i = 0; i<newTree->total_key_num ; i++)
  {
    printf(": %d ",p[i]);
  }
  printf("\n %p \n", tree );

  return 0;
}