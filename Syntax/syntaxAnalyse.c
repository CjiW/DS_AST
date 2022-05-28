#include "syntaxAnalyse.h"
#include "Token/gettoken.h"


extern char token_text[20];
extern char string_num[20];
extern int cnt_lines;
int w, type;  // token type
int haveMistake = 0;
extern FILE* fr;
VDN* Vroot;  // VarName链表根节点
int isVoid, hasReturn, isInRecycle = 0;

void syntaxAnalyse() {
    ASTTree* root = program();
    if (root == NULL || haveMistake == 1) {
        printf("Syntax Error!\n");
        return;
    } else {
        preorderTranverse(root, 0);
    }
    cnt_lines=1;
}

ASTTree* program() {
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    Vroot = (VDN*)malloc(sizeof(VDN));
    Vroot->size = 0;
    Vroot->next = NULL;
    ASTTree* p = ExtDefList();
    if(haveMistake==1)return NULL;
    if (p != NULL) {
        if (isVoid == 0 && hasReturn == 0) {
            printf("Error: Missing return value\n");
            exit(0);
        }
        ASTTree* root = p;  // External Defs
        root->type = EXTDEFLIST;
        return root;
    } else {
        haveMistake = 1;
        return NULL;
    }
}

void freeTree(ASTTree* root) {
    if (root) {
        freeTree(root->l);
        freeTree(root->r);
        free(root);
    }
}

ASTTree* ExtDefList() {
    if (w == -1) {
        return NULL;
    }
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->data.data = NULL;
    root->type = EXTDEFLIST;
    root->l = ExtDef();
    if(haveMistake==1)return NULL;
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    root->r = ExtDefList();
    if(haveMistake==1)return NULL;
    return root;
}

ASTTree* ExtDef() {
    int a;
    if (w != INT && w != DOUBLE && w != CHAR && w != LONG && w != SHORT && w != FLOAT && w != VOID) {
        printf("Mistake in line %d\n", cnt_lines);
        printf("Error: Wrong external define\n");
        haveMistake=1;
        return NULL;
    }
    type = w;
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    if (w != IDENT && w != ARRAY) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: 外部Def出现错误\n");
        haveMistake=1;
        return NULL;
    }
    a = w;
    char token_text0[20];
    strcpy(token_text0, token_text);  // var name
    ASTTree* p;
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    strcpy(token_text, token_text0);
    if (w == LP) {
        p = FuncDef();
        if(haveMistake==1)return NULL;
    } else if (a == ARRAY) {  // def of array
        p = ArrayDef();
        if(haveMistake==1)return NULL;
    } else {
        p = ExtVarDef();
        if(haveMistake==1)return NULL;
    }
    return p;
}

ASTTree* ArrayDef() {
    if (type == VOID) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: Can't declare a void type array\n");
        haveMistake=1;
        return NULL;
    }
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->type = ARRAYDEF;
    root->l = NULL;
    root->r = NULL;
    root->data.data = NULL;
    ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
    p->type = ARRAYTYPE;
    p->l = NULL;
    p->r = NULL;
    if (type == INT) {
        p->data.data = "int";
    }
    if (type == DOUBLE) {
        p->data.data = "double";
    }
    if (type == CHAR) {
        p->data.data = "char";
    }
    if (type == FLOAT) {
        p->data.data = "float";
    }
    if (type == LONG) {
        p->data.data = "long";
    }
    if (type == SHORT) {
        p->data.data = "short";
    }

    root->l = p;
    p = (ASTTree*)malloc(sizeof(ASTTree));
    p->type = ARRAYNAME;
    p->l = NULL;
    p->r = NULL;
    char* token_text3 = (char*)malloc(20 * sizeof(char));
    strcpy(token_text3, token_text);
    p->data.data = token_text3;
    root->r = p;
    ASTTree* q = (ASTTree*)malloc(sizeof(ASTTree));
    q->type = ARRAYSIZE;
    q->l = q->r = NULL;
    q->data.data = string_num;
    p->l = q;
    return root;
}

