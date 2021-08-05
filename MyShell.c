#include"BPlusTree.h"
#define BUFFERSIZE 32
#define ORDERSIZE 8

static int command(int argc, char *argv[]);
static char* trim(char* s);
static BPTree tree;

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

void printhelp()
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
    if(tree == NULL)
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
      destroy(tree);
      tree = NULL;
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

      tree = deserialize(argv[1]);
      if(tree == NULL)
        printf("initDB false\n");
      return 0;
    }

    else if(!strcmp(argv[0], "search"))
    {
      DATATYPE *data = search(tree, atoi(argv[1]));
      if(data == NULL)
      {
        printf("cant find this key!\n");
        return 0;
      }
      printf("%d : %s\n",atoi(argv[1]), data->idcard);
      return 0;
    }

    else if(!strcmp(argv[0], "remove"))
    {
      removeKey(tree, atoi(argv[1]));
      return 0;
    }

    else if(!strcmp(argv[0], "write"))
    {
      int tag = serialize(argv[1], tree);
      if(tag != 1)
        printf("write false\n");
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
      insert(tree, atoi(argv[1]), data);
      free(data);
      return 0;
    }

    else if(!strcmp(argv[0], "search"))
    {
      RangeDataes *dataes = searchRange(tree, atoi(argv[1]), atoi(argv[2]));
      for(int i = 0; i< dataes->num ; i++)
      {
        printf("%d : %s\n", dataes->key[i], dataes->data[i].idcard);
      }

      free(dataes->key);
      free(dataes->data);
      free(dataes);
      return 0;
    }

    else if(!strcmp(argv[0], "update"))
    {
      DATATYPE *data = (DATATYPE*)malloc(sizeof(DATATYPE));
      strcpy(data->idcard, argv[2]);
      update(tree, atoi(argv[1]), data);
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