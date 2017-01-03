/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  {
	  temp = ((temp << SHIFT) + key[i]) % SIZE;
	++i;
  }
  return temp;
}

/* the hash table */
static ScopeList scope[SIZE]; //SAVING ALL SCOPES
static ScopeList scopeStack[SIZE]; //TOOL FOR STATIC SCOPE RULE
static int locationStack[SIZE];
static int n_locationStack = -1;
static int n_scope = 0; //NUMBER OF SCOPES
static int n_scopeStack = -1; //TOP VARIABLE FOR SCOPE STACK
static char *findName;


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

char *findname()
{
	return findName;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
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

void lineno_insert(int lineno, BucketList bucket)
{
	LineList t = bucket->lines;
	while (t->next != NULL) t = t->next;
	t->next = (LineList)malloc(sizeof(struct LineListRec));
	t->next->lineno = lineno;
	t->next->next = NULL;
}

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 * BUT MODIFIED TO RETURN BUCKETLIST
 */
BucketList st_lookup ( char * name )	
{
	int h = hash(name);
	ScopeList sp = scopeStack[n_scopeStack];
  BucketList l=NULL;
  while (sp != NULL)
  {
	  l = sp->hashTable[h];
	  while ((l != NULL) && (strcmp(name, l->name) != 0))
		  l = l->next;
	  if (l != NULL)
	  {
		  findName = sp->name;
		  break;
	  }
	  sp = sp->parent;
  }
  return l;
}

BucketList st_lookup_top(char *name)
{
	int h = hash(name);
	ScopeList sp = scopeStack[n_scopeStack];
	BucketList l=NULL;
	if (sp != NULL)
	{
		l = sp->hashTable[h];
		while ((l != NULL) && (strcmp(name, l->name) != 0))
			l = l->next;
	}
	return l;
}

void scope_insert(char *name)
{
	ScopeList new;
	new = (ScopeList)malloc(sizeof(struct ScopeListRec));
	new->name = name;
	new->parent = scopeStack[n_scopeStack];
	new->nestedLevel = new->parent->nestedLevel + 1;

	scope[n_scope++] = new;
	scopeStack[++n_scopeStack] = new;
	locationStack[++n_locationStack] = 0;
}

void scope_init()
{
	ScopeList new = (ScopeList)malloc(sizeof(struct ScopeListRec));
	new->nestedLevel = 0;
	new->name = "global Init";
	scopeStack[++n_scopeStack] = new;
	scope[n_scope++] = new;
	locationStack[++n_locationStack] = 0;
}

void scope_push(ScopeList sp)
{
	scopeStack[++n_scopeStack] = sp;
}

void scope_pop()
{
	n_scopeStack--;
	n_locationStack--;
}

ScopeList scope_top()
{
	return scopeStack[n_scopeStack];
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ 
	int i,j;
	ScopeList sp;
	BucketList l;
	
	sp = scope[0];
	fprintf(listing, "<<gloabl scope>> (nested level : 0)\n");
	fprintf(listing, "Symbol Name   Symbol Type      Data Type       Line Numbers\n");
	fprintf(listing, "-----------   -----------   ---------------   ---------------\n");
  printSymTabRow(listing, sp->hashTable);
  fprintf(listing, "\n");

  for (i = 1;i < n_scope;i++)
  {
	  sp = scope[i];
	  fprintf(listing, "Functuon Name : %s (nested level : %d)\n", sp->name,sp->nestedLevel);
	  fprintf(listing, "Symbol Name   Symbol Type      Data Type       Line Numbers\n");
	  fprintf(listing, "-----------   -----------   ---------------   ---------------\n");
	  printSymTabRow(listing, sp->hashTable);
	  fprintf(listing, "\n");
  }
  
} /* printSymTab */

void printSymTabRow(FILE *listing, BucketList *hashtable)
{
	int i;
	for (i = 0;i < SIZE;i++)
	{
		BucketList l = hashtable[i];
		
		while (l != NULL)
		{
			LineList t = l->lines;
			TreeNode *node = l->node;

			fprintf(listing, "%-11s   ", l->name);

			switch (node->kind.exp)
			{
			case VarDeclK:
			case VarArrayDeclK:
				if (node->isParam == TRUE)
					fprintf(listing, "Parameter     ");
				else
					fprintf(listing, "Variable      ");
				break;
			case FuncDeclK:
				fprintf(listing, "Function      ");
				break;
			default:
				break;
			}

			switch (node->type) {
			case Void:
				fprintf(listing, "Void              ");
				break;
			case Integer:
				if(node->kind.exp==VarDeclK || node->kind.exp==FuncDeclK)
					fprintf(listing, "Integer           ");
				else
					fprintf(listing, "IntegerArray      ");
				break;
			case Boolean:
				fprintf(listing, "Boolean        ");
				break;
			default:
				break;
			}

			while (t != NULL)
			{
				fprintf(listing, "%d ", t->lineno);
				t = t->next;
			}
			fprintf(listing, "\n");
			l = l->next;
		}
	}
}