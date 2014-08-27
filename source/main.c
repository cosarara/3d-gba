#include <stdio.h>
//#include <string.h>
#include <tonc.h>
#include "utils.h"
#include "3D.h"

void clear() {
	//m4_rect(0, 0, 240, 160, 0);
	m4_fill(0);
}



#define SEG_SIZE 40

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

Segment smesh2[sizeof(smesh1) / sizeof(Segment)];

const Vec2d origin = {120, 80};
const Vec3d origin3d = {100, 60, 0};
const Vec3d empty = {0, 0, 0};


int main() {
	uint angle = 0;
	uint angle2 = 0;
	int ca, sa;
	int ca2, sa2;
	int mesh_size = sizeof(smesh1) / sizeof(Segment);
	int draw_mode = 1;
	int rotate_xz = 1;
	int rotate_xy = 1;
	int rotate_yz = 0;

	vid_page = vid_mem_back;
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	
	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;

	pal_bg_mem[1]= CLR_LIME;
	pal_bg_mem[2]= CLR_RED;
	pal_bg_mem[3]= CLR_BLUE;
	pal_bg_mem[4]= CLR_YELLOW;
	pal_bg_mem[255]= CLR_WHITE;
	
	txt_init_std();
	clear();
	m4_plot(8, 8, 2);
	
	while(1) {
		vid_flip();
		VBlankIntrWait();
		//vid_flip();

		//memcpy(smesh2, smesh1, sizeof(smesh2));
		//memcpy32(smesh2, smesh1, sizeof(smesh2) / 4);
		dma3_cpy(smesh2, smesh1, sizeof(smesh2));

		angle2 = angle / 2;
		ca = lu_cos(angle) / (1 << SHIFTDIF);
		ca2 = lu_cos(angle2) / (1 << SHIFTDIF);
		sa = lu_sin(angle) / (1 << SHIFTDIF);
		sa2 = lu_sin(angle2) / (1 << SHIFTDIF);
		if (rotate_xy) rot_mesh(smesh2, mesh_size, &rot_vec_xy, ca2, sa2);
		if (rotate_xz) rot_mesh(smesh2, mesh_size, &rot_vec_xz, ca, sa);
		if (rotate_yz) rot_mesh(smesh2, mesh_size, &rot_vec_yz, ca, sa);

		key_poll();
		if (key_hit(KEY_A)) draw_mode = !draw_mode;
		if (key_hit(KEY_DOWN)) rotate_xy = !rotate_xy;
		if (key_hit(KEY_RIGHT)) rotate_xz = !rotate_xz;
		if (key_hit(KEY_UP)) rotate_yz = !rotate_yz;

		angle += 200;

		while(REG_VCOUNT < 160);

		clear();
		if (draw_mode) {
			m4_puts(190, 30, "Persp", 3);
			draw_mesh_persp(&origin, smesh2, mesh_size);
		} else {
			m4_puts(190, 30, "Orth", 2);
			draw_mesh_ortho(&origin, smesh2, mesh_size);
		}
		m4_puts(3, 8, "Press the arrow keys and A to", 1);
		m4_puts(3, 150, "change rotation and rendering", 1);
	}
	
	return 0;
}


