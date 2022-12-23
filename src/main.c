#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// in case this function is missing
char *strndup(const char *str, size_t len)
{
    char *s = malloc(len + 1);
    memcpy(s, str, len);
    s[len] = 0;
    return s;
}

#ifdef __WIN32
#include <windows.h>
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#endif

#include "base.h"
#include "util/utilbase.h"

#include "compile/compilebase.h"

#include "util/util.h"
#include "compile/compile.h"
// TOKEOF, // end of file
//     TOKNUMBER, // 0x3123 0 123 8783 0b10010 0c2561
//     TOKSTRING, // "Hello there" "Banana"
//     TOKWORD, // word var func tree whatever
//     TOKANY, // . , ; : - > ( ) [ ] + * / % & | ^ = <  { } ! ? ~
//     TOKKEYWORD, // see list of keywords below

int getTokenType(char *tok)
{
    if (isdigit(tok))
    {
        return TOKNUMBER;
    }
    else if ((*tok >= 33 && *tok <= 47) || (*tok >= 58 && *tok <= 64) || (*tok >= 91 && *tok <= 96) || (*tok >= 123 && *tok <= 126))
    {
        return TOKANY;
    }
    else if (*tok == EOF)
    {
        return TOKEOF;
    }
    for (int i = 0; i < 53; i++)
    {
        if (*tok == *Keywords[i])
        {
            return TOKKEYWORD;
        }
    }
    for (int i = 0; i < strlen(tok); i++)
    {
        if (isspace(tok[i]))
        {
            return TOKSTRING;
        }
    }
    return TOKWORD;
}

int psreadtok(struct parser *parser)
{
    FILE *file = fopen("file.txt", "r");

    char buf[1024];
    int index = 0;
    struct token *tok;

    while (fscanf(file, "%s", buf) != EOF)
    {
        if (parser->fOff == index)
        {
            tok->type = getTokenType(buf);
            tok->pos = parser->fOff;
            switch (tok->type)
            {
            case TOKSTRING:
                tok->str = buf;
                tok->strLen = strlen(buf);
                break;
            case TOKANY:
                tok->c = (int)*buf;
                break;
            case TOKKEYWORD:
                /* code */
                break;
            case TOKNUMBER:
                tok->l = atoi(buf);
                break;

            case TOKWORD:
                strcpy(tok->word, buf);
                break;
            case TOKEOF:
                parser->failOnEof = true;
                break;
            }
            parser->tok = *tok;
        }
        index += 1;
    }
    parser->fOff += 1;
    if (!parser->failOnEof)
    {
        return 0;
    }
    else
    {
        return EOF;
    }
}

local int printtok(const struct token *tok)
{
    assert(tok != NULL && "printtok > error: token is null\n");
    printf("%ld: ", tok->pos);
    switch (tok->type)
    {
    case TOKEOF:
        printf("EOF");
        break;
    case TOKNUMBER:
        printf("NUMBER: %I64d", tok->l);
        break;
    case TOKSTRING:
        printf("STRING: \"%.*s\"", tok->strLen, tok->str);
        break;
    case TOKWORD:
        printf("WORD: %s", tok->word);
        break;
    case TOKANY:
        switch (tok->c)
        {
        case 0x100:
            printf("CHAR: '<<'");
            break;
        case 0x101:
            printf("CHAR: '>>'");
            break;
        case 0x102:
            printf("CHAR: '!!'");
            break;
        default:
            printf("CHAR: '%c'", tok->c);
        }
        break;
    case TOKKEYWORD:
        printf("KEYWORD: %s", tok->word);
        break;
    default:
        printf("CORRUPT TOKEN");
        break;
    }
    return 0;
}

int main(void)
{
    struct parser parser;

    psinit(&parser);
    // test the tokenizer
    psopen(&parser, "../proto.txt");
    while (psreadtok(&parser) != EOF)
    {
        printtok(&parser.tok);
        printf("\n");
    }
    
    psclose(&parser);

    psdiscard(&parser);
    return 0;
}
