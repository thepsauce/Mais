struct token;
struct datapoint;
struct type;
struct label;
struct group;

enum {
    TOKEOF,
    TOKNUMBER,
    TOKSTRING,
    TOKWORD,
    TOKANY,
    TOKKEYWORD,
};

enum {
	OPRLSH = 0x100,
	OPRRSH,
	OPRNOT
};

struct token {
    u32 type;
    long pos;
    union {
        char word[1024];
        struct {
            char *str;
            u32 strLen;
        };
        i64 l;
        int c;
    };
};

struct parser {
	// console information
#ifdef __WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hOut;
#else

#endif

	File file;
	char *fileName;
	struct token tok;
	struct token unTok;
	char buf[1024];
	int nBuf, iBuf;
	long fOff;
	bool stayOnToken; // if this is true, psreadtok doesn't do anything
	int lastTokRet; // last return value of 'pspeektok'
	bool failOnEof; // if this is true and psreadtok would return eof, an error is returned
	
	int *scopes;
	int scopeCnt, scopeCap;
	struct label *labels;
	int labelCnt, labelCap;
};

int psinit(struct parser *parser);
int psdiscard(struct parser *parser);
int psopen(struct parser *parser, const char *file);
int psclose(struct parser *parser);
int pspeek(struct parser *parser);
int psskip(struct parser *parser);
int psread(struct parser *parser);
long pstell(struct parser *parser);
int psreadtok(struct parser *parser);
int pspeektok(struct parser *parser);
int psthrow(struct parser *parser, const char *format, ...);
int pswarn(struct parser *parser, const char *format, ...);
int psaddlabel(struct parser *parser, const struct label *label, bool push);
int psaddgroup(struct parser *parser, const struct group *group);

const char *Keywords[] = {
    "db", "dw", "dd", "dq", "dt", "dr", "df",
    "typedef", "local", "readonly", "times", "this", "sizeof", // 13
    "exit", "test", "jmp", "jz", "jnz", "je", "jne", "jl", "jle", "jg", "jge",
    "ret", "re", "rne", "rl", "rle", "rg", "rge",
    "inc", "dec", "not", "and", "or", "xor", "lsh", "rsh",
    "add", "sub", "mul", "div", "mod", "neg",
    "fadd", "fsub", "fmul", "fdiv", "fmod", "fneg",
    "mov", "call"
};

#define tokischar(tok, _c) (((tok)->type==TOKANY)&&(tok)->c==_c)

#define DATATIMES 1
#define DATANUMBER 2
#define DATANOC 3
#define DATAARRAY 4
#define DATASTRING 5

struct datapoint {
	long pos;
    u32 flags;
    struct datapoint *next;
    union {
        struct {
            struct datapoint *point;
            u32 times;
        };
        i64 number;
        struct {
            char *str;
            int strLen;
        };
        struct {
            char *resource;
            int resourceLen;
        };
    };
};

#define TYPEBYTE 1
#define TYPEWORD 2
#define TYPEDOUBLE 3
#define TYPEQUAD 4
#define TYPESTRUCT 5
#define TYPECLASS 6
#define TYPEGENERIC 7
#define TYPEFUNC 8
#define TYPERESOURCE 9

#define TYPEREADONLY 0x100
#define TYPESTATIC 0x200
struct type {
	u32 flags;
	u32 depth;
	char *genericName; // for <[genericName]>
	char *name; // for dt [name]
	struct type *generic; // for dt [name]<[generic]>
	struct label *params; // for df([params, paramCnt]) [ret]
	int paramCnt; // for df([params, paramCnt]) [ret]
	struct type *ret; // for df([params, paramCnt]) [ret]
};

#define LABELTYPE(label) ((label)->flags&0xFF)
	
#define LABELVAR 1
#define LABELFUNC 2
#define LABELSTRUCT 3
#define LABELCLASS 4
#define LABELGOTO 5
struct label {
	long pos;
	char *name;
	u32 flags;
	char *generic;
	char *extends;
	char *extendGeneric;
	struct type type;
	struct datapoint data;
	struct label *children;
	int childCnt;
	struct group *groups;
	int groupCnt;
	int jump; // index of group to jump to
	// function
    struct label *params;
	int paramCnt;
	struct type *ret;
	char *resource;
	int resourceLen;
};

struct label *lbcreate(const struct label *src);

enum {
    GRNULL,
    GRNUMBER, // integer
    GRSTRING, // "str"
    GRNOC, // ?
    GRACCESS, // [a]
    GRACCESS2, // a[b]
    GRLOPR, // !a, !!, ~, -
    GRMOPR, // a+b, -, *, /, %, &, |, ^, <<, >>
    GRLABEL, // label, other.name, deep.deeper.name
    GRTYPE, // db, dw, [dt A], dt B<db> etc.
    GRTHIS,
    GRSIZEOFEXPR,
    GRSIZEOFTYPE,

    GRTEST,
    GRJMP,
    GRJZ,
    GRJNZ,
    GRJE,
    GRJNE,
    GRJL,
    GRJLE,
    GRJG,
    GRJGE,
    GRRET,
    GRRE,
    GRRNE,
    GRRL,
    GRRLE,
    GRRG,
    GRRGE,
    GRINC,
    GRDEC,
    GRNOT,
    GRAND,
    GROR,
    GRXOR,
    GRLSH,
    GRRSH,
    GRADD,
    GRSUB,
    GRMUL,
    GRDIV,
    GRMOD,
    GRNEG,
    GRFADD,
    GRFSUB,
    GRFMUL,
    GRFDIV,
    GRFMOD,
    GRFNEG,
    GRMOV,
    GRCALL, // calling(args)
    GRINPUT,
    GROUTPUT,
};

#define GRTYPE(gr) ((gr)->flags&0xFF)

struct group {
    u32 flags;
    long pos;
    union {
        i64 num;
        struct {
            char *str;
            int strLen;
        };
        struct { // call
            struct group *args;
            int argCnt;
            char *calling;
            struct group *into;
        };
        struct {
            struct group *lopr;
            short opr;
            struct group *ropr;
        };
        struct { // access
            struct group *accessing;
            struct group *offset;
        };
        char *label;
        struct group *arg;
        struct type type;
    };
};

struct group *grcreate(const struct group *src);