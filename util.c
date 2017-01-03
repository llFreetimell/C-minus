/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
* and its lexeme to the listing file
*/
void printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
	// 6 TOKENS WILL BE USED
	case IF:
	case WHILE:
	case RETURN:
	case INT:
	case VOID:
	case ELSE:

		fprintf(listing,
			"reserved word: %s\n", tokenString);
		break;

	// ADDITION AND MODIFICATION OF SOME TOKENS
	case ASSIGN: fprintf(listing, "=\n"); break;
	case EQ: fprintf(listing, "==\n"); break;
	case NE: fprintf(listing, "!=\n"); break;
	case LT: fprintf(listing, "<\n"); break;
	case LE: fprintf(listing, "<=\n"); break;
	case GT: fprintf(listing, ">\n"); break;
	case GE: fprintf(listing, ">=\n"); break;
	case LPAREN: fprintf(listing, "(\n"); break;
	case RPAREN: fprintf(listing, ")\n"); break;
	case LBRACE: fprintf(listing, "[\n"); break;
	case RBRACE: fprintf(listing, "]\n"); break;
	case LCURLY: fprintf(listing, "{\n"); break;
	case RCURLY: fprintf(listing, "}\n"); break;
	case SEMI: fprintf(listing, ";\n"); break;
	case COMMA: fprintf(listing, ",\n"); break;
	case PLUS: fprintf(listing, "+\n"); break;
	case MINUS: fprintf(listing, "-\n"); break;
	case TIMES: fprintf(listing, "*\n"); break;
	case OVER: fprintf(listing, "/\n"); break;
	case ENDFILE: fprintf(listing, "EOF\n"); break;
	case NUM:
		fprintf(listing,
			"NUM, val= %s\n", tokenString);
		break;
	case ID:
		fprintf(listing,
			"ID, name= %s\n", tokenString);
		break;
	case ERROR:
		fprintf(listing,
			"ERROR: %s\n", tokenString);
		break;
	default: /* should never happen */
		fprintf(listing, "Unknown token: %d\n", token);
	}
}

/* Function newStmtNode creates a new statement
* node for syntax tree construction
*/
TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
	}
	return t;
}

/* Function newExpNode creates a new expression
* node for syntax tree construction
*/
TreeNode * newExpNode(ExpKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
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

/* Function copyString allocates and makes a new
* copy of an existing string
*/
char * copyString(char * s)
{
	int n;
	char * t;
	if (s == NULL) return NULL;
	n = strlen(s) + 1;
	t = malloc(n);
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else strcpy(t, s);
	return t;
}

/* Variable indentno is used by printTree to
* store current number of spaces to indent
*/
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
	int i;
	for (i = 0;i<indentno;i++)
		fprintf(listing, " ");
}

char *typeName(ExpType type)
{
	static char i[] = "int";
	static char v[] = "void";
	static char invalid[] = "<<invalid type>>";

	switch (type)
	{
	case Integer: return i; break;
	case Void:    return v; break;
	default:      return invalid;
	}
}


/* procedure printTree prints a syntax tree to the
* listing file using indentation to indicate subtrees
*/
void printTree(TreeNode * tree)
{
	int i;
	INDENT;
	while (tree != NULL) {
		printSpaces();
		if (tree->nodekind == StmtK)
		{
			switch (tree->kind.stmt) {
			case CompStmtK:
				fprintf(listing, "Compound Statement :\n");
				break;
			case SelStmtK:
				if(tree->child[2]!=NULL)
					fprintf(listing, "If (condition) (body) (else) \n");
				else
					fprintf(listing, "If (condition) (body)\n");
				break;
			case IterStmtK:
				fprintf(listing, "While (condition) (body) \n");
				break;
			case RetStmtK:
				if(tree->child[0]==NULL)
					fprintf(listing, "Return Statement, with NOTHING\n");
				else
					fprintf(listing, "Return Statement, with below\n");
				break;
			case CallK:
				if(tree->child[0]!=NULL)
					fprintf(listing, "Call, name : %s, with arguments below\n",tree->attr.name);
				else
					fprintf(listing, "Call, name : %s, with NOTHING\n", tree->attr.name);
				break;
			default:
				fprintf(listing, "Unknown ExpNode kind\n");
				break;
			}
		}
		else if (tree->nodekind == ExpK)
		{
			switch (tree->kind.exp) {
			case VarDeclK:
				if(tree->isParam==TRUE)
					fprintf(listing, "Single Parameter, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
				else
					fprintf(listing, "Var Declaration, name : %s, type : %s\n",tree->attr.name, typeName(tree->type));
				break;
			case VarArrayDeclK:
				if(tree->isParam==TRUE)
					fprintf(listing, "Array Parameter, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
				else
					fprintf(listing, "Array Var Declaration, name : %s, type : %s, size : %d\n", tree->attr.name, typeName(tree->type), tree->arraysize);
				break;
			case FuncDeclK:
				fprintf(listing, "Function Declaration, name : %s, type : %s\n", tree->attr.name, typeName(tree->type));
				break;
			case AssignK:
				fprintf(listing, "Assign : (destination) (source) \n");
				break;
			case OpK:
				fprintf(listing, "Op : ");
				printToken(tree->attr.op, "\0");
				break;
			case IdK:
				fprintf(listing, "Id : %s\n",tree->attr.name);
				break;
			case ConstK:
				fprintf(listing, "Const : %d\n",tree->attr.val);
				break;
			default:
				fprintf(listing, "Unknown ExpNode kind\n");
				break;
			}
		}
		else fprintf(listing, "Unknown node kind\n");
		for (i = 0;i<MAXCHILDREN;i++)
			printTree(tree->child[i]);
		tree = tree->sibling;
	}
	UNINDENT;
}


