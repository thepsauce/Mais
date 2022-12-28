// A - 10
// b - 11
// 4 - 4
local int pshextochar(int c)
{
    if(c >= '0' && c <= '9')
		return c - '0';
    if(c >= 'a' && c <= 'f')
        return c - ('a' - 10);
    if(c >= 'A' && c <= 'F')
        return c - ('A' - 10);
    return -1;
}

// \x34 - 0x34
// \n   - 0x0A
// 3    - 0x33
local int psdecodechar(struct parser *parser)
{
    int c, r;
	c = psread(parser);
    if(c == EOF)
        return psthrow(parser, "unexpected end of file");
    if(c == '\\')
        switch(psread(parser))
        {
		case EOF: return psthrow(parser, "hanging '\\' at end of file");
        case '0': return 0x00;
        case 'a': return 0x07;
        case 'b': return 0x08;
        case 't': return 0x09;
        case 'n': return 0x0A;
        case 'v': return 0x0B;
        case 'f': return 0x0C;
        case 'r': return 0x0D;
        case 'e': return 0x1B;
        case 'x':
            if((r = pshextochar(psread(parser))) < 0)
                return psthrow(parser, "psdecodechar > error");
            c = r;
            if((r = pshextochar(psread(parser))) < 0)
               return psthrow(parser, "psdecodechar > error");
            c <<= 4;
            c |= r;
        }
    return c;
}

global int psreadtok(struct parser *parser)
{
    int c;
    bool bv;
    long l;
    struct buf buf;
    u64 num;
    int cmtCnt = 0;
    bool comment = 0;
    bool unleashedComment = 0;
	
	if(parser->stayOnToken)
	{
		parser->stayOnToken = 0;
		return parser->lastTokRet;
	}
	
    while(c = psread(parser), c != EOF)
    {
        if(c == ';')
        {
            comment = !comment;
            cmtCnt++;
            if(cmtCnt == 3)
            {
                comment = 0;
                unleashedComment = !unleashedComment;
            }
        }
        else
        {
            cmtCnt = 0;
            if(c == '\n' || c == '\r')
                comment = 0;
            else if(!isspace(c) && !comment && !unleashedComment)
                goto no_eof;
        }
    }
	if(parser->failOnEof)
	{
		psthrow(parser, "unexpected end of file");
		psclose(parser);
		psdiscard(parser);
		exit(-1);
		return -1;
	}
	parser->tok.type = TOKEOF;
	return EOF;
	no_eof:
    parser->tok.pos = pstell(parser) - 1;
    if(isalpha(c) || c == '_')
    {
        l = 1;
        parser->tok.word[0] = c;
        while(c = pspeek(parser), isalnum(c) || c == '_')
        {
            parser->tok.word[l++] = c;
            if(l == sizeof(parser->tok.word))
                return psthrow(parser, "word is too long (exceeds the maximum length of 1024)");
			psskip(parser);
        }
        parser->tok.word[l] = 0;
		for(int i = 0; i < ARRLEN(Keywords); i++)
			if(!strcmp(parser->tok.word, Keywords[i]))
			{
				parser->tok.type = TOKKEYWORD;
				return 0;
			}
        parser->tok.type = TOKWORD;
        return 0;
    }
    if(isdigit(c))
    {
        num = c - '0';
        if(!num)
        {
            switch(pspeek(parser))
            {
            case 'X':
            case 'x':
				psskip(parser);
                while(c = pspeek(parser), isxdigit(c))
				{
                    num *= 16, num += c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - ('A' - 10) : c - ('a' - 10);
					psskip(parser);
				}
                break;
            case 'B':
            case 'b':
				psskip(parser);
                while(c = pspeek(parser), c >= '0' && c <= '1')
				{
                    num *= 2, num += c - '0';
					psskip(parser);
				}
                break;
            case 'C':
            case 'c':
				psskip(parser);
                while(c = pspeek(parser), c >= '0' && c <= '7')
				{
                    num *= 8, num += c - '0';
					psskip(parser);
				}
                break;
            default:
                if(isalpha(c))
                    return psthrow(parser, "invalid number prefix '0%c'", c);
                goto base_10;
            }
        }
        else
        {
            base_10:
            while(c = pspeek(parser), isdigit(c))
			{
                num *= 10, num += c - '0';
				psskip(parser);
			}
        }
        parser->tok.type = TOKNUMBER;
        parser->tok.l = num;
        return 0;
    }
    parser->tok.type = TOKANY;
    switch(c)
    {
    case '\'':
		if((c = psdecodechar(parser)) == EOF)
			return -1;
        parser->tok.l = c;
        c = psread(parser);
        if(c != '\'')
            return psthrow(parser, "expected closing single quotation but got '%c'", c);
        parser->tok.type = TOKNUMBER;
        return 0;
    case '\"':
        parser->tok.type = TOKSTRING;
        bufinit(&buf);
        while(c = pspeek(parser), c != EOF)
        {
            if(c == '\"')
            {
                parser->tok.str = buf.buf;
                parser->tok.strLen = buf.len;
				psskip(parser);
                return 0;
            }
            if((c = psdecodechar(parser)) == EOF)
                return -1;
            bufac(&buf, c);
        }
        return psthrow(parser, "unmatched \" at end of file");
    case '<': // <<
        c = pspeek(parser);
        if(c == '<')
        {
            parser->tok.c = 0x100;
			psskip(parser);
        }
        else
        {
            parser->tok.c = '<';
        }
        return 0;
    case '>': // >>
        c = pspeek(parser);
        if(c == '>')
        {
            parser->tok.c = 0x101;
			psskip(parser);
        }
        else
        {
            parser->tok.c = '>';
        }
        return 0;
    case '!': // ! !! ('!!!'=='!')
        bv = 1;
        while((c = pspeek(parser)) == '!')
		{
            bv = !bv;
			psskip(parser);
		}
        parser->tok.c = bv ? '!' : 0x102;
        return 0;
    }
    parser->tok.c = c;
    return 0;
}

int pspeektok(struct parser *parser)
{
	if(parser->stayOnToken)
	    return 0;
	parser->failOnEof = 0;
	parser->lastTokRet = psreadtok(parser);
	parser->failOnEof = 1;
	parser->stayOnToken = 1;
	return parser->lastTokRet;
}

int psskiptok(struct parser *parser)
{
	parser->stayOnToken = 0;
	return 0;
}