ASTTree* ExtVarDef() {
    if (haveMistake == 1) {
        return NULL;
    }
    int cnt;
    if (type == VOID) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: Can't declare a void type variable\n");
        haveMistake=1;
        return NULL;
    }

    int u;
    u = addName(token_text);
    if(haveMistake==1)return NULL;
    if (u == 1) {
        haveMistake = 1;
        return NULL;
    }

    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->l = NULL;
    root->r = NULL;
    root->data.data = NULL;
    root->type = EXTVARDEF;

    ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
    p->l = p->r = NULL;
    p->data.data = NULL;
    p->type = EXTVARTYPE;
    p->data.type = type;

    if (type == INT) {
        p->data.data = "int";
    }
    if (type == DOUBLE) {
        p->data.data = "double";
    }
    if (type == CHAR) {
        p->data.data = "char";
    }
    if (type == FLOAT) {
        p->data.data = "float";
    }
    if (type == LONG) {
        p->data.data = "long";
    }
    if (type == SHORT) {
        p->data.data = "short";
    }

    root->l = p;
    p = (ASTTree*)malloc(sizeof(ASTTree));
    p->l = p->r = NULL;
    p->data.data = NULL;
    p->type = EXTVARLIST;
    root->r = p;

    p->l = (ASTTree*)malloc(sizeof(ASTTree));
    p->l->l = p->l->r = NULL;
    p->data.data = NULL;
    p->l->type = EXTVAR;
    char* token_text1 = (char*)malloc(25 * sizeof(char));  //@
    strcpy(token_text1, token_text);
    p->l->data.data = token_text1;

    while (1) {
        if (w != COMMA && w != SEMI) {
            if (cnt_lines > cnt) {
                cnt_lines--;
            }
            printf("Error in line %d\n", cnt_lines);  //%
            printf("Error: Wrong external define\n");
            haveMistake=1;
            return NULL;
        }
        if (w == SEMI) {
            return root;
        }
        w = getToken(fr);
        if (w != IDENT) {
            printf("Error in line %d\n", cnt_lines);  //%
            printf("Error: Wrong external define\n");
            haveMistake=1;
            return NULL;
        }

        int u;
        u = addName(token_text);
        if(haveMistake==1)return NULL;
        if (u == 1) {
            haveMistake = 1;
            return NULL;
        }

        ASTTree* q = (ASTTree*)malloc(sizeof(ASTTree));
        q->l = q->r = NULL;
        q->data.data = NULL;
        q->type = EXTVARLIST;
        p->r = q;
        p = q;

        p->l = (ASTTree*)malloc(sizeof(ASTTree));
        p->l->l = p->l->r = NULL;
        p->l->data.data = NULL;
        p->l->type = EXTVAR;
        char* token_text1 = (char*)malloc(25 * sizeof(char));
        strcpy(token_text1, token_text);
        p->l->data.data = token_text1;
        cnt = cnt_lines;
        w = getToken(fr);

        while (w == ANNO || w == INCLUDE) {
            w = getToken(fr);
        }
    }
}

ASTTree* FuncDef() {
    if (haveMistake == 1) {
        return NULL;
    }
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->data.data = NULL;
    root->type = FUNCDEF;
    ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
    p->data.data = NULL;
    p->type = FUNCRETURNTYPE;
    p->data.type = type;
    if (type == INT) {
        p->data.data = "int";
        isVoid = 0;
    }
    if (type == DOUBLE) {
        p->data.data = "double";
        isVoid = 0;
    }
    if (type == CHAR) {
        p->data.data = "char";
        isVoid = 0;
    }
    if (type == FLOAT) {
        p->data.data = "float";
        isVoid = 0;
    }
    if (type == LONG) {
        p->data.data = "long";
        isVoid = 0;
    }
    if (type == SHORT) {
        p->data.data = "short";
        isVoid = 0;
    }
    if (type == VOID) {
        p->data.data = "void";
        isVoid = 1;
    }
    p->l = NULL;
    p->r = NULL;
    root->l = p;

    // param
    ASTTree* q = (ASTTree*)malloc(sizeof(ASTTree));
    q->data.data = NULL;
    q->type = FUNCNAME;
    char* token_text1 = (char*)malloc(25 * sizeof(char));
    strcpy(token_text1, token_text);
    q->data.data = token_text1;

    VDN* last = Vroot;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = (VDN*)malloc(sizeof(VDN));
    last = last->next;
    last->next = NULL;
    last->size = 0;

    root->r = q;
    q->l = FormParaList();
    if(haveMistake==1)return NULL;
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    if (w == SEMI) {
        // prototype declare
        root->r->r = NULL;
        root->type = FUNCCLAIM;
    } else if (w == LB) {
        q->r = CompState();
        if(haveMistake==1)return NULL;
        q->r->type = FUNCBODY;
    } else {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: Error in function declaration\n");
        haveMistake=1;
        return NULL;
    }
    return root;
}

