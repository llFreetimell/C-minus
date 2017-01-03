# C-minus
2016년 컴파일러 수업의 과제였던 C-minus 제작을 위한 프로젝트.

##전체개요
컴파일 과정은 크게 Scan, Parse, Symbol Table 생성, Type Check, Code 생성 등 5가지 단계로 구성된다. 이 프로젝트에서는 Tiny Compiler를 C언어의 간략한 버전인 C-Minus 문법에 맞게 코드를 수정하여 새로운 C-minus 컴파일러를 제작하는 것이 목표다.

* program → declaration-list 
* declaration-list → declaration-list declaration | declaration
* declaration → var-declaration | fun-declaration
* var-declaration → type-specifier ID; | type-specifier ID [NUM];
* type-specifier → int | void
* fun-declaration → type-specifier ID ( params ) compound-stmt
* params → param-list | void
* param-list → param-list , param | param
* param → type-specifier ID | type-specifier ID [ ]
* compound-stmt → { local-declarations statement-list }
* local-declarations → local-declarations var-declaration | empty
* statement-list → statement-list statement | empty
* statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt
* expression-stmt → expression ; | ;
* selction-stmt → if ( expression ) statement | if ( expression ) statement else statement
* iteration-stmt → while ( expression ) statement
* return-stmt → return ; | return expression ;
* expression → var = expression | simple-expression
* var → ID | ID [ expression ]
* simple-expression → additive-expression relop additive-expression | additive-expression
* additive-expression → additive-expression addop term | term
* term → term mulop factor
* factor → ( expression ) | var | call | NUM
* relop → <= | < | >= | > | == | !=
* addop → + | -
* mulop → * | /
* call → ID ( args )
* args → arg-list | empty
* arg-list → arg-list , expression | expression

C-minus 컴파일러 제작은 Ubuntu 14.04 LTS (on VMWare Workstation 12 Pro) 환경에서 c파일들과 Makefile을 수정하며 진행했으며 컴파일러의 마지막 단계에서 생성되는 코드는 Tiny Machine(TM)에서 구동되는 어셈블리어로 생성된다.

###Step 1 – Scanner
####개요
스캐너는 소스코드를 의미 있는 단어(토큰)들로 분석해주는 단계이며 이 단계에서 Lexical Analysis가 수행된다. Lexical Analysis는 일반적으로 DFA를 이용하며 C-minus의 참고용 DFA는 보고서에 첨부했다.

####구현방식 및 구현과정
(1) scan.c 수정방식

* main.c 수정사항

NO_PARSE : parse 이후 단계를 진행할지 나타내는 값

Scanner만 구현할 것이므로 TRUE로 변경한다.
```c
#define NO_PARSE TRUE
```
EchoSource : 소스코드를 출력할 것인지 나타내는 값

디버깅의 편리성을 위해 TRUE로 변경한다. FALSE로 두어도 무방하다.
```c
int EchoSource = FALSE;
```
TraceScan : Scan 과정을 보여줄 것인지 나타내는 값

Scan과정이 잘 이루어지는지 보기 위해 TRUE로 변경한다.
```c
int TraceScan = FALSE;
```
* scan.c 수정사항

reservedWords : 예약어 목록

C-minus 문법에 사용될 예약어 추가
```c
/* lookup table of reserved words */
static struct{ char* str; TokenType tok;} 
reservedWords[MAXRESERVED]= { 
// 6 RESERVED WORDS WILL BE USED 
{ "if",IF },{ "else",ELSE },{ "while",WHILE },{ "return",RETURN },{ "int",INT },{ "void",VOID } 
};
```
getToken() : 토큰을 읽어오는 함수

C-minus 문법의 DFA에 맞도록 함수내용 수정.
<= 기호와 < 기호, /* */ 기호와 / 기호처럼 같은 문자로 시작하지만 토큰이 다를 수 있는 경우 일단 같은 상태(state)로 이동한 후, 다음 문자를 읽어 들여 어떤 토큰인지 구분한다. 토큰을 구분한 후에는 각 토큰에 맞는 행동을 취한다.

* globals.h 수정사항

MAXRESERVED : 예약어의 개수

