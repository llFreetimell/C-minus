/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
	START, INEQ, INCOMMENT, INNUM, INID, DONE, INLT, INGT, INNE, INOVER, INCOMMENT_
}
StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

							 /* getNextChar fetches the next non-blank character
							 from lineBuf, reading in a new line if lineBuf is
							 exhausted */
static int getNextChar(void)
{
	if (!(linepos < bufsize))
	{
		lineno++;
		if (fgets(lineBuf, BUFLEN - 1, source))
		{
			if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct
{
	char* str;
	TokenType tok;
} reservedWords[MAXRESERVED]
= { // 6 RESERVED WORDS WILL BE USED
	{ "if",IF },{ "else",ELSE },{ "while",WHILE },{ "return",RETURN },{ "int",INT },{ "void",VOID }};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char * s)
{
	int i;
	for (i = 0;i<MAXRESERVED;i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
* next token in source file
*/
TokenType getToken(void)
{  /* index for storing into tokenString */
	int tokenStringIndex = 0;
	/* holds current token to be returned */
	TokenType currentToken;
	/* current state - always begins at START */
	StateType state = START;
	/* flag to indicate save to tokenString */
	int save;
	while (state != DONE)
	{
		int c = getNextChar();
		save = TRUE;
		switch (state)
		{
		case START:
			if (isdigit(c))
				state = INNUM;
			else if (isalpha(c))
				state = INID;
			else if ((c == ' ') || (c == '\t') || (c == '\n'))
				save = FALSE;

			/* '{' TOKEN WOULD BE NOT USED FOR COMMENTS
			else if (c == '{')
			{
				save = FALSE;
				state = INCOMMENT;
			}
			*/

			else if (c == '/')
			{
				save = FALSE;
				state = INOVER;
			}
			else if (c == '=')
			{
				save = FALSE;
				state = INEQ;
			}
			else if (c == '<')
			{
				save = FALSE;
				state = INLT;
			}
			else if (c == '>')
			{
				save = FALSE;
				state = INGT;
			}
			else if (c == '!')
			{
				save = FALSE;
				state = INNE;
			}
			else
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '{':
					currentToken = LCURLY;
					break;
				case '}':
					currentToken = RCURLY;
					break;
				case '[':
					currentToken = LBRACE;
					break;
				case ']':
					currentToken = RBRACE;
					break;
				case ';':
					currentToken = SEMI;
					break;
				case ',':
					currentToken = COMMA;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;

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

		// CLASSIFY ASSIGN(=) AND EQUALITY(==)
		case INEQ:
			state = DONE;
			if (c == '=') // EQUALITY(==)
				currentToken = EQ;
			else // ASSIGN(=)
			{
				currentToken = ASSIGN;
				ungetNextChar();
			}
			break;

		// CLASSIFY LESS THAN(<) AND LESS EQUAL(<=)
		case INLT:
			state = DONE;
			if (c == '=') // LESS EQUAL(<=)
				currentToken = LE;
			else // LESS THAN(<)
			{
				currentToken = LT;
				ungetNextChar();
			}
			break;

		// CLASSIFY GREATER THAN(>) AND GREATER EQUAL(>=)
		case INGT:
			state = DONE;
			if (c == '=') // GREATER EQUAL(>=)
				currentToken = GE;
			else // GREATER THAN(>)
			{
				currentToken = GT;
				ungetNextChar();
			}
			break;

		// CLASSIFY NOT EQUAL(!=) AND ERROR
		case INNE:
			state = DONE;
			if (c == '=') // NOT EQUAL(!=)
				currentToken = NE;
			else // ERROR
				currentToken = ERROR;
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

		/* NOT USED ANYMORE
		case INCOMMENT:
			save = FALSE;
			if (c == EOF)
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			else if (c == '}') state = START;
			break;
		*/

		case INNUM:
			if (!isdigit(c))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if (!isalpha(c))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case DONE:
		default: /* should never happen */
			fprintf(listing, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
			tokenString[tokenStringIndex++] = (char)c;
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
				currentToken = reservedLookup(tokenString);
		}
	}
	if (TraceScan) {
		fprintf(listing, "\t%d: ", lineno);
		printToken(currentToken, tokenString);
	}
	return currentToken;
} /* end getToken */