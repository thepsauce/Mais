global void *arrcreate(u32 initCap, u32 elemSize)
{
	struct array *arr = malloc(sizeof(*arr) + elemSize * initCap);
	arr->cap = initCap;
	arr->cnt = 0;
	arr->size = elemSize;
	return (void*) arr + sizeof(*arr);
}

global u32 arrcount(void *ptr)
{
	struct array *arr = ptr - sizeof(*arr);
	return arr->cnt;
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
	if(arr->cnt + 1 > arr->cap)
	{
		arr->cap *= 2;
		arr->cap++;
		arr = realloc(arr, sizeof(*arr) + arr->size * arr->cap);
	}
	ptr = (void*) arr + sizeof(*arr);
	if(elem)
		memcpy(ptr + arr->size * arr->cnt, elem, arr->size);
	arr->cnt++;
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
	memmove(remElem, remElem + arr->size, arr->size * (arr->cnt - index));
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