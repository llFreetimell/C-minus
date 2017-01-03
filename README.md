# C-minus
2016년 컴파일러 수업의 과제였던 C-minus 제작을 위한 프로젝트.

##전체개요
컴파일 과정은 크게 Scan, Parse, Symbol Table 생성, Type Check, Code 생성 등 5가지 단계로 구성된다. 이 프로젝트에서는 Tiny Compiler를 C-minus 문법에 맞게 코드를 수정하여 새로운 C-minus 컴파일러를 제작하는 것이 목표다.

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
<br>
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
```c
  // CLASSIFY COMMENTS(/* */) AND DIVISION(/)  
  case INOVER:
    if (c == '*') // COMMENTS(/* */)  
    {
      state = INCOMMENT_;
      save = FALSE;
    }
    else // DIVISION(/)
    {
      ungetNextChar();
      state = DONE;
      currentToken = OVER;
    }
    break;
  
  // MAKE THE COMMENTS IGNORED WHEN PARSING
  case INCOMMENT_:
    save = FALSE;
    if (c == '*')
    {
      c = getNextChar();
      if (c == '/') // END OF COMMENTS
        state = START;
      else if (c == EOF) // END OF SOURCE CODES
      {
        state = DONE;
        currentToken = ENDFILE;
      }
      else // NOT END OF COMMENTS
        ungetNextChar();
    }
    else if (c == EOF) // END OF COMMENTS
    {
      state = DONE;
      currentToken = ENDFILE;
    }
    break;
```
<br>
* globals.h 수정사항

MAXRESERVED : 예약어의 개수

scan.c의 reservedWords 배열의 개수가 6개이므로 6로 변경한다.
```c
#define MAXRESERVED 6
```

TokenType : 토큰의 타입(더하기, 곱하기 등)

C-minus 문법에서 사용될 토큰들을 추가해준다. 단, 사용하지 않는 토큰 6가지가 있는데 Makefile의 컴파일 에러를 방지하고자 그냥 두도록 한다. 그냥 두어도 C-minus 컴파일 과정에는 문제가 없다.
```c
typedef enum
/* book-keeping tokens */
{
	ENDFILE, ERROR,

	/* reserved words */
	// 6 WILL BE USED FINALLY AFTER SOME ARE ADDED
	IF, ELSE, WHILE, RETURN, INT, VOID,

	// 6 WILL NOT BE USED BUT REMAINED FOR COMPILE ERROR
	THEN, END, REPEAT, UNTIL, READ, WRITE,

	/* multicharacter tokens */
	ID, NUM,
	/* special symbols */
	// SOME ARE NEWLY ADDED
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

각 토큰별로 알맞은 TokenType을 반환하도록 추가, 수정한다. 다만 주석 토큰을 읽어 들이는 경우 주석의 끝까지 혹은 파일의 끝까지 input을 받아들여 주석부분을 무시하도록 아래와 같이 수정한다.
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
