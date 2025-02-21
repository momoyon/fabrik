#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>

#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

static int width  = 1280;
static int height = 720;
static const char *title = "Fabrik";
static bool DEBUG_DRAW = false;

#define JOINT_RADIUS 8.f
#define JOINT_INITIAL_LEN (height * 0.25f)
#define JOINT_COLOR  WHITE
#define LIMB_COLOR   WHITE

typedef struct {
  Vector2 pos;
  float   len;
} Joint;

typedef struct {
  Joint *items;
  size_t count;
  size_t capacity;
} Joints;

Joint make_joint(float x, float y, float len) {
  return (Joint) {
    .pos = (Vector2) { x, y },
    .len = len,
  };
}

typedef struct {
  Joints joints;
  Vector2 target;
  Vector2 root;
} Limb;

Limb make_limb(float rx, float ry, size_t joints_count) {
  Limb res = {0};
  res.root.x = rx;
  res.root.y = ry;

  float ls = JOINT_INITIAL_LEN;
  int sum = 0;
  for (size_t i = 0; i < joints_count; ++i) {
    Joint j = make_joint(width * 0.5f, height - sum, ls);
    da_append(res.joints, j);
    sum += ls;
    ls *= 0.8f;
  }

  res.target = res.joints.items[res.joints.count-1].pos;

  return res;
}

void fabrik_forward(Limb* l, float delta) {
  (void)delta;
  Joint *next = &(l->joints.items[l->joints.count-1]);
  next->pos = l->target;

  for (int i = (int)(l->joints.count-2); i >= 0; --i) {
    Joint *current = &(l->joints.items[i]);
    Vector2 dir = Vector2Normalize(Vector2Subtract(next->pos, current->pos));
    current->pos = Vector2Subtract(next->pos, Vector2Scale(dir, current->len));

    next = current;
  }
}

void fabrik_backward(Limb* l, float delta) {
  (void)delta;

  Joint *previous = &(l->joints.items[0]);
  previous->pos = l->root;

  for (size_t i = 1; i <= l->joints.count; ++i) {
    Joint *current = &(l->joints.items[i]);

    Vector2 dir = Vector2Normalize(Vector2Subtract(current->pos, previous->pos));
    current->pos = Vector2Add(previous->pos, Vector2Scale(dir, previous->len));

    previous = current;
  }
}

void update_limb(Limb* l, float delta) {
  fabrik_forward(l, delta);
  fabrik_backward(l, delta);
}

void draw_limb(Limb *l, bool debug) {
  (void)debug;

  Vector2 c, n;

  c = l->joints.items[0].pos;
  DrawCircleV(c, JOINT_RADIUS, JOINT_COLOR);
  for (size_t i = 0; i < l->joints.count-1; ++i) {
    c = l->joints.items[i].pos;
    n = l->joints.items[i+1].pos;

    DrawLineV(c, n, LIMB_COLOR);
    DrawCircleV(n, JOINT_RADIUS, JOINT_COLOR);
  }
}

int main(void) {
  InitWindow(width, height, title);

  // TODO: Free the joints of the limb
  Limb l = make_limb(width*0.5f, height, 8);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(BLACK);

    if (IsKeyDown(KEY_SPACE)) {
      l.target = GetMousePosition();
    }

    DrawFPS(0, 0);
    update_limb(&l, GetFrameTime());
    draw_limb(&l, DEBUG_DRAW);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
