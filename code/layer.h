#include <stdint.h>

#define internal static
#define global static
#define local_persist static

#define MemorySet memset
#define MemoryMove memmove
#define MemoryCopy memcpy

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef i8 s8;
typedef i16 s16;
typedef i32 s32;
typedef i64 s64;

typedef float f32;
typedef double f64;

