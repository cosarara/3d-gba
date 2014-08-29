#include "quad.h"
#include "3D.h"
#include "tri.h"
#include <stdlib.h>
#include "utils.h"

int comp_vecs_xy(const void * elem1, const void * elem2) {
	const Vec3d* a = elem1;
	const Vec3d* b = elem2;
	if (a->y > b->y) return 1;
	if (a->y < b->y) return -1;
	if (a->x > b->x) return 1;
	if (a->x < b->x) return -1;
	return 0;
}

void sort_points_xy(Quad3d* q) {
	qsort(q, 4, sizeof(Vec3d), comp_vecs_xy);
}

int comp_vecs_y(const void * elem1, const void * elem2) {
	const Vec3d* a = elem1;
	const Vec3d* b = elem2;
	if (a->y > b->y) return 1;
	if (a->y < b->y) return -1;
	return 0;
}

void sort_points_y(Quad3d* q) {
	qsort(q, 4, sizeof(Vec3d), comp_vecs_y);
}

void raster_subtrap(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, Vec3d* d, uint color) {
	// TODO: Optimize for vertical lines
	int ii, dxa, dxb, dy, ystep, dda, ddb, xa, xb;
	int xastep, xbstep;

	int y = a->y;
	int ya = y;
	int yb = y;

	int xa1 = a->x;
	int ya1 = a->y;
	int xa2 = c->x;
	int ya2 = c->y;

	int xb1 = b->x;
	int xb2 = d->x;

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

void raster_trap(const Vec2d* o, Quad3d* q, uint color) {
	int flat_right, flat_left;
	
	sort_points_xy(q);
	//die_int(q->a.x);
	//m4_plot(q->a.x+o->x, q->a.y+o->y, 1);
	//m4_plot(q->b.x+o->x, q->b.y+o->y, 2);
	//m4_plot(q->c.x+o->x, q->c.y+o->y, 3);
	//m4_plot(q->d.x+o->x, q->d.y+o->y, 4);
	//die();
	flat_left = q->a.x == q->c.x;
	flat_right = q->b.x == q->d.x;
	if (flat_left && flat_right) {
		m4_rect(q->a.x+o->x, q->a.y+o->y, q->d.x+o->x, q->d.y+o->y, color);
		return;
	}
	raster_subtrap(o, &q->a, &q->b, &q->c, &q->d, color);
}

// Should profile, not sure what is faster
int x_for_y_in_line(int target_y, int x1, int y1, int x2, int y2) {
	return (int)(x1 + ((float)(target_y - y1) /
		   (float)(y2 - y1)) * (x2 - x1));
}
int x_for_y_in_line_(int target_y, int x1, int y1, int x2, int y2) {
	// XXX: Use bresenham's or something fast please
	/*
	return (int)(x1 + ((float)(target_y - y1) /
		   (float)(y2 - y1)) * (x2 - x1));
	*/

	// Find better alg.
	int ii, dx, dy, xstep, ystep, dd;
	int x = x1;
	int y = y1;

	// --- Normalization ---
	if(x1>x2)
	{	xstep= -1;	dx= x1-x2;	}
	else
	{	xstep= +1;	dx= x2-x1;	}

	if(y1>y2)
	{	ystep= -1;	dy= y1-y2;	}
	else
	{	ystep= +1;	dy= y2-y1;	}

	if(dx>=dy)		// Diagonal, slope <= 1
	{
		dd= 2*dy - dx;

		for(ii=dx; ii>=0; ii--)
		{
			if(dd >= 0)
			{
				dd -= 2*dx;
				y += ystep;
			}

			dd += 2*dy;
			x += xstep;
			if (y == target_y) {
				return x;
			}
		}
	}
	else				// # Diagonal, slope > 1
	{
		dd= 2*dx - dy;

		for(ii=dy; ii>=0; ii--)
		{
			if(dd >= 0)
			{
				dd -= 2*dy;
				x += xstep;	
			}

			dd += 2*dx;
			y += ystep;
			if (y == target_y) {
				return x;
			}
		}
	}
	die_int(0);
	return 0;
}

void raster_quad(const Vec2d* o, Quad3d* q, uint color) {
	// A lot faster than splitting in two triangles, plus no glitches
	int x1, x2;
	Vec3d e, f;
	Vec3d* a = &q->a;
	Vec3d* b = &q->b;
	Vec3d* c = &q->c;
	Vec3d* d = &q->d;
	int flat_top, flat_bottom;
	sort_points_y(q);
	flat_top = q->a.y == q->b.y;
	flat_bottom = q->c.y == q->d.y;
	if (flat_top && flat_bottom) {
		return raster_trap(o, q, color);
	}
	if (!flat_top) {
		/* If willing to fail in some cases:
		if ((a->x < b->x) == (c->x < d->x)) {
			g = q->c;
		} else {
			g = q->d;
		}
		*/
		x1 = x_for_y_in_line_(b->y, a->x, a->y, c->x, c->y);
		x2 = x_for_y_in_line_(b->y, a->x, a->y, d->x, d->y);
		if (abs(x1 - b->x) > abs(x2 - b->x)) {
			e.x = x1;
		} else {
			e.x = x2;
		}
		e.y = q->b.y;
		//d.z = q->b.z; // XXX
		raster_subtriangle_fast(o, &q->a, &q->b, &e, color);
	}
	if (!flat_bottom) {
		x1 = x_for_y_in_line_(c->y, d->x, d->y, a->x, a->y);
		x2 = x_for_y_in_line_(c->y, d->x, d->y, b->x, b->y);
		if (abs(x1 - c->x) > abs(x2 - c->x)) {
			f.x = x1;
		} else {
			f.x = x2;
		}
		f.y = q->c.y;
		raster_subtriangle_fast(o, &q->d, &q->c, &f, color);
	}
	if (!flat_top) {
		q->a.x = e.x;
		q->a.y = e.y;
	}
	if (!flat_bottom) {
		q->d.x = f.x;
		q->d.y = f.y;
	}
	return raster_trap(o, q, color);
}