scan.c의 reservedWords 배열의 개수가 6개이므로 6로 변경한다.
```c
#define MAXRESERVED 6
```

TokenType : 토큰의 타입(더하기, 곱하기 등)

C-minus 문법에서 사용될 토큰들을 추가해주며 사용하지 않는 토큰은 제거한다.
```c
typedef enum
/* book-keeping tokens */
{
	ENDFILE, ERROR,

	/* reserved words */
	IF, ELSE, WHILE, RETURN, INT, VOID,

	/* multicharacter tokens */
	ID, NUM,
	
	/* special symbols */
	ASSIGN, EQ, NE, LT, LE, GT, GE, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, LBRACE, RBRACE, LCURLY, RCURLY, SEMI, COMMA
} TokenType;
```

(2) flex 방식 사용

* Makefile 수정사항

yywrap 오류를 방지하기 위해 컴파일 옵션으로 –lfl을 추가한다.
```
cminus_flex: main.o globals.h util.o lex.yy.o
        $(CC) $(CFLAGS) main.o util.o lex.yy.o –o cminus_flex -lfl

lex.yy.o: cminus.l scan.h util.h globals.h
        flex -o lex.yy.c cminus.l
        $(CC) $(CFLAGS) -c lex.yy.c
```

* tiny.l 수정사항 (cminus.l로 수정)

각 토큰별로 알맞은 TokenType을 반환하도록 추가, 수정한다. 다만 주석 토큰을 읽어 들이는 경우 주석의 끝까지 혹은 파일의 끝까지 input을 받아들여 주석부분을 무시하도록 수정한다.
```c
char c;
while (1)
{
	c = input();
	if (c == EOF) break;
	if (c == '*')
	{
		c = input();
		if (c == '/' || c == EOF) break;
	}
	if (c == '\n') lineno++;
}
```

####실행방법 및 실행결과
(1) scan.c 수정 방식
* 실행방법
```
$ make cminus
$ ./cminus test.cm
```

* 실행결과
```
   1: /* A program to perform Euclid's
   2:    Algorithm to computer gcd */
   3: 
   4: int gcd (int u, int v)
	4: reserved word: int
	4: ID, name= gcd
	4: (
	4: reserved word: int
	4: ID, name= u
	4: ,
	4: reserved word: int
	4: ID, name= v
	4: )
   5: {
	5: {
   6:     if (v == 0) return u;
	6: reserved word: if
	6: (
	6: ID, name= v
	6: ==
	6: NUM, val= 0
	6: )
	6: reserved word: return
	6: ID, name= u
	6: ;
   7:     else return gcd(v,u-u/v*v);
	7: reserved word: else
	7: reserved word: return
	7: ID, name= gcd
	7: (
	7: ID, name= v
	7: ,
	7: ID, name= u
	7: -
	7: ID, name= u
	7: /
	7: ID, name= v
	7: *
	7: ID, name= v
	7: )
	7: ;
   8:     /* u-u/v*v == u mod v */
   9: }
	9: }
  10: 
  11: void main(void)
	11: reserved word: void
	11: ID, name= main
	11: (
	11: reserved word: void
	11: )
  12: {
	12: {
  13:     int x; int y;
	13: reserved word: int
	13: ID, name= x
	13: ;
	13: reserved word: int
	13: ID, name= y
	13: ;
  14:     x = input(); y = input();
	14: ID, name= x
	14: =
	14: ID, name= input
	14: (
	14: )
	14: ;
	14: ID, name= y
	14: =
	14: ID, name= input
	14: (
	14: )
	14: ;
  15:     output(gcd(x,y));
	15: ID, name= output
	15: (
	15: ID, name= gcd
	15: (
	15: ID, name= x
	15: ,
	15: ID, name= y
	15: )
	15: )
	15: ;
  16: }
	16: }
	17: EOF

```

