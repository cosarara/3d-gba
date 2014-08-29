#ifndef H_TRI
#define H_TRI

#include "3D.h"

void raster_triangle(const Vec2d* o, Triangle3d* t, uint color);
void raster_subtriangle_top(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color);
void raster_subtriangle_bot(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color);
void raster_subtriangle_fast(const Vec2d* o, Vec3d* a, Vec3d* b, Vec3d* c, uint color);

#endif
