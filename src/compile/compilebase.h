struct token;
struct datapoint;
struct type;
struct label;
struct group;

enum {
    TOKEOF, // end of file
    TOKNUMBER, // 0x3123 0 123 8783 0b10010 0c2561
    TOKSTRING, // "Hello there" "Banana"
    TOKWORD, // word var func tree whatever
    TOKANY, // . , ; : - > ( ) [ ] + * / % & | ^ = < > { } ! ? ~
    TOKKEYWORD, // see list of keywords below
};

enum {
	OPRLSH = 0x100, // <<
	OPRRSH, // >>
	OPRNOT // !!
};

struct token {
    u32 type; // type of token, check above for possible values
    long pos; // position of this token inside the file (important for returning errors)
    union { 
        char word[1024]; // TOKWORD - we limit the maximum size of a word to 1024 bytes, anyone who goes over this limit is just crazy
        struct { // TOKSTRING - variable length, this go on a little longer
            char *str;
            u32 strLen;
        };
        i64 l; // TOKNUMBER
        int c; // TOKANY
    };
};

struct parser {
	// console information
#ifdef __WIN32 // just stuff for windows to produce colored text output
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hOut;
#else

#endif
	File file; // the file we are working on
	char *fileName; // the name of that file
	struct token tok; // current token
	char buf[1024]; // io file buffer
	int nBuf; // right index of io cursor
	int iBuf; // left index of io cursor
	long fOff; // offset of buf inside the actual file
	bool stayOnToken; // if this is true, psreadtok doesn't do anything (set on a previous call
	int lastTokRet; // last return value of 'pspeektok'
	bool failOnEof; // if this is true and psreadtok would return eof, an error is returned
	
	int *scopes; // this array stores indexes in the labels array, this way, we only need to keep one label array
	struct label *labels; // this array stores all labels like variables, functions, typedefs and gotos
};

// initializes the parser struct
int psinit(struct parser *parser);
// frees all resources associated to this parser struct, note that this doesn't close the file stream
int psdiscard(struct parser *parser);
// opens a file
int psopen(struct parser *parser, const char *file);
// closes last opened file
int psclose(struct parser *parser);
// returns the next character in the buffer but without moving the cursor
// or EOF if the end of the file has been reached
int pspeek(struct parser *parser);
// moves the cursor to the next position inside the buffer
int psskip(struct parser *parser);
// returns the next character inside the buffer and moves the cursor one over
// or EOF if the end of the file has been reached
int psread(struct parser *parser);
// returns the current position of the cursor inside the file
long pstell(struct parser *parser);
// reads in the next token and returns 0 on success or EOF
int psreadtok(struct parser *parser);
// reads in the next token and keeps it for another call to psreadtok (it is kind of a fake peek)
int pspeektok(struct parser *parser);
// throws an error
int psthrow(struct parser *parser, const char *format, ...);
// adds a new label to this parser, if push is true, this label is also pushed to the scope stack
int psaddlabel(struct parser *parser, const struct label *label, bool push);
// adds a new group to this parser
int psaddgroup(struct parser *parser, const struct group *group);

const char *Keywords[] = {
    "db", "dw", "dd", "dq", "dt", "dr", "df",
    "typedef", "local", "readonly", "times", "this", "sizeof",
    "exit", "test", "jmp", "jz", "jnz", "je", "jne", "jl", "jle", "jg", "jge",
    "ret", "re", "rne", "rl", "rle", "rg", "rge",
    "inc", "dec", "not", "and", "or", "xor", "lsh", "rsh",
    "add", "sub", "mul", "div", "mod", "neg",
    "fadd", "fsub", "fmul", "fdiv", "fmod", "fneg",
    "mov", "call"
};

#define tokischar(tok, _c) (((tok)->type==TOKANY)&&(tok)->c==_c)

// will add comments to the following later...
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
	struct label *params; // array for df([params, paramCnt]) [ret]
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
