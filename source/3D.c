#include "3D.h"
#include <tonc.h>
#include "utils.h"
#include "tri.h"
#include "quad.h"
#include <stdlib.h>

void add_Vec3d(Vec3d* dst, Vec3d* a, Vec3d* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
	dst->z = a->z + b->z;
}

void add_Vec2d(Vec2d* dst, Vec2d* a, Vec2d* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
}

int fxmul_(int a, int b) {
	return (a * b) >> SHIFT;
}

void rot_vec_xy(Vec3d* v, int ca, int sa) {
	int x = v->x << SHIFT;
	int y = v->y << SHIFT;
	v->x = (fxmul_(x, ca) - fxmul_(y, sa)) / (1 << SHIFT);
	v->y = (fxmul_(x, sa) + fxmul_(y, ca)) / (1 << SHIFT);
}

void rot_vec_xz(Vec3d* v, int ca, int sa) {
	int x = v->x << SHIFT;
	int z = v->z << SHIFT;
	v->x = (fxmul_(x, ca) + fxmul_(z, sa)) / (1 << SHIFT);
	v->z = (-fxmul_(x, sa) + fxmul_(z, ca)) / (1 << SHIFT);
}
void rot_vec_yz(Vec3d* v, int ca, int sa) {
	int y = v->y << SHIFT;
	int z = v->z << SHIFT;
	v->y = (fxmul_(y, ca) - fxmul_(z, sa)) / (1 << SHIFT);
	v->z = (fxmul_(y, sa) + fxmul_(z, ca)) / (1 << SHIFT);
}

void rot_segments(Segment* segments, uint len, void (*f)(Vec3d*, int, int), int ca, int sa) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &segments[i];
		f(&s->a, ca, sa);
		f(&s->b, ca, sa);
	}
}

void rot_triangles(Triangle3d** triangles, uint len, void (*f)(Vec3d*, int, int), int ca, int sa) {
	int i;
	Triangle3d* t;
	for (i=0; i < len; i++) {
		t = triangles[i];
		f(&t->a, ca, sa);
		f(&t->b, ca, sa);
		f(&t->c, ca, sa);
	}
}

void rot_quads(Quad3d** quads, uint len, void (*f)(Vec3d*, int, int), int ca, int sa) {
	int i;
	Quad3d* q;
	for (i=0; i < len; i++) {
		q = quads[i];
		f(&q->a, ca, sa);
		f(&q->b, ca, sa);
		f(&q->c, ca, sa);
		f(&q->d, ca, sa);
	}
}

void rot_mesh(Mesh* mesh, void (*f)(Vec3d*, int, int), int ca, int sa) {
	rot_triangles(mesh->triangles, mesh->tri_n, f, ca, sa);
	rot_segments(mesh->segments, mesh->seg_n, f, ca, sa);
	rot_quads(mesh->quads, mesh->quad_n, f, ca, sa);
}

void trans_mesh(Mesh* mesh, Vec3d* v) {
	int i;
	Triangle3d* tri;
	Quad3d* quad;
	Segment* seg;
	for (i=0; i<mesh->tri_n; i++) {
		tri = mesh->triangles[i];
		add_Vec3d(&tri->a, &tri->a, v);
		add_Vec3d(&tri->b, &tri->b, v);
		add_Vec3d(&tri->c, &tri->c, v);
	}
	for (i=0; i<mesh->quad_n; i++) {
		quad = mesh->quads[i];
		add_Vec3d(&quad->a, &quad->a, v);
		add_Vec3d(&quad->b, &quad->b, v);
		add_Vec3d(&quad->c, &quad->c, v);
		add_Vec3d(&quad->d, &quad->d, v);
	}
	for (i=0; i<mesh->seg_n; i++) {
		seg = &mesh->segments[i];
		add_Vec3d(&seg->a, &seg->a, v);
		add_Vec3d(&seg->b, &seg->b, v);
	}
}

void draw_line_ortho(const Vec2d* o, const Vec3d* a, const Vec3d* b) {
	int x1 = a->x + o->x;
	int y1 = a->y + o->y;
	int x2 = b->x + o->x;
	int y2 = b->y + o->y;
	m4_line(x1, y1, x2, y2, 1);
}

void draw_segments_ortho(const Vec2d* o, Segment* segments, uint len) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &segments[i];
		draw_line_ortho(o, &s->a, &s->b);
	}
}

void draw_triangle_ortho(const Vec2d* o, Triangle3d* t) {
	raster_triangle(o, t, t->color);
}

void draw_triangles_ortho(const Vec2d* o, Triangle3d** triangles, uint len) {
	int i;
	Triangle3d* t;
	for (i=0; i < len; i++) {
		t = triangles[i];
		draw_triangle_ortho(o, t);
	}
}

void draw_quad_ortho(const Vec2d* o, Quad3d* q) {
	//Triangle3d t1 = {{q->a.x, q->a.y}, {q->b.x, q->b.y}, {q->c.x, q->c.y}};
	//Triangle3d t2 = {{q->b.x, q->b.y}, {q->c.x, q->c.y}, {q->d.x, q->d.y}};
	//raster_triangle(o, &t1, q->color);
	//raster_triangle(o, &t2, q->color);
	raster_quad(o, q, q->color);
}

#define PLANE_DISTANCE 90

