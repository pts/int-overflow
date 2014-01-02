/* !! detect signed type */
/* !! subtraction? */

/* !! ({ and __typeof__ are gcc-specific */
/* !! clang? */
#define is_signed_add_overflow(x, y, c) ({ \
    __typeof__(x) _x = (x), _y = (y), _s = (__typeof__(x))(_x + _y + (c)); \
    (int)((((_s ^ _x) & (_s ^ _y)) >> (sizeof(_x) * 8 - 1)) & 1); })

#define is_unsigned_add_overflow(x, y, c) ({ \
    __typeof__(x) _x = (x), _y = (y), _s = (__typeof__(x))(_x + _y + (c)); \
    (int)((((_x & _y) | ((_x | _y) & ~_s)) >> (sizeof(_x) * 8 - 1)) & 1); })

#define is_add_overflow(x, y, c) ( \
    (((__typeof__(x))(-1)) >> (sizeof(x) * 8 - 1)) < 2 ? \
    is_unsigned_add_overflow((x), (y), (c)) : \
    is_signed_add_overflow((x), (y), (c)))

#include <stdio.h>

int main(int argc, char **argv) {
  const char cx = 100, cy = 50;
  const unsigned char ucx = 200, ucy = 100;
  const short sx = 100, sy = 50;
  const unsigned short usx = 200, usy = 100;
  (void)argc; (void)argv;
  printf("%d,%d,%d\n",  /* 1,1,0 */
      is_signed_add_overflow(cx, cy, 0),
      is_signed_add_overflow(cx, cx, 0),
      is_signed_add_overflow(cy, cy, 0));
  printf("%d,%d,%d\n",  /* 1,1,0 */
      is_unsigned_add_overflow(ucx, ucy, 0),
      is_unsigned_add_overflow(ucx, ucx, 0),
      is_unsigned_add_overflow(ucy, ucy, 0));
  printf("%d,%d,%d\n",  /* 0,0,0 */
      is_signed_add_overflow(sx, sy, 0),
      is_signed_add_overflow(sx, sx, 0),
      is_signed_add_overflow(sy, sy, 0));
  printf("%d,%d,%d\n",  /* 0,0,0 */
      is_unsigned_add_overflow(usx, usy, 0),
      is_unsigned_add_overflow(usx, usx, 0),
      is_unsigned_add_overflow(usy, usy, 0));
  printf("%d,%d,%d\n",  /* 1,1,0 */
      is_unsigned_add_overflow(-3U, 4U, 0),
      is_unsigned_add_overflow(-3U, 3U, 0),
      is_unsigned_add_overflow(-3U, 2U, 0));
  return 0;
}