ASTTree* FormParaList() {
    if (haveMistake == 1) {
        return NULL;
    }

    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    if (w == RP) {
        return NULL;
    }
    if (w == COMMA) {
        w = getToken(fr);
        while (w == ANNO || w == INCLUDE) {
            w = getToken(fr);
        }
    }
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->data.data = NULL;
    root->type = FUNCFORMALPARALIST;
    root->l = FormParaDef();
    if(haveMistake==1)return NULL;
    root->r = FormParaList();
    if(haveMistake==1)return NULL;
    return root;
}

ASTTree* FormParaDef() {
    if (haveMistake == 1) {
        return NULL;
    }
    if (w != INT && w != DOUBLE && w != CHAR && w != LONG && w != SHORT &&
        w != FLOAT) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: error in parameter\n");
        haveMistake=1;
        return NULL;
    }
    type = w;
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    if (w != IDENT) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error:  error in parameter\n");
        haveMistake=1;
        return NULL;
    }
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->data.data = NULL;
    root->type = FUNCFORMALPARADEF;
    ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
    p->data.data = NULL;
    p->type = FUNCFORMALPARATYPE;
    p->data.type = type;
    if (type == INT) {
        p->data.data = "int";
    }
    if (type == DOUBLE) {
        p->data.data = "double";
    }
    if (type == CHAR) {
        p->data.data = "char";
    }
    if (type == FLOAT) {
        p->data.data = "float";
    }
    if (type == LONG) {
        p->data.data = "long";
    }
    if (type == SHORT) {
        p->data.data = "short";
    }
    if (type == VOID) {
        p->data.data = "void";
    }
    p->l = p->r = NULL;
    root->l = p;
    p = (ASTTree*)malloc(sizeof(ASTTree));
    p->data.data = NULL;
    p->type = FUNCFORMALPARA;

    int u;
    u = addName(token_text);
    if(haveMistake==1)return NULL;
    if (u == 1) {
        haveMistake = 1;
        return NULL;
    }

    char* token_text1 = (char*)malloc(25 * sizeof(char));
    strcpy(token_text1, token_text);
    p->data.data = token_text1;
    p->l = p->r = NULL;
    root->r = p;
    return root;
}

ASTTree* CompState() {
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->data.data = NULL;
    root->l = NULL;
    root->r = NULL;
    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    if (w == INT || w == DOUBLE || w == CHAR || w == LONG || w == SHORT ||
        w == FLOAT) {
        root->l = LocalVarDefList();
        if(haveMistake==1)return NULL;
    } else {
        root->l = NULL;
    }
    root->r = StateList();
    if(haveMistake==1)return NULL;
    if (w == RB) {
        return root;
    } else {
        printf("Error: error in compound statement\n");
        haveMistake = 1;
        freeTree(root);
        return NULL;
    }
}

ASTTree* LocalVarDefList() {
    if (haveMistake == 1) {
        return NULL;
    }

    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->data.data = NULL;
    root->type = LOCALVARDEFLIST;
    root->l = NULL;
    root->r = NULL;
    ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
    p->data.data = NULL;
    p->type = LOCALVARDEF;
    p->l = p->r = NULL;
    root->l = p;
    p->l = (ASTTree*)malloc(sizeof(ASTTree));
    p->l->data.data = NULL;
    p->l->type = LOCALVARTYPE;
    char* token_text1 = (char*)malloc(25 * sizeof(char));
    strcpy(token_text1, token_text);
    p->l->data.data = token_text1;
    p->l->l = p->l->r = NULL;

    w = getToken(fr);
    while (w == ANNO || w == INCLUDE) {
        w = getToken(fr);
    }
    ASTTree* q = (ASTTree*)malloc(sizeof(ASTTree));  // VarNameS结点
    q->data.data = NULL;
    q->l = q->r = NULL;

    p->r = q;
    q->type = LOCALVARNAMELIST;
    q->l = (ASTTree*)malloc(sizeof(ASTTree));  // LocalVarName结点
    q->l->data.data = NULL;
    q->l->type = LOCALVARNAME;
    char* token_text2 = (char*)malloc(25 * sizeof(char));
    strcpy(token_text2, token_text);
    q->l->data.data = token_text2;
    q->l->l = q->l->r = NULL;

    int u;
    u = addName(token_text);
    if(haveMistake==1)return NULL;
    if (u == 1) {
        haveMistake = 1;
        return NULL;
    }

    while (1) {
        w = getToken(fr);
        while (w == ANNO || w == INCLUDE) {
            w = getToken(fr);
        }
        if (w == SEMI) {
            q->r = NULL;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            break;
        } else if (w == COMMA) {
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            ASTTree* s = (ASTTree*)malloc(sizeof(ASTTree));
            s->data.data = NULL;
            q->r = s;
            q = q->r;
            q->type = LOCALVARNAMELIST;
            q->l = (ASTTree*)malloc(sizeof(ASTTree));
            q->l->data.data = NULL;
            q->l->type = LOCALVARNAME;
            char* token_text1 = (char*)malloc(25 * sizeof(char));
            strcpy(token_text1, token_text);
            q->l->data.data = token_text1;
            u = addName(token_text);
            if (u == 1) {
                freeTree(root);
                haveMistake = 1;
                return NULL;
            }
        } else {
            printf("Error in line %d\n", cnt_lines);  //%
            printf("Error: Wrong definition of variable\n");
            haveMistake=1;
            return NULL;
        }
    }

    if (w == INT || w == DOUBLE || w == CHAR || w == LONG || w == SHORT ||
        w == FLOAT) {
        root->r = LocalVarDefList();
        if(haveMistake==1)return NULL;
    } else {
        root->r = NULL;
    }
    root->r = NULL;
    return root;
}

