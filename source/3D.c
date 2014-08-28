#include "3D.h"
#include <tonc.h>
#include <stdlib.h>
#include "utils.h"

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
	Triangle3d t1 = {{q->a.x, q->a.y}, {q->b.x, q->b.y}, {q->c.x, q->c.y}};
	Triangle3d t2 = {{q->b.x, q->b.y}, {q->c.x, q->c.y}, {q->d.x, q->d.y}};
	raster_triangle(o, &t1, q->color);
	raster_triangle(o, &t2, q->color);
}

#define PLANE_DISTANCE 90

void project(Vec3d* out, const Vec3d* point) {
	int obj_distance = PLANE_DISTANCE + 30 + point->z;
	out->x = point->x * PLANE_DISTANCE / obj_distance;
	out->y = point->y * PLANE_DISTANCE / obj_distance;
	out->z = point->z;
}

void draw_line_persp(const Vec2d* o, const Vec3d* a, const Vec3d* b) {
	Vec3d a2, b2;
	project(&a2, a);
	project(&b2, b);
	draw_line_ortho(o, &a2, &b2);
}

void draw_segments_persp(const Vec2d* o, Segment* segments, uint len) {
	int i;
	Segment* s;
	for (i=0; i < len; i++) {
		s = &segments[i];
		draw_line_persp(o, &s->a, &s->b);
	}
}

void draw_triangle_persp(const Vec2d* o, const Triangle3d* t) {
	Triangle3d t2;
	project(&t2.a, &t->a);
	project(&t2.b, &t->b);
	project(&t2.c, &t->c);
	t2.color = t->color;
	draw_triangle_ortho(o, &t2);
}

void draw_triangles_persp(const Vec2d* o, Triangle3d** triangles, uint len) {
	int i;
	Triangle3d* t;
	for (i=0; i < len; i++) {
		t = triangles[i];
		draw_triangle_persp(o, t);
	}
}

void draw_quad_persp(const Vec2d* o, const Quad3d* q) {
	Quad3d q2;
	project(&q2.a, &q->a);
	project(&q2.b, &q->b);
	project(&q2.c, &q->c);
	project(&q2.d, &q->d);
	q2.color = q->color;
	draw_quad_ortho(o, &q2);
}

