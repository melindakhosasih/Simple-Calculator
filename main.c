#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXLEN 256
#define TBLSIZE 64

// Set PRINTERR to 1 to print error message while calling error()
// Make sure you set PRINTERR to 0 before you submit your code
#define PRINTERR 1

// Call this macro to print error message and exit the program
// This will also print where you called it in your program
#define error(errorNum) { \
    printf("EXIT 1\n"); \
    if (PRINTERR) \
        fprintf(stderr, "error() called at %s:%d: ", __FILE__, __LINE__); \
    err(errorNum); \
}

// Token types
typedef enum {
    UNKNOWN, END, ENDFILE,
    INT, ID,
    ADDSUB, MULDIV,
    INCDEC, AND, OR, XOR,
    ASSIGN,
    LPAREN, RPAREN
} TokenSet;

// Test if a token matches the current token
extern int match(TokenSet token);

// Get the next token
extern void advance(void);

// Get the lexeme of the current token
extern char *getLexeme(void);

static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN;
static char lexeme[MAXLEN];

// Error types
typedef enum {
    UNDEFINED, MISPAREN, NOTNUMID, NOTFOUND, RUNOUT, NOTLVAL, DIVZERO, SYNTAXERR, SIGNERR
} ErrorType;

// Structure of the symbol table
typedef struct {
    int val;
    char name[MAXLEN];
} Symbol;

// Structure of a tree node
typedef struct _Node {
    TokenSet data;
    int val;
    char lexeme[MAXLEN];
    struct _Node *left;
    struct _Node *right;
} BTNode;

extern int sbcount = 0;
int idx = 0;
int assign = 0;
int PlusMin = 0;
int calcu = 0;
int div_var = 0;
// The symbol table
extern Symbol table[TBLSIZE];
Symbol table[TBLSIZE];

// Initialize the symbol table with builtin variables
extern void initTable(void);

// Get the value of a variable
extern int getval(char *str);

// Set the value of a variable
extern int setval(char *str, int val);

// Make a new node according to token type and lexeme
extern BTNode *makeNode(TokenSet tok, const char *lexe);

// Free the syntax tree
extern void freeTree(BTNode *root);

extern BTNode *factor(void);
extern BTNode *unary_expr(void);
extern BTNode *term(void);
extern BTNode *term_tail(BTNode *left);
extern BTNode *addsub_expr(void);
extern BTNode *addsub_expr_tail(BTNode *left);
extern BTNode *and_expr(void);
extern BTNode *and_expr_tail(BTNode *left);
extern BTNode *xor_expr(void);
extern BTNode *xor_expr_tail(BTNode *left);
extern BTNode *or_expr(void);
extern BTNode *or_expr_tail(BTNode *left);
extern BTNode *assign_expr(void);
extern void statement(void);

// Print error message and exit the program
extern void err(ErrorType errorNum);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);
extern void reset();

//// Print the syntax tree in prefix
//extern void printPrefix(BTNode *root);

