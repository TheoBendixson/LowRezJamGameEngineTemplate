#include <stdint.h> 
#include <stddef.h>

#if SLOW 
#define Assert(Expression) if(!(Expression)) { __builtin_trap(); }
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef size_t memory_index;

typedef int8_t b8;
typedef int16_t b16;
typedef int32_t b32;

typedef float r32;
typedef double r64;
