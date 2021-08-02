#include"BPlusTree.h"

int main()
{
  BPTree T = initialize();
  DATATYPE dataes[15];
  DATATYPE data = { "abc" };
  char buffer[20] = "abc";

  srand((unsigned int)time(0));
  for(int i=0; i<15 ; i++)
  {
    strcpy(dataes[0].idcard, buffer); 
    insert(T,  i , dataes[0]);
  }
#if 0
  for(int i=0; i<8 ; i++)
  {
    strcpy(dataes[0].idcard, buffer); 
    removeKey(T, /*rand()%2000*/ i);
    showBPlusTree(T);
    printf("\n######################\n");
  }
#endif
  showBPlusTree(T);
    printf("\n######################\n");
        printf("\n######################\n");
            printf("\n######################\n");
   removeKey(T,  8);
    showBPlusTree(T);
    printf("\n######################\n");
        removeKey(T, 6);
    showBPlusTree(T);
    printf("\n######################\n");
        removeKey(T,  1);
    showBPlusTree(T);
    printf("\n######################\n");
        removeKey(T,  2);
    showBPlusTree(T);
    printf("\n######################\n");
    printf("\n%d\n", T->total_key_num);
  
  insert(T,  3 , (DATATYPE){"A"});
  insert(T,  3 , (DATATYPE){"B"});
  insert(T,  3 , (DATATYPE){"C"});
  //update(T, 10, (DATATYPE){"aaa"});
  showBPlusTree(T);

  RangeDataes range_data = searchRange(T, 3, 3);
  showRange(range_data);
  
  KEYTYPE* keys = travel(T);
  for(int i = 0; i<T->total_key_num ; i++)
  {
    printf("%d ", keys[i]);
  }
  printf("\n");
  
  destroy(T);
  T = NULL;
#if 0
  DATATYPE* travelDataes = travelData(T);
  for(int i = 0; i<8 ; i++)
  {
    printf("%s ", travelDataes[i].idcard);
  }
  printf("\n");
#endif
  return 0;
}