ASTTree* StateList() {
    if (haveMistake == 1) {
        return NULL;
    }
    ASTTree* root = NULL;
    ASTTree* r1 = Statement();
    if(haveMistake==1)return NULL;
    if (r1 == NULL) {
        return NULL;
    } else {
        root = (ASTTree*)malloc(sizeof(ASTTree));
        root->data.data = NULL;
        root->type = STATELIST;
        root->l = r1;
        root->r = NULL;
        w = getToken(fr);
        while (w == ANNO || w == INCLUDE) {
            w = getToken(fr);
        }
        if (w != RB) {
            root->r = StateList();
            if(haveMistake==1)return NULL;
            return root;
        } else {
            return root;
        }
    }
}

ASTTree* Statement() {
    if (haveMistake == 1) {
        return NULL;
    }
    ASTTree* root = (ASTTree*)malloc(sizeof(ASTTree));
    root->l = NULL;
    root->r = NULL;
    root->data.data = NULL;
    switch (w) {
        case IF: {
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != LP) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: Wrong IF statement\n");
                haveMistake=1;
                return NULL;
            }

            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            ASTTree* p1 = (ASTTree*)malloc(sizeof(ASTTree));
            p1->data.data = NULL;
            p1->type = IFPART;
            p1->l = Expression(RP);
            if(haveMistake==1)return NULL;
            if (p1->l == NULL) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: ifStatementCondition部分出错\n");
                haveMistake=1;
                return NULL;
            }
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w == LB) {
                w = getToken(fr);
                while (w == ANNO || w == INCLUDE) {
                    w = getToken(fr);
                }
                p1->r = StateList();
                if(haveMistake==1)return NULL;
            } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST ||
                       w == IDENT || w == KEYWORD || w == IF || w == WHILE ||
                       w == ELSE || w == FOR || w == DO) {
                p1->r = Statement();
                if(haveMistake==1)return NULL;
                p1->r->r = NULL;
            } else {
                printf("Error: Error within IF\n");
                haveMistake = 1;
                return NULL;
            }
            root->l = p1;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w == ELSE) {
                root->type = IFELSESTATEMENT;
                ASTTree* p2 = (ASTTree*)malloc(sizeof(ASTTree));
                p2->data.data = NULL;
                p2->type = ELSEPART;
                root->r = p2;
                w = getToken(fr);
                while (w == ANNO || w == INCLUDE) {
                    w = getToken(fr);
                }
                if (w == LB) {
                    w = getToken(fr);
                    while (w == ANNO || w == INCLUDE) {
                        w = getToken(fr);
                    }
                    p2->r = StateList();
                    if(haveMistake==1)return NULL;
                } else if (w == INT_CONST || w == FLOAT_CONST ||
                           w == CHAR_CONST || w == IDENT || w == KEYWORD) {
                    p2->r = Statement();
                    if(haveMistake==1)return NULL;
                    p2->r->r = NULL;
                } else if (w == IF) {
                    p2->l = Statement();
                    if(haveMistake==1)return NULL;
                } else {
                    printf("Error: Error in ELSE\n");
                    haveMistake = 1;
                    return NULL;
                }
            } else {
                root->type = IFSTATEMENT;
                returnToken(fr);
            }
            return root;
        }
        case WHILE: {
            isInRecycle = 1;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != LP) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: Error in WHILE\n");
                haveMistake = 1;
                return NULL;
            }

            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            ASTTree* p1 = (ASTTree*)malloc(sizeof(ASTTree));
            p1->data.data = NULL;
            p1->type = WHILEPART;
            p1->l = NULL;
            p1->r = NULL;
            p1->l = Expression(RP);
            if(haveMistake==1)return NULL;
            if (p1->l == NULL) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: whileStatementCondition部分出错\n");
                haveMistake = 1;
                return NULL;
            }
            ASTTree* p2 = (ASTTree*)malloc(sizeof(ASTTree));
            p2->data.data = NULL;
            p2->type = WHILEBODY;
            p2->l = NULL;
            p2->r = NULL;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w == LB) {
                w = getToken(fr);
                while (w == ANNO || w == INCLUDE) {
                    w = getToken(fr);
                }
                p2->r = StateList();
            } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST ||
                       w == IDENT || w == KEYWORD) {
                p2->r = Statement();
                p2->r->r = NULL;
            } else {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: Error in WHILE\n");
                haveMistake = 1;
                return NULL;
            }
            root->type = WHILESTATEMENT;
            root->l = p1;
            root->r = p2;
            isInRecycle = 0;
            return root;
        }
        case FOR: {
            isInRecycle = 1;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != LP) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: Error in FOR\n");
                haveMistake = 1;
                return NULL;
            }
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            ASTTree* p1 = (ASTTree*)malloc(sizeof(ASTTree));
            p1->data.data = NULL;
            p1->type = FORPART;
            ASTTree* q = (ASTTree*)malloc(sizeof(ASTTree));  // FOR part 1
            p1->l = q;
            q->type = FORPART1;
            q->data.data = NULL;
            q->l = Expression(SEMI);
            if(haveMistake==1)return NULL;
            if (q->l == NULL) {
                q->data.data = "None";
            }
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            q->r = (ASTTree*)malloc(sizeof(ASTTree));  // FOR part 2
            q = q->r;
            q->type = FORPART2;
            q->data.data = NULL;
            q->l = Expression(SEMI);
            if(haveMistake==1)return NULL;
            if (q->l == NULL) {
                q->data.data = "None";
            }
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            q->r = (ASTTree*)malloc(sizeof(ASTTree));  // FOR part 3
            q = q->r;
            q->r = NULL;
            q->type = FORPART3;
            q->data.data = NULL;
            q->l = Expression(RP);
            if(haveMistake==1)return NULL;
            if (q->l == NULL) {
                q->data.data = "None";
            }

            ASTTree* p2 = (ASTTree*)malloc(sizeof(ASTTree));  // FOR body
            p2->l = NULL;
            p2->r = NULL;
            p2->type = FORBODY;
            p2->data.data = NULL;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w == LB) {
                w = getToken(fr);
                while (w == ANNO || w == INCLUDE) {
                    w = getToken(fr);
                }
                p2->r = StateList();
                if(haveMistake==1)return NULL;
            } else if (w == INT_CONST || w == FLOAT_CONST || w == CHAR_CONST ||
                       w == IDENT || w == KEYWORD) {
                p2->r = Statement();
                if(haveMistake==1)return NULL;
                p2->r->r = NULL;
            } else {
                printf("Error: Error in FOR\n");
                haveMistake=1;
                return NULL;
            }
            root->type = FORSTATEMENT;
            root->l = p1;
            root->r = p2;
            isInRecycle = 0;
            return root;
        }
        case RETURN: {
            hasReturn = 1;
            if (isVoid == 1) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: There should be no return statement\n");
                haveMistake=1;
                return NULL;
            }
            root->type = RETURNSTATEMENT;
            root->l = NULL;
            root->r = NULL;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            root->r = Expression(SEMI);
            if(haveMistake==1)return NULL;
            return root;
        }
        case DO: {
            isInRecycle = 1;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != LB) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: mising bracket in do-while\n");
                haveMistake=1;
                return NULL;
            }

            ASTTree* p1 = (ASTTree*)malloc(sizeof(ASTTree));
            p1->type = DOWHILEBODY;
            p1->l = p1->r = NULL;

            ASTTree* p2 = (ASTTree*)malloc(sizeof(ASTTree));
            p2->type = DOWHILECONDITION;
            p2->l = p2->r = NULL;

            root->l = p1;
            root->r = p2;
            root->data.data = p1->data.data = p2->data.data = NULL;

            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            p1->l = StateList();
            if(haveMistake==1)return NULL;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != WHILE) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: missing WHILE in do-while\n");
                freeTree(root);
                haveMistake=1;
                return NULL;
            }
            root->type = DOWHILESTATEMENT;
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != LP) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: missing condition in do-while\n");
                freeTree(root);
                haveMistake=1;
                return NULL;
            }
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            p2->l = Expression(RP);
            if(haveMistake==1)return NULL;
            if (p2->l == NULL) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: missing condition in do-while\n");
                haveMistake=1;
                return NULL;
            }
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != SEMI) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: mising semicolon in do-while\n");
                freeTree(root);
                haveMistake=1;
                return NULL;
            }
            isInRecycle = 0;
            return root;
        }
        case BREAK: {
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != SEMI) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: mising semicolon in BREAK\n");
                haveMistake=1;
                return NULL;
            }
            if (isInRecycle == 0) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: unexpected BREAK\n");
                haveMistake=1;
                return NULL;
            }
            root->type = BREAKSTATEMENT;
            return root;
        }
        case CONTINUE: {
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
            if (w != SEMI) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: mising semicolon in continue\n");
                haveMistake=1;
                return NULL;
            }
            if (isInRecycle == 0) {
                printf("Error in line %d\n", cnt_lines);
                printf("Error: unexpected continue\n");
                haveMistake=1;
                return NULL;
            }
            root->type = CONTINUESTATEMENT;
            return root;
        }
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
        case IDENT:
        case ARRAY:
            return Expression(SEMI);
    }
    return root;
}

