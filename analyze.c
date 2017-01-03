/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"

int wasFunc = FALSE;
char *funcname = NULL;

/* counter for variable memory locations */
//static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{
	if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

static void typeError(TreeNode * t, char * message)
{
	fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
	Error = TRUE;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ 
	BucketList l;
	switch (t->nodekind)
	{
	case StmtK:
	  switch (t->kind.stmt)
	  {
	  case CallK:
		  l = st_lookup(t->attr.name);
		  if (l != NULL)
		  {
			  lineno_insert(t->lineno, l);
			  t->type = l->node->type;
		  }
		  else
			  symbolError(t->lineno, "Undeclared Function",t->attr.name);
		  break;
	  case CompStmtK:
		  if (wasFunc == TRUE)
			  wasFunc = FALSE;
		  else
		  {
			  scope_insert(scope_top()->name);
			  copylocation();
		  }
		  t->attr.scope = scope_top();
		  break;
	  default:
		  break;
	  }
	  break;
    case ExpK:
      switch (t->kind.exp)
      {
	  case VarDeclK:
	  case VarArrayDeclK:
		  if (t->isParam==FALSE && t->type == Void)
		  {
			  symbolError(t->lineno, "Void Cannot Be Variable", t->attr.name);
			  break;
		  }
		  if (t->isParam == TRUE && t->type == Void)
			  break;
		  if (st_lookup(t->attr.name) == NULL)
		  {
			  if(t->kind.exp==VarArrayDeclK && t->isParam==FALSE)
				  st_insert(t->attr.name, t->lineno, addlocation(t->arraysize), t);
			  else
				st_insert(t->attr.name, t->lineno, addlocation(1), t);
		  }
		  else
		  {
			  l=st_lookup_top(t->attr.name);
			  if(l!=NULL)
				symbolError(t->lineno, "Duplicate Variable Declaration", t->attr.name);
			  else
			  {
				  if (t->kind.exp == VarArrayDeclK && t->isParam == FALSE)
					  st_insert(t->attr.name, t->lineno, addlocation(t->arraysize), t);
				  else
					  st_insert(t->attr.name, t->lineno, addlocation(1), t);
			  }
		  }
          break;
	  case FuncDeclK:
		  if (st_lookup(t->attr.name) == NULL)
		  {
			  wasFunc = TRUE;
			  st_insert(t->attr.name, t->lineno, addlocation(1), t);
			  scope_insert(t->attr.name);
		  }
		  else
			  symbolError(t->lineno, "Duplicate Function Declaration",t->attr.name);
		  break;
	  case IdK:
		  l = st_lookup(t->attr.name);
		  if (l != NULL)
		  {
			  if (l->node->kind.exp==VarDeclK && t->child[0]!=NULL)
				  symbolError(t->lineno, "Integer Variable Cannot Be Used As Array Variable", t->attr.name);
			  else
				  lineno_insert(t->lineno, l);
		  }
		  else
			  symbolError(t->lineno, "Undeclared Variable",t->attr.name);
		  break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void symbolinit()
{
	TreeNode *t, *p;
	scope_init();

	t = newExpNode(FuncDeclK);
	t->attr.name = "input";
	t->type = Integer;
	p = newExpNode(VarDeclK);
	p->isParam = TRUE;
	p->type = Void;
	p->lineno = 0;
	t->child[0] = p;
	p = newStmtNode(CompStmtK);
	p->lineno = 0;
	p->child[0] = p->child[1] = NULL;
	t->child[1] = p;
	t->lineno = 0;
	traverse(t, insertNode, afterinsertNode);

	t = newExpNode(FuncDeclK);
	t->attr.name = "output";
	t->type = Void;
	p = newExpNode(VarDeclK);
	p->isParam = TRUE;
	p->type = Integer;
	p->attr.name = "arg";
	p->lineno = 0;
	t->child[0] = p;
	p = newStmtNode(CompStmtK);
	p->lineno = 0;
	p->child[0] = p->child[1] = NULL;
	t->child[1] = p;
	t->lineno = 0;
	traverse(t, insertNode, afterinsertNode);
}

void afterinsertNode(TreeNode *t)
{
	if ((t->nodekind == StmtK && t->kind.stmt == CompStmtK))
		scope_pop();
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
	symbolinit();
	addlocation(-2);
	traverse(syntaxTree,insertNode,afterinsertNode);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}


/* Procedure checkNode performs
 * type checking at a single tree node
 */

void beforeCheckNode(TreeNode *t)
{
	if (t->nodekind == ExpK && t->kind.exp == FuncDeclK)
		funcname = t->attr.name;

	if (t->nodekind == StmtK && t->kind.exp == CompStmtK)
		scope_push(t->attr.scope);
}

static void checkNode(TreeNode * t)
{ 
	ScopeList sp;
	BucketList l;
	TreeNode *args, *params, *tmp;
	int isArray;

	switch (t->nodekind)
  { 
	case ExpK:
      switch (t->kind.exp)
      { 
	  case AssignK:
		  if (t->child[1]->type==Void)
			  typeError(t, "There Is Nothing To Assign");
		  else
		  {
			  tmp = t->child[0];
			  l = st_lookup(tmp->attr.name);
			  if (l == NULL)
				  break;
			  else
			  {
				  if (tmp->child[0] == NULL && l->node->kind.exp == VarArrayDeclK)
					  typeError(t, "Array Variable Cannot Be Used As Integer Variable");
			  }
			  t->type = Integer;
		  }
		  break;
	  case OpK:
		  if (t->child[0]->kind.exp == ConstK || t->child[0]->kind.exp == OpK)
		  {
			  t->type = Integer;
			  break;
		  }
		  if (isCorrectType(t->child[0]) == FALSE)
		  {
			  typeError(t->child[0], "Array Variable Cannot Be Used As Integer Variable");
			  break;
		  }

		  if (t->child[1]->kind.exp == ConstK || t->child[1]->kind.exp == OpK)
		  {
			  t->type = Integer;
			  break;
		  }
		  if (isCorrectType(t->child[1]) == FALSE)
		  {
			  typeError(t->child[1], "Array Variable Cannot Be Used As Integer Variable");
			  break;
		  }
		  t->type = Integer;
		  break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { 
	  case CompStmtK:
		  scope_pop();
		  break;
	  case RetStmtK:
		  l = st_lookup(funcname);
		  if (l->node->type == Void && t->child[0] != NULL)
			  typeError(t, "Return Type Inconsistance (Void should be returned)");
		  else if (l->node->type == Integer && t->child[0] == NULL)
			  typeError(t, "Return Type Inconsistance (Integer value should be returned)");
		  else if (l->node->type == Integer && t->child[0]->kind.exp==IdK && t->child[0]->child[0]==NULL)
		  {
			  l = st_lookup(t->child[0]->attr.name);
			  if (l!=NULL && l->node->kind.exp == VarArrayDeclK)
				  typeError(t, "Array Variable Cannot Be Used As Integer Variable");
		  }
		  break;
	  case CallK:
		  l = st_lookup(t->attr.name);
		  if (l == NULL)
			  break;
		  args = t->child[0];
		  params = l->node->child[0];
		  if (l->node->kind.exp!=FuncDeclK)
		  {
			  typeError(t, "Variable Cannot Be Called as Function");
			  break;
		  }
		  if (params->type == Void)
		  {
			  if(args!=NULL)
				typeError(t, "Invalid Function Call (Parameter Type Error)");
			  break;
		  }
		  while (args != NULL && params != NULL)
		  {
			  if (params->kind.exp == VarArrayDeclK)
			  {
				  if (args->kind.exp == OpK || args->kind.exp == ConstK || args->kind.exp == AssignK || args->child[0]!=NULL)
					  break;
				  l = st_lookup(args->attr.name);
				  if (l->node->kind.exp == VarDeclK)
					  break;
				  args = args->sibling;
				  params = params->sibling;
				  continue;
			  }

			  if(args->kind.exp==OpK || args->kind.exp==ConstK || args->kind.exp==AssignK)
			  {
				  args = args->sibling;
				  params = params->sibling;
				  continue;
			  }
			  if (args->type != params->type)
				  break;
			  if (isCorrectType(args)==FALSE)
				  break;
			  args = args->sibling;
			  params = params->sibling;
		  }
		  if (args == NULL && params == NULL)
			  break;
		  else if (args == NULL || params == NULL)
			  typeError(t, "Invalid Function Call (Number of Parameter Error)");
		  else
			  typeError(t, "Invalid Function Call (Parameter Type Error)");
		  break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */

void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,beforeCheckNode,checkNode);
}

void symbolError(int lineno, char *msg, char *var)
{
	if (var != NULL)
		printf("Symbol Error %s at line %d: %s\n", var, lineno, msg);
	else
		printf("Symbol Error at line %d: %s\n", lineno, msg);
	Error = TRUE;
}

int isCorrectType(TreeNode *t)
{//only IDk
	BucketList l;
	l = st_lookup(t->attr.name);
	if (t->child[0] == NULL && l->node->kind.exp == VarDeclK)
		return TRUE;
	else if (t->child[0] != NULL && l->node->kind.exp == VarArrayDeclK)
		return TRUE;
	else
		return FALSE;
}