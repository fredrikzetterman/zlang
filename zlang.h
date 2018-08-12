#ifndef ZLANG_H_
#define ZLANG_H_

#if defined _MSC_VER
#define Z_DBGBRK() __debugbreak()
#define Z_ASSERT_WARNING_DISABLE
#define Z_ASSERT_WARNING_ENABLE
#else
#define Z_DBGBRK() __builtin_debugtrap()
#define Z_ASSERT_WARNING_DISABLE \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wstring-conversion\"")
#define Z_ASSERT_WARNING_ENABLE \
_Pragma("clang diagnostic pop")
#endif

#ifdef __cplusplus
extern "C" {
#endif
unsigned int
assert_handler( const char* test, const char* file, int line, const char* fmt, ... );
#ifdef __cplusplus
}
#endif

#define ASSERTX(cond, fmt, ...)\
do {\
  Z_ASSERT_WARNING_DISABLE \
  if (!(cond)) {\
    if (assert_handler(#cond, __FILE__, __LINE__, fmt, __VA_ARGS__)){\
      Z_DBGBRK(); \
    } \
  } \
  Z_ASSERT_WARNING_ENABLE \
} while(0)

#define ASSERT(cond)\
do {\
  Z_ASSERT_WARNING_DISABLE \
  if (!(cond)) {\
    if (assert_handler(#cond, __FILE__, __LINE__, "" )){\
      Z_DBGBRK(); \
    } \
  } \
  Z_ASSERT_WARNING_ENABLE \
} while(0)

#endif

