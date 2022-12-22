global int psinit(struct parser *parser)
{
	memset(parser, 0, sizeof(*parser));
	
	parser->scopes = arrcreate(10, sizeof*parser->scopes);
	arradd(parser->scopes, &(int) { 0 });
	
	parser->labels = arrcreate(10, sizeof*parser->labels);
	memset(arradd(parser->labels, NULL), 0, sizeof(*parser->labels));
#ifdef __WIN32
	parser->hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(parser->hOut, &parser->csbiInfo);
#endif
	return 0;
}

global int psdiscard(struct parser *parser)
{
	arrfree(parser->scopes);
	arrfree(parser->labels);
	return 0;
}

global int psopen(struct parser *parser, const char *filePath)
{
	File file;
	file = fopen(filePath, "rb");
	if(!file)
	{
		printf("invalid argument: file '%s' does not exist\n", filePath);
		return -1;
	}
	parser->file = file;
	parser->fileName = strdup(filePath);
	return 0;
}

global int psclose(struct parser *parser)
{
	fclose(parser->file);
	free(parser->fileName);
	parser->fileName = NULL;
	return 0;
}

global int pspeek(struct parser *parser)
{
	if(!parser->nBuf)
	{
		parser->fOff = ftell(parser->file);
		parser->nBuf = fread(parser->buf, 1, sizeof(parser->buf), parser->file);
		parser->iBuf = 0;
	}
	if(!parser->nBuf)
		return EOF;
	return parser->buf[parser->iBuf];
}

global int psskip(struct parser *parser)
{
	parser->nBuf--;
	parser->iBuf++;
	return 0;
}

global int psread(struct parser *parser)
{
	if(!parser->nBuf)
	{
		parser->fOff = ftell(parser->file);
		parser->nBuf = fread(parser->buf, 1, sizeof(parser->buf), parser->file);
		parser->iBuf = 0;
	}
	if(!parser->nBuf)
		return EOF;
	parser->nBuf--;
	return parser->buf[parser->iBuf++];
}

global long pstell(struct parser *parser)
{
	return parser->fOff + parser->iBuf;
}

global int psaddlabel(struct parser *parser, const struct label *label, bool push)
{
	int cnt;
	if(push)
	{
		cnt = arrcount(parser->labels);
		arradd(parser->scopes, &cnt);
	}
	arradd(parser->labels, label);
	return 0;
}

global int psaddgroup(struct parser *parser, const struct group *group)
{
	struct label *scope;
	int cnt;
	
	cnt = arrcount(parser->labels);
	scope = parser->labels + parser->scopes[cnt - 1];
	arradd(scope->groups, group);
	return 0;
}

global int psthrow(struct parser *parser, const char *format, ...)
{
    int line = 1;
    int col = 1;
    va_list args;
    fseek(parser->file, 0, SEEK_SET);
    while(ftell(parser->file) != parser->fOff)
	{
        if(fgetc(parser->file) == '\n')
        {
            line++;
            col = 1;
        }
        else
            col++;
	}
	for(int i = 0; i < parser->iBuf; i++)
	{
		if(parser->buf[i] == '\n')
        {
            line++;
            col = 1;
        }
        else
            col++;
	}
	fseek(parser->file, pstell(parser), SEEK_SET);
#ifdef __WIN32
	SetConsoleTextAttribute(parser->hOut, FOREGROUND_WHITE | FOREGROUND_INTENSITY);
    printf("%s", parser->fileName);
	SetConsoleTextAttribute(parser->hOut, parser->csbiInfo.wAttributes);
	printf(":");
	SetConsoleTextAttribute(parser->hOut, FOREGROUND_WHITE | FOREGROUND_INTENSITY);
    printf("%d", line);
	SetConsoleTextAttribute(parser->hOut, parser->csbiInfo.wAttributes);
	printf(":");
	SetConsoleTextAttribute(parser->hOut, FOREGROUND_WHITE | FOREGROUND_INTENSITY);
    printf("%d", col);
	SetConsoleTextAttribute(parser->hOut, parser->csbiInfo.wAttributes);
	printf(":");
	SetConsoleTextAttribute(parser->hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf(" error: ");
	SetConsoleTextAttribute(parser->hOut, parser->csbiInfo.wAttributes);
#else
    printf("%s:%d:%d: error: ", parser->fileName, line, col);
#endif
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
	printf("\n");
    return -1;
}

int parsedata(struct parser *parser, struct datapoint *dp);
int parseexpr(struct parser *parser, struct group *group);
int parsetypedef(struct parser *parser);
int parsevarfuncgoto(struct parser *parser);

#include "expr.h"
#include "data.h"
#include "type.h"
#include "keyword.h"
#include "typedef.h"
#include "varfuncgoto.h"

struct {
	const char *word;
	int (*parse)(struct parser *parser);
} ParseMap[] = {
	{ "typedef", parsetypedef },
	{ "test", parsetest },
	{ "jmp", parsejmp },
	{ "jz", parsejz },
	{ "jnz", parsejnz },
	{ "je", parseje },
	{ "jne", parsejne },
	{ "jl", parsejl },
	{ "jle", parsejle },
	{ "jg", parsejg },
	{ "jge", parsejge },
	{ "ret", parseret },
	{ "re", parsere },
	{ "rne", parserne },
	{ "rl", parserl },
	{ "rg", parserg },
	{ "rge", parserge },
	{ "inc", parseinc },
	{ "dec", parsedec },
	{ "not", parsenot },
	{ "and", parseand },
	{ "or", parseor },
	{ "xor", parsexor },
	{ "lsh", parselsh },
	{ "rsh", parsersh },
	{ "add", parseadd },
	{ "sub", parsesub },
	{ "mul", parsemul },
	{ "div", parsediv },
	{ "mod", parsemod },
	{ "neg", parseneg },
	{ "fadd", parsefadd },
	{ "fsub", parsefsub },
	{ "fmul", parsefmul },
	{ "fdiv", parsefdiv },
	{ "fmod", parsefmod },
	{ "fneg", parsefneg },
	{ "mov", parsemov },
	{ "call", parsecall },
};

global int parse(struct parser *parser)
{
	int err = 0;
	int i;
    while(parser->failOnEof = 0, psreadtok(parser) != EOF)
    {
		parser->failOnEof = 1;
        if(parser->tok.type == TOKKEYWORD)
		{
			// TODO: implement perfect hash function for a hash table instead of linear search
			for(i = 0; i < ARRLEN(ParseMap); i++)
				if(!strcmp(ParseMap[i].word, parser->tok.word))
				{
					err |= ParseMap[i].parse(parser);
					break;
				}
			if(i == ARRLEN(ParseMap))
				err = psthrow(parser, "invalid use of keyword '%s'", parser->tok.word);
		}
		else if(tokischar(&parser->tok, '}'))
		{
			// exit scope
			if(parser->scopeCnt == 1)
				err = psthrow(parser, "closing '}' doesn't match with an open one");
			else
				parser->scopeCnt--;
		}
		else if(parser->tok.type == TOKWORD)
		{
			// variable, function or goto
			err |= parsevarfuncgoto(parser);
		}
		else
		{
			err = psthrow(parser, "invalid token(%d)", parser->tok.type);
		}
    }
    return err;
}
