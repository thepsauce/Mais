void bufinits(Buf buf, const char *str)
{
	int l = strlen(str);
	buf->buf = malloc(l);
	memcpy(buf->buf, str, l);
	buf->len = buf->cap = l;
}

void bufinitsl(Buf buf, const char *str, int len)
{
	buf->buf = malloc(len);
	memcpy(buf->buf, str, len);
	buf->len = buf->cap = len;
}

bool bufs(Buf buf, const char *str)
{
    if(!str)
    {
        free(buf->buf);
        buf->len = buf->cap = 0;
        return 0;
    }
    return bufsl(buf, str, strlen(str));
}

bool bufsl(Buf buf, const char *str, int len)
{
    if(str)
    {
        free(buf->buf);
        buf->buf = malloc(len);
        buf->len = buf->cap = len;
        memcpy(buf->buf, str, len);
        return 1;
    }
    if(!len)
    {
        free(buf->buf);
        buf->len = buf->cap = 0;
    }
    return 0;
}

char *bufexd(Buf buf)
{
    char *str;
    str = malloc(buf->len + 1);
    memcpy(str, buf->buf, buf->len);
    str[buf->len] = 0;
    free(buf->buf);
    return str;
}

char *bufex(Buf buf)
{
    char *str;
    str = malloc(buf->len + 1);
    memcpy(str, buf->buf, buf->len);
    str[buf->len] = 0;
    return str;
}

char *bufcpy(Buf buf, int index, int len)
{
    char *str;
    str = malloc(len + 1);
    memcpy(str, buf->buf + index, len);
    str[len] = 0;
    return str;
}

int bufaf(Buf buf, const char *fmt, ...)
{
	int c;
	va_list l;
	va_start(l, fmt);
    c = bufivf(buf, buf->len, fmt, l);
	va_end(l);
	return c;
}

int bufif(Buf buf, int index, const char *fmt, ...)
{
	int c;
	va_list l;
	va_start(l, fmt);
    c = bufivf(buf, index, fmt, l);
	va_end(l);
	return c;
}

int bufavf(Buf buf, const char *fmt, va_list l)
{
	return bufivf(buf, buf->len, fmt, l);
}

int bufivf(Buf buf, int index, const char *fmt, va_list l)
{
    int len;
	char *dest;
	char ch;
	
	// get length of string to insert
    // len = _vscprintf(fmt, l);
    len = vsnprintf(NULL, 0, fmt, l); // this seems to be more portable than _vscprintf
	bufgrow(buf, buf->len + len + 1);
	// move out of the way
	dest = buf->buf + index;
	memmove(dest + len, dest, buf->len - index);
	ch = dest[len];
	// insert string
    vsprintf(dest, fmt, l);
	// since vsprintf writes a null terminator, this must be overwritten again
	dest[len] = ch;
    buf->len += len;
    return len;
}

char *bufas(Buf buf, const char *apd)
{
    return bufisl(buf, buf->len, apd, strlen(apd));
}

char *bufasl(Buf buf, const char *apd, int len)
{
    return bufisl(buf, buf->len, apd, len);
}

char *bufic(Buf buf, int index, char ch)
{
    char *dest;

	bufgrow(buf, buf->len + 1);
    dest = buf->buf + index;
    memmove(dest + 1, dest, buf->len - index);
    *dest = ch;
	buf->len++;
    return dest;
}

char *bufac(Buf buf, char ch)
{
    return bufic(buf, buf->len, ch);
}

char *bufis(Buf buf, int index, const char *ins)
{
    return bufisl(buf, index, ins, strlen(ins));
}

char *bufisl(Buf buf, int index, const char *ins, int len)
{
    char *dest;

	bufgrow(buf, buf->len + len);
    dest = buf->buf + index;
    memmove(dest + len, dest, buf->len - index);
    if(ins)
        memcpy(dest, ins, len);
    else
        memset(dest, 0, len);
    buf->len += len;
    return dest;
}

char *bufris(Buf buf, int index, int rem, const char *ins)
{
	return bufrisl(buf, index, rem, ins, strlen(ins));
}

