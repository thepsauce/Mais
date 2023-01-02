typedef  int8_t   i8;
typedef uint8_t   u8;
typedef  int16_t i16;
typedef uint16_t u16;
typedef  int32_t i32;
typedef uint32_t u32;
typedef  int64_t i64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;
typedef long double f128;

typedef FILE *File;

#define local
#define global
#define synchronized
#define unused __attribute__((unused))

#define cast(class, object) ((class) (object))
#define cmp(class, o1, o2) ((class) (o2) - (class) (o1))
