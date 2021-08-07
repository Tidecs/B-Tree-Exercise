#include"BPlusTree.h"
#define BUFFERSIZE 32
#define ORDERSIZE 8

static int command(int argc, char *argv[]);
static char* trim(char* s);
static BPTree tree;
static clock_t start, end;
static double duration;

int main(int argc, char **argv)
{
  int argc1;
  char *token;
  char *argv_3[3]; 
  char *line = (char *)malloc(BUFFERSIZE);
  char whitespace[2] = " ";
  tree = NULL;

  for(int i = 0; i<3 ; i++)
  {
    argv_3[i] = (char *)malloc(ORDERSIZE);
  }

  while(1)
  {
    printf("[byk@TestShell]$ ");

    if(fgets(line, BUFFERSIZE, stdin))
    {
      argc1 = 0;
      trim(line);
      token = strtok(line, whitespace);
      while(token != NULL && argc1 < 3)
      {
        strcpy(argv_3[argc1++], token);
        token = strtok(NULL, whitespace);
      }
    }

    /*
    **在这里使用线程执行命令，就是并发。并发的前提要理解锁的机制
    */
    if(command(argc1, argv_3) < 0)
      break;
  }

  free(line);
  for(int i = 0; i<3 ; i++)
  {
    free(argv_3[i]);
  }

  return 0;
}

static char* trim(char *s)
{
 char *p = s + strlen(s) - 1;
 while(' ' == *p || '\t' == *p || '\n'==*p || 'r'==*p)
 {
   *p = '\0';
   p--;
 } 
 return s;
}

static void printhelp()
{
  printf("please input commands:\n");
  printf("exit \ninitDB file \ninsert key data \nsearch key \nsearch key1 key2 \n");
  printf("update key data \nremove key \ntravel \ndestroy \nwrite file \nhelp \n");
}

static int command(int argc, char *argv[])
{
  if(argc <= 0 || argc > 3)
  {
    printf("error 01\n");
    return -1;
  }

  if(!strcmp(argv[0], "exit"))
  {
    printf("Exit\n");
    return -1;
  }

  if(argc == 1)
  {
    if(tree == NULL && strcmp(argv[0], "init")!=0)
    {
      printf("please init!\n");
      return 0;
    }

    else if(!strcmp(argv[0], "travel"))
    {
      KEYTYPE *key = travel(tree);
      DATATYPE *data = travelData(tree);

      for(int i = 0; i<tree->total_key_num; i++)
      {
        printf("%d : %s\n",key[i], data[i].idcard);
      }

      free(key);
      free(data);
      return 0;
    }

    else if(!strcmp(argv[0], "destroy"))
    {
      start = clock();
      destroy(tree);
      end = clock();
      duration = (double)(end - start)/CLOCKS_PER_SEC;
      printf("time of destroy is %f\n", duration);
      tree = NULL;
      return 0;
    }

    else if(!strcmp(argv[0], "init"))
    {
      if(tree != NULL){
        printf("tree has been created, please destroy\n");
        return 0;
      }
      tree = initialize();
      if(tree == NULL)
        printf("init false\n");
      return 0;
    }

    else
    {
      printhelp();
      return 0;
    }
  }

  if(argc == 2)
  {
    if(tree == NULL && strcmp(argv[0], "initDB")!=0)
    {
      printf("please initDB!\n");
      return 0;
    }

    if(!strcmp(argv[0], "initDB"))
    {
      if(tree != NULL){
        printf("tree has been created, please destroy\n");
        return 0;
      }

      start = clock();
      tree = deserialize(argv[1]);
      end = clock();
      duration = (double)(end - start)/CLOCKS_PER_SEC;
      printf("time of initDB %s is %f\n", argv[1], duration);

      if(tree == NULL)
        printf("initDB false\n");
      return 0;
    }

    else if(!strcmp(argv[0], "search"))
    {
      start = clock();
      DATATYPE *data = search(tree, atoi(argv[1]));
      end = clock();
      if(data == NULL)
      {
        printf("cant find this key!\n");
        return 0;
      }
      printf("%d : %s\n",atoi(argv[1]), data->idcard);
      duration = (double)(end - start)/CLOCKS_PER_SEC;
      printf("time of search %s is %f\n", argv[1], duration);
      return 0;
    }

    else if(!strcmp(argv[0], "remove"))
    {
      start = clock();
      if(NULL == removeKey(tree, atoi(argv[1])))
      {
        return 0;
      }
      end = clock();
      duration = (double)(end - start)/CLOCKS_PER_SEC;
      printf("time of remove %s is %f\n", argv[1], duration);
      return 0;
    }

    else if(!strcmp(argv[0], "write"))
    {
      start = clock();
      int tag = serialize(argv[1], tree);
      end = clock();
      if(tag != 1)
      {
        printf("write false\n");
        return 0;
      }
      duration = (double)(end - start)/CLOCKS_PER_SEC;
      printf("time of write %s is %f\n", argv[1], duration);
      return 0;
    }

    else
    {
      printhelp();
      return 0;
    }
  }

  if(argc == 3)
  {
    if(tree == NULL)
    {
      printf("please init!\n");
      return 0;
    }

    if(!strcmp(argv[0], "insert"))
    {
      DATATYPE *data = (DATATYPE*)malloc(sizeof(DATATYPE));
      strcpy(data->idcard, argv[2]);
      start = clock();
      insert(tree, atoi(argv[1]), data);
      end = clock();
      duration = (double)(end - start)/CLOCKS_PER_SEC;

      printf("insert: %d : %s\n", atoi(argv[1]), data->idcard);
      printf("time of insert %s : %s is %f\n", argv[1], argv[2], duration);
      free(data);
      return 0;
    }

    else if(!strcmp(argv[0], "search"))
    {
      start = clock();
      RangeDataes *dataes = searchRange(tree, atoi(argv[1]), atoi(argv[2]));
      end = clock();
      duration = (double)(end - start)/CLOCKS_PER_SEC;

      for(int i = 0; i< dataes->num ; i++)
      {
        printf("%d : %s\n", dataes->key[i], dataes->data[i].idcard);
      }
      printf("time of search %s to %s is %f\n", argv[1], argv[2], duration);

      free(dataes->key);
      free(dataes->data);
      free(dataes);
      return 0;
    }

    else if(!strcmp(argv[0], "update"))
    {
      DATATYPE *data = (DATATYPE*)malloc(sizeof(DATATYPE));
      strcpy(data->idcard, argv[2]);
      start = clock();
      update(tree, atoi(argv[1]), data);
      end = clock();
      duration = (double)(end - start)/CLOCKS_PER_SEC;

      printf("updata: %d : %s\n", atoi(argv[1]), data->idcard);
      printf("time of update %s : %s is %f\n", argv[1], argv[2], duration);
      free(data);
      return 0;
    }

    else
    {
      printhelp();
      return 0;
    }
  }

  return 0;
}