#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#include <Windows.h>
#include <cstdint>

namespace capture {
	extern int w, h;

	extern void init();
	extern void destroy();
	extern HBITMAP capture_screen();
	extern void get_bitmap_info(BITMAPINFO* info, uint8_t** data);
}

#endif