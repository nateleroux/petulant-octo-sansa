/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *                     TABLEAUS                           * */
/* *                                                        * */
/* *  $Module:   TABLEAU                                    * */
/* *                                                        * */
/* *  Copyright (C) 1998, 1999, 2000, 2001                  * */
/* *  MPI fuer Informatik                                   * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the GNU        * */
/* *  General Public License as published by the Free       * */
/* *  Software Foundation; either version 2 of the License, * */
/* *  or (at your option) any later version.                * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the GNU General Public * */
/* *  License for more details.                             * */
/* *                                                        * */
/* *  You should have received a copy of the GNU General    * */
/* *  Public License along with this program; if not, write * */
/* *  to the Free Software Foundation, Inc., 59 Temple      * */
/* *  Place, Suite 330, Boston, MA  02111-1307  USA         * */
/* *                                                        * */
/* *                                                        * */
/* $Revision: 21527 $                                        * */
/* $State$                                            * */
/* $Date: 2005-04-24 23:10:28 -0500 (Sun, 24 Apr 2005) $                             * */
/* $Author: duraid $                                       * */
/* *                                                        * */
/* *             Contact:                                   * */
/* *             Christoph Weidenbach                       * */
/* *             MPI fuer Informatik                        * */
/* *             Stuhlsatzenhausweg 85                      * */
/* *             66123 Saarbruecken                         * */
/* *             Email: weidenb@mpi-sb.mpg.de               * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/* $RCSfile$ */

#ifndef _TABLEAU_
#define _TABLEAU_


#include "list.h"
#include "clause.h"

typedef struct TABLEAU_HELP {
  LIST   Clauses;               /* all clauses generated on the split level */
  CLAUSE SplitClause;           /* this levels split clause */

  CLAUSE LeftSplitClause;       /* first clause generated by split  */
  LIST   RightSplitClauses;     /* other clauses generated by split */

  struct TABLEAU_HELP* LeftBranch;  /* branch corresponding to first split clause  */
  struct TABLEAU_HELP* RightBranch; /* branch corresponding to other split clauses */

  int  Label;
} TABLEAU_NODE, *TABLEAU;


typedef struct {
  TABLEAU *Path;      /* An array of tableaux */
  int     Length;
  int     MaxLength;
  
} TABPATH_NODE, *TABPATH;

/* tableau output formats */
typedef enum { DAVINCI, XVCG } GRAPHFORMAT;


TABPATH tab_PathCreate(int, TABLEAU);
void    tab_PathDelete(TABPATH);
BOOL    tab_PathContainsClause(TABPATH, CLAUSE);
void    tab_AddClauseOnItsLevel(CLAUSE, TABPATH);
void    tab_AddSplitAtCursor(TABPATH, BOOL);
BOOL    tab_IsClosed(TABLEAU);
TABLEAU tab_PruneClosedBranches(TABLEAU, LIST*);
int     tab_Depth(TABLEAU);
void    tab_SetSplitLevels(TABLEAU T);
TABLEAU tab_RemoveIncompleteSplits(TABLEAU, LIST*);
void    tab_PathDelete(TABPATH);
void    tab_Delete(TABLEAU);
void    tab_ToClauseList(TABLEAU, LIST*);
void    tab_GetEarliestEmptyClauses(TABLEAU, LIST*);
void    tab_WriteTableau(TABLEAU, const char*, GRAPHFORMAT);
void    tab_CheckEmpties(TABLEAU);
void    tab_GetAllEmptyClauses(TABLEAU, LIST*);
void    tab_LabelNodes(TABLEAU);

/* inline functions for tableau paths */

static __inline__ int  tab_PathLength(TABPATH TabPath)
{
  return TabPath->Length;
}

static __inline__ TABLEAU tab_PathNthNode(TABPATH TabPath, int n)
{
#ifdef CHECK
  if (n > TabPath->MaxLength) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_PathNthNode:");
    misc_ErrorReport(" Path length is %d,", tab_PathLength(TabPath));
    misc_ErrorReport("\nnode %d was requested.\n", n);
    misc_FinishErrorReport();
  }
#endif  

  return TabPath->Path[n];
}

static __inline__ TABPATH tab_PathPush(TABLEAU Tab, TABPATH TabPath)
{
  TabPath->Length++;
  TabPath->Path[TabPath->Length] = Tab;
  
#ifdef CHECK
  if (TabPath->Length > TabPath->MaxLength) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_PathPush: Maximum path length %d exceeded\n",
		     TabPath->MaxLength);
    misc_FinishErrorReport();
  }
