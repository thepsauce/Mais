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
    else if(buf->cap < len)
    {
        free(buf->buf);
        buf->cap = len;
        buf->len = 0;
        buf->buf = malloc(len);
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

char *bufcv(Buf buf, int index, int len)
{
    char *str;
    str = malloc(len + 1);
    memcpy(str, buf->buf + index, len);
    str[len] = 0;
    return str;
}


int buff(Buf buf, const char *fmt, ...)
{
    int len;

    va_list l;
    va_start(l, fmt);
    len = _vscprintf(fmt, l);
    va_end(l);

    if(buf->len + len + 1 > buf->cap)
    {
        buf->cap *= 2;
        buf->cap += len + 1; // plus one because vsprinf also writes a null terminator
        buf->buf = realloc(buf->buf, buf->cap);
    }

    va_start(l, fmt);
    vsprintf(buf->buf + buf->len, fmt, l);
    va_end(l);

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
    char *txt;
    int bufLen;
    bufLen = buf->len++;
    if(buf->len > buf->cap)
    {
        buf->cap *= 2;
        buf->cap++;
        buf->buf = realloc(buf->buf, buf->cap);
    }
    txt = buf->buf + index;
    memmove(txt + 1, txt, bufLen - index);
    *txt = ch;
    return txt;
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
    char *txt;
    int bufLen;
    bufLen = buf->len;
    buf->len += len;
    if(buf->len > buf->cap)
    {
        buf->cap *= 2;
        buf->cap += len;
        buf->buf = realloc(buf->buf, buf->cap);
    }
    txt = buf->buf + index;
    memmove(txt + len, txt, bufLen - index);
    if(ins)
        memcpy(txt, ins, len);
    else
        memset(txt, 0, len);
    return txt;
}

char *bufris(Buf buf, int index, int rem, const char *ins)
{
	return bufrisl(buf, index, rem, ins, strlen(ins));
}

char *bufrisl(Buf buf, int index, int rem, const char *ins, int len)
{
    char *txt;
    int bufLen;
    int delta;
    delta = len - rem;
    bufLen = buf->len;
    buf->len += delta;
    if(delta > 0)
    {
        if(buf->len > buf->cap)
        {
            buf->cap *= 2;
            buf->cap += delta;
            buf->buf = realloc(buf->buf, buf->cap);
        }
        txt = buf->buf + index;
        memmove(txt + delta, txt, bufLen - index);
    }
    else if(delta < 0)
    {
        txt = buf->buf + index;
        memmove(txt, txt - delta, bufLen - index);
    }
    else
    {
        txt = buf->buf + index;
    }
    if(ins)
        memcpy(txt, ins, len);
    else
        memset(txt, 0, len);
    return txt;
}

bool bufrra(Buf buf, int fromIndex, int removeLen)
{
    memmove(buf->buf + fromIndex, buf->buf + fromIndex + removeLen, buf->len - fromIndex - removeLen);
    buf->len -= removeLen;
    return 1;
}

int buffc(Buf buf, char ch, int fromIndex)
{
    char *str = buf->buf + fromIndex;
    int len = buf->len - fromIndex;
    while(len--)
        if(*(str++) == ch)
            return buf->len - len - 1;
    return -1;
}

int buffcr(Buf buf, char ch, int fromIndex)
{
    char *str = buf->buf + fromIndex;
    int len = fromIndex;
    while(len--)
        if(*(str--) == ch)
            return buf->len - len - 1;
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
    while(len--)
    {
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
	return buffs(buf, find, strlen(find), fromIndex);
}

int buffsl(Buf buf, const char * restrict find, int findLen, int fromIndex)
{
    if(!find || !findLen)
        return -1;
    return _buffs(buf->buf, buf->len, find, findLen, fromIndex);
}

int buffrs(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace)
{
	return buffrsl(buf, find, strlen(find), fromIndex, replace, strlen(replace));
}

int buffrsl(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace, int replaceLen)
{
	return buffrsl(buf, find, strlen(find), fromIndex, replace, replaceLen);
}

int bufflrs(Buf buf, const char * restrict find, int findLen, int fromIndex, const char * restrict replace)
{
	return buffrsl(buf, find, findLen, fromIndex, replace);
}

int buffrsl(Buf buf, const char * restrict find, int findLen, int fromIndex, const char * restrict replace, int replaceLen)
{
    char *txt;
    int aLen;
    int index;
    if(!find || !findLen)
        return -1;
    if((index = _buffs(buf->buf, buf->len, find, findLen, fromIndex)) < 0)
        return -1;
    rLen = strlen(replace);
    aLen = findLen - replaceLen;
    txt = buf->buf + index;
    memmove(txt, txt + aLen, buf->len - index - aLen);
    memcpy(txt, replace, replaceLen);
    buf->len -= aLen;
    return index;
}