void draw_quads_persp(const Vec2d* o, Quad3d** quads, uint len) {
	int i;
	Quad3d* q;
	for (i=0; i < len; i++) {
		q = quads[i];
		draw_quad_persp(o, q);
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
void draw_mesh_persp(const Vec2d* o, Mesh* mesh) {
	draw_triangles_persp(o, mesh->triangles, mesh->tri_n);
	draw_quads_persp(o, mesh->quads, mesh->quad_n);
//	draw_segments_persp(o, mesh->segments, mesh->seg_n);
}
void draw_mesh_ortho(const Vec2d* o, Mesh* mesh) {
	draw_triangles_ortho(o, mesh->triangles, mesh->tri_n);
//	draw_segments_ortho(o, mesh->segments, mesh->seg_n);
	draw_quads_ortho(o, mesh->quads, mesh->quad_n);
}

void sort_points(Triangle3d* t) {
	Vec3d tmp;
	Vec3d* a = &t->a;
	Vec3d* b = &t->b;
	Vec3d* c = &t->c;
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

void raster_subtriangle_top(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color);
void raster_subtriangle_bot(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color);
void raster_subtriangle_fast(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color);

// This algorithm divides therefore it's horrible and must be optimized :)
void raster_triangle(const Vec2d* o, Triangle3d* t, uint color) {
	Vec3d d;
	sort_points(t);
	
	if (t->a.y == t->b.y) {
		//raster_subtriangle_bot(o, &t->a, &t->b, &t->c, color);
		raster_subtriangle_fast(o, &t->c, &t->a, &t->b, color);
		return;
	}
	if (t->b.y == t->c.y) {
		//raster_subtriangle_top(o, &t->a, &t->b, &t->c, color);
		raster_subtriangle_fast(o, &t->a, &t->b, &t->c, color);
		return;
	}

	d.x = (int)(t->a.x + ((float)(t->b.y - t->a.y) /
	           (float)(t->c.y - t->a.y)) * (t->c.x - t->a.x));
	d.y = t->b.y;
	d.z = t->b.z; // XXX
	raster_subtriangle_fast(o, &t->a, &t->b, &d, color);
	raster_subtriangle_fast(o, &t->c, &t->b, &d, color);
	//raster_subtriangle_top(o, &t->a, &t->b, &d, color);
	//raster_subtriangle_bot(o, &t->b, &d, &t->c, color);
	//m4_plot(t->a.x+o->x, t->a.y+o->y, 1);
	//m4_plot(t->b.x+o->x, t->b.y+o->y, 2);
	//m4_plot(t->c.x+o->x, t->c.y+o->y, 3);
}

void draw_hline(const Vec2d* o, int y, int x1, int x2, int z, uint color) {
	int tmp;
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
	if (x1 != x2) { // XXX
		y = clamp(y, 5, 155);
		m4_hline(x1, y, x2, color);
		//memset32(zb, z, x2-x1-1);
	}
}

void raster_subtriangle_fast(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color) {
	int ii, dxa, dxb, dy, ystep, dda, ddb, xa, xb;
	int xastep, xbstep;

	int y = a->y;
	int ya = y;
	int yb = y;

	int xa1 = a->x;
	int ya1 = a->y;
	int xa2 = b->x;
	int ya2 = b->y;

	int xb1 = a->x;
	int xb2 = c->x;

	xa = xa1;
	xb = xb1;

	// --- Normalization ---
	if(xa1>xa2)
	{	xastep= -1;	dxa= xa1-xa2;	}
	else
	{	xastep= +1;	dxa= xa2-xa1;	}

	if(ya1>ya2)
	{	ystep= -1;	dy= ya1-ya2;	}
	else
	{	ystep= +1;	dy= ya2-ya1;	}

	if(xb1>xb2)
	{	xbstep= -1;	dxb= xb1-xb2;	}
	else
	{	xbstep= +1;	dxb= xb2-xb1;	}

	xa += o->x;
	xb += o->x;

	if(dxa>=dy)
	{
		dda= 2*dy - dxa;
		ddb= 2*dy - dxb;

		for(ii=dxa; ii>=0; ii--)
		{
			draw_hline(o, ya, xa, xb, b->z, color);
			if (dda >= 0)
			{
				dda -= 2*dxa;
				ya += ystep;
			}

			dda += 2*dy;
			xa += xastep;

			while (ya != yb) {
				if (dxb>=dy)		// Diagonal, slope <= 1
				{
					if (ddb >= 0) {
						ddb -= 2*dxb;
						yb += ystep;
					}
					ddb += 2*dy;
					xb += xbstep;
				} else {
					if (ddb >= 0) {
						ddb -= 2*dy;
						xb += xbstep;
					}
					ddb += 2*dxb;
					yb += ystep;
				}
			}
		}
	} else {
		dda= 2*dxa - dy;
		ddb= 2*dxb - dy;

		for(ii=dy; ii>=0; ii--)
		{
			draw_hline(o, ya, xa, xb, b->z, color);
			if (dda >= 0)
			{
				dda -= 2*dy;
				xa += xastep;
			}

			dda += 2*dxa;
			ya += ystep;

			while (ya != yb) {
				if (dxb>=dy)		// Diagonal, slope <= 1
				{
					if (ddb >= 0) {
						ddb -= 2*dxb;
						yb += ystep;
					}
					ddb += 2*dy;
					xb += xbstep;
				} else {
					if (ddb >= 0) {
						ddb -= 2*dy;
						xb += xbstep;
					}
					ddb += 2*dxb;
					yb += ystep;
				}
			}
		}
	}
}
void raster_subtriangle_top(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color) {
	int scanlineY;

	float invslope1 = (float)(b->x - a->x) / (b->y - a->y);
	float invslope2 = (float)(c->x - a->x) / (c->y - a->y);
	int x1, x2, y, tmp;

	float curx1, curx2;
	curx1 = curx2 = a->x + o->x;

	for (scanlineY = a->y; scanlineY <= b->y; scanlineY++)
	{
		x1 = curx1;
		x2 = curx2;
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		y = scanlineY+o->y;
		//s32* zb = &zbuffer[y*240+x1];
		//if (x1 != x2 && *zb > b->z) { // XXX
		if (x1 != x2) { // XXX
			m4_hline(x1, y, x2, color);
			//memset32(zb, b->z, x2-x1-1);
		}
		curx1 += invslope1;
		curx2 += invslope2;
	}
}
void raster_subtriangle_bot(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color) {
	int scanlineY;
	float invslope1 = (float)(c->x - a->x) / (c->y - a->y);
	float invslope2 = (float)(c->x - b->x) / (c->y - b->y);
	int x1, x2, y, tmp;

	float curx1, curx2;
	curx1 = curx2 = c->x + o->x;

	for (scanlineY = c->y; scanlineY > b->y; scanlineY--)
	{
		x1 = curx1;
		x2 = curx2;
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		y = scanlineY+o->y;
		//s32* zb = &zbuffer[y*240+x1];
		//if (x1 != x2 && *zb > b->z) { // XXX
		if (x1 != x2) { // XXX
			m4_hline(x1, y, x2, color);
			//memset32(zb, b->z, x2-x1-1);
		}
		curx1 -= invslope1;
		curx2 -= invslope2;
	}
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
