#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>

#ifdef __WIN32
	#include <windows.h>
	#define FOREGROUND_WHITE (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)
#endif

#include "base.h"
#include "util/utilbase.h"
#include "compile/compilebase.h"

#include "util/util.h"
#include "compile/compile.h"

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
