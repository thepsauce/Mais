global void *arrcreate(u32 initCap, u32 elemSize)
{
	struct array *arr = malloc(sizeof(*arr) + elemSize * initCap);
	arr->cap = initCap;
	arr->cnt = 0;
	arr->cursor = 0;
	arr->size = elemSize;
	return (void*) arr + sizeof(*arr);
}

global u32 arrcount(void *ptr)
{
	struct array *arr = ptr - sizeof(*arr);
	return arr->cnt;
}

global u32 arrmovecursor(void *ptr, u32 pos)
{
	struct array *arr = ptr - sizeof(*arr);
	u32 oldPos;
	oldPos = arr->cursor;
	arr->cursor = pos;
	return oldPos;
}

global void *arrgrow(void *ptr, u32 cap)
{
	struct array *arr = ptr - sizeof(*arr);
	if(cap > arr->cap)
	{
		arr->cap = cap;
		arr = realloc(arr, sizeof(*arr) + arr->size * arr->cap);
	}
	return (void*) arr + sizeof(*arr);
}

global void *arradd(void *ptr, const void *elem)
{
	struct array *arr = ptr - sizeof(*arr);
	struct array *newArr;
	void *dest;
	if(arr->cnt + 1 > arr->cap)
	{
		arr->cap *= 2;
		arr->cap++;
		newArr = malloc(sizeof(*arr) + arr->size * arr->cap);
		// copy array header and elements up until the cursor
		memcpy(newArr, arr, sizeof(*arr) + arr->size * arr->cursor);
		// copy the new element
		dest = (void*) newArr + sizeof(*arr) + arr->size * arr->cursor; 
		memcpy(dest, elem, arr->size);
		// copy tail of previous array to new array
		memcpy(dest + arr->size, 
			ptr + arr->size * arr->cursor,
			arr->size * (arr->cnt - arr->cursor));
		// free unsused memory
		free(arr);
		
		arr = newArr;
		ptr = (void*) newArr + sizeof(*newArr);
	}
	else
	{
		dest = ptr + arr->size * arr->cursor; 
		memmove(dest + arr->size, dest, arr->size * (arr->cnt - arr->cursor));
		memcpy(dest, elem, arr->size);
	}
	arr->cnt++;
	arr->cursor++;
	return ptr;
}

global void *arrremove(void *ptr, u32 index)
{
	struct array *arr = ptr - sizeof(*arr);
	void *arrEnd;
	void *remElem;
	u8 cpyElem[arr->size]; // the removed element will be put at the end of the array and returned so that it can be deleted or other
	remElem = ptr + arr->size * index;
	memcpy(cpyElem, remElem, arr->size);
	arr->cnt--;
	if(arr->cursor > arr->cnt)
		arr->cursor = arr->cnt - 1;
	memmove(remElem, remElem + arr->size, arr->size * (arr->cnt - index));
	arrEnd = ptr + arr->size * arr->cnt;
	memcpy(arrEnd, cpyElem, arr->size);
	return arrEnd;
}

global void *arrpush(void *ptr, const void *elem)
{
	return arradd(ptr, elem);
}

global void *arrpeek(void *ptr)
{
	struct array *arr = ptr - sizeof(*arr);
	if(!arr->cursor)
		return arr->cnt ? ptr : NULL;
    return ptr + arr->size * (arr->cursor - 1);
}

global void *arrpop(void *ptr)
{
	struct array *arr = ptr - sizeof(*arr);
	void *arrEnd;
	void *remElem;
	if(!arr->cnt)
		return NULL;
	u8 cpyElem[arr->size]; // the removed element will be put at the end of the array and returned so that it can be deleted or other
	if(arr->cursor)
		arr->cursor--;
	remElem = ptr + arr->size * arr->cursor;
	memcpy(cpyElem, remElem, arr->size);
	arr->cnt--;
	memmove(remElem, remElem + arr->size, arr->size * (arr->cnt - arr->cursor));
	arrEnd = ptr + arr->size * arr->cnt;
	memcpy(arrEnd, cpyElem, arr->size);
	return arrEnd;
}

global void *arrfind(void *ptr, const void *elem, comparator compare)
{
	struct array *arr = ptr - sizeof(*arr);
	void *p;
	u32 c;
	for(c = arr->cnt, p = ptr; c; c--, p += arr->size)
		if(!compare(p, elem))
			return p;
	return NULL;
}

global void *arrfindptr(void *ptr, const void *elem)
{
	struct array *arr = ptr - sizeof(*arr);
	void *p;
	u32 c;
	for(c = arr->cnt, p = ptr; c; c--, p += sizeof(void*))
		if(*(void**) p == elem)
			return p;
	return NULL;
}

global void *arrfindstr(void *ptr, const char *str)
{
	struct array *arr = ptr - sizeof(*arr);
	char *s;
	u32 c;
	for(c = arr->cnt, s = ptr; c; c--, s++)
		if(!strcmp(s, str))
			return s;
	return NULL;
}

global void *arrfindmem(void *ptr, const void *elem)
{
	struct array *arr = ptr - sizeof(*arr);
	void *p;
	u32 c;
	for(c = arr->cnt, p = ptr; c; c--, p += arr->size)
		if(!memcmp(p, elem, arr->size))
			return p;
	return NULL;
}

global void arrsort(void *ptr, int (*compare)(const void*, const void*))
{
	struct array *arr = ptr - sizeof(*arr);
	qsort(ptr, arr->cnt, arr->size, compare);
}

global void *arrdup(void *ptr)
{
	struct array *arr = ptr - sizeof(*arr);
	struct array *dup = malloc(sizeof(*arr) + arr->size * arr->cap);
	memcpy(dup, arr, sizeof(*arr) + arr->size * arr->cap);
	return (void*) dup + sizeof(*arr);
}

global void *arrget(void *ptr, u32 index)
{
	struct array *arr = ptr - sizeof(*arr);
	return ptr + arr->size * index;
}

global void arrfree(void *ptr)
{
	free(ptr - sizeof(struct array));
}
