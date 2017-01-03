/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the TINY compiler                            */
/* (generates code for the TM machine)              */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

#define ofpFO 0
#define retFO -1
#define initFO -2

static char buffer[1000];
static int globalOffset = 0;
static int localOffset = initFO;
static int isInFunc = FALSE;
static int mainFuncLoc = 0;	
static int numOfParams = 0;

static int getBlockOffset(TreeNode *list) {
	TreeNode *node = list;
	int offset = 0;
	if (list == NULL) {
		return 0;
	}
	else if (list->nodekind == ExpK && (list->kind.exp==VarDeclK || list->kind.exp==VarArrayDeclK))
	{
		if (list->isParam == FALSE)
		{
			while (node != NULL) 
			{
				switch (node->kind.exp)
				{
					case VarDeclK:
						++offset;
						break;
					case VarArrayDeclK:
						offset += node->arraysize;
						break;
					default:
						break;
				}
				node = node->sibling;
			}
		}
		else
		{
			while (node != NULL) 
			{
				++offset;
				node = node->sibling;
			}
		}
	}
	return offset;
}


/* prototype for internal recursive code generator */
static void cGen (TreeNode * tree);
static void genExp(TreeNode *tree, int lhs);

/* Procedure genStmt generates code at a statement node */
static void genStmt( TreeNode * tree)
{ TreeNode * p1, * p2, * p3;
  int savedLoc1,savedLoc2,currentLoc;
  int loc,numOfArgs,offset;
  switch (tree->kind.stmt) {

      case SelStmtK :
         if (TraceCode) emitComment("-> if") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         p3 = tree->child[2] ;
         /* generate code for test expression */
         cGen(p1);
         savedLoc1 = emitSkip(1) ;
         emitComment("if: jump to else belongs here");
         /* recurse on then part */
         cGen(p2);
         savedLoc2 = emitSkip(1) ;
         emitComment("if: jump to end belongs here");
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc1) ;
         emitRM_Abs("JEQ",ac,currentLoc,"if: jmp to else");
         emitRestore() ;
         /* recurse on else part */
         cGen(p3);
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc2) ;
         emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
         emitRestore() ;
         if (TraceCode)  emitComment("<- if") ;
         break;

      case IterStmtK:
         if (TraceCode) emitComment("-> while") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         savedLoc1 = emitSkip(0);
         emitComment("while : jump after body comes back here");
         /* generate code for test */
         cGen(p1);
		 savedLoc2 = emitSkip(1);
		 emitComment("while : jump to end belongs here");
         /* generate code for body */
         cGen(p2);
		 emitRM_Abs("LDA", pc, savedLoc1, "while : jmp back to test");
		 currentLoc = emitSkip(0);
		 emitBackup(savedLoc2);
		 emitRM_Abs("JEQ", ac, currentLoc, "while : jmp to end");
		 emitRestore();
         if (TraceCode)  emitComment("<- while") ;
         break;

	  case RetStmtK:
		  if (TraceCode) emitComment("-> return");
		  p1 = tree->child[0];
		  cGen(p1);
		  emitRM("LD", pc, retFO, mp, "return : to caller");
		  if (TraceCode) emitComment("<- return");
		  break;

	  case CallK:
		  if (TraceCode) {
			  sprintf(buffer, "-> Call (%s)", tree->attr.name);
			  emitComment(buffer);
		  }
		  numOfArgs = 0;
		  p1 = tree->child[0];
		  while (p1 != NULL) {
			  if (p1->nodekind == StmtK)
				  genStmt(p1);
			  else if (p1->kind.exp==VarArrayDeclK)
				  genExp(p1, TRUE);
			  else
				  genExp(p1, FALSE);
			  emitRM("ST", ac, localOffset + initFO - (numOfArgs++), mp, "call : push argument");
			  p1 = p1->sibling;
		  }

		  if (strcmp(tree->attr.name, "input") == 0) {
			  emitRO("IN", ac, 0, 0, "read integer value");
		  }
		  else if (strcmp(tree->attr.name, "output") == 0) {
			  emitRM("LD", ac, localOffset + initFO, mp, "load arg to ac");
			  emitRO("OUT", ac, 0, 0, "write ac");
		  }
		  else {
			  emitRM("ST", mp, localOffset + ofpFO, mp, "call : store current mp");
			  emitRM("LDA", mp, localOffset, mp, "call : push new frame");
			  emitRM("LDA", ac, 1, pc, "call : save return in ac");
			  loc = -(st_lookup(tree->attr.name)->memloc);
			  emitRM("LD", pc, loc, gp, "call : relative jump to function entry");
			  emitRM("LD", mp, ofpFO, mp, "call : pop current frame");
		  }
		  if (TraceCode) {
			  sprintf(buffer, "<- Call (%s)", tree->attr.name);
			  emitComment(buffer);
		  }
		  break;
		  
	  case CompStmtK:
		  if (TraceCode) emitComment("-> compound");
		  p1 = tree->child[0];
		  p2 = tree->child[1];
		  offset = getBlockOffset(p1);
		  localOffset -= offset;
		  scope_push(tree->attr.scope);
		  cGen(p2);
		  scope_pop();
		  localOffset += offset;
		  if (TraceCode) emitComment("<- compound");
		  break;

      default:
         break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp( TreeNode * tree,int lhs)
{ int loc,loc2,numOfArgs,size,varOffset, loadFuncLoc, jmpLoc, funcBodyLoc, nextDeclLoc,offset;
  TreeNode * p1, * p2;
  BucketList bucket;
  switch (tree->kind.exp) {

    case ConstK :
      if (TraceCode) emitComment("-> Const") ;
      /* gen code to load integer constant using LDC */
      emitRM("LDC",ac,tree->attr.val,0,"load const");
      if (TraceCode)  emitComment("<- Const") ;
      break; /* ConstK */

    case OpK :
         if (TraceCode) emitComment("-> Op") ;
         p1 = tree->child[0];
         p2 = tree->child[1];
         /* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,localOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++localOffset,mp,"op: load left");
         switch (tree->attr.op) {
		 case PLUS:
			 emitRO("ADD", ac, ac1, ac, "op +");
			 break;
		 case MINUS:
			 emitRO("SUB", ac, ac1, ac, "op -");
			 break;
		 case TIMES:
			 emitRO("MUL", ac, ac1, ac, "op *");
			 break;
		 case OVER:
			 emitRO("DIV", ac, ac1, ac, "op /");
			 break;
		 case LT:
			 emitRO("SUB", ac, ac1, ac, "op <");
			 emitRM("JLT", ac, 2, pc, "br if true");
			 emitRM("LDC", ac, 0, ac, "false case");
			 emitRM("LDA", pc, 1, pc, "unconditional jmp");
			 emitRM("LDC", ac, 1, ac, "true case");
			 break;
		 case LE:
			 emitRO("SUB", ac, ac1, ac, "op <=");
			 emitRM("JLE", ac, 2, pc, "br if true");
			 emitRM("LDC", ac, 0, ac, "false case");
			 emitRM("LDA", pc, 1, pc, "unconditional jmp");
			 emitRM("LDC", ac, 1, ac, "true case");
			 break;
		 case GT:
			 emitRO("SUB", ac, ac1, ac, "op >");
			 emitRM("JGT", ac, 2, pc, "br if true");
			 emitRM("LDC", ac, 0, ac, "false case");
			 emitRM("LDA", pc, 1, pc, "unconditional jmp");
			 emitRM("LDC", ac, 1, ac, "true case");
			 break;
		 case GE:
			 emitRO("SUB", ac, ac1, ac, "op >=");
			 emitRM("JGE", ac, 2, pc, "br if true");
			 emitRM("LDC", ac, 0, ac, "false case");
			 emitRM("LDA", pc, 1, pc, "unconditional jmp");
			 emitRM("LDC", ac, 1, ac, "true case");
			 break;
		 case EQ:
			 emitRO("SUB", ac, ac1, ac, "op ==");
			 emitRM("JEQ", ac, 2, pc, "br if true");
			 emitRM("LDC", ac, 0, ac, "false case");
			 emitRM("LDA", pc, 1, pc, "unconditional jmp");
			 emitRM("LDC", ac, 1, ac, "true case");
			 break;
		 case NE:
			 emitRO("SUB", ac, ac1, ac, "op !=");
			 emitRM("JNE", ac, 2, pc, "br if true");
			 emitRM("LDC", ac, 0, ac, "false case");
			 emitRM("LDA", pc, 1, pc, "unconditional jmp");
			 emitRM("LDC", ac, 1, ac, "true case");
			 break;
		 default:
			 emitComment("BUG: Unknown operator");
			 break;

         } /* case op */
         if (TraceCode)  emitComment("<- Op") ;
         break; /* OpK */

	case AssignK:
		if (TraceCode) emitComment("-> assign");
		p1 = tree->child[0];
		p2 = tree->child[1];

		genExp(p1, TRUE);
		emitRM("ST", ac, localOffset--, mp, "assign : push left address");

		cGen(p2);
		emitRM("LD", ac1, ++localOffset, mp, "assign : load left address");
		emitRM("ST", ac, 0, ac1, "assign : store value");
		if (TraceCode) emitComment("<- assign");
		break;

	case VarDeclK:
	case VarArrayDeclK:
		if (tree->isParam == FALSE)
		{
			if (TraceCode) emitComment("-> Variable declaration");

			if (tree->kind.exp == VarArrayDeclK)
				size = tree->arraysize;
			else
				size = 1;

			if (isInFunc == TRUE)
				localOffset -= size;
			else
				globalOffset -= size;

			if (TraceCode) emitComment("<- Variable declaration");
		}
		else if(tree->type!=Void)
		{
			if (TraceCode)
			{
				emitComment("-> param");
				emitComment(tree->attr.name);
			}

			--localOffset;
			++numOfParams;

			if (TraceCode) emitComment("<- param");
		}
		break;

	case IdK:
		if (TraceCode) {
			sprintf(buffer, "-> Id (%s)", tree->attr.name);
			emitComment(buffer);
		}
		bucket = st_lookup(tree->attr.name);

		if (bucket != NULL && strcmp("global Init", findname()) != 0)
				loc = bucket->memloc;
		else
			loc = -1;

		if (loc >= 0)
			varOffset = initFO - loc;
		else
		{
			bucket = st_lookup(tree->attr.name);
			varOffset = -(bucket->memloc);
		}

		emitRM("LDC", ac, varOffset, 0, "id: load varOffset");


		if (bucket->node->kind.exp==VarArrayDeclK) 
		{
			if (loc >= 0 && loc < numOfParams) {
				emitRO("ADD", ac, mp, ac, "id : load the memory address of base address of array to ac");
				emitRO("LD", ac, 0, ac, "id : load the base address of array to ac");
			}
			else {
				if (loc >= 0)
					emitRO("ADD", ac, mp, ac, "id : calculate the address");
				else
					emitRO("ADD", ac, gp, ac, "id : calculate the address");
			}

			emitRM("ST", ac, localOffset--, mp, "id : push base address");

			p1 = tree->child[0];
			if (p1 == NULL)
			{
				lhs = TRUE;
				emitRM("LDC", ac, 0, 0, "id : no need for index");
			}
			cGen(p1);
			emitRM("LD", ac1, ++localOffset, mp, "id : pop base address");
			emitRO("SUB", ac, ac1, ac, "id : calculate element address with index");				
		}
		else 
		{
			if (loc >= 0)
				emitRO("ADD", ac, mp, ac, "id : calculate the address");
			else
				emitRO("ADD", ac, gp, ac, "id : calculate the address");
		}

		if (lhs)
			emitRM("LDA", ac, 0, ac, "load id address");
		else
			emitRM("LD", ac, 0, ac, "load id value");

		if (TraceCode) {
			sprintf(buffer, "<- Id (%s)", tree->attr.name);
			emitComment(buffer);
		}
		break;

	case FuncDeclK:
		if (TraceCode) {
			sprintf(buffer, "-> Function Declaration (%s)", tree->attr.name);
			emitComment(buffer);
		}

		p1 = tree->child[0];
		p2 = tree->child[1];

		isInFunc = TRUE;
		bucket = st_lookup(tree->attr.name);
		loc = -(bucket->memloc);
		loadFuncLoc = emitSkip(1);
		emitRM("ST", ac1, loc, gp, "function : store the location of func. entry");
		--globalOffset;

		jmpLoc = emitSkip(1);
		emitComment("function : unconditional jump to next declaration belongs here");

		funcBodyLoc = emitSkip(0);
		emitComment("function : function body starts here");

		emitBackup(loadFuncLoc);
		emitRM("LDC", ac1, funcBodyLoc, 0, "function : load function location");
		emitRestore();

		emitRM("ST", ac, retFO, mp, "function : store return address");

		localOffset = initFO;
		numOfParams = 0;
		cGen(p1);

		if (strcmp(tree->attr.name, "main") == 0)
			mainFuncLoc = funcBodyLoc;

		cGen(p2);

		emitRM("LD", pc, retFO, mp, "func : load pc with return address");

		nextDeclLoc = emitSkip(0);
		emitBackup(jmpLoc);
		emitRM_Abs("LDA", pc, nextDeclLoc,"func : unconditional jump to next declaration");
		emitRestore();

		isInFunc = FALSE;

		if (TraceCode) {
			sprintf(buffer, "<- Function Declaration (%s)", tree->attr.name);
			emitComment(buffer);
		}

		break;

    default:
      break;
  }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen( TreeNode * tree)
{ if (tree != NULL)
  {
	  switch (tree->nodekind) {
      case StmtK:
        genStmt(tree);
        break;
      case ExpK:
        genExp(tree,FALSE);
        break;
      default:
        break;
    }
    cGen(tree->sibling);
  }
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile)
{  char * s = malloc(strlen(codefile)+7);
   strcpy(s,"File: ");
   strcat(s,codefile);
   emitComment("TINY Compilation to TM Code");
   emitComment(s);
   /* generate standard prelude */
   emitComment("Standard prelude:");

   emitRM("LD",gp,0,ac,"load maxaddress from location 0");
   emitRM("LDA", mp, 0, gp, "copy gp to mp");
   emitRM("ST",ac,0,ac,"clear location 0");
   emitComment("End of standard prelude.");
   
   /* generate code for TINY program */
   cGen(syntaxTree);
   scope_pop();
   /* finish */

   emitRM("LDC", ac, globalOffset, 0, "init: load globalOffset");
   emitRO("ADD", mp, mp, ac, "init: initialize mp with globalOffset");

   emitRM("ST", mp, ofpFO, mp, "call: store current mp");
   emitRM("LDA", mp, 0, mp, "call: push new frame");
   emitRM("LDA", ac, 1, pc, "call: save return in ac");
   emitRM("LDC", pc, mainFuncLoc, 0, "jump to main");
   emitRM("LD", mp, ofpFO, mp, "call: pop current frame");

   emitComment("End of execution.");
   emitRO("HALT",0,0,0,"");	
}