ASTTree* Expression(int endsym) {
    if (w == endsym) {
        return NULL;
    }
    int error = 0;
    Stack op;
    stack_init(&op,1000);
    ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
    p->data.data = NULL;
    p->type = OPERATOR;
    p->data.type = POUND;
    stack_push(&op,p);
    Stack opn;
    stack_init(&opn,1000);
    ASTTree *t, *t1, *t2, *root;
    while (((w != endsym) || (((ASTTree*)stack_top(&op))->data.type != POUND)) && !error) {
        if (((ASTTree*)stack_top(&op))->data.type == RP) {
            if (op.len < 3) {
                error++;
                break;
            }
            stack_pop(&op);
            stack_pop(&op);
        }
        if (w == IDENT) {
            if (checkname_exist(token_text) == 0) {
                haveMistake = 1;
                return NULL;
            }
        }
        if (w == IDENT || w == INT_CONST || w == FLOAT_CONST ||
            w == CHAR_CONST || w == ARRAY || w == STRING_CONST) {
            p = (ASTTree*)malloc(sizeof(ASTTree));
            p->data.data = NULL;
            p->type = OPERAND;
            char* token_text1 = (char*)malloc(25 * sizeof(char));
            strcpy(token_text1, token_text);
            p->data.data = token_text1;
            stack_push(&opn,p);
            w = getToken(fr);
            while (w == ANNO || w == INCLUDE) {
                w = getToken(fr);
            }
        } else if (w == endsym) {
            ASTTree* p = (ASTTree*)malloc(sizeof(ASTTree));
            p->data.data = NULL;
            p->type = OPERATOR;
            p->data.type = POUND;
            while (((ASTTree*)stack_top(&op))->data.type != POUND) {
                t2 = ((ASTTree*)stack_top(&opn));
                if (!t2 && (((ASTTree*)stack_top(&op))->data.type) != PLUSPLUS &&
                    (((ASTTree*)stack_top(&op))->data.type) != MINUSMINUS) {
                    error++;
                    break;
                }
                if (t2 != NULL)
                    stack_pop(&opn);
                if (opn.len == 0) {
                    t1 = NULL;
                } else {
                    t1 = ((ASTTree*)stack_top(&opn));
                }
                if (!t1 && (((ASTTree*)stack_top(&op))->data.type) != PLUSPLUS &&
                    (((ASTTree*)stack_top(&op))->data.type) != MINUSMINUS) {
                    error++;
                    break;
                }
                if (t1 != NULL)
                    stack_pop(&opn);
                t = ((ASTTree*)stack_top(&op));
                if (!t) {
                    error++;
                    break;
                }
                stack_pop(&op);
                t->l = t1;
                t->r = t2;
                stack_push(&opn,t);
            }
            if (opn.len != 1) {
                error++;
            }

        } else if (w >= EQ && w <= MINUSMINUS) {
            char* token_text1 = (char*)malloc(25 * sizeof(char));  //@
            switch (Precede(((ASTTree*)stack_top(&op))->data.type, w)) {
                case '<':
                    p = (ASTTree*)malloc(sizeof(ASTTree));
                    p->data.data = NULL;
                    p->type = OPERATOR;
                    p->data.type = w;
                    strcpy(token_text1, token_text);
                    p->data.data = token_text1;
                    stack_push(&op,p);
                    w = getToken(fr);
                    while (w == ANNO || w == INCLUDE) {
                        w = getToken(fr);
                    }
                    break;
                case '=':
                    t = ((ASTTree*)stack_top(&op));
                    if (!t) {
                        error++;
                        stack_pop(&op);
                    }
                    w = getToken(fr);
                    while (w == ANNO || w == INCLUDE) {
                        w = getToken(fr);
                    }
                    break;
                case '>':
                    t2 = ((ASTTree*)stack_top(&opn));
                    if (!t2 && (((ASTTree*)stack_top(&op))->data.type) != PLUSPLUS &&
                        (((ASTTree*)stack_top(&op))->data.type) != MINUSMINUS) {
                        error++;
                        break;
                    }
                    if (t2 != NULL)
                        stack_pop(&opn);
                    if (opn.len == 0) {
                        t1 = NULL;
                    } else {
                        t1 = ((ASTTree*)stack_top(&opn));
                    }
                    if (!t1 && (((ASTTree*)stack_top(&op))->data.type) != PLUSPLUS &&
                        (((ASTTree*)stack_top(&op))->data.type) != MINUSMINUS) {
                        error++;
                        break;
                    }
                    if (t1 != NULL)
                        stack_pop(&opn);
                    t = ((ASTTree*)stack_top(&op));
                    if (!t) {
                        error++;
                        break;
                    }
                    stack_pop(&op);
                    t->l = t1;
                    t->r = t2;
                    stack_push(&opn,t);

                    p = (ASTTree*)malloc(sizeof(ASTTree));
                    p->data.data = NULL;
                    p->type = OPERATOR;
                    p->data.type = w;
                    strcpy(token_text1, token_text);
                    p->data.data = token_text1;
                    stack_push(&op,p);

                    w = getToken(fr);
                    while (w == ANNO || w == INCLUDE) {
                        w = getToken(fr);
                    }
                    break;
                case '\0':
                    printf("Error in line %d\n", cnt_lines);
                    printf("Error: unknown operator\n");
                    haveMistake=1;
                    return NULL;
            }
        } else {
            error = 1;
        }
    }
    if ((opn.len == 1) && (((ASTTree*)stack_top(&op))->data.type == POUND) && error == 0) {
        t = ((ASTTree*)stack_top(&opn));
        stack_pop(&opn);
        root = (ASTTree*)malloc(sizeof(ASTTree));
        root->data.data = NULL;
        root->l = NULL;
        root->r = NULL;
        root->type = EXPRESSION;
        root->l = t;
        return root;
    } else {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: wrong expression\n");
        haveMistake=1;
        return NULL;
    }
}

