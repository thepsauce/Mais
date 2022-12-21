#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
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
	#define FOREGROUND_WHITE (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)
#endif

#include "base.h"
#include "util/utilbase.h"
#include "compile/compilebase.h"

#include "util/util.h"
#include "compile/compile.h"

local int printtok(const struct token *tok)
{
	assert(tok != NULL && "printtok > error: token is null\n");
    printf("%ld: ", tok->pos);
    switch(tok->type)
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
		switch(tok->c)
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
    while(psreadtok(&parser) != EOF)
    {
        printtok(&parser.tok);
        printf("\n");
    }
	psclose(&parser);
	
	psdiscard(&parser);
    return 0;
}
