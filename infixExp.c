/*file	 : infixExp.c
*authors : Radu Rebeja (r.rebeja@strudel.rug.nl), Eugen Falca (e.falca@strudel.rug.nl )
*date	 : 14.03.2021 */
/*version: 2.0 */
/* Description: */

#include <stdio.h>  /* printf */
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include <string.h>
#include "scanner.h"
#include "recognizeExp.h"
#include "infixExp.h"

/* The function neuExpTreeNd creates a new node for an expression tree.
 */

ETree neuExpTreeNd(TokenType tt, Token t, ETree tL, ETree tR) {
    ETree new = malloc(sizeof(ExTreeNd));
    assert (new != NULL);
    new->tt = tt;
    new->t = t;
    new->left = tL;
    new->right = tR;
    return new;
}

void freeTrees(ETree tr) {
    if (tr == NULL) {
        return;
    }
    freeTrees(tr->left);
    freeTrees(tr->right);
    free(tr);
}

/* Function cloneTrie will make a deep-copy  of all nodes that are linked to the tree by recursion to all branches
 */

ETree cloneTrie(ETree *root) {
    if (*root == NULL)
        return NULL;
    ETree newNd = neuExpTreeNd((*root)->tt, (*root)->t , NULL, NULL);
    newNd->left = cloneTrie(&(*root)->left);
    newNd->right = cloneTrie(&(*root)->right);
    return newNd;
}

/* The function valIdent recognizes an identifier in a token list and
 * makes the second parameter point to it.
 */


/* Functions simpL, simpR & simMul are part of the simplification procedure that either cuts out the left,right or both
 * branches and frees the allocated memory depending on the case that requires simplifying.
 */
void simpL(ETree *tr) {
    freeTrees((*tr)->left);
    ETree temp = (*tr)->right;
    free(*tr);
    *tr = temp;
}

void simpR(ETree *tr) {
    freeTrees((*tr)->right);
    ETree temp = (*tr)->left;
    free(*tr);
    *tr = temp;
}

void simMul(ETree *tr) {
    (*tr)->tt = Number;
    (*tr)->t.number = 0;
    freeTrees((*tr)->right);
    freeTrees((*tr)->left);
    (*tr)->right = NULL;
    (*tr)->left = NULL;
}

/* Function simplify is the main recursive procedure that checks for all possible 8 cases that have to be simplified
 * by using nested switch functions for each operator branching to each switch case relevant to it.
 */

void simplify (ETree *tr) {
    if ((*tr)->tt == Number || (*tr)->tt == Identifier ) {
        return;
    }
    simplify(&(*tr)->left);
    simplify(&(*tr)->right);
    switch ((*tr)->t.symbol) {
        case '+':
            switch (((*tr)->right)->t.number) {
                case 0:
                    simpR(tr);
                    return;
            }
            switch (((*tr)->left)->t.number) {
                case 0:
                    simpL(tr);
                    return;
                default:
                    return;
            }
        case '-':
            switch (((*tr)->right)->t.number) {
                case 0:
                    simpR(tr);
                    return;
                default:
                    return;
            }
        case '*':
            switch (((*tr)->right)->t.number) {
                case 0:
                    simMul(tr);
                    return;
                case 1:
                    simpR(tr);
                    return;
            }
            switch (((*tr)->left)->t.number) {
                case 0:
                    simMul(tr);
                    return;
                case 1:
                    simpL(tr);
                    return;
            }
        case '/':
            switch (((*tr)->right)->t.number) {
                case 1 :
                    simpR(tr);
                    return;
                default:
                    return;
            }
        default:
            return;
    }
}

/* Function isX simply checks if the identifiers string matches the value of the string containing only 'x'
 */
int isX(char *c) {
    return(strcmp(c, "x")==0);
}

/* Function mulForm applies the multiplication formula for differentiation by restructuring the main node involved.
 * It creates deep-copies of the original left & right branches of the main node which  and are later fused into
 * newly created children nodes of the main node as right and left child participant in the formula as non-diff
 * values/variables and we assign the symbol to this secondary nodes '+' as well.
 */