TokenSet getToken(void)
{
    int i = 0;
    char c = '\0';

    while ((c = fgetc(stdin)) == ' ' || c == '\t');
    if (isdigit(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while (isdigit(c) && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        lexeme[i] = '\0';
        PlusMin = 1;
        return INT;
    } else if (c == '|') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        PlusMin = 0;
        calcu = 1;
        return OR;
    } else if (c == '^') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        PlusMin = 0;
        calcu = 1;
        return XOR;
    } else if (c == '&') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        PlusMin = 0;
        calcu = 1;
        return AND;
    } else if (c == '+' && PlusMin == 0) {
        lexeme[0] = c;
        c = fgetc(stdin);
        if (c == '+') {
            lexeme[1] = c;
            c = fgetc(stdin);
            if (isalpha(c) || c == '_') {
                ungetc(c, stdin);
                lexeme[2] = '\0';
                PlusMin = 0;
                calcu = 1;
                return INCDEC;
            }
            else
                error(SIGNERR);
        }
        else if (isdigit(c)){
            lexeme[1] = c;
            c = fgetc(stdin);
            i = 2;
            while(isdigit(c) && i < MAXLEN) {
                lexeme[i] = c;
                ++i;
                c = fgetc(stdin);
            }
            ungetc(c, stdin);
            lexeme[i] = '\0';
            PlusMin = 1;
            return INT;
        }
        else if (isalpha(c) || c == '(') {
            ungetc(c, stdin);
            lexeme[1] = '\0';
            PlusMin = 0;
            calcu = 1;
            return ADDSUB;
        }
        else if (c == ' ' || c == '-') {
            ungetc(c, stdin);
            lexeme[1] = '\0';
            PlusMin = 0;
            calcu = 1;
            return ADDSUB;
        }
        else
            error(SIGNERR);
    } else if (c == '-' && PlusMin == 0) {
        lexeme[0] = c;
        c = fgetc(stdin);
        if (c == '-') {
            lexeme[1] = c;
            c = fgetc(stdin);
            if (isalpha(c) || c == '_') {
                ungetc(c, stdin);
                lexeme[2] = '\0';
                PlusMin = 0;
                calcu = 1;
                return INCDEC;
            }
            else
                error(SIGNERR);
        }
        else if (isdigit(c)) {
            lexeme[1] = c;
            c = fgetc(stdin);
            i = 2;
            while(isdigit(c) && i < MAXLEN) {
                lexeme[i] = c;
                ++i;
                c = fgetc(stdin);
            }
            ungetc(c, stdin);
            lexeme[i] = '\0';
            PlusMin = 1;
            return INT;
        }
        else if (isalpha(c) || c == '(') {
//            lexeme[1] = c;
//            c = fgetc(stdin);
//            i = 2;
//            if(isalpha(c)) {
//                while((isalpha(c) || isdigit(c) || c == '_') && i < MAXLEN) {
//                    lexeme[i] = c;
//                    ++i;
//                    c = fgetc(stdin);
//                }
//            }
            ungetc(c, stdin);
            lexeme[1] = '\0';
            PlusMin = 0;
            calcu = 1;
            return ADDSUB;
        }
        else if (c == ' ' || c == '+') {
//            lexeme[1] = '/0';
//            PlusMin = 0;
//            calcu = 1;
            ungetc(c, stdin);
            lexeme[1] = '\0';
            PlusMin = 0;
            calcu = 1;
            return ADDSUB;
        }
        else
            error(SIGNERR);
    } else if ((c == '+' || c == '-') && (PlusMin == 1)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        if ((lexeme[0] == '+' && c == '+') || (lexeme[0] == '-' && c == '-'))
            error(SIGNERR);
        ungetc(c, stdin);
        lexeme[1] = '\0';
        PlusMin = 0;
        calcu = 1;
        return ADDSUB;
    } else if (c == '*' || c == '/') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        PlusMin = 0;
        calcu = 1;
//        if(c == '/') div_var = 0;
        return MULDIV;
    } else if (c == '\n') {
        lexeme[0] = '\0';
        return END;
    } else if (c == '=') {
        strcpy(lexeme, "=");
        PlusMin = 0;
        //if(assign > 0) idx++;
        assign++;
        return ASSIGN;
    } else if (c == '(') {
        strcpy(lexeme, "(");
        PlusMin = 0;
        calcu = 0;
        return LPAREN;
    } else if (c == ')') {
        strcpy(lexeme, ")");
        calcu = 0;
        PlusMin = 1;
        return RPAREN;
    } else if (isalpha(c) || c == '_') {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while((isalpha(c) || c == '_' || isdigit(c)) && i < MAXLEN){
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        if(i >= MAXLEN)
            error(RUNOUT);
        ungetc(c, stdin);
        lexeme[i] = '\0';
        PlusMin = 1;
        div_var = 1;
        return ID;
    } else if (c == EOF) {
        return ENDFILE;
    } else {
        error(NOTFOUND);
        return UNKNOWN;
    }
}

void advance(void) {
    curToken = getToken();
}

void reset(void) {
    PlusMin = 0;
    idx = 0;
    assign = 0;
    calcu = 0;
    div_var = 0;
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)
        advance();
    return token == curToken;
}

char *getLexeme(void) {
    return lexeme;
}

void initTable(void) {
    strcpy(table[0].name, "x");
    table[0].val = 0;
    strcpy(table[1].name, "y");
    table[1].val = 0;
    strcpy(table[2].name, "z");
    table[2].val = 0;
    sbcount = 3;
}

int getval(char *str) {
    int i = 0;

    for (i = 0; i < sbcount; i++)
        if (strcmp(str, table[i].name) == 0)
            return table[i].val;

    if (sbcount >= TBLSIZE)
        error(RUNOUT);

    if (assign != 0)
        error(NOTFOUND);

    strcpy(table[sbcount].name, str);
    table[sbcount].val = 0;
    sbcount++;
    return 0;
}