char Precede(int c1, int c2) {
    if (haveMistake == 1) {
        return -1;
    }
    if (c1 == PLUS || c1 == MINUS) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case RP:
            case POUND:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case ASSIGN:
                return '>';
            case TIMES:
            case DIVIDE:
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            default:
                return '\0';
        }
    } else if (c1 == TIMES || c1 == DIVIDE) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case RP:
            case POUND:
            case TIMES:
            case DIVIDE:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
                return '>';
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            default:
                return '\0';
        }
    } else if (c1 == LP) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case RP:
                return '=';
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if (c1 == RP) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case PLUSPLUS:
            case MINUSMINUS:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if (c1 == ASSIGN) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if (c1 == MORE || c1 == LESS || c1 == MOREEQUAL || c1 == LESSEQUAL) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case RP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if (c1 == EQ || c1 == NEQ) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case RP:
            case EQ:
            case NEQ:
            case POUND:
                return '>';
            default:
                return '\0';
        }
    } else if (c1 == POUND) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case RP:
            case EQ:
            case NEQ:
            case ASSIGN:
            case PLUSPLUS:
            case MINUSMINUS:
                return '<';
            case POUND:
                return '=';
            default:
                return '\0';
        }
    } else if (c1 == PLUSPLUS || c1 == MINUSMINUS) {
        switch (c2) {
            case PLUS:
            case MINUS:
            case TIMES:
            case DIVIDE:
            case LP:
            case MORE:
            case LESS:
            case MOREEQUAL:
            case LESSEQUAL:
            case EQ:
            case NEQ:
            case ASSIGN:
            case POUND:
                return '>';
            case RP:
                return '<';
            default:
                return '\0';
        }
    }
}

