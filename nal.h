#define MAXNUM 2000
#define MAXSIZE 500

#define ON_ERROR(STR) {fprintf(stderr, "Error : %s\n", STR); exit(EXIT_FAILURE);}

typedef struct Program
{
  char wds[MAXNUM][MAXSIZE];
  int cw;
}Program;

/*I create a structure and set an array rather than mvm to store variable.*/
typedef struct Variable
{
  char var[MAXSIZE];
  char value[MAXSIZE];
}Variable;

void test(void);

Program* init_Program(void);

void readfile(char *filename, Program *p);

void ifsame(char *str1, char *str2);

void PROG(Program *p);

void INSTRS(Program *p);

void INSTRUCT(Program *p);

void PRINT(Program *p);

void VARCON(Program *p);

void VAR(Program *p);

void STRVAR(Program *p);

void NUMVAR(Program *p);

void CON(Program *p);

void STRCON(Program *p);

void NUMCON(Program *p);

void SET(Program *p);

void IFCOND(Program *p);

void JUMP(Program *p);

void FILE_OPEN(Program *p);

void RND(Program *p);

void INC(Program *p);

void ABORT(Program *p);

void INNUM(Program *p);

void IN2STR(Program *p);

void COMMENT(Program *p);

void ipt_PRINT(Program *p);

void ipt_JUMP(Program *p);

void ipt_FILE_OPEN(Program *p);

void ipt_SET(Program *p);

int itp_IFCOND(Program *p);

void itp_RND(Program *p);

void itp_INC(Program *p);

void itp_INNUM(Program *p);

void itp_IN2STR(Program *p);

void ipt_ABORT(Program *p);