int setval(char *str, int val) {
    int i = 0;

    for (i = 0; i < sbcount; i++) {
        if (strcmp(str, table[i].name) == 0) {
            table[i].val = val;
            return val;
        }
    }

    if (sbcount >= TBLSIZE)
        error(RUNOUT);

    strcpy(table[sbcount].name, str);
    table[sbcount].val = val;
    sbcount++;
    return val;
}

BTNode *makeNode(TokenSet tok, const char *lexe) {
    BTNode *node = (BTNode*)malloc(sizeof(BTNode));
    strcpy(node->lexeme, lexe);
    node->data = tok;
    node->val = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(BTNode *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// factor := INT | ADDSUB INT |
//		   	 ID  | ADDSUB ID  |
//		   	 ID ASSIGN expr |
//		   	 LPAREN expr RPAREN |
//		   	 ADDSUB LPAREN expr RPAREN
BTNode *factor(void) {
    BTNode *retp = NULL, *left = NULL;

    if (match(INT)) {
        retp = makeNode(INT, getLexeme());
        advance();
    } else if (match(ID)) {
        //printf("factor\n");
        left = makeNode(ID, getLexeme());
        advance();
        if (!match(ASSIGN)) {
            retp = left;
        } else {
            //printf("asignnn\n");
            //printf("%s\n", left->lexeme);
//            if (strcmp(left->lexeme, "=") != 0)
//                error(SIGNERR);
            if(calcu == 1)
                error(SIGNERR);
            retp = makeNode(ASSIGN, getLexeme());
            advance();
            retp->left = left;
            retp->right = assign_expr();
            //printf("lexeme %s\n", retp->left);
        }

//    } else if (match(ADDSUB)) {
//        retp = makeNode(ADDSUB, getLexeme());
//        retp->left = makeNode(INT, "0");
//        advance();
//        if (match(INT)) {
//            retp->right = makeNode(INT, getLexeme());
//            advance();
//        } else if (match(ID)) {
//            retp->right = makeNode(ID, getLexeme());
//            advance();
//        } else if (match(LPAREN)) {
//            advance();
//            retp->right = assign_expr();
//            if (match(RPAREN))
//                advance();
//            else
//                error(MISPAREN);
//        } else {
//            error(NOTNUMID);
//        }
    } else if (match(INCDEC)) {
        //printf("incdec\n");
        retp = makeNode(ADDSUB, getLexeme());
        retp->right = makeNode(INT, "1");
        advance();
        if (match(ID)) {
            retp->left = makeNode(ID, getLexeme());
            advance();
        } /*else if (match(LPAREN)) {
            advance();
            retp->right = assign_expr();
            if (match(RPAREN))
                advance();
            else
                error(MISPAREN);
        } else {
            error(NOTNUMID);
        }*/
    } else if (match(LPAREN)) {
        if(assign == 0)
            error(SYNTAXERR);
        div_var = 0;
        advance();
        retp = assign_expr();
        if (match(RPAREN))
            advance();
        else
            error(MISPAREN);
    } else {
        error(NOTNUMID);
    }
    return retp;
}

BTNode *unary_expr(void) {
    BTNode *node = NULL, *left = NULL;
    if (match(ADDSUB)) {
        //printf("unary\n");
        if (strcmp(lexeme, "-") == 0) {
            node = makeNode(ADDSUB, getLexeme());
            //printf("%s\n", node->lexeme);
            node->left = makeNode(INT, "0");
            advance();
            node->right = unary_expr();
        }
        else if (strcmp(lexeme, "+") == 0) {
            advance();
            node = unary_expr();
        }
//        if (match(INT)) {
//            node->right = makeNode(INT, getLexeme());
//            advance();
//        } else if (match(ID)) {
//            node->right = makeNode(ID, getLexeme());
//            advance();
//        } else if (match(LPAREN)) {
//            printf("here\n");
//            advance();
//            node->right = unary_expr();
//            printf("%s\n", node->right->lexeme);
//            if (match(RPAREN))
//                advance();
//            else
//                error(MISPAREN);
//        } else {
//            error(NOTNUMID);
//        }
    } else {
        return factor();
    }
    return node;
}

// term := unary_expr term_tail
BTNode *term(void) {
    BTNode *node = unary_expr();
    return term_tail(node);
}

// term_tail := MULDIV factor term_tail | NiL
BTNode *term_tail(BTNode *left) {
    BTNode *node = NULL;
    if (match(MULDIV)) {
        node = makeNode(MULDIV, getLexeme());
        advance();
        node->left = left;
        node->right = unary_expr();
        return term_tail(node);
    } else {
        return left;
    }
}

// addsub_expr := term addsub_expr_tail
BTNode *addsub_expr(void) {
    BTNode *node = term();
    return addsub_expr_tail(node);
}

// addsub_expr_tail := ADDSUB term addsub_expr_tail | NiL
BTNode *addsub_expr_tail(BTNode *left) {
    BTNode *node = NULL;
    if (match(ADDSUB)) {
        //printf("addsub_expr\n");
        node = makeNode(ADDSUB, getLexeme());
        advance();
        node->left = left;
        node->right = term();
        return addsub_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *and_expr(void) {
    BTNode *node = addsub_expr();
    return and_expr_tail(node);
}

BTNode *and_expr_tail(BTNode *left) {
    BTNode *node = NULL;
    if (match(AND)) {
        node = makeNode(AND, getLexeme());
        advance();
        node->left = left;
        node->right = addsub_expr();
        return and_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *xor_expr(void) {
    BTNode *node = and_expr();
    return xor_expr_tail(node);
}

BTNode *xor_expr_tail(BTNode *left) {
    BTNode *node = NULL;
    if (match(XOR)) {
        node = makeNode(XOR, getLexeme());
        advance();
        node->left = left;
        node->right = and_expr();
        return xor_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *or_expr(void) {
    BTNode *node = xor_expr();
    return or_expr_tail(node);
}

BTNode *or_expr_tail(BTNode *left) {
    BTNode *node = NULL;
    if (match(OR)) {
        node = makeNode(OR, getLexeme());
        advance();
        node->left = left;
        node->right = xor_expr();
        return or_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *assign_expr(void) {
    BTNode *node = NULL, *left = NULL;
    left = or_expr();
    //printf("assign_expr %s\n", left->lexeme);
    if (match(ID)) {
        //printf("assign id\n");
        node = makeNode(ID, getLexeme());
        advance();
        node->left = left;
        node->right = assign_expr();
        return node;
    } else if (match(ASSIGN)) {
        if (strcmp(left->lexeme, "=") != 0)
            error(SIGNERR);
        node = makeNode(ASSIGN, getLexeme());
        advance();
        node->left = left;
        node->right = assign_expr();
        return node;
    } else {
        return left;
    }
}

// statement := ENDFILE | END | expr END
void statement(void) {
    BTNode *retp = NULL;

    if (match(ENDFILE)) {
        printf("MOV r0 [0]\n");
        printf("MOV r1 [4]\n");
        printf("MOV r2 [8]\n");
        printf("EXIT 0\n");
        exit(0);
    } else if (match(END)) {
//        printf(">> ");
        reset();
        advance();
    } else {
        retp = assign_expr();
//        if(retp->data != ASSIGN)
//            error(SYNTAXERR);
        if (match(END)) {
            evaluateTree(retp);
            //printf("%d\n", evaluateTree(retp));
//            printf("Prefix traversal: ");
            //printPrefix(retp);
            //printf("\n");
            freeTree(retp);
            //printf(">> ");
            reset();
            advance();
        } else {
            error(SYNTAXERR);
        }
    }
}

void err(ErrorType errorNum) {
    if (PRINTERR) {
        fprintf(stderr, "error: ");
        switch (errorNum) {
            case MISPAREN:
                fprintf(stderr, "mismatched parenthesis\n");
                break;
            case NOTNUMID:
                fprintf(stderr, "number or identifier expected\n");
                break;
            case NOTFOUND:
                fprintf(stderr, "variable not defined\n");
                break;
            case RUNOUT:
                fprintf(stderr, "out of memory\n");
                break;
            case NOTLVAL:
                fprintf(stderr, "lvalue required as an operand\n");
                break;
            case DIVZERO:
                fprintf(stderr, "divide by constant zero\n");
                break;
            case SYNTAXERR:
                fprintf(stderr, "syntax error\n");
                break;
            case SIGNERR:
                fprintf(stderr, "sign error\n");
                break;
            default:
                fprintf(stderr, "undefined error\n");
                break;
        }
    }
    exit(0);
}

int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;
//    if(root == NULL || (root->left->data != INT || root->left->data != ID) && (root->right->data != INT || root->right->data != ID)) return;
//    evaluateTree(root->left);
//    evaluateTree(root->right);
    if (root != NULL) {
        switch (root->data) {
            case ID:
                retval = getval(root->lexeme);
                for(int i = 0; i < sbcount; i++){
                    if(strcmp(table[i].name, root->lexeme) == 0){
                        printf("MOV r%d [%d]\n", idx++, i*4);
                        break;
                    }
                }
                break;
            case INT:
                retval = atoi(root->lexeme);
                printf("MOV r%d %d\n", idx++, retval);
                break;
            case ASSIGN:
                rv = evaluateTree(root->right);
//                if (calcu == 1) error(SYNTAXERR);
                retval = setval(root->left->lexeme, rv);
                for(int i = 0; i < sbcount; i++){
                    if(strcmp(table[i].name, root->left->lexeme) == 0){
                        if(idx == 0) idx++;
                        printf("MOV [%d] r%d\n", i*4, --idx);
                        idx++;
                    }
                }
                break;
//            case INCDEC:
//                rv = evaluateTree(root->right);
//                if (strcmp(root->lexeme, "++") == 0) {
//                    retval = ++rv;
//                    printf("INC r%d\n", --idx);
//                    idx++;
//
//                } else if (strcmp(root->lexeme, "--") == 0) {
//                    retval = --rv;
//                    printf("DEC r%d\n", --idx);
//                    idx++;
//                }
//                break;
            case OR:
            case XOR:
            case AND:
            case ADDSUB:
            case MULDIV:
//                if(assign == 0)
//                    error(SYNTAXERR);
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                if (strcmp(root->lexeme, "|") == 0) {
                    retval = lv | rv;
                    if(idx == 0) idx++;
                    printf("OR r%d r%d\n", idx-1, --idx);
                } else if (strcmp(root->lexeme, "^") == 0) {
                    retval = lv ^ rv;
                    if(idx == 0) idx++;
                    printf("XOR r%d r%d\n", idx-1, --idx);
                } else if (strcmp(root->lexeme, "&") == 0) {
                    retval = lv & rv;
                    if(idx == 0) idx++;
                    printf("AND r%d r%d\n", idx-1, --idx);
                } else if (strcmp(root->lexeme, "++") == 0) {
                    retval = lv + rv;
                    if(idx == 0) idx++;
                    printf("ADD r%d r%d\n", idx-1, --idx);
                    for(int i = 0; i < sbcount; i++){
                        if(strcmp(table[i].name, root->left->lexeme) == 0){
                            if(idx == 0) idx++;
                            printf("MOV [%d] r%d\n", i*4, --idx);
                        }
                    }
                    idx++;
                } else if( strcmp(root->lexeme, "--") ==  0) {
                    retval = lv - rv;
                    if(idx == 0) idx++;
                    printf("SUB r%d r%d\n", idx-1, --idx);
                    for(int i = 0; i < sbcount; i++){
                        if(strcmp(table[i].name, root->left->lexeme) == 0){
                            if(idx == 0) idx++;
                            printf("MOV [%d] r%d\n", i*4, --idx);
                        }
                    }
                    idx++;
                } else if (strcmp(root->lexeme, "+") == 0) {
                    retval = lv + rv;
                    if(idx == 0) idx++;
                    printf("ADD r%d r%d\n", idx-1, --idx);
                } else if (strcmp(root->lexeme, "-") == 0) {
                    retval = lv - rv;
                    if(idx == 0) idx++;
                    printf("SUB r%d r%d\n", idx-1, --idx);
                } else if (strcmp(root->lexeme, "*") == 0) {
                    retval = lv * rv;
                    if(idx == 0) idx++;
                    printf("MUL r%d r%d\n", idx-1, --idx);
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (strcmp(root->right->lexeme, "0") == 0) {
                        error(DIVZERO);
                    }
                    else if (rv == 0 && root->right->data != ID && div_var == 0) {
                        error(DIVZERO);
                    }
                    if(rv != 0) {
                        retval = lv / rv;
                        //if(retval == 0 && lv != 0) retval++;
                    }
                    if(idx == 0) idx++;
                    printf("DIV r%d r%d\n", idx-1, --idx);
                }
                break;
            default:
                retval = 0;
        }
    }
    return retval;
}

//void printPrefix(BTNode *root){
//    if(root == NULL) return;
//    printf("%s ", root->lexeme);
//    printPrefix(root->left);
//    printPrefix(root->right);
//}

int main() {
    initTable();
    //freopen("input.txt", "w", stdout);
//    printf(">> ");
    while (1) {
        statement();
    }
    return 0;
}
