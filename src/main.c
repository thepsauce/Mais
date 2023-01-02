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
	#define FOREGROUND_WHITE (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)
#endif

#include "base.h"
#include "util/utilbase.h"
#include "compile/compilebase.h"

#include "util/util.h"
#include "compile/compile.h"
#include "debugprint.h"

void printintarr(int *arr)
{
	int cnt = arrcount(arr);
	for(int i = 0; i < cnt; i++)
		printf("%d\n", arr[i]);
	printf("\n");
}

int main(void)
{
	// trial and error to create perfect hash function
	/*
	for(int i = 0; i < ARRLEN(typeIdentfs); i++)
		printf("%d\n", (typeIdentfs[i][0] + typeIdentfs[i][1] - 198) % 10);
	int comp(const char **ps1, const char **ps2)
	{
		const char *s1 = *ps1;
		const char *s2 = *ps2;
		return -((s2[0] + s2[1] - 198) % 10) + ((s1[0] + s1[1] - 198) % 10);
	}
	qsort(typeIdentfs, ARRLEN(typeIdentfs), sizeof(void*), comp);
	for(int i = 0; i < ARRLEN(typeIdentfs); i++)
		printf("\"%s\", ", typeIdentfs[i]);
	for(int i = 0; i < ARRLEN(typeIdentfs); i++)
		printf("%d\n", (typeIdentfs[i][0] + typeIdentfs[i][1] - 198) % 10);*/
	/*LARGE_INTEGER start, end, freq;
	static const char *typeIdentfs[] = {
		"db", "dw", "dd", "static", "df", "dq", "dr", "readonly", "dt"
	};
	QueryPerformanceFrequency(&freq);
	
	bool hashexists(const char *str)
	{
		int hash;
		hash = (str[0] + str[1] - 198) % 10;
		if(hash < ARRLEN(typeIdentfs) && !strcmp(typeIdentfs[hash], str))
			return 1;
		return 0;
	}
	bool linearexists(const char *str)
	{
		for(int i = 0; i < ARRLEN(typeIdentfs); i++)
			if(!strcmp(str, typeIdentfs[i]))
				return 1;
		return 0;
	}
	
	const char *testSet[] = {
		"db", "dw", "dh", "static", "df", "dq", "dr", "renly", "d",
		"db", "dw", "gdd", "static", "df", "dq", "r", "readoly", "dt",
		"dab", "dawdw", "fdd", "ic", "df", "dq", "dr", "ready", "dt",
		"db", "dw", "d", "static", "df", "dq", "dr", "readly", "dt",
		"dbt", "dw", "dd", "stattjyic", "dfads", "dq", "rd", "readoly", "dtjga",
		"db", "awdwdw", "dd", "atic", "df", "daq", "d", "readl", "dt",
		"zxcb", "dw", "dd", "stic", "df", "dqr", "der", "realy", "dt",
		"djhub", "tdw", "ddsd", "static", "f", "dawq", "dr", "readonly", "dtz",
		"db", "dw", "dd", "static", "df", "dq", "dhhr", "rgeadonly", "adt",
		"db", "dawdsw", "dtyjd", "stattyjic", "df", "dq", "dfr", "readonlay", "dxct",
		"db", "dw", "dd", "static", "df", "dq", "dr", "redadonly", "zxcdt"
	};
	
	const int tries = 1000000;
	bool arr[ARRLEN(testSet)];
	int t;
	
	QueryPerformanceCounter(&start);
	for(t = 0; t < tries; t++)
		for(int i = 0; i < ARRLEN(testSet); i++)
			arr[i] = hashexists(testSet[i]);
	QueryPerformanceCounter(&end);
	printf("HASH:   %lf\n", (double) (end.QuadPart - start.QuadPart) / (double) freq.QuadPart);
	
	QueryPerformanceCounter(&start);
	for(t = 0; t < tries; t++)
		for(int i = 0; i < ARRLEN(testSet); i++)
			if(linearexists(testSet[i]) != arr[i])
			{
				printf("NO NO: %s\n", testSet[i]);
				exit(-1);
			}
	QueryPerformanceCounter(&end);
	printf("LINEAR: %lf\n", (double) (end.QuadPart - start.QuadPart) / (double) freq.QuadPart);
	return 0;*/
	/*int *arr;
	arr = arrcreate(10, sizeof*arr);
	for(int i = 0; i < 12; i++)
		arr = arrpush(arr, &(int) { rand() });
	arrmovecursor(arr, 6);
	for(int i = 0; i < 12; i++)
		arr = arrpush(arr, &(int) { rand() });
	printintarr(arr);
	printf("\n");
	for(int i = 0, c = arrcount(arr); i < c; i++)
		printf("%d\n", *(int*) arrpop(arr));
	
	arrfree(arr);*/
	
	/*struct buf b;
	bufinits(&b, "Hey");
	bufac(&b, ' ');
	bufas(&b, "Bananna");
	
	printf("%.*s\n", b.len, b.buf);
	
	printf("%d\n", buffc(&b, 'a', -1));
	printf("%d\n", buffc(&b, 'B', 0));
	printf("%d\n", buffcr(&b, 'a', -1));
	printf("%d\n", buffcr(&b, 'H', -1));
	
	printf("%d\n", buffs(&b, "anna", -1));
	printf("%d\n", buffs(&b, "Hey ", -1));
	
	printf("%d\n", buffsrs(&b, "Hey ", -1, "Get outta here: "));
	
	printf("%.*s\n", b.len, b.buf);
	
	bufaf(&b, "POWER: %d, %d, !%s!", 123, 456, "PENIS");
	bufif(&b, 4, "POWER: %d, %d, !%s!", 123, 456, "PENIS");
	
	char *s = bufexd(&b);
	printf("%s\n", s);
	free(s);*/
	struct parser parser;
	struct type type;
	
	psinit(&parser);
	// test the tokenizer
	psopen(&parser, "../proto.txt");
	while(psreadtok(&parser) != EOF)
	{
		if(psistype(&parser))
		{
			if(!parsetype(&parser, &type))
			{
				printf("TYPE: ");
				printtype(&type);
				printf("\n");
			}
		}
	}
	psclose(&parser);
	
	psdiscard(&parser);
	return 0;
}
