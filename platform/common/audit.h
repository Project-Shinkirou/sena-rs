#ifndef SENA_CONSOLE_AUDIT_H
#define SENA_CONSOLE_AUDIT_H
#include "console.h"
#include <stdio.h>

/* Optional local input replay for emulator diagnostics. The first line holds
 * timestep, frame limit, and optional checkpoint slot; zero timestep preserves
 * the real host clock/audio. Vita restores a checkpoint before the first frame.
 * Subsequent CSV rows are frame,x,y,left,skip.
 * Coordinates are logical PAL pixels. Missing files leave normal input intact. */
typedef struct {
  FILE *file;
  unsigned dt, limit, next, x, y, left, skip;
  int restore_slot;
  bool active, pending;
} SenaAudit;

static SenaAudit sena_audit_open(const char *path) {
  SenaAudit a = {.restore_slot = -1};
  a.file = fopen(path, "r");
  if (!a.file)
    return a;
  char header[128];
  if (!fgets(header, sizeof(header), a.file) ||
      sscanf(header, "%u,%u,%d", &a.dt, &a.limit, &a.restore_slot) < 2 ||
      a.dt > 100 || !a.limit) {
    fclose(a.file);
    a.file = NULL;
    return a;
  }
  a.active = true;
  fprintf(stderr, "[audit] fixed_dt=%u limit=%u\n", a.dt, a.limit);
  return a;
}

static void sena_audit_input(SenaAudit *a, unsigned frame, void *host,
                             SenaCursor *cursor, SenaViewport view,
                             int lw, int lh) {
  while (a->file) {
    if (!a->pending) {
      unsigned f, x, y, left, skip;
      if (fscanf(a->file, " %u,%u,%u,%u,%u", &f, &x, &y, &left, &skip) != 5) {
        fclose(a->file);
        a->file = NULL;
        break;
      }
      a->next = f;
      /* Keep the currently held state until the scheduled row is due. */
      if (f > frame) {
        a->pending = true;
        a->x = x; a->y = y; a->left = left; a->skip = skip;
        break;
      }
      a->x = x; a->y = y; a->left = left; a->skip = skip;
    } else if (a->next > frame) {
      break;
    } else {
      a->pending = false;
    }
    cursor->x = view.x + (double)a->x * view.w / lw;
    cursor->y = view.y + (double)a->y * view.h / lh;
    sena_cursor_update(host, cursor, view, lw, lh, 0, 0, false, 0, 0,
                       a->left != 0, false, a->skip != 0, false);
    fprintf(stderr, "[audit] frame=%u x=%u y=%u left=%u skip=%u\n",
            frame, a->x, a->y, a->left, a->skip);
  }
  sena_host_resize(host, lw, lh);
  sena_host_touch(host, 1, (cursor->x - view.x) * lw / view.w,
                  (cursor->y - view.y) * lh / view.h);
}
#endif
