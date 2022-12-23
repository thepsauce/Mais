/* Array utility */
struct array {
	u32 cap, cnt, size, cursor;
	u8 elems[0];
};

typedef int (*comparator)(const void*, const void*);

/* array example(int array):
int *array;
int *result;
int *rem;
int index;

array = arrcreate(10, sizeof*array);
// note that we have to say array = ... because the pointer might change due to realloc but if you are sure that your array has enough capacity, you may omit it
array = arradd(array, 22);
arradd(array, 41);
arradd(array, 0);
result = arrfindmem(array, &(int) {  41 });
if(result)
{
	index = result - array;
	printf("Index of element %d: %d\n", *result, index);
	rem = arrremove(array, index);
	printf("Now removed element %d\n", *rem);
}
arrfree(array);
*/
// creates a new array
void *arrcreate(u32 initCap, u32 elemSize);
// returns the number of elements
#define arrcount(ptr) (((struct array*) ((void*) ptr - sizeof(struct array)))->cnt)
// returns capacity
#define arrcapacity(ptr) (((struct array*) ((void*) ptr - sizeof(struct array))))->cap)
// returns cursor
#define arrcursor(ptr) (((struct array*) ((void*) ptr - sizeof(struct array))))->cursor)
// grows to given capacity
void *arrgrow(void *ptr, u32 cap);
// moves to the cursor, returns the old cursor position
u32 arrmovecursor(void *ptr, u32 cursor);
// adds a new element at the cursor position and moves the cursor on to the right
void *arradd(void *ptr, const void *elem);
// removes an element at given index, if the cursor is outside the array, it moves it accordingly
void *arrremove(void *ptr, u32 index);
// same as arradd
void *arrpush(void *ptr, const void *elem);
// returns the element that is at the cursor or NULL if there are no elements
void *arrpeek(void *ptr);
// returns the element that is at the cursor or NULL if there are no elements, it also removes that element
void *arrpop(void *ptr);
// finds given element and using given compare function
void *arrfind(void *ptr, const void *elem, comparator compare);
// finds a pointer by comparing the address of each pointer element with the given element like this:
// if(*(void**) p == elem) 
// it assumes that the pointer holds values of type void*
void *arrfindptr(void *ptr, const void *elem);
// finds given string
// if(!strcmp((char*) p, str))
void *arrfindstr(void *ptr, const char *str);
// finds given memory
// if(!memcmp(p, elem, elemSize))
void *arrfindmem(void *ptr, const void *elem);
// sorts the array, following given compare function
void arrsort(void *ptr, comparator compare);
// duplicates the array
void *arrdup(void *ptr);
void *arrget(void *ptr, u32 index);
// frees the array
void arrfree(void *ptr);

/* Bool256 */
typedef struct bool256 { // warning: bold assumption that u16 is in fact 2 bytes
	u16 bits[256 / 8 / sizeof(u16)];
} bool256;

#define B256CHECK(b256, b) ({u8 _CHECK_b=(u8)(b); (b256).bits[_CHECK_b>>4]&(1<<(_CHECK_b&0xF));})
#define B256SET(b256, b) ({u8 _SET_b=(u8)(b); (b256).bits[_SET_b>>4]|=(1<<(_SET_b&0xF));})
#define B256TOGGLE(b256, b) ({u8 _TOGGLE_b=(u8)(b); (b256).bits[_TOGGLE_b>>4]^=(1<<(_TOGGLE_b&0xF));})

/* Buffer utility */
typedef struct buf {
    char *buf;
    int len, cap;
} *Buf;

// note: a function in many cases ends in either s or sl:
//   s - which stands for null-terminated string
//   sl - stands for string with length len

// memset buffer to 0
#define bufinit(b) memset((b), 0, sizeof(struct buf))
// initialize buffer not to 0 but any null-terminated string
void bufinits(Buf buf, const char *str);
// initialize buffer to any string of length len
void bufinitsl(Buf buf, const char *str, int len);
// important: all the following buffer functions should only be used after the buffer was initialized
// set length of buffer
#define bufl(b, l) ((b)->len = (l))
// sets buffer to a string
bool bufs(Buf buf, const char *str);
bool bufsl(Buf buf, const char *str, int len);
// allocates a string of length len and copies the region from index to index + len into that string and returns it
char *bufcv(Buf buf, int index, int len);
// extracts this buffer as null-terminated string and frees the buffer, note that after this, the buffer has to be re-initialized if used any further
char *bufexd(Buf buf);
// extracts this buffer as null-terminated string but doesn't free it
char *bufex(Buf buf);
// appends the string with given format and var args, similar to sprintf
int buff(Buf buf, const char *fmt, ...);
// appends a string
char *bufas(Buf buf, const char *apd);
char *bufasl(Buf buf, const char *apd, int len);
// inserts a char at given index
char *bufic(Buf buf, int index, char ch);
// appends a char
char *bufac(Buf buf, char ch);
// inserts a string
char *bufis(Buf buf, int index, const char *ins);
char *bufisl(Buf buf, int index, const char *ins, int len);
// removes the section from index to index+rem and replaces it with given string
char *bufris(Buf buf, int index, int rem, const char *ins);
char *bufrisl(Buf buf, int index, int rem, const char *ins, int len);
// removes char at given index
#define bufrca(b, index) bufrra((b), (index), 1)
// removes range from fromIndex to fromIndex+removeLen
bool bufrra(Buf buf, int fromIndex, int removeLen);
// finds given char, starting from the index
int buffc(Buf buf, char ch, int fromIndex);
// does the same as buffc but searching in reverse
int buffcr(Buf buf, char ch, int fromIndex);
// finds given string inside this buffer
int buffs(Buf buf, const char * restrict find, int fromIndex);
int buffsl(Buf buf, const char * restrict find, int findLen, int fromIndex);
// finds given string inside the buffer and replaces it with given string
int buffrs(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace);
int buffrsl(Buf buf, const char * restrict find, int fromIndex, const char * restrict replace, int replaceLen);
int bufflrs(Buf buf, const char * restrict find, int findLen, int fromIndex, const char * restrict replace);
int bufflrsl(Buf buf, const char * restrict find, int findLen, int fromIndex, const char * restrict replace, int replaceLen);

// returns whether two floats are equal, prec is the precision used
#define F32EQUAL(f1, f2, prec) (fabsf((f2) - (f1))>=prec)
#define F64EQUAL(f1, f2, prec) (fabs((f2) - (f1))>=prec)

/* Utility for static array */
// length of static array
#define ARRLEN(a) (sizeof(a)/sizeof*(a))
// for each header for static array
// usage example:
// int arr[5];
// int v;
// ARRFOREACH(v, arr)
// {
//    printf("%d\n", v);
// }
#define ARRFOREACH(var, array) \
	for(int _ARRFOREACH_counter = 0;  \
		_ARRFOREACH_counter < ARRLEN(array) && ((var = (array)[_ARRFOREACH_counter]) || 1); \
		_ARRFOREACH_counter++)
// searches for a target value inside static array
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
// searches for string inside given static array
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

const char *strnum(u32 num, int radix);
