#include "buf.h"

global char StrNumBuf[33];

global int strnumlen(void)
{
    return strlen(StrNumBuf);
}

global const char *strnum(u32 num, int radix)
{
    char ch;
	char *buf;
	buf = StrNumBuf + sizeof(StrNumBuf) - 2;
    if(!num)
		*buf = '0';
    else
    {
		*buf = 0;
        while(num)
        {
            buf--;
            switch(radix)
            {
            case 2:
                *buf = (num & 0x1) + '0';
                num >>= 1;
                break;
            case 8:
                *buf = (num & 0x7) + '0';
                num >>= 3;
                break;
            case 10:
                *buf = (num % 10) + '0';
                num /= 10;
                break;
            case 16:
                ch = num & 0xF;
                if(ch >= 0xA)
                    ch += 'A' - 0xA;
                else
                    ch += '0';
                *buf = ch;
                num >>= 4;
                break;
            }
        }
    }
    return buf;
}

global int throwerror(File file, int pos, const char *err, ...)
{
    int line = 1;
    int col = 1;
    int oldPos;
    oldPos = ftell(file);
    fseek(file, 0, SEEK_SET);
    while(pos != ftell(file))
        if(fgetc(file) == '\n')
        {
            line++;
            col = 1;
        }
        else
            col++;
    printf("error at line %d, %d: ", line, col);
    va_list args;
    va_start(args, err);
    vprintf(err, args);
    va_end(args);
    exit(-1);
    fseek(file, oldPos, SEEK_SET);
    return -1;
}