
#ifndef H_3D
#define H_3D

#include <tonc.h>

// For Fixed point arithmetic
#define SHIFT 12
#define SHIFTDIF (12 - SHIFT)

// I use the same data type for vectors and points and yes I'm horrible
typedef struct Vec3d {
	int x;
	int y;
	int z;
} Vec3d;

typedef struct Vec2d {
	int x;
	int y;
} Vec2d;

typedef struct Segment {
	Vec3d a;
	Vec3d b;
} Segment;

typedef struct Triangle2d {
	Vec2d a;
	Vec2d b;
	Vec2d c;
} Triangle2d;

typedef struct Triangle3d {
	Vec3d a;
	Vec3d b;
	Vec3d c;
} Triangle;

void add_Vec3d(Vec3d* dst, Vec3d* a, Vec3d* b);
void add_Vec2d(Vec2d* dst, Vec2d* a, Vec2d* b);
int fxmul_(int a, int b);
void rot_vec_xy(Vec3d* v, int ca, int sa);
void rot_vec_xz(Vec3d* v, int ca, int sa);
void rot_vec_yz(Vec3d* v, int ca, int sa);
void rot_mesh(Segment* mesh, uint len, void (*f)(Vec3d*, int, int), int ca, int sa);
void draw_line_ortho(const Vec2d* o, const Vec3d* a, const Vec3d* b);
void draw_mesh_ortho(const Vec2d* o, Segment* mesh, uint len);
void project(Vec3d* out, const Vec3d* point);
void draw_line_persp(const Vec2d* o, const Vec3d* a, const Vec3d* b);
void draw_mesh_persp(const Vec2d* o, Segment* mesh, uint len);

#endif
