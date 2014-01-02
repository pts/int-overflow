#include "int_overflow.h"

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
