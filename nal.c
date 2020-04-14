#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#include "nal.h"

#define ROT5 5
#define ROT13 13
#define SHIFT_1 1
#define SHIFT_2 2
#define SHIFT_3 3
#define SHIFT_5 5

/*I set an array of structure to store variable.*/
Variable storage[MAXNUM];
int num = 0;

int main(int argc, char **argv)
{
  test();

  if(argc == 2)
  {
    Program *p;

    srand(time(NULL));
    p = init_Program();

    readfile(argv[1], p);

    PROG(p);

    free(p);

    #ifdef INTERP
      printf("interpret OK\n");
    #else
      printf("parsed OK\n");
    #endif
  }
  else
  {
    ON_ERROR("incorrect execution format.")
  }

  return 0;
}

void test(void)
{
  int i, j;
  Program *p;

  p = init_Program();

  /*Test initialize.*/
  for(i=0; i<MAXNUM; i++)
  {
    for(j=0; j<MAXSIZE; j++)
    {
      assert(p->wds[i][j] == '\0');
    }
  }
  assert(p->cw == 0);

  /*Test ifsame.*/
  ifsame("(", "(");
  ifsame("\"", "\"");

  /*Start to test all VAR&CON, if grammer is right, function will return nothing.*/

  /*Test STRCON*/
  /*All str stored in Program will in type of "str".*/
  strcpy(p->wds[0], "\"Happy New Year\"");
  p->cw = 0;
  STRCON(p);

  /*Test NUMCON*/
  strcpy(p->wds[0], "17");
  p->cw = 0;
  NUMCON(p);
  /*I first forgot to consider "0", if(atoi("0")) will no execute, so i added another judgement.*/
  strcpy(p->wds[0], "0");
  p->cw = 0;
  NUMCON(p);

  /*Test CON*/
  strcpy(p->wds[0], "17");
  strcpy(p->wds[1], "\"Happy New Year\"");
  p->cw = 0;
  CON(p);
  p->cw = 1;
  CON(p);

  /*Test STRVAR*/
  strcpy(p->wds[0], "$ABCD");
  p->cw = 0;
  STRVAR(p);

  /*Test NUMVAR*/
  strcpy(p->wds[0], "%ABCD");
  p->cw = 0;
  NUMVAR(p);

  /*Test VAR*/
  strcpy(p->wds[0], "$ABCD");
  strcpy(p->wds[1], "%ABCD");
  p->cw = 0;
  VAR(p);
  p->cw = 1;
  VAR(p);

  /*Test VARCON*/
  strcpy(p->wds[0], "$ABCD");
  strcpy(p->wds[1], "%ABCD");
  strcpy(p->wds[2], "17");
  strcpy(p->wds[3], "\"Happy New Year!\"");
  p->cw = 0;
  VARCON(p);
  p->cw = 1;
  VARCON(p);
  p->cw = 2;
  VARCON(p);
  p->cw = 3;
  VARCON(p);

  free(p);
}

Program* init_Program(void)
{
  int i, j;
  Program *p;

  p = (Program*)calloc(1, sizeof(Program));

  p->cw = 0;

  for(i=0; i<MAXNUM; i++)
  {
    for(j=0; j<MAXSIZE; j++)
    {
      p->wds[i][j] = '\0';
    }
  }

  return p;
}

