#include "console.h"
#include <assert.h>
static double pointer_x, pointer_y;
static int left_edges, left_state, skip_edges;
void sena_host_resize(void *h, uint32_t w, uint32_t v) {
  (void)h;
  assert(w == 1280 && v == 720);
}
void sena_host_touch(void *h, int32_t phase, double x, double y) {
  (void)h;
  assert(phase == 1);
  pointer_x = x;
  pointer_y = y;
}
void sena_host_mouse_button(void *h, uint32_t button, int32_t down) {
  (void)h;
  if (button == 0) {
    ++left_edges;
    left_state = down;
  }
}
void sena_host_key(void *h, const char *name, int32_t down) {
  (void)h;
  (void)down;
  if (!strcmp(name, "Control"))
    ++skip_edges;
}
int main(void) {
  SenaViewport v = sena_fit(960, 544, 1280, 720);
  assert(v.x == 0 && v.y == 2 && v.w == 960 && v.h == 540);
  SenaCursor c = {.x = 480, .y = 272};
  sena_cursor_update(NULL, &c, v, 1280, 720, 0, 0, false, 0, 0, true, false,
                     true, false);
  assert(pointer_x == 640 && pointer_y == 360 && left_edges == 1 &&
         left_state == 1 && skip_edges == 1);
  sena_cursor_update(NULL, &c, v, 1280, 720, 0, 0, true, 480, 272, false, false,
                     true, false);
  assert(left_edges == 1 && skip_edges == 1);
  sena_cursor_update(NULL, &c, v, 1280, 720, 0, 0, true, 480, 0, false, false,
                     false, false);
  assert(left_edges == 2 && left_state == 0 && skip_edges == 2);
  uint8_t pixels[8 * 8 * 4], source[4] = {12, 34, 56, 255};
  c.x = 100;
  c.y = 100;
  sena_blit(pixels, 8 * 4, 8, 8, source, 1, 1, (SenaViewport){2, 2, 4, 4}, &c);
  assert(pixels[3] == 255 && pixels[0] == 0);
  assert(!memcmp(pixels + (2 * 8 + 2) * 4, source, 4));
  assert(!memcmp(pixels + (5 * 8 + 5) * 4, source, 4));
  assert(pixels[(6 * 8 + 6) * 4] == 0);
  return 0;
}
