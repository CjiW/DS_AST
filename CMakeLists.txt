cmake_minimum_required(VERSION 3.22)
project(AST)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(AST
        main.c
        Token/gettoken.c Token/gettoken.h
        Lex/lexicalAnalyse.c Lex/lexicalAnalyse.h
        Syntax/syntaxAnalyse.c Syntax/syntaxAnalyse.h
        Syntax/stack/stack.c Syntax/stack/stack.h
        Format/format.c Format/format.h)