void returnToken(FILE* fp) {
    int digit = strlen(token_text);
    int i;
    for (i = 0; i < digit; i++) {
        ungetc(token_text[digit - 1 - i], fp);
    }
}

void showType(int type) {
    switch (type) {
        case 1:
            printf("ExtDefS\n");
            break;
        case 2:
            printf("ExtDef\n");
            break;
        case 3:
            printf("ExtVarType\n");
            break;
        case 4:
            printf("ExtVarNameS\n");
            break;
        case 5:
            printf("ExtVarName\n");
            break;
        case 6:
            printf("FuncDef\n");
            break;
        case 7:
            printf("FuncRetTypr\n");
            break;
        case 8:
            printf("FuncName\n");
            break;
        case 9:
            printf("FuncParamS\n");
            break;
        case 10:
            printf("FuncPramaDef\n");
            break;
        case 11:
            printf("FuncPramaType\n");
            break;
        case 12:
            printf("FuncPramaName\n");
            break;
        case 13:
            printf("FuncBody\n");
            break;
        case 14:
            printf("LocalVarDefS\n");
            break;
        case 15:
            printf("LocalVarDef\n");
            break;
        case 16:
            printf("LocalVarType\n");
            break;
        case 17:
            printf("LocalVarNameS\n");
            break;
        case 18:
            printf("LocalVarName\n");
            break;
        case 19:
            printf("StatementS\n");
            break;
        case 20:
            printf("Operatee\n");
            break;
        case 21:
            printf("Operator\n");
            break;
        case 22:
            printf("Statement\n");
            break;
        case 23:
            printf("IF-ConditionStatement\n");
            break;
        case 24:
            printf("ELSE-StatementBody\n");
            break;
        case 25:
            printf("IF-Statement\n");
            break;
        case 26:
            printf("IF-ELSE-Statement\n");
            break;
        case 27:
            printf("WHILE-Statement\n");
            break;
        case 28:
            printf("WHILE-ConditionStatement\n");
            break;
        case 29:
            printf("WHILE-StatementBody\n");
            break;
        case 30:
            printf("FOR-Statement\n");
            break;
        case 31:
            printf("FOR-ConditionStatement\n");
            break;
        case 32:
            printf("FOR-StatementCondition1\n");
            break;
        case 33:
            printf("FOR-StatementCondition2\n");
            break;
        case 34:
            printf("FOR-StatementCondition3\n");
            break;
        case 35:
            printf("FOR-StatementBody\n");
            break;
        case 36:
            printf("RETURN-Statement\n");
            break;
        case 37:
            printf("BREAK-Statement\n");
            break;
        case 38:
            printf("DO-WHILE-Statement\n");
            break;
        case 39:
            printf("DO-WHILE-StatementBody\n");
            break;
        case 40:
            printf("DO-WHILE-Condition\n");
            break;
        case 41:
            printf("continueStatement\n");
            break;
        case 42:
            printf("FuncDeclare\n");
            break;
        case 43:
            printf("ArrDef\n");
            break;
        case 44:
            printf("ArrType\n");
            break;
        case 45:
            printf("ArrName\n");
            break;
        case 46:
            printf("ArrSize\n");
            break;
    }
}

