#include "imgcomp/capture.hpp"

#include <Windows.h>
#include <cstdint>
#include <chrono>
#include <iostream>

namespace capture {
	HBITMAP bitmap;
	HDC desktop_dc;
	HDC capture_dc;
	int w, h;

	void init() {
		w = GetSystemMetrics(SM_CXSCREEN);
		h = GetSystemMetrics(SM_CYSCREEN);

		desktop_dc = GetDC(NULL);
		capture_dc = CreateCompatibleDC(desktop_dc);

		bitmap = CreateCompatibleBitmap(desktop_dc, w, h);
		SelectObject(capture_dc, bitmap);
	}

	void destroy() {
		ReleaseDC(NULL, desktop_dc);
		DeleteDC(capture_dc);
		DeleteObject(bitmap);
	}

	HBITMAP capture_screen() {
		BitBlt(capture_dc, 0, 0, w, h, desktop_dc, 0, 0, SRCCOPY);
		return bitmap;
	}

	void get_bitmap_info(BITMAPINFO* info, uint8_t** data) {
		info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info->bmiHeader.biBitCount = 0;

		// populate the info
		GetDIBits(capture_dc, bitmap, 0, 0, nullptr, &*info, DIB_RGB_COLORS);
		// create a new array for the raw bitmap data
		*data = new uint8_t[info->bmiHeader.biSizeImage];
		// load with data
		info->bmiHeader.biCompression = 0;
		GetDIBits(capture_dc, bitmap, 0, info->bmiHeader.biHeight, *data, &*info, DIB_RGB_COLORS);
	}
}