(2) flex 사용 방식
* 실행방법
```
$ make cminus_flex
$ ./cminus_flex test.cm
```
* 실행결과
```
TINY COMPILATION: test.cm
	4: reserved word: int
	4: ID, name= gcd
	4: (
	4: reserved word: int
	4: ID, name= u
	4: ,
	4: reserved word: int
	4: ID, name= v
	4: )
	5: {
	6: reserved word: if
	6: (
	6: ID, name= v
	6: ==
	6: NUM, val= 0
	6: )
	6: reserved word: return
	6: ID, name= u
	6: ;
	7: reserved word: else
	7: reserved word: return
	7: ID, name= gcd
	7: (
	7: ID, name= v
	7: ,
	7: ID, name= u
	7: -
	7: ID, name= u
	7: /
	7: ID, name= v
	7: *
	7: ID, name= v
	7: )
	7: ;
	9: }
	11: reserved word: void
	11: ID, name= main
	11: (
	11: reserved word: void
	11: )
	12: {
	13: reserved word: int
	13: ID, name= x
	13: ;
	13: reserved word: int
	13: ID, name= y
	13: ;
	14: ID, name= x
	14: =
	14: ID, name= input
	14: (
	14: )
	14: ;
	14: ID, name= y
	14: =
	14: ID, name= input
	14: (
	14: )
	14: ;
	15: ID, name= output
	15: (
	15: ID, name= gcd
	15: (
	15: ID, name= x
	15: ,
	15: ID, name= y
	15: )
	15: )
	15: ;
	16: }
	17: EOF
```

###Step 2 – Parser
####개요
파서는 스캐너를 통해 얻어진 토큰들을 이용해 구문 분석(Syntax Analysus)을 하여 파스 트리를 만드는 과정을 수행한다.

####구현방식 및 구현과정
이 프로젝트에서 파서를 구현하는 방법은 이전 단계에서 구현한 C-Minus Scanner를 이용하는 방법과 Yacc(bison)을 이용하는 방법이 있다. 하지만 컴파일러의 전체적인 구현 과정을 구체적으로 파악하고자 이전 단계에서 구현한 C-Minus Scanner를 이용한다.

* main.c 수정사항

NO_ANALYZE : parse 이후 단계를 진행할지 나타내는 값

Parser까지만 구현할 것이므로 TRUE로 변경한다.
```c
#define NO_PARSE FALSE
#define NO_ANALYZE TRUE
```
TraceParse : Parse 과정을 보여줄 것인지 나타내는 값

Parse과정이 잘 이루어지는지 보기 위해 TRUE로 변경한다.
```c
int TraceScan = FALSE;
int TraceParse = TRUE;
```

* globals.h 수정사항

StmtKind, ExpKind : Statement와 Expression의 종류
```c
typedef enum { CompStmtK, SelStmtK, IterStmtK, RetStmtK, CallK } StmtKind;
typedef enum { VarDeclK, VarArrayDeclK, FuncDeclK, AssignK, OpK, IdK, ConstK } ExpKind;
```
ExpType : Expression의 타입
Boolean은 C-Minus에서 사용되지 않으므로 제거한다.
```c
typedef enum { Void, Integer } ExpType;
```

TreeNode : 파스 트리의 노드
VarDeclK와 VarArrayDeclK의 경우 변수 선언과 매개변수 모두로 사용될 수 있다. 따라서 이들을 구분하기 위해 isParam 변수를 추가한다. 또한 VarArrayDeclK의 경우 배열의 크기를 저장할 필요가 있으므로 arraysize 변수도 추가한다.
```c
typedef struct treeNode
{
	...
	int isParam;
	int arraysize;
} TreeNode;
```

* parse.c 수정사항

C-Minus 문법에서 각 규칙의 LHS들을 이름으로 하는 함수들을 만들어 각 규칙에 맞는 파싱과정을 수행하도록 한다. 하지만 fun-declaration이나 var와 같은 일부 규칙들은 함수로 만들지 않는다. fun-declaration의 경우 var-declaration과 마찬가지로 type-specifier로 시작하기 때문에 var_decl함수에서 같이 처리해주도록 해야 하며 var의 경우 call과 마찬가지로 ID로 시작하기 때문에 call함수에서 처리하도록 한다. relop, addop, mulop의 경우 각각 simple-expression, additive-expression, term에서만 사용되므로 각각의 경우에서 처리하는 것이 효율적이라고 생각하여 별도로 함수를 생성하지 않았다.

* util.c 수정사항

