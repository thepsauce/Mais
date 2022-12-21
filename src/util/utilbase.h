typedef struct buf {
    char *buf;
    int len, cap;
} *Buf;

#define bufinit(b) memset((b), 0, sizeof(struct buf)) // initialize to 0
void bufinits(Buf buf, const char *str);
void bufinitsl(Buf buf, const char *str, int len);
#define bufl(b, l) ((b)->len = (l)) // set length
bool bufs(struct buf *buf, const char *str);
bool bufsl(Buf buf, const char *str, int len);
bool bufr(Buf buf, FILE *fp);
bool bufw(Buf buf, FILE *fp);
char *bufcv(Buf buf, int index, int len);
char *bufexd(Buf buf);
char *bufex(Buf buf);
int buff(Buf buf, const char *fmt, ...);
char *bufas(Buf buf, const char *apd);
char *bufasl(Buf buf, const char *apd, int len);
char *bufic(Buf buf, int index, char ch);
char *bufac(Buf buf, char ch);
char *bufis(Buf buf, int index, const char *ins);
char *bufisl(Buf buf, int index, const char *ins, int len);
char *bufris(Buf buf, int index, int rem, const char *ins, int len);
#define bufrca(b, index) bufrra((b), (index), 1)
bool bufrra(Buf buf, int fromIndex, int removeLen);
int buffc(Buf buf, char ch, int fromIndex);
int buffs(Buf buf, const char * restrict find, int fromIndex);
int buffrs(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace);

#define F32EQUAL(f1, f2, prec) (fabsf((f2) - (f1))>=prec)
#define F64EQUAL(f1, f2, prec) (fabs((f2) - (f1))>=prec)

#define ARRLEN(a) (sizeof(a)/sizeof*(a))

#define ARRFOREACH(var, array) \
	for(int _ARRFOREACH_counter = 0;  \
		_ARRFOREACH_counter < ARRLEN(array) && ((var = (array)[_ARRFOREACH_counter]) || 1); \
		_ARRFOREACH_counter++)

#define ARRSEARCH(array, target) \
({ \
	__label__ _ARRSEARCH_found; \
	__auto_type _ARRSEARCH_target = (target); \
	typeof(*(array)) *_ARRSEARCH_array = (array); \
	int _ARRSEARCH_counter, _ARRSEARCH_index; \
	for(_ARRSEARCH_counter = 0; _ARRSEARCH_counter < ARRLEN(array); _ARRSEARCH_counter++) \
		if(_ARRSEARCH_array[_ARRSEARCH_counter] == _ARRSEARCH_target) \
		{ _ARRSEARCH_index = _ARRSEARCH_counter; goto _ARRSEARCH_found; } \
	_ARRSEARCH_index = -1; \
	_ARRSEARCH_found: \
	_ARRSEARCH_index; \
})

#define ARRSTRSEARCH(array, target) \
({ \
	__label__ _ARRSTRSEARCH_found; \
	const char *_ARRSTRSEARCH_target = (target); \
	const char **_ARRSTRSEARCH_array = (array); \
	int _ARRSEARCH_counter, _ARRSEARCH_index; \
	for(_ARRSEARCH_counter = 0; _ARRSEARCH_counter < ARRLEN(array); _ARRSEARCH_counter++) \
		if(!strcmp(_ARRSTRSEARCH_array[_ARRSEARCH_counter], _ARRSTRSEARCH_target)) \
		{ _ARRSEARCH_index = _ARRSEARCH_counter; goto _ARRSTRSEARCH_found; } \
	_ARRSEARCH_index = -1; \
	_ARRSTRSEARCH_found: \
	_ARRSEARCH_index; \
})

#define DARRGROW(arr, cap, newCap) \
{ \
	if((newCap) > cap) \
	{ \
		cap *= 2; \
		cap++; \
		arr = realloc(arr, sizeof(*arr) * cap); \
	} \
}

#define DARRADD(arr, cnt, cap, elem) \
{ \
	DARRGROW(arr, cap, cnt + 1) \
	arr[cnt++] = (elem); \
}
	