void readfile(char *filename, Program *p)
{
  FILE *ifp;
  /*When flag is 0, it is the normal fscanf.
    When flag is 1, it means the strings are being merged.*/
  int i = 0, j, flag = 0;

  if((ifp=fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "cannot open \"%s\"\n", filename);
    exit(EXIT_FAILURE);
  }

  while((fscanf(ifp, "%s", p->wds[i]) != EOF) && i < MAXNUM)
  {
    /*Combine the contents between "" and ## into a single string.*/
    if(p->wds[i][0] == '\"' || p->wds[i][0] == '#' || flag)
    {
      /*If there is no space in "str" or #str# .*/
      if((p->wds[i][strlen(p->wds[i])-1] == '\"' || p->wds[i][strlen(p->wds[i])-1] == '#') && flag == 0)
      {
        flag = 0;
      }
      else if(flag)
      {
        /*If the string to be strcat ends with " or # , the strcat ends.*/
        if(p->wds[i-SHIFT_1][strlen(p->wds[i-SHIFT_1])-1] == '\"' || p->wds[i-SHIFT_1][strlen(p->wds[i-SHIFT_1])-1] == '#')
        {
          flag = 0;
        }
        else if(flag)
        {
          strcat(p->wds[i-SHIFT_1], " ");
          strcat(p->wds[i-SHIFT_1], p->wds[i]);
        }
      }
      else
      {
        /*If there are space in quates or hashs, the strcat loop will first come here.*/
        flag = 1;
        i++;
      }
    }
    /*Normal scanf.*/
    if(flag == 0)
    {
      i++;
    }
  }

  /*Turn #str# of rot18 rule into "str" ​​of general rule.*/
  for(i=0; i<MAXNUM; i++)
  {
    if(p->wds[i][0] == '#' && p->wds[i][strlen(p->wds[i])-1] == '#')
    {
      for(j=0; j<(int)strlen(p->wds[i]); j++)
      {
        if(p->wds[i][j] == '#')
        {
          p->wds[i][j] = '\"';
        }
        else
        {
          if(isupper(p->wds[i][j]))
          {
            if(p->wds[i][j] - ROT13 < 'A')
            {
              p->wds[i][j] = p->wds[i][j] + ROT13;
            }
            else
            {
              p->wds[i][j] = p->wds[i][j] - ROT13;
            }
          }
          else if(islower(p->wds[i][j]))
          {
            if(p->wds[i][j] - ROT13 < 'a')
            {
              p->wds[i][j] = p->wds[i][j] + ROT13;
            }
            else
            {
              p->wds[i][j] = p->wds[i][j] - ROT13;
            }
          }
          else if(isdigit(p->wds[i][j]))
          {
            if(p->wds[i][j] - ROT5 < '0')
            {
              p->wds[i][j] = p->wds[i][j] + ROT5;
            }
            else
            {
              p->wds[i][j] = p->wds[i][j] - ROT5;
            }
          }
        }
      }
    }
  }

  fclose(ifp);
}

/*Program start.*/
void PROG(Program *p)
{
  ifsame(p->wds[p->cw], "{");

  p->cw = p->cw + 1;

  INSTRS(p);
}

/*Run INSTRUCT and end.*/
void INSTRS(Program *p)
{
  if(!strcmp(p->wds[p->cw], "}"))
  {
    return;
  }

  INSTRUCT(p);

  p->cw = p->cw + 1;

  INSTRS(p);

}

/*INSTRUCT includes <FILE> | <ABORT> | <INPUT> | <IFCOND> | <INC> |
                    <SET>  | <JUMP>  | <PRINT> | <RND>    | <COMMENT> */