printToken 함수 : 토큰을 출력해주는 함수
예약어는 C-Minus에 사용되는 예약어로 수정해주고 사용하지 않는 예약어는 제거한다. 또한 새로 추가된 또는 수정된 토큰들에 맞는 출력이 이루어지도록 출력내용을 수정한다.

newExpNode 함수 : Expression 노드를 생성해주는 함수
TreeNode에 isParam과 arraysize가 추가되었지만 arraysize는 VarArrayDeclK를 사용할 때 값이 초기화 되므로 isParam만 초기화되도록 수정한다.
```c
TreeNode * newExpNode(ExpKind kind)
{
	...
	else {
		for (i = 0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Void;
		t->isParam = FALSE;
	}
	return t;
}
```

####실행방법 및 실행결과
* 실행방법

```
$ make cminus
$ ./cminus test.cm
```
* 실행결과

```
Syntax tree:
  Function Declaration, name : gcd, type : int
    Single Parameter, name : u, type : int
    Single Parameter, name : v, type : int
    Compound Statement :
      If (condition) (body) (else) 
        Op : ==
          Id : v
          Const : 0
        Return Statement, with below
          Id : u
        Return Statement, with below
          Call, name : gcd, with arguments below
            Id : v
            Op : -
              Id : u
              Op : *
                Op : /
                  Id : u
                  Id : v
                Id : v
  Function Declaration, name : main, type : void
    Single Parameter, name : (null), type : void
    Compound Statement :
      Var Declaration, name : x, type : int
      Var Declaration, name : y, type : int
      Assign : (destination) (source) 
        Id : x
        Call, name : input, with NOTHING
      Assign : (destination) (source) 
        Id : y
        Call, name : input, with NOTHING
      Call, name : output, with arguments below
        Call, name : gcd, with arguments below
          Id : x
          Id : y
```
###Step 3 – Semantic Analysis
####개요
의미 분석기는 파서를 통해 생성된 파스 트리를 이용하여 Symbol Table을 생성하고 Type Checker를 구현하는 과정을 수행한다.

####구현방식 및 구현과정
C-Minus는 static scope rule을 사용하며 global scope가 최상위 scope다. 스택에 global scope를 push하여 초기화한 후, Compound-Statement(복합문)를 만나면 새로운 scope를 스택에 push하고 해당 Compound-Statement가 끝나면 pop을 한다. 모든 scope들은 스택에서 제거되더라도 실제 scope는 제거되지 않고 남아있다. 

Symbol Table은 스택의 가장 위에 있는 scope에 변수이름, 타입, 메모리 위치 등을 저장하며 생성된다. 전역변수와 함수의 경우, 저장되는 scope는 항상 global scope이며 지역변수만 새로 생성되는 scope들에 저장된다. 또한 input과 output함수는 코드에서 별도로 정의되지 않았더라도 기본적으로 포함되도록 한다.

Type Checker는 Symbol Table을 구현하면서 발생하는 오류나 Symbol Table을 구현한 후 타입 검사를 통해 발견한 오류를 알려준다. Symbol Table을 구현하며 발생하는 오류는 주로 변수의 중복선언 등이 있으며 타입 검사를 통해 발견되는 오류는 주로 미선언 변수 사용, 리턴 타입 에러 등이 있다.

* main.c 수정사항

NO_CODE : Semantic Analysis 이후 단계를 진행할지 나타내는 값

Semantic Avalysis까지만 구현할 것이므로 TRUE로 변경한다.
```c
#define NO_ANALYZE FALSE
#define NO_CODE TRUE
```

TraceAnalyze : Semantic Analysis 과정을 보여줄 것인지 나타내는 값
Semantic Analysis 과정이 잘 이루어지는지 보기 위해 TRUE로 변경한다.
```c
int TraceParse = FALSE;
int TraceAnalyze = TRUE;
```

* symtab.c & symtab.h 수정사항

LineList : 변수가 어느 라인에 나타났는지 나타내주는 선형 리스트

BucketList : 변수에 대한 정보들을 저장하는 구조체

다른 코드에서 이들 구조체를 참조할 수 있도록 symtab.c에 있던 기존 정보를 symtab.h로 이동한다. 또한 BucketList에 저장되어 있는 변수가 어떤 TreeNode를 갖고 있었는지를 기억하기 위해 TreeNode 변수를 추가한다.
```c
typedef struct BucketListRec
{
	...
	TreeNode *node;
} *BucketList;
```