char *bufrisl(Buf buf, int index, int rem, const char *ins, int len)
{
    char *dest;
    int delta;
	
    delta = len - rem;
    if(delta > 0)
    {
		bufgrow(buf, buf->len + delta);
        dest = buf->buf + index;
        memmove(dest + delta, dest, buf->len - index);
    }
    else if(delta < 0)
    {
        dest = buf->buf + index;
        memmove(dest, dest - delta, buf->len - index);
    }
    else
    {
        dest = buf->buf + index;
    }
    if(ins)
        memcpy(dest, ins, len);
    else
        memset(dest, 0, len);
    buf->len += delta;
    return dest;
}

bool bufrra(Buf buf, int fromIndex, int removeLen)
{
    memmove(buf->buf + fromIndex, buf->buf + fromIndex + removeLen, buf->len - fromIndex - removeLen);
    buf->len -= removeLen;
    return 1;
}

int buffc(Buf buf, char ch, int fromIndex)
{
	char *str;
	int len;
	if(!buf->len)
		return -1;
	if(fromIndex < 0)
		fromIndex = 0;
	for(len = buf->len - fromIndex, str = buf->buf + fromIndex; len; len--, str++)
        if(*str == ch)
            return buf->len - len;
    return -1;
}

int buffcr(Buf buf, char ch, int fromIndex)
{
	char *str;
	int len;
	if(!buf->len)
		return -1;
	if(fromIndex < 0)
		fromIndex = buf->len - 1;
	for(len = fromIndex + 1, str = buf->buf + fromIndex; len; str--)
	{
		len--;
        if(*str == ch)
            return len;
	}
    return -1;
}

static inline int _buffsl(const char * restrict str, int len, const char * restrict find, int findLen, int fromIndex)
{
    int sLen, matchCnt;
    sLen = len;
    matchCnt = 0;
    // TODO: TEST WHAT ALGORITHM IS FASTER
    /*while(1)
    {
        while(len && *str != *find)
            len--, str++;
        while(len--, str++, match++, 1)
        {
            if(match == findLen)
                return sLen - len - match;
            if(len < 0)
                return -1;
            if(find[match] != *str)
                break;
        }
    }*/
	str += fromIndex;
	len -= fromIndex;
    while(len)
    {
		len--;
        if(*(find + matchCnt) == *str)
        {
            matchCnt++;
            if(matchCnt == findLen)
                return sLen - len - matchCnt;
        }
        else if(matchCnt)
        {
            matchCnt = *find == *str;
        }
        str++;
    }
    return -1;
}

int buffs(Buf buf, const char * restrict find, int fromIndex)
{
	return buffsl(buf, find, strlen(find), fromIndex);
}

int buffsl(Buf buf, const char * restrict find, int findLen, int fromIndex)
{
	if(fromIndex < 0)
		fromIndex = 0;
    if(!find || !findLen)
        return -1;
    return _buffsl(buf->buf, buf->len, find, findLen, fromIndex);
}

int buffsrs(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace)
{
	return buffslrsl(buf, find, strlen(find), fromIndex, replace, strlen(replace));
}

int buffsrsl(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace, int replaceLen)
{
	return buffslrsl(buf, find, strlen(find), fromIndex, replace, replaceLen);
}

int buffslrs(Buf buf, const char * restrict find, int findLen, int fromIndex, const char * restrict replace)
{
	return buffslrsl(buf, find, findLen, fromIndex, replace, strlen(replace));
}

int buffslrsl(Buf buf, const char * restrict find, int findLen, int fromIndex, const char * restrict replace, int replaceLen)
{
    char *dest;
    int aLen;
    int index;
    if(!find || !findLen)
        return -1;
	if(fromIndex < 0)
		fromIndex = 0;
    if((index = _buffsl(buf->buf, buf->len, find, findLen, fromIndex)) < 0)
        return -1;
    aLen = findLen - replaceLen;
	bufgrow(buf, buf->len - aLen);
    dest = buf->buf + index;
    memmove(dest, dest + aLen, buf->len - index - aLen);
    memcpy(dest, replace, replaceLen);
    buf->len -= aLen;
    return index;
}
