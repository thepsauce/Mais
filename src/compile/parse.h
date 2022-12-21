global int psinit(struct parser *parser)
{
	memset(parser, 0, sizeof(*parser));
	
	parser->scopes = arrcreate(10, sizeof*parser->scopes);
	arradd(parser->scopes, &(int) { 0 });
	
	parser->labels = arrcreate(10, sizeof*parser->labels);
	arradd(parser->labels, &(struct label) { 0 });
#ifdef __WIN32
	parser->hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(parser->hOut, &parser->csbiInfo);
#endif
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
	if(push)
		arradd(parser->scopes, &parser->labelCnt);
	arradd(scope->labels, label);
	return 0;
}

global int psaddgroup(struct parser *parser, const struct group *group)
{
	struct label *scope = parser->labels + parser->scopes[parser->scopeCnt - 1];
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