void INSTRUCT(Program *p)
{
  if(!strcmp(p->wds[p->cw], "PRINT") || !strcmp(p->wds[p->cw], "PRINTN"))
  {
    p->cw = p->cw + 1;
    PRINT(p);
  }
  else if(p->wds[p->cw][0] == '$' || p->wds[p->cw][0] == '%')
  {
    SET(p);
  }
  else if(!strcmp(p->wds[p->cw], "IFEQUAL") || !strcmp(p->wds[p->cw], "IFGREATER"))
  {
    p->cw = p->cw + 1;
    IFCOND(p);
  }
  else if(!strcmp(p->wds[p->cw], "JUMP"))
  {
    p->cw = p->cw + 1;
    JUMP(p);
  }
  else if(!strcmp(p->wds[p->cw], "FILE"))
  {
    p->cw = p->cw + 1;
    FILE_OPEN(p);
  }
  else if(!strcmp(p->wds[p->cw], "RND"))
  {
    p->cw = p->cw + 1;
    RND(p);
  }
  else if(!strcmp(p->wds[p->cw], "INC"))
  {
    p->cw = p->cw + 1;
    INC(p);
  }
  else if(!strcmp(p->wds[p->cw], "ABORT"))
  {
    ABORT(p);
  }
  else if(!strcmp(p->wds[p->cw], "INNUM"))
  {
    p->cw = p->cw + 1;
    INNUM(p);
  }
  else if(!strcmp(p->wds[p->cw], "IN2STR"))
  {
    p->cw = p->cw + 1;
    IN2STR(p);
  }
  else if(p->wds[p->cw][0] == '@')
  {
    COMMENT(p);
  }
  else
  {
    ON_ERROR("wrong instruction ?");
  }
}

/*Check the syntax of PRINT & PRINTN.*/
void PRINT(Program *p)
{
  VARCON(p);
  #ifdef INTERP
    ipt_PRINT(p);
  #endif
}

/*Output the value with(without) a linefeed*/
void ipt_PRINT(Program *p)
{
  /*PRINT  : flag=1
    PRINTN : flag=0*/
  int i, j, flag;

  if(!strcmp(p->wds[p->cw-SHIFT_1], "PRINT"))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
  }

  /*If after PRINT is variable.*/
  if(p->wds[p->cw][0] == '%' || p->wds[p->cw][0] == '$')
  {
    for(i=0; i<MAXNUM; i++)
    {
      if(!strcmp(storage[i].var, p->wds[p->cw]))
      {
        if(p->wds[p->cw][0] == '%')
        {
          printf("%s\n", storage[i].value);
          return;
        }
        else
        {
          for(j=1; j<(int)strlen(storage[i].value)-1; j++)
          {
            if(storage[i].value[j] == '\\' && storage[i].value[j+1] == 'n')
            {
              printf("\n");
              j++;
            }
            else
            {
              printf("%c", storage[i].value[j]);
            }
          }
          if(flag)
          {
            printf("\n");
          }
          return;
        }
      }
    }
    ON_ERROR("you havn't set the variable yet.");
  }
  else if(p->wds[p->cw][0] == '\"')/*If after print is condition.*/
  {
    for(j=1; j<(int)strlen(p->wds[p->cw])-1; j++)
    {
      if(p->wds[p->cw][j] == '\\' && p->wds[p->cw][j+SHIFT_1] == 'n')
      {
        printf("\n");
        j++;
      }
      else
      {
        printf("%c", p->wds[p->cw][j]);
      }
    }
    if(flag)
    {
      printf("\n");
    }
  }
}

/*VARCON includes variable str & num and condition str & num.*/
void VARCON(Program *p)
{
  /*if(atoi("0")) will not execute, so another judgement is added.*/
  if(p->wds[p->cw][0] == '\"' || atoi(p->wds[p->cw]) || !strcmp(p->wds[p->cw], "0"))
  {
    CON(p);
  }
  else if(p->wds[p->cw][0] == '$' || p->wds[p->cw][0] == '%')
  {
    VAR(p);
  }
  else
  {
    ON_ERROR("wrong varcon, did you miss \" or # or $ or % ?");
  }
}

/*CON includes STRCON & NUMCON*/
void CON(Program *p)
{
  /*If p->wds[p->cw] equals 0 or other numbers, get into NUMCON function.*/
  if(atoi(p->wds[p->cw]) || !strcmp(p->wds[p->cw], "0"))
  {
    NUMCON(p);
    return;
  }
  else if(p->wds[p->cw][0] == '\"')
  {
    STRCON(p);
    return;
  }
}

void STRCON(Program *p)
{
  if(p->wds[p->cw][strlen(p->wds[p->cw])-1] == '\"')
  {
    return;
  }
  else
  {
    ON_ERROR("wrong con, did you miss \" or # ?");
  }
}