void preorderTranverse(ASTTree* root, int depth) {
    if (root == NULL);
    else {
        int i;
        for (i = 0; i < depth; i++) {
            printf("   ");
        }
        showType(root->type);
        if (root->data.data != NULL) {
            for (i = 0; i < depth; i++) {
                printf("   ");
            }
            printf("   %s\n", root->data.data);
        }
        preorderTranverse(root->l, depth + 1);
        preorderTranverse(root->r, depth + 1);
    }
}

int addName(char* token_text) {
    if (haveMistake == 1) {
        return -1;
    }
    int i, flag = 0;
    VDN* p = Vroot;
    while (p->next != NULL) {
        p = p->next;
    }

    for (i = 0; i < (p->size); i++) {
        if (strcmp(token_text, p->variable[i]) == 0) {
            flag = 1;
            break;
        }
    }
    if (flag == 1) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: Duplicate variable define\n");
        haveMistake = 1;
        return flag;
    }
    char* savename = (char*)malloc(25 * sizeof(char));
    strcpy(savename, token_text);
    p->variable[p->size] = savename;
    p->size++;
    return flag;
}

int checkname_exist(char* token_text) {
    if (haveMistake == 1) {
        return -1;
    }

    int i;
    int flag = 0;
    VDN* p = Vroot;
    while (p->next != NULL) {
        p = p->next;
    }
    for (i = 0; i < (p->size); i++) {
        if (strcmp(token_text, p->variable[i]) == 0) {
            flag = 1;
            break;
        }
    }
    for (i = 0; i < (Vroot->size); i++) {
        if (strcmp(token_text, Vroot->variable[i]) == 0) {
            flag = 1;
            break;
        }
    }
    if (flag == 0) {
        printf("Error in line %d\n", cnt_lines);
        printf("Error: Using undeclared variable\n");
        exit(0);
        // haveMistake = 1;
    }
    return flag;
}