ScopeList : scope의 정보들을 저장하는 구조체

scope 내에 있는 변수들의 정보를 담고 있는 BucketList들을 저장하는 해시 테이블과 부모 scope, scope nested level 등을 저장할 수 있도록 추가한다.
```c
typedef struct ScopeListRec
{
	BucketList hashTable[SIZE];
	struct ScopeListRec *parent;
	char *name;
	int nestedLevel;
} *ScopeList;
```

scope, n_scope : 만들어진 모든 scope를 저장할 수 있는 배열과 scope 개수

scopeStack, n_scopeStack : static scope rule을 적용하기 위한 scope 스택과 스택의 top변수 역할
```c
static ScopeList scope[SIZE]; //SAVING ALL SCOPES
static ScopeList scopeStack[SIZE]; //TOOL FOR STATIC SCOPE RULE
static int n_scope = 0; //NUMBER OF SCOPES
static int n_scopeStack = -1; //TOP VARIABLE FOR SCOPE STACK
```

st_insert 함수 : Symbol Table에 변수 정보를 삽입해주는 함수

BucketList에 TreeNode를 저장할 수 있도록 매개변수 형식을 변경하고 scope rule을 적용할 수 있도록 알맞게 수정한다.
```c
void st_insert( char * name, int lineno, int loc, TreeNode *node)
{
	int h = hash(name);
	ScopeList sp = scopeStack[n_scopeStack];
	BucketList l;

	l = (BucketList) malloc(sizeof(struct BucketListRec));
	l->name = name;
	l->lines = (LineList) malloc(sizeof(struct LineListRec));
	l->lines->lineno = lineno;
	l->memloc = loc;
	l->lines->next = NULL;
	l->next = sp->hashTable[h];
	l->node = node;
	sp->hashTable[h] = l;
} /* st_insert */
```

기타 함수들

st_lookup 함수는 BucketList를 반환하도록 수정하였으며 printSymTab 함수는 수정된 자료구조에 맞게 Symbol Table을 출력하도록 수정하였다. 그리고 Semantic Analysis 과정에서 필요한 기타 함수들을 추가한다.
```c
BucketList st_lookup ( char * name )	
BucketList st_lookup_top(char *name);
void scope_init();
void scope_insert(char *name);
void scope_push(ScopeList sp);
void scope_pop();
ScopeList scope_top();
void printSymTabRow(FILE *listing, BucketList *hashtable);
void lineno_insert(int lineno, BucketList bucket);
```
* globals.h 수정사항

struct scopeListRec

다른 파일에서 scopeListRec 구조체를 참조할 수 있도록 선언해준다.

TreeNode

TreeNode 중 CompStmtK 노드의 경우 해당 TreeNode가 어떤 scope에 속해있는지 저장될 필요가 있기 때문에 scope를 저장할 수 있는 변수를 추가해줘야 한다. CompStmtK의 경우 attr에서 사용하는 속성이 없으므로 scope를 union attr에 추가하도록 한다.
```c
typedef struct treeNode
{
	...
	union {
		TokenType op;
		int val;
		char * name;
		struct ScopeListRec *scope;
	} attr;
	...
} TreeNode;
```
* analyze.c 수정사항

input함수와 output함수는 기본적으로 존재하는 함수이므로 이들에 대한 정보를 추가한다. 

이후 파스트리의 노드들을 순회하면서 변수선언이 있으면 insertNode 함수를 통해 이들에 대한 정보를 Symbol Table에 삽입하고 Compound Statement가 나오면 새로운 scope를 삽입해준다. 만약 Compound Statement의 자식노드들이 모두 순회를 마친 경우 afterinsertNode 함수를 통해 현재 scope를 스택에서 제거해주도록 한다.

Symbol Table 생성이 완료되면 checkNode 함수를 통해 다시 파스트리를 순회하며 Type Check를 수행하는데 Compound Statement는 Symbol Table 생성할 때와 비슷하게 처리한다. 각 TreeNode별로 발생할 수 있는 오류를 각각에 맞게 검출할 수 있도록 한다.

