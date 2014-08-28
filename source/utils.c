#include <stdio.h>
#include <tonc.h>
#include "utils.h"

void puts_int(int i, int y) {
	char s[20];
	sprintf(s, "%d", i);
	//m4_puts(8, 8, "hello", 1);
	m4_puts(8, y, s, 1);
}

void die_int(int i) {
	puts_int(i, 38);
	vid_flip();
	puts_int(i, 38);
	die();
}

void die() {
	while(1)
	{
		VBlankIntrWait();
		vid_flip();
		m4_plot(8, 8, 3);
		m4_plot(8, 10, 3);
		m4_plot(8, 12, 3);
	}
}
