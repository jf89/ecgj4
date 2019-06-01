#pragma once

#include <stdint.h>

#define APPLICATION_NAME "Influence"
#define MAX_BASE_PATH_LENGTH 1024

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 700

typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef u8 bool;

#define V(type) \
	typedef struct { \
		union { type x; type r; type w; }; \
		union { type y; type g; type h; }; \
	} v2_ ## type; \
	typedef struct { \
		union { type x; type r; type w; }; \
		union { type y; type g; type h; }; \
		union { type z; type b; type d; }; \
	} v3_ ## type; \
	typedef struct { \
		union { type x; type r; }; \
		union { type y; type g; }; \
		union { type z; type b; }; \
		union { type w; type a; }; \
	} v4_ ## type;

V(i8) V(u8) V(i16) V(u16) V(i32) V(u32) V(i64) V(u64) V(f32) V(f64)
typedef v2_f32 v2;
typedef v3_f32 v3;
typedef v4_f32 v4;

#undef V

#define M(type) \
	typedef union { \
		type elems[16]; \
		type row_col[4][4]; \
	} m4_ ## type; \
	typedef union { \
		type elems[12]; \
		type row_col[4][3]; \
	} m4x3_ ## type;

M(i8) M(u8) M(i16) M(u16) M(i32) M(u32) M(i64) M(u64) M(f32) M(f64)

typedef m4_f32 m4;
typedef m4x3_f32 m4x3;

#undef M

#define M4(type, A1, B1, C1, D1, A2, B2, C2, D2, A3, B3, C3, D3, A4, B4, C4, D4, ...) \
	(m4_ ## type) { .elems = { \
		A1, A2, A3, A4, \
		B1, B2, B3, B4, \
		C1, C2, C3, C4, \
		D1, D2, D3, D4, \
	} }

#define TRUE  1
#define FALSE 0

#define MEMBER_SIZE(type, member) sizeof(((type *)0)->member)
#define ARRAY_LENGTH(xs) (sizeof(xs) / sizeof((xs)[0]))
#define MIN(x, y) ({ typeof(x) _x = (x); typeof(y) _y = (y); _x < _y ? _x : _y; })
#define MAX(x, y) ({ typeof(x) _x = (x); typeof(y) _y = (y); _x > _y ? _x : _y; })
#define CLAMP(v, min, max) MAX(min, MIN(max, v))
#define SWAP(x, y) ({ typeof(x) tmp = x; x = y; y = tmp; })

#ifdef DEBUG_DEFER
#define DEFER__(num, block) \
	void __defer_func_ ## num() { \
		SDL_Log("Executing defer function: '" #block "'"); \
		block \
	} \
	i8 __attribute__((cleanup(__defer_func_ ## num), unused)) __defer_var_ ## num;
#else
#define DEFER__(num, block) \
	void __defer_func_ ## num() { block; } \
	i8 __attribute__((cleanup(__defer_func_ ## num), unused)) __defer_var_ ## num;
#endif
#define DEFER_(num, block) DEFER__(num, block)
#define DEFER(block) DEFER_(__COUNTER__, block)

