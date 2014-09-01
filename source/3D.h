
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
	uint color;
} Triangle3d;

typedef struct Quad2d {
	Vec2d a;
	Vec2d b;
	Vec2d c;
	Vec2d d;
} Quad2d;

typedef struct Quad3d {
	Vec3d a;
	Vec3d b;
	Vec3d c;
	Vec3d d;
	uint color;
} Quad3d;

typedef struct Mesh {
	Segment* segments;
	uint seg_n;
	Triangle3d** triangles;
	uint tri_n;
	Quad3d** quads;
	uint quad_n;
} Mesh;

void add_Vec3d(Vec3d* dst, Vec3d* a, Vec3d* b);
void add_Vec2d(Vec2d* dst, Vec2d* a, Vec2d* b);
int fxmul_(int a, int b);
void rot_vec_xy(Vec3d* v, int ca, int sa);
void rot_vec_xz(Vec3d* v, int ca, int sa);
void rot_vec_yz(Vec3d* v, int ca, int sa);
void rot_segments(Segment* segments, uint len, void (*f)(Vec3d*, int, int), int ca, int sa);
void rot_mesh(Mesh* mesh, void (*f)(Vec3d*, int, int), int ca, int sa);
void draw_line_ortho(const Vec2d* o, const Vec3d* a, const Vec3d* b);
void draw_segments_ortho(const Vec2d* o, Segment* mesh, uint len);
void project(const Vec3d* o, Vec3d* out, const Vec3d* point);
//void draw_line_persp(const Vec3d* o, const Vec3d* a, const Vec3d* b);
//void draw_segments_persp(const Vec3d* o, Segment* mesh, uint len);
void draw_mesh_persp(const Vec2d* o2d, const Vec3d* o3d, Mesh* mesh);
void draw_mesh_ortho(const Vec2d* o, Mesh* mesh);

void raster_triangle(const Vec2d* o, Triangle3d* t, uint color);
void sort_triangles(Triangle3d** triangles, uint num);
void sort_quads(Quad3d** quads, uint num);
void draw_hline(const Vec2d* o, int y, int x1, int x2, uint color);
void trans_mesh(Mesh* mesh, Vec3d* v);
//extern s32 zbuffer[240*160];

#endif