#endif
  
  return TabPath;
}

static __inline__ TABLEAU tab_EmptyTableau(void)
{
  return (TABLEAU)NULL;
}

static __inline__ TABPATH tab_PathPop(TABPATH TabPath)
{
#ifdef CHECK
  if (TabPath->Length <= 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_PathPop: Popping from empty path.\n");
    misc_FinishErrorReport();
  }
#endif

  TabPath->Path[TabPath->Length--] = tab_EmptyTableau();
  
  return TabPath;
}

static __inline__ BOOL tab_PathEmpty(TABPATH P)
{
  return (tab_PathLength(P) == 0);
}

static __inline__ TABLEAU tab_CreateNode(void)
{
  TABLEAU Node;

  Node = (TABLEAU)memory_Malloc(sizeof(TABLEAU_NODE));
  Node->RightBranch = (TABLEAU)NULL;
  Node->LeftBranch  = (TABLEAU)NULL;
  Node->SplitClause = (CLAUSE)NULL;
  Node->LeftSplitClause   = (CLAUSE)NULL;
  Node->RightSplitClauses = list_Nil();

  Node->Clauses     = list_Nil();
 
#ifdef USE_LABEL
  Node->Label       = 0;
#endif
  
  return Node;
}

static __inline__ TABPATH tab_PathPrefix(int Level, TABPATH TabPath)
{
  TabPath->Length = Level;
  return TabPath;
}

static __inline__ TABLEAU tab_PathTop(TABPATH Path)
{
  return tab_PathNthNode(Path, tab_PathLength(Path));
}


static __inline__ BOOL tab_IsEmpty(TABLEAU Tab)
{
 return (Tab == tab_EmptyTableau());
}

static __inline__ TABLEAU tab_RightBranch(TABLEAU Tab)
{
 return Tab->RightBranch;
}

static __inline__ TABLEAU tab_LeftBranch(TABLEAU Tab)
{
 return Tab->LeftBranch;
}

static __inline__ void tab_SetRightBranch(TABLEAU Tab, TABLEAU SubTab)
{
  Tab->RightBranch = SubTab;
} 

static __inline__ void tab_SetLeftBranch(TABLEAU Tab, TABLEAU SubTab)
{
  Tab->LeftBranch = SubTab;
} 

static __inline__ BOOL tab_RightBranchIsEmpty(TABLEAU Tab)
{
  return (Tab->RightBranch == tab_EmptyTableau());
}

static __inline__ BOOL tab_LeftBranchIsEmpty(TABLEAU Tab)
{
  return (Tab->LeftBranch == tab_EmptyTableau());
}

static __inline__ CLAUSE tab_SplitClause(TABLEAU Tab)
{
  return Tab->SplitClause;
}

static __inline__ void tab_SetSplitClause(TABLEAU Tab, CLAUSE C)
{
  Tab->SplitClause = C;
}

static __inline__ BOOL tab_HasSplit(TABLEAU T)
{
  return (tab_SplitClause(T) != clause_Null());
}

static __inline__ void tab_AddClause(CLAUSE C,TABLEAU T)
{
  T->Clauses = list_Cons(C,T->Clauses);
}

static __inline__ LIST tab_Clauses(TABLEAU T)
{
  return T->Clauses;
}

static __inline__ void tab_SetClauses(TABLEAU T, LIST Clauses)
{
  T->Clauses = Clauses;
}

static __inline__ CLAUSE tab_LeftSplitClause(TABLEAU T)
{
  return T->LeftSplitClause;
}

static __inline__ void tab_SetLeftSplitClause(TABLEAU T, CLAUSE C)
{
  T->LeftSplitClause = C;
}


static __inline__ LIST tab_RightSplitClauses(TABLEAU T)
{
  return T->RightSplitClauses;
}


static __inline__ void tab_SetRightSplitClauses(TABLEAU T, LIST L)
{
  T->RightSplitClauses = L;
}

static __inline__ void tab_AddRightSplitClause(TABLEAU T, CLAUSE C)
{
  T->RightSplitClauses = list_Cons(C, T->RightSplitClauses);
}

static __inline__ BOOL tab_IsLeaf(TABLEAU T)
{
  return (tab_RightBranchIsEmpty(T) && tab_LeftBranchIsEmpty(T));
}

#endif