void mulForm(ETree *backup, ETree *tr){
    (*tr)->t.symbol = '+';
    (*tr)->tt = Symbol;
    ETree tempR = cloneTrie(&(*backup)->right);
    ETree tempL = cloneTrie(&(*backup)->left);

    (*tr)->left = neuExpTreeNd(Symbol, ((*tr)->left)->t,(*tr)->left , tempR);
    (*tr)->right = neuExpTreeNd(Symbol, ((*tr)->right)->t, tempL, (*tr)->right );
    ((*tr)->left)->t.symbol = ((*tr)->right)->t.symbol = '*';
}
/* Function divForm applies the quotient formula for differentiation by restructuring the main node involved.
 * It creates 4 deep-copies of the original left & right branches of the main node which  and are later fused into
 * newly created children nodes of the main node as right and left child participant in the formula as non-diff
 * values/variables and we assign the minus symbol '-' to the  secondary (left child) node  and '*' to the right child
 * node. The main node serves as a hub that connects both children with the division operator.
 */

void divForm(ETree *backup, ETree *tr){
    (*tr)->t.symbol = '/';
    (*tr)->tt = Symbol;

    ETree tempL = cloneTrie(&(*backup)->left);
    ETree tempR = cloneTrie(&(*backup)->right);
    ETree tempR1 = cloneTrie(&(*backup)->right);
    ETree tempR2 = cloneTrie(&(*backup)->right);

    ETree leftL = neuExpTreeNd(Symbol, ((*tr)->left)->t,(*tr)->left , tempR);
    ETree rightL = neuExpTreeNd(Symbol, ((*tr)->right)->t, tempL, (*tr)->right );
    leftL->t.symbol = rightL->t.symbol = '*';

    (*tr)->left = neuExpTreeNd(Symbol, ((*tr)->left)->t,leftL , rightL);
    (*tr)->right = neuExpTreeNd(Symbol, ((*tr)->right)->t, tempR1, tempR2);

    ((*tr)->left)->t.symbol ='-';
    ((*tr)->right)->t.symbol = '*';
}

/* Function differentiate is a recursive function that branches throughout the created trie and
 * applies basic differentiation to all operands - where identifiers != 'x' => = 0, identif == x => = 1,
 * number ? number = 0 : number;
 * After reaching all leaf nodes, we start applying mulForm and divForm appropriately.
 */
void differentiate(ETree *backup, ETree *tr) {
    if ((*tr)->tt == Number || (*tr)->tt == Identifier ) {
        if ((*tr)->tt == Identifier) {
            (*tr)->tt = Number;
            (*tr)->t.number = isX((*tr)->t.identifier);
            return;
        }
        if ((*tr)->tt == Number) {
            (*tr)->t.number = 0;
            return;
        }
    }
    differentiate(&(*backup)->left,&(*tr)->left);
    differentiate(&(*backup)->right,&(*tr)->right);
    switch ((*tr)->t.symbol) {
        case '+':
            return;
        case '-':
            return;
        case '*':
            mulForm(backup,tr);
            return;
        case '/':
            divForm(backup,tr);
        default:
            return;
    }
}

int valOper(List *lp, char *cp, char c) {
    if (*lp != NULL && (*lp)->tt == Symbol && ((*lp)->t).symbol == c) {
        *cp = ((*lp)->t).symbol;
        *lp = (*lp)->next;
        return 1;
    }
    return 0;
}

/*valIdent collects a number variable, increments the list pointer and
 * creates an Number node.
 */

int valNumber(List *lp, ETree *tp, double *wp) {
    Token t;
    if (*lp != NULL && (*lp)->tt == Number) {
        *wp = ((*lp)->t).number;
        *lp = (*lp)->next;
        t.number = (int) *wp;
        *tp = neuExpTreeNd(Number, t, NULL, NULL);
        return 1;
    }
    return 0;
}

/*valIdent collects an identifier variable, increments the list pointer and
 * creates an Identifier node.
 */

int valIdent(List *lp, ETree *tp, char **sp) {
    Token t;
    if (*lp != NULL && (*lp)->tt == Identifier) {
        *sp = ((*lp)->t).identifier;
        *lp = (*lp)->next;
        t.identifier = *sp;
        *tp = neuExpTreeNd(Identifier, t, NULL, NULL);
        return 1;
    }
    return 0;
}

/*Generic acceptFactor function used here for convenience( of direct overview )
 */

int valFactor(List *lp, ETree *tp, double *wp, char **sp) {
    return
            (valNumber(lp, tp, wp)
             || valIdent(lp, tp, sp)
             || (acceptCharacter(lp, '(')
                 && valExpress(lp, tp)
                 && acceptCharacter(lp, ')')
             )
            );
}


/* valTerm is a modified version of the acceptTerm function; it sets up a term for
 * the valExpress function and generates tree nodes for multiplication and division
 * operations.
 */