#define DARRADDUNIQUE(arr, cnt, cap, elem) \
{ \
	__label__ _DARRADDUNIQUER_alreadyexists; \
	__auto_type _DARRADDUNIQUE_elem = (elem); \
	for(int i = 0; i < cnt; i++) \
		if(arr[i] == _DARRADDUNIQUE_elem) \
			goto _DARRADDUNIQUER_alreadyexists; \
	if(cnt + 1 > cap) \
	{ \
		cap *= 2; \
		cap++; \
		arr = realloc(arr, sizeof(*arr) * cap); \
	} \
	arr[cnt++] = _DARRADDUNIQUE_elem; \
	_DARRADDUNIQUER_alreadyexists:; \
}
	
#define DARRADDUNIQUER(arr, cnt, cap, elem) \
{ \
	__label__ _DARRADDUNIQUER_alreadyexists; \
	__auto_type _DARRADDUNIQUER_elem = (elem); \
	if(cnt >= 0) \
	for(__auto_type i = cnt - 1; i >= 0; i--) \
		if(arr[i] == _DARRADDUNIQUER_elem) \
			goto _DARRADDUNIQUER_alreadyexists; \
	if(cnt + 1 > cap) \
	{ \
		cap *= 2; \
		cap++; \
		arr = realloc(arr, sizeof(*arr) * cap); \
	} \
	arr[cnt++] = _DARRADDUNIQUER_elem; \
	_DARRADDUNIQUER_alreadyexists:; \
}
	
#define DARRREMOVE(arr, cnt, elem) \
{ \
	typeof(arr) _DARRREMOVE_ptr; \
	typeof(cnt) _DARRREMOVE_remain; \
	__auto_type _DARRREMOVE_elem = (elem); \
	for(_DARRREMOVE_ptr = arr, _DARRREMOVE_remain = cnt; _DARRREMOVE_remain--; _DARRREMOVE_ptr++) \
		if(*_DARRREMOVE_ptr == _DARRREMOVE_elem) \
		{ \
			memmove(_DARRREMOVE_ptr, _DARRREMOVE_ptr + 1, sizeof(*_DARRREMOVE_ptr) * _DARRREMOVE_remain); \
			cnt--; \
			break; \
		} \
}

#define DARRGROW2(arr, cnt, cap, newCap) \
{ \
	if(newCap > cap) \
    { \
		void *_DARRADD2_free; \
		typeof(arr) _DARRADD2_newArr; \
        cap++; \
        cap *= 2; \
        _DARRADD2_free = arr; \
        _DARRADD2_newArr = malloc(sizeof(*_DARRADD2_newArr) * cap); \
        if(arr) \
            memcpy(_DARRADD2_newArr, arr, sizeof(*arr) * cnt); \
        arr = _DARRADD2_newArr; \
        free(_DARRADD2_free); \
    } \
}

#define DARRADD2(arr, cnt, cap, elem) \
{ \
	__auto_type _DARRADD2_elem = (elem); \
	if(cnt + 1 > cap) \
    { \
		void *_DARRADD2_free; \
		typeof(arr) _DARRADD2_newArr; \
        cap++; \
        cap *= 2; \
        _DARRADD2_free = arr; \
        _DARRADD2_newArr = malloc(sizeof(*_DARRADD2_newArr) * cap); \
        if(arr) \
            memcpy(_DARRADD2_newArr, arr, sizeof(*arr) * cnt); \
        _DARRADD2_newArr[cnt] = _DARRADD2_elem; \
        arr = _DARRADD2_newArr; \
        cnt++; \
        free(_DARRADD2_free); \
    } \
    else \
    { \
        arr[cnt++] = _DARRADD2_elem; \
    } \
}

#define DARRREMOVE2(arr, cnt, cap, elem) \
{ \
	__auto_type _DARRREMOVE2_elem = (elem); \
	for(int i = 0; i < cnt; i++) \
        if(arr[i] == _DARRREMOVE2_elem) \
        { \
			void *_DARRREMOVE2_free; \
			typeof(arr) _DARRREMOVE2_newArr; \
            _DARRREMOVE2_newArr = malloc(sizeof(*_DARRREMOVE2_newArr) * cap); \
            memcpy(_DARRREMOVE2_newArr, scene->entities.elems, sizeof(*_DARRREMOVE2_newArr) * i); \
            memcpy(_DARRREMOVE2_newArr + i, arr + i + 1, sizeof(*_DARRREMOVE2_newArr) * (cnt - 1 - i)); \
            _DARRREMOVE2_free = arr; \
            arr = _DARRREMOVE2_newArr; \
            cnt--; \
            free(_DARRREMOVE2_free); \
			break; \
        } \
}

const char *strnum(u32 num, int radix);