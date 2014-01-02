/* !! detect signed type */
/* !! subtraction? */
/* !! works if signed types are allowed to overflow (not in C), needs gcc
 *    -fwarpv  or -fno-strict-overflow */
/* !! https://www.securecoding.cert.org/confluence/display/seccode/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow?showComments=false */
/* !! http://p99.gforge.inria.fr/p99-html/ */

/* !! ({ and __typeof__ are gcc-specific */
/* !! clang? */

#undef MAX_INT_SIZE
#if __SIZEOF_INT128__ >= 16 || ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) && __SIZEOF_SIZE_T__ >= 8)
#define TO_UNSIGNED_LOW(x) ( \
    __builtin_choose_expr(sizeof(x) <= sizeof(int), (unsigned)(x), \
    __builtin_choose_expr(sizeof(x) <= sizeof(long long), (unsigned long long)(x), \
    __builtin_choose_expr(sizeof(x) <= sizeof(__int128_t), (__uint128_t)(x), \
    (void)0))))  /* Compile-time error when assigned to something. */
#define MAX_INT_SIZE (sizeof(long long) > sizeof(__int128_t) ? sizeof(long long) : sizeof(__int128_t))
#else
#define TO_UNSIGNED_LOW(x) ( \
    __builtin_choose_expr(sizeof(x) <= sizeof(int), (unsigned)(x), \
    __builtin_choose_expr(sizeof(x) <= sizeof(long long), (unsigned long long)(x), \
    (void)0)))  /* Compile-time error when assigned to something. */
#define MAX_INT_SIZE (sizeof(long long))
#endif
/* Converts to the the corresponding (or a bit larger) unsigned type.
 */
#define to_unsigned(x) TO_UNSIGNED_LOW(x)

/* Doesn't evaluate x. Returns (int)0 or (int)1 indicating whether the value or
 * type x is unsigned.
 */
#define is_unsigned(a) (((__typeof__(a))-1) > 0)

/* Detect signed addition overflow, without executing a single overflowing
 * operation.
 */
#define _IS_ADD_OVERFLOW_S(x, y, c) ({ \
    __typeof__(x) _x = (x), _y  = (y), _z = \
        (_x ^ ~_y) & (1 << (sizeof(_x) * 8 - 1)); \
    (int)(((_z & (((_x ^ _z) + _y + (c)) ^ ~_y)) >> \
    (sizeof(_x) * 8 - 1)) & 1); })

/* Detect signed subtraction overflow, without executing a single overflowing
 * operation.
 */
#define _IS_SUBTRACT_OVERFLOW_S(x, y, c) ({ \
    __typeof__(x) _x = (x), _y  = (y), _z = \
        (_x ^ ~_y) & (1 << (sizeof(_x) * 8 - 1)); \
    (int)(((_z & (((_x ^ _z) - _y - (c)) ^ _y)) >> \
    (sizeof(_x) * 8 - 1)) & 1); })

/* Returns (int)0 or (int)1 indicating whether the addition x + y + c would
 * overflow or underflow. x and y must be of the same (signed or unsigned)
 * integer type, and c must be 0 or 1, of any integer (or bool) type.
 */
#define is_add_overflow(x, y, c) ( \
    sizeof(x) > MAX_INT_SIZE && !is_unsigned(x) ? \
    _IS_ADD_OVERFLOW_S(x, y, c) : ({ \
    __typeof__(x) _x = (x), _y = (y), _s = \
        (__typeof__(x))(to_unsigned(_x) + _y + (c)); \
    is_unsigned(_x) ? \
    (int)((((_x & _y) | ((_x | _y) & ~_s)) >> (sizeof(_x) * 8 - 1)) & 1) : \
    (int)((((_s ^ _x) & (_s ^ _y)) >> (sizeof(_x) * 8 - 1)) & 1); }))

/* Returns (int)0 or (int)1 indicating whether the subtraction x - y - c would
 * overflow or underflow. x and y must be of the same (signed or unsigned)
 * integer type, and c must be 0 or 1, of any integer (or bool) type.
 */
#define is_subtract_overflow(x, y, c) ( \
    sizeof(x) > MAX_INT_SIZE && !is_unsigned(x) ? \
    _IS_SUBTRACT_OVERFLOW_S(x, y, c) : ({ \
    __typeof__(x) _x = (x), _y = (y), _s = \
        (__typeof__(x))(to_unsigned(_x) - _y - (c)); \
    is_unsigned(_x) ? \
    (int)((((~_x & _y) | ((~_x | _y) & _s)) >> (sizeof(_x) * 8 - 1)) & 1) : \
    (int)((((_x ^ _y) & (_s ^ _x)) >> (sizeof(_x) * 8 - 1)) & 1); }))

#include <stdio.h>

int main(int argc, char **argv) {
  const char cx = 100, cy = 50, cz = -50;
  const unsigned char ucx = 200, ucy = 100;
  const short sx = 100, sy = 50;
  const unsigned short usx = 200, usy = 100;
  (void)argc; (void)argv;
  printf("%d,%d,%d\n",  /* 1,1,0 */
      is_add_overflow(cx, cy, 0),
      is_add_overflow(cx, cx, 0),
      is_add_overflow(cy, cy, 0));
  printf("%d,%d,%d\n",  /* 1,1,0 */
      is_add_overflow(ucx, ucy, 0),
      is_add_overflow(ucx, ucx, 0),
      is_add_overflow(ucy, ucy, 0));
  printf("%d,%d,%d,%d\n",  /* 0,1,0,0 */
      is_subtract_overflow(ucx, ucy, 0),
      is_subtract_overflow(ucy, ucx, 0),
      is_subtract_overflow(ucx, ucx, 0),
      is_subtract_overflow(ucy, ucy, 0));
  printf("%d,%d,%d,%d,%d\n",  /* 1,0,0,0,0 */
      is_subtract_overflow(cx, cz, 0),
      is_subtract_overflow(cx, cy, 0),
      is_subtract_overflow(cy, cx, 0),
      is_subtract_overflow(cx, cx, 0),
      is_subtract_overflow(cy, cy, 0));
  printf("%d,%d,%d\n",  /* 0,0,0 */
      is_add_overflow(sx, sy, 0),
      is_add_overflow(sx, sx, 0),
      is_add_overflow(sy, sy, 0));
  printf("%d,%d,%d\n",  /* 0,0,0 */
      is_add_overflow(usx, usy, 0),
      is_add_overflow(usx, usx, 0),
      is_add_overflow(usy, usy, 0));
  printf("%d,%d,%d\n",  /* 1,1,0 */
      is_add_overflow(-3U, 4U, 0),
      is_add_overflow(-3U, 3U, 0),
      is_add_overflow(-3U, 2U, 0));
  return 0;
}