####실행방법 및 실행결과
* 실행방법
```c
make cminus
./cminus test.cm
./cminus sort.cm
```
* 실행결과

(test.cm)
```
Building Symbol Table...

Symbol table:

<<gloabl scope>> (nested level : 0)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
main          Function      Void              11 
input         Function      Integer           0 14 14 
output        Function      Void              0 15 
gcd           Function      Integer           4 7 15 

Functuon Name : input (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------

Functuon Name : output (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
arg           Parameter     Integer           0 

Functuon Name : gcd (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
u             Parameter     Integer           4 6 7 7 
v             Parameter     Integer           4 6 7 7 7 

Functuon Name : main (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
x             Variable      Integer           13 14 15 
y             Variable      Integer           13 14 15 


Checking Types...

Type Checking Finished
```

(sort.cm)
```
Building Symbol Table...

Symbol table:

<<gloabl scope>> (nested level : 0)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
main          Function      Void              32 
sort          Function      Void              19 40 
input         Function      Integer           0 36 
minloc        Function      Integer           3 24 
output        Function      Void              0 43 
x             Variable      IntegerArray      2 36 40 43 

Functuon Name : input (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------

Functuon Name : output (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
arg           Parameter     Integer           0 

Functuon Name : minloc (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
low           Parameter     Integer           3 5 6 7 
a             Parameter     IntegerArray      3 6 9 10 
i             Variable      Integer           4 7 8 9 10 11 13 13 
k             Variable      Integer           4 5 11 16 
x             Variable      Integer           4 6 9 10 
high          Parameter     Integer           3 8 

Functuon Name : minloc (nested level : 2)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------

Functuon Name : minloc (nested level : 3)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------

Functuon Name : sort (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
low           Parameter     Integer           19 21 
a             Parameter     IntegerArray      19 24 25 26 26 27 
i             Variable      Integer           20 21 22 24 26 27 28 28 
k             Variable      Integer           20 24 25 26 
high          Parameter     Integer           19 22 24 

Functuon Name : sort (nested level : 2)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
t             Variable      Integer           23 25 27 

Functuon Name : main (nested level : 1)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------
i             Variable      Integer           33 34 35 36 37 37 41 42 43 44 44 

Functuon Name : main (nested level : 2)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------

Functuon Name : main (nested level : 2)
Symbol Name   Symbol Type      Data Type       Line Numbers
-----------   -----------   ---------------   ---------------


Checking Types...

Type Checking Finished
```
###Step 4 – Code Generation
####개요
코드 생성기는 중간 코드가 기계에서 정상적으로 작동하기 위한 올바른 insruction들을 생성한다. 이 프로젝트에서는 Tiny Machine(TM)에서 동작하는 코드를 생성하고 생성된 코드를 Tiny Machine에서 동작시켜 본다.

####구현방법 및 구현과정
메모리는 위에서 아랫방향으로 성장하도록 구현했으며 static scope rule이 적용된다. 메모리는 아래의 구조로 성장하며 코드 생성기에서 사용하는 frame과 메모리와 관련된 레지스터의 이름 및 용도도 아래와 같다.
```
=============================================================
| 레지스터  |  메모리주소 |    호출함수   |        용도
=============================================================
|    gp    |    1023    |	   전역변수 / 전역함수
|          |     ...    |	   전역변수 / 전역함수
-------------------------------------------------------------
|          |     ...    |               | Old Frame Pointer
|          |     ...    | main Function | Return Frame
|          |     ...    |               | 지역변수
|          |     ...    |               | 임시 값 저장
-------------------------------------------------------------
|          |     ...    |               | Old Frame Pointer
|          |     ...    |               | Return Frame
|          |     ...    |  Function 1   | 매개변수
|          |     ...    |               | 지역변수
|          |     ...    |               | 임시 값 저장
-------------------------------------------------------------
|    mp    |     ...    |  Function 2   | Old Frame Pointer
|          |     ...    |               | ...
=============================================================
```
gp : 전역영역을 가리키는 레지스터 주소

mp : 현재 작업중인 함수를 가리키는 레지스터 주소

Old Frame Pointer : static scope rule에서 부모 scope의 주소를 담고 있는 frame

Return Frame : return 주소를 담고 있는 frame