int valTerm(List *lp, ETree *term) {
    Token t;
    ETree right = NULL;
    double wp;
    char *sp;
    char cp;
    if (!valFactor(lp, term, &wp, &sp)) {
        return 0;
    }
    while (valOper(lp, &cp, '*') || valOper(lp, &cp, '/')) {
        t.symbol = cp;//check here as well pointer
        if (!valFactor(lp, &right, &wp, &sp)) {
            return 0;
        }
        *term = neuExpTreeNd(Symbol, t, *term, right);
    }
    /* no * or /, so we reached the end of the term */
    return 1;
}

/* valExpress is a modified version of the acceptExpression function; it collects terms for
 * itself and generates tree nodes out of secondary terms for addition and subtraction operations.
 */

int valExpress(List *lp, ETree *tp) {
    Token t;
    ETree lefts = NULL, rights = NULL;
    char c;
    if (!valTerm(lp, &lefts)) {
        return 0;
    }
    *tp = lefts;
    while (valOper(lp, &c, '+') || valOper(lp, &c, '-')) {
        t.symbol = c;//check here pointer
        if (!valTerm(lp, &rights)) {
            return 0;
        }
        *tp = neuExpTreeNd(Symbol, t, *tp, rights);
    }
    /* no + or -, so we reached the end of the expression */
    return 1;
}


/* The function prntPrfxtree does what its name suggests.
 */

void prntPrfxtree(ETree tr) {
    if (tr == NULL) {
        return;
    }
    switch (tr->tt) {
        case Number:
            printf("%d", (tr->t).number);
            break;
        case Identifier:
            printf("%s", (tr->t).identifier);
            break;
        case Symbol:
            printf("(");
            prntPrfxtree(tr->left);
            printf(" %c ", (tr->t).symbol);
            prntPrfxtree(tr->right);
            printf(")");
            break;
    }
}

/* The function isNumrcl checks for an expression tree whether it represents
 * a numerical expression, i.e. without identifiers.
 */

int isNumrcl(ETree tr) {
    assert(tr != NULL);
    if (tr->tt == Number) {
        return 1;
    }
    if (tr->tt == Identifier) {
        return 0;
    }
    return (isNumrcl(tr->left) && isNumrcl(tr->right));
}

/* The function valExpTree computes the value of an expression tree that represents a
 * numerical expression.
 */

double valExpTree(ETree tr) {  /* precondition: isNumrcl(tr)) */
    double lval, rval;
    assert(tr != NULL);
    if (tr->tt == Number) {
        return (tr->t).number;
    }
    lval = valExpTree(tr->left);
    rval = valExpTree(tr->right);
    switch ((tr->t).symbol) {
        case '+':
            return (lval + rval);
        case '-':
            return (lval - rval);
        case '*':
            return (lval * rval);
        case '/':
            assert(rval != 0);
            return (lval / rval);
        default:
            abort();
    }
}

/* the function prefExpressionExpTrees performs a dialogue with the user and tries
 * to recognize the input as a prefix expression. When it is a numerical prefix
 * expression, its value is computed and printed.
 */

void infixExpTrees() {
    char *ar;
    int check = 0;
    List tl, tl1;
    ETree t = NULL;
    printf("give an expression: ");
    ar = readInput();
    while (ar[0] != '!') {
        tl = tokenList(ar);
        printList(tl);
        tl1 = tl;
        if (valExpress(&tl1, &t) && tl1 == NULL) {
            /* there should be no tokens left */
            printf("in infix notation: ");
            prntPrfxtree(t);
            printf("\n");
            if (isNumrcl(t)) {
                printf("the value is %g\n", valExpTree(t));
            } else {
                printf("this is not a numerical expression\n");
                simplify(&t);
                printf("simplified: ");
                prntPrfxtree(t);
                printf("\n");
                ETree backup = cloneTrie(&t);
                // printf("backup:    ");
                //
                differentiate(&backup, &t);
                simplify(&t);
                printf("derivative to x: ");
                prntPrfxtree(t);
                //printf("\n");
                // prntPrfxtree(backup);
                freeTrees(backup);
                backup = NULL;
                printf("\n");
            }

        } else {
            printf("this is not an expression\n");
        }
        freeTrees(t);
        t = NULL;
        freeTokenList(tl);
        free(ar);
        printf("\ngive an expression: ");
        ar = readInput();
    }
    free(ar);
    printf("good bye\n");
}
