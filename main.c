#include <stdio.h>
#include <stdlib.h>
#include "Token/gettoken.h"
#include "Lex/lexicalAnalyse.h"
#include "Syntax/syntaxAnalyse.h"
#include "Format/format.h"
char fileIn[128],fileOut[128];
FILE *fr,*fw;
int main(void) {
    int option;
    while (1) {
        system("cls");
        printf(" +----------------------------+\n");
        printf(" |    Choose A Function       |\n");
        printf(" |    1. Lexical Analysis     |\n");
        printf(" |    2. Syntax Analysis      |\n");
        printf(" |    3. Show Formatted File  |\n");
        printf(" |    0. Exit                 |\n");
        printf(" +----------------------------+\n");
        scanf("%d", &option);
        switch (option) {
            case 1:
                printf("请输入源文件路径:\n");
                scanf("%s",fileIn);
                fr = fopen(fileIn, "r");
                lexicalAnalyse();
                printf("Press any key to continue\n");
                fclose(fr);
                getchar();
                getchar();
                break;
            case 2:
                printf("请输入源文件路径:\n");
                scanf("%s",fileIn);
                fr = fopen(fileIn, "r");
                syntaxAnalyse();
                printf("Press any key to continue\n");
                fclose(fr);
                getchar();
                getchar();
                break;
            case 3:
                printf("请输入源文件路径:\n");
                scanf("%s",fileIn);
                fr = fopen(fileIn, "r");
                printf("请输入目标文件路径:\n");
                scanf("%s",fileOut);
                fw = fopen(fileOut, "w");
                formatCode();
                printf("Press any key to continue\n");
                fclose(fr);
                fclose(fw);
                getchar();
                getchar();
                break;
            case 0:
                system("cls");
                printf("Thanks for using!\n");
                return 0;
            default:
                printf("Unknown key, please enter again\n");
                printf("Press any key to continue\n");
                getchar();
                getchar();
                break;
        }
    }
}
