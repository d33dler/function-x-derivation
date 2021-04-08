/* prefixExp.h, Gerard Renardel, 29 January 2014 */

#include "scanner.h"
#ifndef INFIXEXP_H
#define INFIXEXP_H

/* Here the definition of the type tree of binary trees with nodes containing tokens.
 */

typedef struct ExTreeNd *ETree;

typedef struct ExTreeNd {
    TokenType tt;
    Token t;
    ETree left;
    ETree right;
} ExTreeNd;
void simplify(ETree *tr);
ETree neuExpTreeNd(TokenType tt, Token t, ETree tL, ETree tR);
void differentiate(ETree *backup, ETree *tr);
ETree cloneTrie(ETree *root);
int valNumber(List *lp, ETree *tp, double *wp);
int isX(char *c);
void simpR(ETree *tr);
void simpL(ETree *tr);
void simMul(ETree *tr);
void simplify (ETree *tr);
int valIdent(List *lp, ETree *tp, char **sp);
void mulForm(ETree *backup, ETree *tr);
void divForm(ETree *backup, ETree *tr);
int valFactor(List *lp, ETree *tp, double *wp, char **sp);
int checkToken(ETree *tr, TokenType new);
int valExpress(List *lp, ETree *tp);
void diffDiv(ETree *backup, ETree *tr);
int isNumrcl(ETree tr);
void diffMul(ETree *backup, ETree *tr);
double valExpTree(ETree tr);
void diffAddSub(ETree *tr);
void prntPrfxtree(ETree tr);

void infixExpTrees();

int valOper(List *lp, char *cp, char c);

int valTerm(List *lp, ETree *tp);

#endif