void NUMCON(Program *p)
{
  if(atoi(p->wds[p->cw]) || !strcmp(p->wds[p->cw], "0"))
  {
    return;
  }
  else
  {
    ON_ERROR("wrong numcon");
  }
}

/*VAR includes STRVAR & NUMVAR*/
void VAR(Program *p)
{
  if(p->wds[p->cw][0] == '$')
  {
    assert(p->wds[p->cw][0] == '$');
    STRVAR(p);
    return;
  }
  else if(p->wds[p->cw][0] == '%')
  {
    assert(p->wds[p->cw][0] == '%');
    NUMVAR(p);
    return;
  }
}

void STRVAR(Program *p)
{
  int i;

  if(p->wds[p->cw][0] != '$')
  {
    ON_ERROR("wrong strvar, did you miss $ ?");
  }

  /*If character after $ isn't capital letters, outout error.*/
  for(i=1; i<(int)strlen(p->wds[p->cw]); i++)
  {
    if(!isupper(p->wds[p->cw][i]))
    {
      ON_ERROR("wrong strvar");
    }
  }

  return;
}

void NUMVAR(Program *p)
{
  int i;

  if(p->wds[p->cw][0] != '%')
  {
    ON_ERROR("wrong numvar, did you miss % ?");
  }

  /*If character after % isn't capital letters, outout error.*/
  for(i=1; i<(int)strlen(p->wds[p->cw]); i++)
  {
    if(!isupper(p->wds[p->cw][i]))
    {
      ON_ERROR("wrong numvar");
    }
  }

  return;
}

/*Check the syntax of SET.*/
void SET(Program *p)
{
  VAR(p);

  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], "=");

  p->cw = p->cw + 1;
  VARCON(p);
  #ifdef INTERP
    ipt_SET(p);
  #endif
}

/*Let VAR equals to an VAR or CON.*/
void ipt_SET(Program *p)
{
  int i;
  char str[MAXSIZE]={0};

  /*If string after equal is var, find its real value first.*/
  if(p->wds[p->cw][0] == '$' || p->wds[p->cw][0] == '%')
  {
    for(i=0; i<MAXNUM; i++)
    {
      if(!strcmp(storage[i].var, p->wds[p->cw]))
      {
        strcpy(str, storage[i].value);
      }
    }
  }
  else
  {
    strcpy(str, p->wds[p->cw]);
  }

  /*IF the variable is set before, change its value.*/
  for(i=0; i<MAXNUM; i++)
  {
    if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_2]))
    {
      strcpy(storage[i].value, str);
      return;
    }
  }

  /*IF the variable is new, create a new one and set it.*/
  strcpy(storage[num].var, p->wds[p->cw-SHIFT_2]);
  strcpy(storage[num].value, str);
  num++;
  return;
}

/*Check the syntax of IFEQUAL or IFGREATER.*/
void IFCOND(Program *p)
{
  int flag = 0, start, end;
  ifsame(p->wds[p->cw], "(");

  p->cw = p->cw + 1;
  VARCON(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ",");

  p->cw = p->cw + 1;
  VARCON(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ")");
  start = p->cw;

  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], "{");

  do
  {
    if(strcmp(p->wds[p->cw], "{"))
    {
      flag++;
    }
    if(strcmp(p->wds[p->cw], "}"))
    {
      flag--;
    }
    if(flag != 0)
    {
      p->cw = p->cw + 1;
    }
  }while(flag);

  end = p->cw;

  #ifdef INTERP
    p->cw = start;
    if(itp_IFCOND(p))
    {
      p->cw = p->cw + 1;
      p->cw = p->cw + 1;
      INSTRS(p);
    }
    p->cw = end;
  #endif
}

