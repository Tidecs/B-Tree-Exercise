#include"BPlusTree.h"

int main()
{
  BPTree tree = initialize();
  DATATYPE data = { "abc" };

  for(int i = 0; i<1000 ; i++)
  {
    insert(tree, i, data);
  }

  showBPlusTree(tree);
  serialize("data1", tree);
  KEYTYPE *p = travel(tree);
  for(int i = 0; i<tree->total_key_num ; i++)
  {
    printf(": %d ",p[i]);
  }

  destroy(tree);
  BPTree newTree = deserialize("data1");
  showBPlusTree(newTree);
  p = travel(tree);
  for(int i = 0; i<tree->total_key_num ; i++)
  {
    printf(": %d ",p[i]);
  }
  printf("\n");

  return 0;
}