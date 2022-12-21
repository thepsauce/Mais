typedef          char i8;
typedef unsigned char u8;
typedef          short i16;
typedef unsigned short u16;
typedef          int i32;
typedef unsigned int u32;
typedef          long long int i64;
typedef unsigned long long int u64;

typedef float f32;
typedef double f64;
typedef long double f128;

typedef FILE *File;

#define local
#define global
#define synchronized

#define cast(class, object) ((class) (object))
#define cmp(class, o1, o2) ((class) (o2) - (class) (o1))