/*If (condition) is true, return 1.*/
int itp_IFCOND(Program *p)
{
  /*Set flag to make sure that the variable is defined.*/
  /*IFEQUAL: flag = 1; IFGREATER: flag = 0*/
  int i, flag, flag1 = 1, flag2 = 1;
  char str1[MAXSIZE]={0}, str2[MAXSIZE]={0};

  if(!strcmp(p->wds[p->cw-SHIFT_5], "IFEQUAL"))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
  }

  /*Find the value of second varcon.*/
  if(p->wds[p->cw-SHIFT_1][0] == '%' || p->wds[p->cw-SHIFT_1][0] == '$')
  {
    for(i=0; i<MAXNUM; i++)
    {
      if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_1]))
      {
        strcpy(str2, storage[i].value);
        flag2 = 0;
      }
    }
    if(flag2)
    {
      ON_ERROR("you havn't set the variable yet.");
    }
  }
  else
  {
    strcpy(str2, p->wds[p->cw-SHIFT_1]);
  }

  /*Find the value of first varcon.*/
  if(p->wds[p->cw-SHIFT_3][0] == '%' || p->wds[p->cw-SHIFT_3][0] == '$')
  {
    for(i=0; i<MAXNUM; i++)
    {
      if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_3]))
      {
        strcpy(str1, storage[i].value);
        flag1 = 0;
      }
    }
    if(flag1)
    {
      ON_ERROR("you havn't set the variable yet.");
    }
  }
  else
  {
    strcpy(str1, p->wds[p->cw-SHIFT_3]);
  }

  if(flag)
  {
    if(!strcmp(str1, str2))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    if(atoi(str1)>atoi(str2))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

/*Check the syntax of JUMP.*/
void JUMP(Program *p)
{
  NUMCON(p);
  #ifdef INTERP
    ipt_JUMP(p);
  #endif
}

/*Jump to the nth(NUMCON) word in the file.*/
void ipt_JUMP(Program *p)
{
  if(atoi(p->wds[p->cw]) == 0)
  {
    p->cw = 0;
  }
  else
  {
    p->cw = atoi(p->wds[p->cw])-1;
  }
}

/*Check the syntax of FILE_OPEN.*/
void FILE_OPEN(Program *p)
{
  if(p->wds[p->cw][0] == '\"')
  {
    STRCON(p);
    #ifdef INTERP
      ipt_FILE_OPEN(p);
    #endif
  }
  else
  {
    ON_ERROR("wrong con, did you miss \" or # ?");
  }
}

/*Open the file and execute.*/
void ipt_FILE_OPEN(Program *p)
{
  Program *t;
  char *filename;

  filename = (char*)calloc(strlen(p->wds[p->cw])-1, sizeof(char));
  strncpy(filename, p->wds[p->cw]+1, strlen(p->wds[p->cw])-2);

  t = init_Program();

  readfile(filename, t);

  PROG(t);

  free(filename);
  free(t);
}

/*Check the syntax of RND.*/
void RND(Program *p)
{
  ifsame(p->wds[p->cw], "(");

  p->cw = p->cw + 1;
  NUMVAR(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ")");

  #ifdef INTERP
    itp_RND(p);
  #endif
}

/*Set a random number(0-99) to a NUMVAR.*/
void itp_RND(Program *p)
{
  int i, randnum;
  char str[MAXSIZE]={0};

  randnum = rand()%100;
  sprintf(str, "%d", randnum);

  for(i=0; i<MAXNUM; i++)
  {
    if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_1]))
    {
      strcpy(storage[i].value, str);
      return;
    }
  }

  strcpy(storage[num].var,  p->wds[p->cw-SHIFT_1]);
  strcpy(storage[num].value,  str);
  num++;
}

/*Check the syntax of INC.*/
void INC(Program *p)
{
  ifsame(p->wds[p->cw], "(");

  p->cw = p->cw + 1;
  NUMVAR(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ")");

  #ifdef INTERP
    itp_INC(p);
  #endif
}