임시 값 저장 : 프로그램 실행 중 임시로 값을 저장할 필요가 있는 경우 사용되는 frame들

메모리 접근은 레지스터와 frame offset을 통해 접근된다. 예를 들어 위 메모리 구조에서 Function 2의 Old Frame Pointer에 접근하고자 할 때에는 mp+ofpFO를 통해 접근하게 된다. 변수나 함수의 frame offset(FO)은 해당 변수나 함수의 정보를 담고 있는 BucketList의 memloc 변수를 통해 알 수 있다.

* main.c 수정사항
```c
#define NO_CODE FALSE
```
TreceCode : Code Generation 과정을 보여줄 것인지 나타내는 값

Code Generation 과정이 잘 이루어지는지 보기 위해 TRUE로 변경한다.
```c
int TraceAnalyze = FALSE;
int TraceCode = TRUE;
```

* symtab.c & symtab.h 수정사항

static scope rule에서 지역변수들은 scope가 달라도 같은 함수에 속해 있다면 메모리에서 같은 함수 frame에 속하게 된다. 따라서 memloc값을 할당할 때 scope가 달라져도 같은 함수에 속한 경우 memloc을 이전 scope에서 연속적으로 할당받을 수 있도록 수정한다.

locationStack, n_locationStack : 각 scope별로 다음에 할당될 memloc값을 저장하는 스택과 스택의 top변수 역할
copylocation 함수 : 같은 함수에 속한채로 scope만 달라진 경우 이전 scope에서 할당해줘야 했던 memloc값을 복사해서 가져온다.
addlocation 함수 : 매개변수 size만큼 메모리를 할당해준다.
```c
static int locationStack[SIZE];
static int n_locationStack = -1;
...
void copylocation()
{
	locationStack[n_locationStack] = locationStack[n_locationStack - 1];
	return;
}

int addlocation(int size)
{
	int r = locationStack[n_locationStack];
	locationStack[n_locationStack] += size;
	return r;
}
```

* analyze.c 수정사항

함수나 일반 변수의 경우 메모리는 하나만 할당받으면 되지만 배열인 경우 배열의 크기만큼 메모리를 할당받아야 한다. 따라서 VarDeclK, VarArrayDeclK, FuncDeclK의 경우를 수정해준다.

* cgen.c 수정사항

각 노드의 종류에 맞는 instruction을 생성하도록 코드를 수정한다. Control flow가 필요한 instruction의 경우 emitSkip함수와 emitRestore함수를 사용한다. instruction 생성이 모두 끝났다면 codeGen함수에서 Tiny Machine 초기화 과정을 거친 후 main함수를 실행하도록 코드를 추가한다.

####실행방법 및 실행결과
* 실행방법

cm파일을 C-Minus 컴파일러를 통해 컴파일하면 tm파일이 생성된다. 이 tm파일을 Tiny Machine에서 작동시키면 된다. 생성된 tm파일은 첨부된 test.tm, sort.tm파일을 참고하면 된다.
```
$ make cminus
$ ./cminus test.cm
$ ./tm test.tm
TM  simulation (enter h for help)...
Enter command: g

(sort.cm도 같은 방식으로 실행)
```
* 실행결과
```
(test.tm)
TM  simulation (enter h for help)...
Enter command: g
Enter value for IN instruction: 24
Enter value for IN instruction: 16
OUT instruction prints: 8
HALT: 0,0,0
Halted

(sort.tm)
TM  simulation (enter h for help)...
Enter command: g
Enter value for IN instruction: 99
Enter value for IN instruction: 88
Enter value for IN instruction: 77
Enter value for IN instruction: 66
Enter value for IN instruction: 55
Enter value for IN instruction: 44
Enter value for IN instruction: 33
Enter value for IN instruction: 22
Enter value for IN instruction: 11
Enter value for IN instruction: 444
OUT instruction prints: 11
OUT instruction prints: 22
OUT instruction prints: 33
OUT instruction prints: 44
OUT instruction prints: 55
OUT instruction prints: 66
OUT instruction prints: 77
OUT instruction prints: 88
OUT instruction prints: 99
OUT instruction prints: 444
HALT: 0,0,0
Halted
```