void project(const Vec3d* o, Vec3d* out, const Vec3d* point) {
	int obj_distance = PLANE_DISTANCE + 40 + point->z + o->z;
	out->x = point->x * PLANE_DISTANCE / obj_distance;
	out->y = point->y * PLANE_DISTANCE / obj_distance;
	out->z = point->z;
}

void vec3dto2d(Vec2d* out, const Vec3d* in) {
	out->x = in->x;
	out->y = in->y;
}

void draw_line_persp(const Vec2d* o2d, const Vec3d* o3d, const Vec3d* a, const Vec3d* b) {
	Vec3d a2, b2;
	project(o3d, &a2, a);
	project(o3d, &b2, b);
	draw_line_ortho(o2d, &a2, &b2);
}

void draw_segments_persp(const Vec2d* o2d, const Vec3d* o3d, Segment* segments, uint len) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &segments[i];
		draw_line_persp(o2d, o3d, &s->a, &s->b);
	}
}

void draw_triangle_persp(const Vec2d* o2d, const Vec3d* o3d, const Triangle3d* t) {
	Triangle3d t2;
	project(o3d, &t2.a, &t->a);
	project(o3d, &t2.b, &t->b);
	project(o3d, &t2.c, &t->c);
	t2.color = t->color;
	draw_triangle_ortho(o2d, &t2);
}

void draw_triangles_persp(const Vec2d* o2d, const Vec3d* o3d, Triangle3d** triangles, uint len) {
	int i;
	Triangle3d* t;
	for (i=0; i < len; i++) {
		t = triangles[i];
		draw_triangle_persp(o2d, o3d, t);
	}
}

void draw_quad_persp(const Vec2d* o2d, const Vec3d* o3d, const Quad3d* q) {
	Quad3d q2;
	project(o3d, &q2.a, &q->a);
	project(o3d, &q2.b, &q->b);
	project(o3d, &q2.c, &q->c);
	project(o3d, &q2.d, &q->d);
	q2.color = q->color;
	draw_quad_ortho(o2d, &q2);
}

void draw_quads_persp(const Vec2d* o2d, const Vec3d* o3d, Quad3d** quads, uint len) {
	int i;
	Quad3d* q;
	for (i=0; i < len; i++) {
		q = quads[i];
		draw_quad_persp(o2d, o3d, q);
	}
}
void draw_quads_ortho(const Vec2d* o, Quad3d** quads, uint len) {
	int i;
	Quad3d* q;
	for (i=0; i < len; i++) {
		q = quads[i];
		draw_quad_ortho(o, q);
	}
}
void draw_mesh_persp(const Vec2d* o2d, const Vec3d* o3d, Mesh* mesh) {
	draw_triangles_persp(o2d, o3d, mesh->triangles, mesh->tri_n);
	draw_quads_persp(o2d, o3d, mesh->quads, mesh->quad_n);
//	draw_segments_persp(o, mesh->segments, mesh->seg_n);
}
void draw_mesh_ortho(const Vec2d* o, Mesh* mesh) {
	draw_triangles_ortho(o, mesh->triangles, mesh->tri_n);
//	draw_segments_ortho(o, mesh->segments, mesh->seg_n);
	draw_quads_ortho(o, mesh->quads, mesh->quad_n);
}

//void draw_hline(const Vec2d* o, int y, int x1, int x2, int z, uint color) {
void draw_hline(const Vec2d* o, int y, int x1, int x2, uint color) {
	int tmp;
	if (x1 == x2)
		return;
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	y += o->y;
	//s32* zb = &zbuffer[y*240+x1];
	x1 = clamp(x1, 5, 235);
	x2 = clamp(x2, 5, 235);
	//if (x1 != x2 && *zb > z) { // XXX
		y = clamp(y, 5, 155);
		m4_hline(x1, y, x2, color);
		//memset32(zb, z, x2-x1-1);
	//}
}

int comp_triangles(const void * elem1, const void * elem2) {
	const Triangle3d* t1 = *((Triangle3d**)elem1);
	const Triangle3d* t2 = *((Triangle3d**)elem2);
	int z1 = fxmul_(((t1->a.z + t1->b.z + t1->c.z) << SHIFT), ((1 << SHIFT)/3)) / (1 << SHIFT);
	int z2 = fxmul_(((t2->a.z + t2->b.z + t2->c.z) << SHIFT), ((1 << SHIFT)/3)) / (1 << SHIFT);
	if (z1 > z2) return -1;
	if (z1 < z2) return 1;
	return 0;
}
int comp_quads(const void * elem1, const void * elem2) {
	const Quad3d* q1 = *((Quad3d**)elem1);
	const Quad3d* q2 = *((Quad3d**)elem2);
	int z1 = fxmul_(((q1->a.z + q1->b.z + q1->c.z + q1->d.z) << SHIFT), ((1 << SHIFT)/4)) / (1 << SHIFT);
	int z2 = fxmul_(((q2->a.z + q2->b.z + q2->c.z + q1->d.z) << SHIFT), ((1 << SHIFT)/4)) / (1 << SHIFT);
	if (z1 > z2) return -1;
	if (z1 < z2) return 1;
	return 0;
}
void sort_triangles(Triangle3d** triangles, uint num) {
	qsort(triangles, num, sizeof(Triangle3d*), comp_triangles);
}
void sort_quads(Quad3d** quads, uint num) {
	qsort(quads, num, sizeof(Quad3d*), comp_quads);
}