/*Add 1 to a NUMVAR.*/
void itp_INC(Program *p)
{
  int i, n;
  char str[MAXSIZE] = {0};

  for(i=0; i<MAXNUM; i++)
  {
    if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_1]))
    {
      n = atoi(storage[i].value);
      n++;
      sprintf(str, "%d", n);
      strcpy(storage[i].value, str);
      return;
    }
  }
  ON_ERROR("you havn't set the variable yet.");
}

/*Abort the execution.*/
void ABORT(Program *p)
{
  p->cw = p->cw; /*Avoid warning.*/
  #ifdef INTERP
    ipt_ABORT(p);
  #endif
}

void ipt_ABORT(Program *p)
{
  free(p);
  #ifdef INTERP
    printf("interpret OK\n");
    exit(0);
  #endif
}

/*Check the syntax of INNUM.*/
void INNUM(Program *p)
{
  ifsame(p->wds[p->cw], "(");

  p->cw = p->cw + 1;
  NUMVAR(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ")");

  #ifdef INTERP
    itp_INNUM(p);
  #endif
}

/*Scanf a number.*/
void itp_INNUM(Program *p)
{
  int i, input;
  char str[MAXSIZE]={0};

  scanf("%d", &input);
  sprintf(str, "%d", input);

  for(i=0; i<MAXNUM; i++)
  {
    if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_1]))
    {
      strcpy(storage[i].value, str);
      return;
    }
  }

  strcpy(storage[num].var, p->wds[p->cw-SHIFT_1]);
  strcpy(storage[num].value, str);
  num++;
  return;
}

/*Check the syntax of IN2STR.*/
void IN2STR(Program *p)
{
  ifsame(p->wds[p->cw], "(");

  p->cw = p->cw + 1;
  STRVAR(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ",");

  p->cw = p->cw + 1;
  STRVAR(p);
  p->cw = p->cw + 1;

  ifsame(p->wds[p->cw], ")");

  #ifdef INTERP
    itp_IN2STR(p);
  #endif
}

/*Scanf two strings.*/
void itp_IN2STR(Program *p)
{
  int i, flag1=1, flag2=1;
  char str1[MAXSIZE]={0}, str2[MAXSIZE]={0};
  char input1[MAXSIZE]={0}, input2[MAXSIZE]={0};

  scanf("%s%s", input1, input2);
  sprintf(str1, "\"%s\"", input1);
  sprintf(str2, "\"%s\"", input2);

  for(i=0; i<MAXNUM; i++)
  {
    if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_3]))
    {
      strcpy(storage[i].value, str1);
      flag1 = 0;
    }
    if(!strcmp(storage[i].var, p->wds[p->cw-SHIFT_1]))
    {
      strcpy(storage[i].value, str2);
      flag2 = 0;
    }
  }

  if(flag1)
  {
    strcpy(storage[num].var, p->wds[p->cw-SHIFT_3]);
    strcpy(storage[num].value, str1);
    num++;
  }
  if(flag2)
  {
    strcpy(storage[num].var, p->wds[p->cw-SHIFT_1]);
    strcpy(storage[num].value, str2);
    num++;
  }
  return;
}

/*Do not execute the comments between @@.*/
void COMMENT(Program *p)
{
  /*To avoid single @.*/
  if(p->wds[p->cw][1] == '\0')
  {
    ON_ERROR("did you miss comment and @ ?");
  }

  while(p->wds[p->cw][strlen(p->wds[p->cw])-1] != '@')
  {
    if(p->wds[p->cw][0] == '\0' || p->cw > MAXNUM)
    {
      ON_ERROR("did you miss @ ?");
    }
    p->cw = p->cw + 1;
  }
}

void ifsame(char *str1, char *str2)
{
  if(!strcmp(str1, str2))
  {
    return;
  }
  else
  {
    fprintf(stderr, "Error : did you miss %s ?\n", str2);
    exit(EXIT_FAILURE);
  }
}
