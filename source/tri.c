#include "tri.h"
#include "3D.h"
#include <stdlib.h>

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

void raster_subtriangle_fast(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color) {
	// TODO: Optimize for vertical lines
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
			draw_hline(o, ya, xa, xb, color);
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
			draw_hline(o, ya, xa, xb, color);
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
		if (x1 != x2) { // XXX
			m4_hline(x1, y, x2, color);
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
