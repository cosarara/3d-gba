#include "3D.h"
#include <tonc.h>

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
	//puts_int(v->x, 18);
	//puts_int((v->x << 12) * lu_cos(a), 28);
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

void rot_mesh(Segment* mesh, uint len, void (*f)(Vec3d*, int, int), int ca, int sa) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &mesh[i];
		f(&s->a, ca, sa);
		f(&s->b, ca, sa);
	}
}

void draw_line_ortho(const Vec2d* o, const Vec3d* a, const Vec3d* b) {
	int x1 = a->x + o->x;
	int y1 = a->y + o->y;
	int x2 = b->x + o->x;
	int y2 = b->y + o->y;
	m4_line(x1, y1, x2, y2, 1);
}

void draw_mesh_ortho(const Vec2d* o, Segment* mesh, uint len) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &mesh[i];
		draw_line_ortho(o, &s->a, &s->b);
	}
}

#define PLANE_DISTANCE 90

void project(Vec3d* out, const Vec3d* point) {
	int obj_distance = PLANE_DISTANCE + 30 + point->z;
	out->x = point->x * PLANE_DISTANCE / obj_distance;
	out->y = point->y * PLANE_DISTANCE / obj_distance;
	out->z = 0;
}

void draw_line_persp(const Vec2d* o, const Vec3d* a, const Vec3d* b) {
	Vec3d a2, b2;
	project(&a2, a);
	project(&b2, b);
	draw_line_ortho(o, &a2, &b2);
}

void draw_mesh_persp(const Vec2d* o, Segment* mesh, uint len) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &mesh[i];
		draw_line_persp(o, &s->a, &s->b);
	}
}

void sort_points(Triangle2d* t) {
	Vec2d tmp;
	Vec2d* a = &t->a;
	Vec2d* b = &t->b;
	Vec2d* c = &t->c;
	if (t->a.y > t->b.y) {
		tmp = *a;
		*a = *b;
		*b = tmp;
	}
	if (t->b.y > t->c.y) {
		tmp = *b;
		*b = *c;
		*c = tmp;
	}
	if (t->a.y > t->b.y) {
		tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

void raster_subtriangle_top(const Vec2d* o, Vec2d* a, Vec2d* b, Vec2d* c);

void raster_triangle(const Vec2d* o, Triangle2d* t) {
	Vec2d d;
	sort_points(t);
	
	d.x = (int)(t->a.x + ((float)(t->b.y - t->a.y) /
	           (float)(t->c.y - t->a.y)) * (t->c.x - t->a.x));
	d.y = t->b.y;
	raster_subtriangle_top(o, &t->a, &t->b, &d);
	//raster_subtriangle_bot(t->b, d, t->c);
}

void raster_subtriangle_top(const Vec2d* o, Vec2d* a, Vec2d* b, Vec2d* c) {
	int scanlineY;
	float invslope1 = (b->x - a->x) / (b->y - a->y);
	float invslope2 = (c->x - a->x) / (c->y - a->y);

	float curx1, curx2;
	curx1 = curx2 = a->x + o->x;

	for (scanlineY = a->y; scanlineY <= b->y; scanlineY++)
	{
		m4_hline((int)curx1, scanlineY+o->y, (int)curx2, 1);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}
