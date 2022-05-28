#include "gettoken.h"

char token_text[20];  //存放单词自身值
char string_num[20];  //存放数字的字符串
int cnt_lines = 1;

int isNum(char c) {
    return (c >= '0' && c <= '9');
}

int isXNum(char c) {
    return ((isNum(c)) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

int isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isLetterOrNum(char c) {
    return isAlpha(c) || isNum(c);
}

int add2token(char* token, char c) {
    int i = 0;
    while (*(token + i) != '\0') {
        i++;
    }
    /* max len : 20 */
    if (i >= 19) {
        return -1;  //添加失败
    }
    *(token + i) = c;
    *(token + i + 1) = '\0';
    return 1;
}

int getToken(FILE* fp) {
    char c;
    *(token_text) = '\0';
    do {
        c = fgetc(fp);
        if (c == '\n') {
            cnt_lines++;
        }
    } while (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f');

    if (isAlpha(c) || c == '_') {
        do {
            add2token(token_text, c);
        } while (isLetterOrNum(c = fgetc(fp)));
        ungetc(c, fp);
        if (strcmp(token_text, "int") == 0) {
            return INT;
        }
        if (strcmp(token_text, "double") == 0) {
            return DOUBLE;
        }
        if (strcmp(token_text, "char") == 0) {
            return CHAR;
        }
        if (strcmp(token_text, "short") == 0) {
            return SHORT;
        }
        if (strcmp(token_text, "long") == 0) {
            return LONG;
        }
        if (strcmp(token_text, "float") == 0) {
            return FLOAT;
        }

        /* is keyword */
        if (strcmp(token_text, "if") == 0) {
            return IF;
        }
        if (strcmp(token_text, "else") == 0) {
            return ELSE;
        }
        if (strcmp(token_text, "do") == 0) {
            return DO;
        }
        if (strcmp(token_text, "while") == 0) {
            return WHILE;
        }
        if (strcmp(token_text, "for") == 0) {
            return FOR;
        }
        if (strcmp(token_text, "struct") == 0) {
            return STRUCT;
        }
        if (strcmp(token_text, "break") == 0) {
            return BREAK;
        }
        if (strcmp(token_text, "switch") == 0) {
            return SWITCH;
        }
        if (strcmp(token_text, "case") == 0) {
            return CASE;
        }
        if (strcmp(token_text, "typedef") == 0) {
            return TYPEDEF;
        }
        if (strcmp(token_text, "return") == 0) {
            return RETURN;
        }
        if (strcmp(token_text, "continue") == 0) {
            return CONTINUE;
        }
        if (strcmp(token_text, "void") == 0) {
            return VOID;
        }

        for (int i = 0; i < KEYWORD_LEN; i++) {
            if (strcmp(token_text, KeyWords[i]) == 0) {
                return KEYWORD;
            } else {
                c = fgetc(fp);
                if (c == '[') {
                    //识别数组
                    add2token(token_text, c);
                    c = fgetc(fp);
                    while (c >= '0' && c <= '9') {
                        add2token(token_text, c);
                        add2token(string_num, c);
                        c = fgetc(fp);
                    }
                    if (c != ']') {
                        return ERROR_TOKEN;
                    }
                    add2token(token_text, c);
                    return ARRAY;
                } else {
                    ungetc(c, fp);
                }
            }
        }
        return IDENT;
    }

    // number
    if (isNum(c)) {
        do {
            add2token(token_text, c);
        } while (isNum(c = fgetc(fp)));
        if (c != '.' && c != 'u' && c != 'l') {
            if (c != ' ' && c != ';' && c != ')' && c != '+' && c != '-' &&
                c != '*' && c != '/') {
                return ERROR_TOKEN;
            }
            ungetc(c, fp);
            return INT_CONST;
        } else if (c == '.') {
            c = fgetc(fp);
            if (!isNum(c)) {
                return ERROR_TOKEN;
            } else {
                ungetc(c, fp);
                c = '.';
                add2token(token_text, c);
                c = fgetc(fp);
                do {
                    add2token(token_text, c);
                } while (isNum(c = fgetc(fp)));
                if (c == 'f') {
                    add2token(token_text, c);
                    return FLOAT_CONST;
                } else {
                    ungetc(c, fp);
                }
                return FLOAT_CONST;
            }
        } else if (c == 'u') {
            add2token(token_text, c);
            c = fgetc(fp);
            if (c == 'l') {//ul
                add2token(token_text, c);
                c = fgetc(fp);
                if (c == 'l') {//ull
                    add2token(token_text, c);
                    return INT_CONST;
                } else {
                    ungetc(c, fp);
                    return INT_CONST;
                }
            } else {//u
                ungetc(c, fp);
                return INT_CONST;
            }
        } else{//l
            add2token(token_text, c);
            return INT_CONST;
        }
    }

    switch (c) {
        // float that starts with `.`
        case '.':
            do {
                add2token(token_text, c);
            } while (isNum(c = fgetc(fp)));
            ungetc(c, fp);
            return FLOAT_CONST;

        // char
        case '\'':
            add2token(token_text, '\'');
            if ((c = fgetc(fp)) != '\\') {
                add2token(token_text, c);
                if ((c = fgetc(fp)) == '\'') {
                    add2token(token_text, c);
                    return CHAR_CONST;
                } else {
                    return ERROR_TOKEN;
                }
            } else { //  '\*'
                c = fgetc(fp);
                if (c == 'n' || c == 't' || c == '\\' || c == '\'' || c == '\"') {
                    add2token(token_text, c);
                    if ((c = fgetc(fp)) == '\'') {
                        add2token(token_text, c);
                        return CHAR_CONST;
                    } else {
                        return ERROR_TOKEN;
                    }
                } else if (c == 'x') {// hex
                    add2token(token_text, c);
                    if (isXNum((c = fgetc(fp)))) {
                        add2token(token_text, c);
                        if (isXNum((c = fgetc(fp)))) {
                            add2token(token_text, c);
                        } else {
                            ungetc(c, fp);
                        }
                        if ((c = fgetc(fp)) == '\'') {
                            add2token(token_text, c);
                            return CHAR_CONST;
                        } else {
                            return ERROR_TOKEN;
                        }
                    } else {
                        return ERROR_TOKEN;
                    }
                } else if (c >= '0' && c <= '7') {
                    // oct
                    add2token(token_text, c);
                    if ((c = fgetc(fp)) >= '0' && c <= '7') {
                        add2token(token_text, c);
                        if ((c = fgetc(fp)) >= '0' && c <= '7') {
                            add2token(token_text, c);
                            if ((c = fgetc(fp)) == '\'') {
                                add2token(token_text, c);
                                return CHAR_CONST;
                            } else {
                                return ERROR_TOKEN;
                            }
                        } else if (c == '\'') {
                            add2token(token_text, '\'');
                            return CHAR_CONST;
                        } else {
                            return ERROR_TOKEN;
                        }
                    } else {
                        if (c == '\'') {
                            add2token(token_text, c);
                            return CHAR_CONST;
                        } else {
                            ungetc(c, fp);
                            return ERROR_TOKEN;
                        }
                    }
                } else {
                    return ERROR_TOKEN;
                }
            }

        // string
        case '"':
            do {
                if (c != '\\')
                    add2token(token_text, c);
                if (c == '\\') {
                    c = fgetc(fp);
                    add2token(token_text, c);
                }
            } while ((c = fgetc(fp)) != '"' && c != '\n');
            if (c == '"') {
                add2token(token_text, '"');
                return STRING_CONST;
            } else {
                return ERROR_TOKEN;
            }

        // div / comment
        case '/':
            add2token(token_text, c);
            if ((c = fgetc(fp)) == '/') {
                do {
                    add2token(token_text, c);
                } while ((c = fgetc(fp)) != '\n');
                ungetc(c, fp);
                return ANNO;
            } else if (c == '*') {
                while (1) {
                    add2token(token_text, c);
                    c = fgetc(fp);
                    if (c == '*') {
                        add2token(token_text, c);
                        if ((c = fgetc(fp)) == '/') {
                            add2token(token_text, c);
                            return ANNO;
                        }
                    }
                    if (c == '\n') {
                        add2token(token_text, c);
                        c = '\t';
                        add2token(token_text, c);
                        add2token(token_text, c);
                    }
                }
            } else {
                ungetc(c, fp);
                return DIVIDE;
            }

        // macro
        case '#':
            add2token(token_text, c);
            if (isAlpha(c = fgetc(fp))) {
                do {
                    add2token(token_text, c);
                } while (isAlpha(c = fgetc(fp)));
                if (strcmp(token_text, "#include") == 0) {
                    do {
                        add2token(token_text, c);
                    } while ((c = fgetc(fp)) != '\n');
                    ungetc(c, fp);
                    return INCLUDE;
                } else if (strcmp(token_text, "#define") == 0) {
                    do {
                        add2token(token_text, c);
                    } while ((c = fgetc(fp)) != '\n');
                    return MACRO;
                } else {
                    return ERROR_TOKEN;
                }
            } else {
                return ERROR_TOKEN;
            }
        case ',':
            add2token(token_text, c);
            return COMMA;
        case ';':
            add2token(token_text, c);
            return SEMI;
        case '=':
            c = fgetc(fp);
            if (c == '=') {
                add2token(token_text, c);
                add2token(token_text, c);
                return EQ;
            }
            ungetc(c, fp);
            add2token(token_text, '=');
            return ASSIGN;
        case '!':
            c = fgetc(fp);
            if (c == '=') {
                add2token(token_text, '!');
                add2token(token_text, '=');
                return NEQ;
            } else {
                return ERROR_TOKEN;
            }
        case '+':
            c = fgetc(fp);
            if (c == '+') {
                add2token(token_text, c);
                add2token(token_text, c);
                return PLUSPLUS;
            }
            ungetc(c, fp);
            add2token(token_text, '+');
            return PLUS;
        case '-':
            c = fgetc(fp);
            if (c == '-') {
                add2token(token_text, c);
                add2token(token_text, c);
                return MINUSMINUS;
            }
            ungetc(c, fp);
            add2token(token_text, '-');
            return MINUS;
        case '(':
            add2token(token_text, c);
            return LP;
        case ')':
            add2token(token_text, c);
            return RP;
        case '{':
            add2token(token_text, c);
            return LB;
        case '}':
            add2token(token_text, c);
            return RB;
        case '[':
            add2token(token_text, c);
            return LM;
        case ']':
            add2token(token_text, c);
            return RM;
        case '*':
            add2token(token_text, c);
            return TIMES;
        case '>':
            add2token(token_text, c);
            if ((c = fgetc(fp)) == '=') {
                add2token(token_text, c);
                return MOREEQUAL;
            } else {
                ungetc(c, fp);
                return MORE;
            }
        case '<':
            add2token(token_text, c);
            if ((c = fgetc(fp)) == '=') {
                add2token(token_text, c);
                return LESSEQUAL;
            } else {
                ungetc(c, fp);
                return LESS;
            }
        case EOF:
            return EOF;
        default:
            return ERROR_TOKEN;
    }
}
