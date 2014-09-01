#include <stdio.h>
#include <string.h>
#include <tonc.h>
#include "utils.h"
#include "3D.h"

//EWRAM_DATA int zbuffer[240*160] = {0};
//EWRAM_DATA s32 zbuffer[240*160] = {0};

void clear() {
//	m4_rect(0, 0, 240, 160, 0);
	m4_fill(0);
//	memset32(zbuffer, 200, 240*160);
}



#define SEG_SIZE 40
#define SEG_SIZES SEG_SIZE - 1
#define SEG_SIZEB SEG_SIZE + 1

// Weird thing
/*
Segment smesh1[2] = {
		{{-SEG_SIZE, SEG_SIZE, 0}, {SEG_SIZE, -SEG_SIZE, 0}},
		{{-SEG_SIZE, -SEG_SIZE, 0}, {SEG_SIZE, SEG_SIZE, 0}}
};
*/

// Cube
Segment smesh1[] = {
	{{-SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, SEG_SIZE, -SEG_SIZE}},
	{{SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, -SEG_SIZE}},
	{{-SEG_SIZE, -SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, -SEG_SIZE}},
	{{-SEG_SIZE, -SEG_SIZE, -SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, -SEG_SIZE}},
	{{-SEG_SIZE, SEG_SIZE, SEG_SIZE}, {SEG_SIZE, SEG_SIZE, SEG_SIZE}},
	{{SEG_SIZE, SEG_SIZE, SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, SEG_SIZE}},
	{{-SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, SEG_SIZE}},
	{{-SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, SEG_SIZE}},
	{{SEG_SIZE, SEG_SIZE, SEG_SIZE}, {SEG_SIZE, SEG_SIZE, -SEG_SIZE}},
	{{SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, -SEG_SIZE}},
	{{SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, -SEG_SIZE}},
	{{SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {SEG_SIZE, SEG_SIZE, SEG_SIZE}},
	{{-SEG_SIZE, SEG_SIZE, SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, -SEG_SIZE}},
	{{-SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {-SEG_SIZE, -SEG_SIZE, -SEG_SIZE}},
	{{-SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {-SEG_SIZE, -SEG_SIZE, -SEG_SIZE}},
	{{-SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, SEG_SIZE}}
};

Triangle3d tmesh1[] = {
//	{{-SEG_SIZE-1, -SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, -SEG_SIZE},
//		{SEG_SIZE, SEG_SIZE+1, -SEG_SIZE}, 1},
//	{{-SEG_SIZE, -SEG_SIZE-1, -SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, -SEG_SIZE},
//		{SEG_SIZE+1, SEG_SIZE, -SEG_SIZE}, 1},
//
//	{{-SEG_SIZE-1, -SEG_SIZE, SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, SEG_SIZE},
//		{SEG_SIZE, SEG_SIZE+1, SEG_SIZE}, 2},
//	{{-SEG_SIZE, -SEG_SIZE-1, SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, SEG_SIZE},
//		{SEG_SIZE+1, SEG_SIZE, SEG_SIZE}, 2},
//
//	{{-SEG_SIZES, -SEG_SIZES-1, -SEG_SIZEB}, {-SEG_SIZES, SEG_SIZES, -SEG_SIZEB},
//		{-SEG_SIZEB, SEG_SIZES, SEG_SIZES+1}, 3},
//	{{-SEG_SIZES, -SEG_SIZES, -SEG_SIZEB-1}, {-SEG_SIZES, -SEG_SIZES, SEG_SIZEB},
//		{-SEG_SIZEB, SEG_SIZES+1, SEG_SIZES}, 3},
//
//	{{SEG_SIZEB, -SEG_SIZES-1, SEG_SIZES}, {SEG_SIZEB, SEG_SIZES, SEG_SIZES},
//		{SEG_SIZEB, SEG_SIZES, -SEG_SIZES-1}, 4},
//	{{SEG_SIZEB, -SEG_SIZES, SEG_SIZES+1}, {SEG_SIZEB, -SEG_SIZES, -SEG_SIZES},
//		{SEG_SIZEB, SEG_SIZES+1, -SEG_SIZES}, 4},
};
Quad3d qmesh1[] = {
	{{-SEG_SIZE, -SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, -SEG_SIZE},
	 {-SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, SEG_SIZE, -SEG_SIZE}, 1},

	{{-SEG_SIZE, -SEG_SIZE, SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, SEG_SIZE},
	 {-SEG_SIZE, SEG_SIZE, SEG_SIZE}, {SEG_SIZE, SEG_SIZE, SEG_SIZE}, 2},

	{{-SEG_SIZE, -SEG_SIZE, -SEG_SIZE}, {-SEG_SIZE, -SEG_SIZE, SEG_SIZE},
	 {-SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {-SEG_SIZE, SEG_SIZE, SEG_SIZE}, 3},

	{{SEG_SIZE, -SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, -SEG_SIZE, SEG_SIZE},
	 {SEG_SIZE, SEG_SIZE, -SEG_SIZE}, {SEG_SIZE, SEG_SIZE, SEG_SIZE}, 4},
};

// Manually to be able to use the value in the preprocessor
//#define N_TRIANGLES (sizeof(tmesh1) / sizeof(Triangle3d))
#define N_TRIANGLES 0
//#define N_QUADS (sizeof(qmesh1) / sizeof(Quad3d))
#define N_QUADS 4

Segment smesh2[sizeof(smesh1) / sizeof(Segment)];
Triangle3d tmesh2[N_TRIANGLES];
Triangle3d* tmesh2_[N_TRIANGLES];

Quad3d qmesh2[N_QUADS];
Quad3d* qmesh2_[N_QUADS];

Mesh mesh2 = {smesh2, sizeof(smesh2) / sizeof(Segment), tmesh2_, N_TRIANGLES, qmesh2_, N_QUADS};

const Vec2d origin = {120, 80};
const Vec2d origin0 = {0, 0};
Vec3d origin3d = {120, 80, 0};
Vec3d origin3d2 = {120, 80, 0};
const Vec3d empty = {0, 0, 0};

int main() {
	int i;
	uint angle = 0;
	//uint angle2 = 0;
	uint angle3 = 0;
	int ca, sa;
	//int ca2, sa2;
	int ca3, sa3;
	int draw_mode = 1;
	int rotate_xz = 1;
	//int rotate_xy = 1;
	int rotate_yz = 1;

	int camera_x = 0;
	int camera_y = 0;
	int camera_z = 0;
	Vec3d camera_t;
	//u32 time = 0;
	//Triangle3d t1 = {{0, 0, 0}, {10, 10, 0}, {5, 20, 0}};

	vid_page = vid_mem_back;
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	
	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;

	pal_bg_mem[1]= CLR_LIME;
	pal_bg_mem[2]= CLR_RED;
	pal_bg_mem[3]= CLR_BLUE;
	pal_bg_mem[4]= CLR_YELLOW;
	pal_bg_mem[5]= CLR_GREEN;
	pal_bg_mem[6]= CLR_PURPLE;
	pal_bg_mem[7]= CLR_CYAN;
	pal_bg_mem[8]= CLR_ORANGE;
	pal_bg_mem[255]= CLR_WHITE;
	
	txt_init_std();
	clear();
	m4_plot(8, 8, 2);

	for(i=0; i<N_TRIANGLES; i++) {
		tmesh2_[i] = &tmesh2[i];
	}
	for(i=0; i<N_QUADS; i++) {
		qmesh2_[i] = &qmesh2[i];
	}
	//die_int(N_TRIANGLES);
	
	while(1) {
		//puts_int(REG_VCOUNT, 60);

		vid_flip();
		VBlankIntrWait();

		//profile_start();
		//clear();
		//raster_triangle(&origin0, &t1, 2);
		//draw_triangle_ortho(&origin0, &t1);
		//draw_triangle_persp(&origin, &t1);
		
		clear();
		//memcpy(smesh2, smesh1, sizeof(smesh2));
		//memcpy32(smesh2, smesh1, sizeof(smesh2) / 4);
		dma3_cpy(smesh2, smesh1, sizeof(smesh2));
#if N_TRIANGLES > 0
		dma3_cpy(tmesh2, tmesh1, sizeof(tmesh2));
#endif
#if N_QUADS > 0
		dma3_cpy(qmesh2, qmesh1, sizeof(qmesh2));
#endif
		memcpy(&origin3d2, &origin3d, sizeof(Vec3d));

		//angle2 = angle / 2;
		ca = lu_cos(angle) / (1 << SHIFTDIF);
		//ca2 = lu_cos(angle2) / (1 << SHIFTDIF);
		ca3 = lu_cos(angle3) / (1 << SHIFTDIF);
		sa = lu_sin(angle) / (1 << SHIFTDIF);
		//sa2 = lu_sin(angle2) / (1 << SHIFTDIF);
		sa3 = lu_sin(angle3) / (1 << SHIFTDIF);
		//rot_mesh(&mesh2, &rot_vec_xz, ca, sa);
		if (rotate_xz) rot_mesh(&mesh2, &rot_vec_xz, ca, sa);
		//if (rotate_xy) rot_mesh(&mesh2, &rot_vec_xy, ca2, sa2);
		if (rotate_yz) rot_mesh(&mesh2, &rot_vec_yz, ca3, sa3);
		rot_vec_xz(&origin3d2, ca, sa);

		camera_t.x = -camera_x;
		camera_t.y = -camera_y;
		camera_t.z = -camera_z;
		trans_mesh(&mesh2, &camera_t);

		key_poll();
		if (key_hit(KEY_START)) draw_mode = !draw_mode;
		//if (key_hit(KEY_DOWN)) rotate_xy = !rotate_xy;
		//if (key_hit(KEY_RIGHT)) rotate_xz = !rotate_xz;
		//if (key_hit(KEY_UP)) rotate_yz = !rotate_yz;

#if N_TRIANGLES > 0
		sort_triangles(tmesh2_, N_TRIANGLES);
#endif
#if N_QUADS > 0
		sort_quads(qmesh2_, N_QUADS);
#endif

		//angle += 400*bit_tribool(__key_curr, KI_RIGHT, KI_LEFT);
		camera_x += 2*bit_tribool(__key_curr, KI_RIGHT, KI_LEFT);
		angle += 400*bit_tribool(__key_curr, KI_A, KI_B);
		angle3 += 400*bit_tribool(__key_curr, KI_UP, KI_DOWN);

		profile_start();
		//puts_int(REG_VCOUNT, 50);
		if (draw_mode) {
			m4_puts(190, 30, "Persp", 3);
			draw_mesh_persp(&origin, &origin3d2, &mesh2);
		} else {
			m4_puts(190, 30, "Orth", 2);
			draw_mesh_ortho(&origin, &mesh2);
		}
		puts_int(profile_stop(), 50);
		m4_puts(13, 8, "Arrow keys and A/B to move", 1);
		m4_puts(5, 150, "START switches rendering mode", 1);
		//puts_int(zbuffer[80*240+90], 30);
		//puts_int(tmesh2->, 30);
	}
	
	return 0;
}


