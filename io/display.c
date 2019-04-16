// File author is Ítalo Lima Marconato Matias
//
// Created on July 18 of 2018, at 21:12 BRT
// Last edited on April 15 of 2019, at 22:39 BRT

#define __CHICAGO_DISPLAY__

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/debug.h>
#include <chicago/display.h>
#include <chicago/mm.h>
#include <chicago/string.h>

UIntPtr DispFrameBuffer = 0;
UIntPtr DispBackBuffer = 0;
UInt8 DispBytesPerPixel = 0;
UIntPtr DispWidth = 0;
UIntPtr DispHeight = 0;
UInt32 DispProgressBar = 0;

UIntPtr DispGetFrameBuffer(Void) {
	return DispFrameBuffer;
}

UInt8 DispGetBytesPerPixel(Void) {
	return DispBytesPerPixel;
}

UIntPtr DispGetWidth(Void) {
	return DispWidth;
}

UIntPtr DispGetHeight(Void) {
	return DispHeight;
}

Void DispExtractARGB(UIntPtr c, PUInt8 a, PUInt8 r, PUInt8 g, PUInt8 b) {
	if (a != Null) {
		*a = (UInt8)((c >> 24) & 0xFF);
	}
	
	if (r != Null) {
		*r = (UInt8)((c >> 16) & 0xFF);
	}
	
	if (g != Null) {
		*g = (UInt8)((c >> 8) & 0xFF);
	}
	
	if (b != Null) {
		*b = (UInt8)(c & 0xFF);
	}
}

Void DispRefresh(Void) {
	StrCopyMemory((PUInt8)DispFrameBuffer, (PUInt8)DispBackBuffer, DispWidth * DispHeight * DispBytesPerPixel);
}

Void DispClearScreen(UIntPtr c) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a;																													// Big endian...
	UInt8 a;
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	DispExtractARGB(c, &a, &r, &g, &b);																							// Extract the ARGB values
	
	c = a | (r << 8) | (g << 16) | (b << 24);																					// Convert to little endian
#endif
	
	if (DispBytesPerPixel == 3) {																								// Fill the screen with the color c
		StrSetMemory24((PUInt32)DispBackBuffer, c, DispWidth * DispHeight);
	} else if (DispBytesPerPixel == 4) {
		StrSetMemory32((PUInt32)DispBackBuffer, c, DispWidth * DispHeight);
	}
}

Void DispScrollScreen(UIntPtr c) {
	UIntPtr lsz = DispWidth * 16;
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a;																													// Big endian...
	UInt8 a;
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	DispExtractARGB(c, &a, &r, &g, &b);																							// Extract the ARGB values
	
	c = a | (r << 8) | (g << 16) | (b << 24);																					// Convert to little endian
#endif
	
	if (DispBytesPerPixel == 3) {																								// Scroll the screen contents up and clear the last line!
		StrCopyMemory24((PUInt32)DispBackBuffer, (PUInt32)(DispBackBuffer + (lsz * 3)), DispWidth * DispHeight - lsz);
		StrSetMemory24((PUInt32)(DispBackBuffer + (DispWidth * DispHeight * 3) - (lsz * 3)), c, lsz);
	} else if (DispBytesPerPixel == 4) {
		StrCopyMemory32((PUInt32)DispBackBuffer, (PUInt32)(DispBackBuffer + (lsz * 4)), DispWidth * DispHeight - lsz);
		StrSetMemory32((PUInt32)(DispBackBuffer + (DispWidth * DispHeight * 4) - (lsz * 4)), c, lsz);
	}
}

UIntPtr DispGetPixel(UIntPtr x, UIntPtr y) {
	if (x >= DispWidth) {																										// Fix the x and the y if they are bigger than the screen dimensions
		x = DispWidth - 1;
	}
	
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
	UIntPtr c = 0;
	
	if (DispBytesPerPixel == 3) {																								// Get the pixel!
		c = *((PUInt16)(DispBackBuffer + (y * (DispWidth * 3)) + (x * 3)));
		*((PUInt8)(((UIntPtr)&c) + 2)) = *((PUInt8)(DispBackBuffer + (y * (DispWidth * 3)) + (x * 3) + 2));
	} else if (DispBytesPerPixel == 4) {
		c = *((PUInt32)(DispBackBuffer + (y * (DispWidth * 4)) + (x * 4)));
	}
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a;																													// Big endian...
	UInt8 a;
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	DispExtractARGB(c, &a, &r, &g, &b);																							// Extract the ARGB values
	
	c = a | (r << 8) | (g << 16) | (b << 24);																					// Convert to big endian
#endif
	
	return c;
}

Void DispPutPixel(UIntPtr x, UIntPtr y, UIntPtr c) {
	if (x >= DispWidth) {																										// Fix the x and the y if they are bigger than the screen dimensions
		x = DispWidth - 1;
	}
	
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a;																													// Big endian...
	UInt8 a;
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	DispExtractARGB(c, &a, &r, &g, &b);																							// Extract the ARGB values
	
	c = a | (r << 8) | (g << 16) | (b << 24);																					// Convert to little endian
#endif
	
	if (DispBytesPerPixel == 3) {																								// Write the pixel!
		*((PUInt16)(DispBackBuffer + (y * (DispWidth * 3)) + (x * 3))) = (UInt16)c;
		*((PUInt8)(DispBackBuffer + (y * (DispWidth * 3)) + (x * 3) + 2)) = (UInt8)(c << 16);
	} else if (DispBytesPerPixel == 4) {
		*((PUInt32)(DispBackBuffer + (y * (DispWidth * 4)) + (x * 4))) = c;
	}
}

Void DispDrawLine(UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c) {
	if (x0 >= DispWidth) {																										// Fix the x and the y if they are bigger than the screen dimensions
		x0 = DispWidth - 1;																										//     Sorry, this function will not have a lot of documentation (not that my other ones are commented in a good way...)
	}
	
	if (y0 >= DispHeight) {
		y0 = DispHeight - 1;
	}
	
	if (x1 >= DispWidth) {
		x1 = DispWidth - 1;
	}
	
	if (y1 >= DispHeight) {
		y1 = DispHeight - 1;
	}
	
	IntPtr dx = x1 - x0;																										// Let's get the line direction
	IntPtr dy = y1 - y0;
	IntPtr sdx = (dx < 0) ? -1 : 1;
	IntPtr sdy = (dy < 0) ? -1 : 1;
	IntPtr px = x0;
	IntPtr py = y0;
	IntPtr x = 0;
	IntPtr y = 0;
	
	dx = sdx * dx + 1;
	dy = sdy * dy + 1;
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a;																													// Big endian...
	UInt8 a;
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	DispExtractARGB(c, &a, &r, &g, &b);																							// Extract the ARGB values
	
	c = a | (r << 8) | (g << 16) | (b << 24);																					// Convert to little endian
#endif
	
	if (dx >= dy) {																												// This line is more horizontal?
		for (x = 0; x < dx; x++) {																								// Yes
			if (DispBytesPerPixel == 3) {
				*((PUInt16)(DispBackBuffer + (py * (DispWidth * 3)) + (px * 3))) = (UInt16)c;
				*((PUInt8)(DispBackBuffer + (py * (DispWidth * 3)) + (px * 3) + 2)) = (UInt8)(c << 16);
			} else if (DispBytesPerPixel == 4) {
				*((PUInt32)(DispBackBuffer + (py * (DispWidth * 4)) + (px * 4))) = c;
			}
			
			y += dy;
			
			if (y >= dx) {
				y -= dx;
				py += sdy;
			}
			
			px += sdx;
		}
	} else {
		for (y = 0; y < dy; y++) {																								// No, so is more vertical
			if (DispBytesPerPixel == 3) {
				*((PUInt16)(DispBackBuffer + (py * (DispWidth * 3)) + (px * 3))) = (UInt16)c;
				*((PUInt8)(DispBackBuffer + (py * (DispWidth * 3)) + (px * 3) + 2)) = (UInt8)(c << 16);
			} else if (DispBytesPerPixel == 4) {
				*((PUInt32)(DispBackBuffer + (py * (DispWidth * 4)) + (px * 4))) = c;
			}
			
			x += dx;
			
			if (x >= dy) {
				x -= dy;
				px += sdx;
			}
			
			py += sdy;
		}
	}
}

Void DispDrawRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	DispDrawLine(x, y, x, y + h - 1, c);																						// Let's use the DispDrawLine to draw the rectangle (we just need to draw 4 lines)
	DispDrawLine(x, y, x + w - 1, y, c);
	DispDrawLine(x, y + h - 1, x + w - 1, y + h - 1, c);
	DispDrawLine(x + w - 1, y, x + w - 1, y + h - 1, c);
}

Void DispFillRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	if (x >= DispWidth) {																										// Fix the x and the y if they are bigger than the screen dimensions
		x = DispWidth - 1;
	}
	
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
	if ((x + w) > DispWidth) {																									// Fix the width and height of our rectangle if they are bigger than the screen dimensions
		w = DispWidth - x;
	}
	
	if ((y + h) > DispHeight) {
		h = DispHeight - y;
	}
	
	PUInt32 fb = (PUInt32)(DispBackBuffer + (y * (DispWidth * DispBytesPerPixel)) + (x * DispBytesPerPixel));
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a;																													// Big endian...
	UInt8 a;
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	DispExtractARGB(c, &a, &r, &g, &b);																							// Extract the ARGB values
	
	c = a | (r << 8) | (g << 16) | (b << 24);																					// Convert to little endian
#endif
	
	for (UIntPtr i = 0; i < h; i++) {																							// And fill our rectangle
		if (DispBytesPerPixel == 3) {
			StrSetMemory24(fb, c, w);
		} else if (DispBytesPerPixel == 4) {
			StrSetMemory32(fb, c, w);
		}
		
		fb += DispWidth;
	}
}

Void DispDrawRoundedRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c) {
	if (r == 0) {																												// Radius is 0?
		DispDrawRectangle(x, y, w, h, c);																						// Yes, so just draw an rectangle
		return;
	}
	
	IntPtr f = 1 - r;
	IntPtr dfx = 1;
	IntPtr dfy = -2 * r;
	UIntPtr xx = 0;
	UIntPtr yy = r;
	
	if (w > 0) {
		w--;
	}
	
	if (h > 0) {
		h--;
	}
	
	while (xx < yy) {
		if (f >= 0) {
			yy--;
			dfy += 2;
			f += dfy;
		}
		
		xx++;
		dfx += 2;
		f += dfx;
		
		DispPutPixel(x - xx + r, y + h + yy - r, c);																			// Bottom left
		DispPutPixel(x - yy + r, y + h + xx - r, c);
		DispPutPixel(x - xx + r, y - yy + r, c);																				// Top left
		DispPutPixel(x - yy + r, y - xx + r, c);
		DispPutPixel(x + w + xx - r, y + h + yy - r, c);																		// Bottom right
		DispPutPixel(x + w + yy - r, y + h + xx - r, c);
		DispPutPixel(x + w + xx - r, y - yy + r, c);																			// Top right
		DispPutPixel(x + w + yy - r, y - xx + r, c);
	}
	
	DispDrawLine(x + r, y, x + w - r, y, c);																					// Draw the top line
	DispDrawLine(x + r, y + h, x + w - r, y + h, c);																			// The bottom one
	DispDrawLine(x, y + r, x, y + h - r, c);																					// The left one
	DispDrawLine(x + w, y + r, x + w, y + h - r, c);																			// And, the right one!
}

Void DispFillRoundedRectangle(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c) {
	if (r == 0) {																												// Radius is 0?
		DispFillRectangle(x, y, w, h, c);																						// Yes, so just fill an rectangle
		return;
	}
	
	IntPtr f = 1 - r;
	IntPtr dfx = 1;
	IntPtr dfy = -2 * r;
	UIntPtr xx = 0;
	UIntPtr yy = r;
	
	if (w > 0) {
		w--;
	}
	
	if (h > 0) {
		h--;
	}
	
	while (xx < yy) {
		if (f >= 0) {
			yy--;
			dfy += 2;
			f += dfy;
		}
		
		xx++;
		dfx += 2;
		f += dfx;
		
		DispDrawLine(x - xx + r, y + h + yy - r, x - xx + r, y - yy + r, c);													// Left
		DispDrawLine(x - yy + r, y + h + xx - r, x - yy + r, y - xx + r, c);
		DispDrawLine(x + w + xx - r, y + h + yy - r, x + w + xx - r, y - yy + r, c);											// Right
		DispDrawLine(x + w + yy - r, y + h + xx - r, x + w + yy - r, y - xx + r, c);
	}
	
	DispFillRectangle(x + r, y, w - r * 2 + 1, h + 1, c);																		// Fill the center rectangle
	DispDrawLine(x, y + r, x, y + h - r, c);																					// The left line
	DispDrawLine(x + w, y + r, x + w, y + h - r, c);																			// And the right one
}

static UIntPtr DispBlend(UIntPtr a, UIntPtr b) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt8 a1;																													// Big endian...
	UInt8 r1;
	UInt8 g1;
	UInt8 b1;
	
	DispExtractARGB(a, &a1, &r1, &g1, &b1);																						// Extract the ARGB values for the A value
	a = a1 | (r1 << 8) | (g1 << 16) | (b1 << 24);																				// Convert to little endian
	
	DispExtractARGB(b, &a1, &r1, &g1, &b1);																						// Extract the ARGB values for the B value
	b = a1 | (r1 << 8) | (g1 << 16) | (b1 << 24);																				// Convert to little endian
#endif
	
	UIntPtr ba = (b >> 24) & 0xFF;																								// Extract the alpha value from the B val
	
	if (ba == 0) {																												// Transparent?
		return a;																												// Yes, just return the A val
	}
	
	UIntPtr rb = (((b & 0xFF00FF) * ba) + ((a & 0xFF00FF) * (0xFF - ba))) & 0xFF00FF00;											// Extract and blend the red and blue (0xFF00FF)
	UIntPtr g = (((b & 0xFF00) * ba) + ((a & 0xFF00) * (0xFF - ba))) & 0xFF0000;												// Extract and blend the green (0xFF0000)
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    UIntPtr c = (b & 0xFF000000) | ((rb | g) >> 8);																				// Big endian... prepare the result (in little endian)
	
	DispExtractARGB(c, &a1, &r1, &g1, &b1);																						// Extract the ARGB value
	c = a1 | (r1 << 8) | (g1 << 16) | (b1 << 24);																				// Convert to big endian
	
	return c;																													// And return
#else
	return (b & 0xFF000000) | ((rb | g) >> 8);																					// Return the blended color
#endif
}

Void DispCopy(UIntPtr src, UInt8 srcbpp, UIntPtr srcx, UIntPtr srcy, UIntPtr srcw, UIntPtr srch, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UInt8 mode) {
	if (((mode & DISP_MODE_COPY) != DISP_MODE_COPY) && ((mode & DISP_MODE_BLEND) != DISP_MODE_BLEND)) {							// Invalid mode?
		return;																													// Yes :(
	} else if (src == 0) {																										// src == framebuffer?
		src = DispBackBuffer;																									// Yes :)
		srcbpp = DispBytesPerPixel;
		srcw = DispWidth;
		srch = DispHeight;
	} else if (srcbpp == 0) {																									// Invalid bpp?
		return;																													// Yes :(
	}
	
	if (srcx >= srcw) {																											// Fix the srcx and the srcy if they are bigger than the bitmap dimensions
		srcx = srcw - 1;
	}
	
	if (srcy >= srch) {
		srcy = srch - 1;
	}
	
	if ((srcx + w) > srcw) {																									// Fix the width and height if they are bigger than the bitmap dimensions
		w = srcw - srcx;
	}
	
	if ((srcy + h) > srch) {
		h = srch - srcy;
	}
	
	if (x >= DispWidth) {																										// Fix the x and the y if they are bigger than the screen dimensions
		x = DispWidth - 1;
	}
	
	if (y >= DispHeight) {
		y = DispHeight - 1;
	}
	
	if ((x + w) > DispWidth) {																									// Fix the width and height if they are bigger than the screen dimensions
		w = DispWidth - x;
	}
	
	if ((y + h) > DispHeight) {
		h = DispHeight - y;
	}
	
	PUInt8 srcb = (PUInt8)(src + (srcy * srcw * srcbpp) + (srcx * srcbpp));
	PUInt8 dstb = (PUInt8)(DispBackBuffer + (y * DispWidth * DispBytesPerPixel) + (x * DispBytesPerPixel));
	
	if ((srcbpp == DispBytesPerPixel) && ((mode & DISP_MODE_COPY) == DISP_MODE_COPY)) {											// Same BPP and mode is DISP_MODE_COPY?
		Boolean invert = (mode & DISP_MODE_INVERT) == DISP_MODE_INVERT;															// Yes, first let's see if we need to copy the lines in the reverse order
		
		for (UIntPtr ys = invert ? h + 1 : 0, yd = 0; invert ? ys != 0 : ys < h; invert ? ys-- : ys++, yd++) {				// Now, let's copy them!			
			StrCopyMemory(&dstb[yd * DispWidth * srcbpp], &srcb[ys * srcw * srcbpp], w * srcbpp);
		}
	} else if ((mode & DISP_MODE_COPY) == DISP_MODE_COPY) {																		// mode is DISP_MODE_COPY?
		Boolean invert = (mode & DISP_MODE_INVERT) == DISP_MODE_INVERT;															// Yes, first let's see if we need to copy the lines in the reverse order
		
		for (UIntPtr ys = invert ? h + 1 : 0, yd = 0; invert ? ys != 0 : ys < h; invert ? ys-- : ys++, yd++) {				// Now, let's copy them!
			PUInt8 row = &srcb[ys * srcw * srcbpp];
			
			for (UIntPtr xx = 0; xx < w; xx++) {
				if (srcbpp == 3) {
					DispPutPixel(x + xx, y + yd, ((row[xx * 3 + 2] << 16) | (row[xx * 3 + 1] << 8) | row[xx * 3]) | 0xFF000000);
				} else if (srcbpp == 4) {
					DispPutPixel(x + xx, y + yd, *((PUInt32)(row + xx * 4)));
				}
			}
		}
	} else {																													// mode should be DISP_MODE_BLEND
		Boolean invert = (mode & DISP_MODE_INVERT) == DISP_MODE_INVERT;															// Yes, first let's see if we need to copy the lines in the reverse order
		
		for (UIntPtr ys = invert ? srch + 1 : 0, yd = 0; invert ? ys != 0 : ys < h; invert ? ys-- : ys++, yd++) {				// Now, let's copy them!
			PUInt8 row = &srcb[ys * srcw * srcbpp];
			
			for (UIntPtr xx = 0; xx < w; xx++) {
				UIntPtr a = DispGetPixel(x + xx, y + yd);																		// Get the A pixel
				UIntPtr b = 0;																									// Get the B pixel
				
				if (srcbpp == 3) {
					b = ((row[xx * 3 + 2] << 16) | (row[xx * 3 + 1] << 8) | row[xx * 3]) | 0xFF000000;
				} else if (srcbpp == 4) {
					b = *((PUInt32)(row + xx * 4));
				}
				
				DispPutPixel(x + xx, y + yd, DispBlend(a, b));																	// Alpha blend the colors and put the pixel
			}
		}
	}
}

Void DispDrawBitmap(PUInt8 bmp, UIntPtr x, UIntPtr y) {
	PBmpHeader hdr = (PBmpHeader)bmp;																							// Get the BMP file header
	PBmpInfoHeader ihdr = (PBmpInfoHeader)(((UIntPtr)bmp) + sizeof(BmpHeader));													// The BMP info header
	UIntPtr src = ((UIntPtr)bmp) + hdr->off;																					// And the image itself
	
	DispCopy(src, 3, 0, 0, ihdr->width, ihdr->height, x, y, ihdr->width, ihdr->height, DISP_MODE_COPY | DISP_MODE_INVERT);		// Now just use the DispCopy function
}

static Void DispWriteFormatedCharacter(PUIntPtr x, PUIntPtr y, UIntPtr bg, UIntPtr fg, WChar data) {
	switch (data) {
		case '\n': {																											// Line feed
			*y += 16;
			break;
		}
		case '\r': {																											// Carriage return
			*x = 0;
			break;
		}
		case '\t': {																											// Tab
			*x = (*x + 32) & ~31;
			break;
		}
		default: {																												// Normal character (probably)
			PPCScreenFont font = (PPCScreenFont)DispFontStart;
			PUInt8 glyph = (PUInt8)(DispFontStart + font->hdr_size + (data * font->bytes_per_glyph));
			
			for (UIntPtr i = 0; i < 16; i++) {
				UIntPtr mask = 1 << 7;
				
				for (UIntPtr j = 0; j < 8; j++) {
					DispPutPixel(*x + j, *y + i, ((UIntPtr)*glyph) & (mask) ? fg : bg);
					mask >>= 1;
				}
				
				glyph++;
			}
			
			*x += 8;
			
			break;
		}
	}
}

static Void DispWriteFormatedString(PUIntPtr x, PUIntPtr y, UIntPtr bg, UIntPtr fg, PWChar data) {
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {
		DispWriteFormatedCharacter(x, y, bg, fg, data[i]);
	}
}

static Void DispWriteFormatedInteger(PUIntPtr x, PUIntPtr y, UIntPtr bg, UIntPtr fg, UIntPtr data, UInt8 base) {
	if (data == 0) {
		DispWriteFormatedCharacter(x, y, bg, fg, '0');
		return;
	}
	
	static WChar buf[32] = { 0 };
	Int i = 30;
	
	for (; data && i; i--, data /= base) {
		buf[i] = "0123456789ABCDEF"[data % base];
	}
	
	DispWriteFormatedString(x, y, bg, fg, &buf[i + 1]);
}

Void DispWriteFormated(UIntPtr x, UIntPtr y, UIntPtr bg, UIntPtr fg, Const PWChar data, ...) {
	VariadicList va;
	VariadicStart(va, data);																									// Let's start our va list with the arguments provided by the user (if any)
	
	UIntPtr cx = x;
	UIntPtr cy = y;
	
	for (UInt32 i = 0; data[i] != '\0'; i++) {
		if (data[i] != '%') {																									// It's an % (integer, string, character or other)?
			DispWriteFormatedCharacter(&cx, &cy, bg, fg, data[i]);																// Nope
		} else {
			switch (data[++i]) {																								// Yes! So let's parse it!
			case 's': {																											// String
				DispWriteFormatedString(&cx, &cy, bg, fg, (PWChar)VariadicArg(va, PWChar));
				break;
			}
			case 'c': {																											// Character
				DispWriteFormatedCharacter(&cx, &cy, bg, fg, (WChar)VariadicArg(va, Int));
				break;
			}
			case 'd': {																											// Decimal Number
				DispWriteFormatedInteger(&cx, &cy, bg, fg, (UIntPtr)VariadicArg(va, UIntPtr), 10);
				break;
			}
			case 'x': {																											// Hexadecimal Number
				DispWriteFormatedInteger(&cx, &cy, bg, fg, (UIntPtr)VariadicArg(va, UIntPtr), 16);
				break;
			}
			default: {																											// None of the others...
				DispWriteFormatedCharacter(&cx, &cy, bg, fg, data[i]);
				break;
			}
			}
		}
	}
	
	VariadicEnd(va);
}

Void DispIncrementProgessBar(Void) {
	if (DispProgressBar == 0) {																									// This first one is different
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 3, DispHeight - 24, 1, 9, 0x0000A8);
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 4, DispHeight - 25, 1, 11, 0x0000A8);
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 5, DispHeight - 26, 1, 13, 0x0000A8);
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 6, DispHeight - 27, 11, 15, 0x0000A8);
		DispProgressBar += 10;
	} else if (DispProgressBar < 90) {
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2), DispHeight - 27, 17, 15, 0x0000A8);						// The others are... normal
		DispProgressBar += 10;
	} else if (DispProgressBar < 100) {
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2), DispHeight - 27, 15, 15, 0x0000A8);						// The last one is also different
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 15, DispHeight - 26, 1, 13, 0x0000A8);
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 16, DispHeight - 25, 1, 11, 0x0000A8);
		DispFillRectangle(DispWidth / 2 - 100 + (DispProgressBar * 2) + 17, DispHeight - 24, 1, 9, 0x0000A8);
		DispProgressBar += 10;
	}
	
	DispRefresh();
}

Void DispFillProgressBar(Void) {
	while (DispProgressBar < 100) {																								// Just fill the progress bar until it's 100%
		DispIncrementProgessBar();
	}
}

Void DispDrawProgessBar(Void) {
	DispDrawRoundedRectangle(DispWidth / 2 - 100, DispHeight - 30, 201, 21, 7, 0xFFFFFF);										// Draw the progress bar border
	DispWriteFormated(DispWidth / 2 - 56, 7, 0x000000, 0xFFFFFF, L"Starting up...");											// And the "Starting..." text
	DispDrawBitmap(DispBootSplashImage, DispWidth / 2 - 150, DispHeight / 2 - 50);												// Draw the logo
	DispRefresh();
}

Void DispInit(UIntPtr w, UIntPtr h, UIntPtr bpp, UIntPtr fb) {
	if ((bpp != 3) && (bpp != 4)) {																								// We only support 24 and 32 bpp
		DbgWriteFormated("PANIC! Couldn't init the display\r\n");
		ArchHalt();																												// Halt
	}
	
	DispFrameBuffer = MemAAllocate(w * h * bpp, MM_PAGE_SIZE);																	// Alloc some virt space for the frame buffer
	DispBackBuffer = MemAllocate(w * h * bpp);
	DispBytesPerPixel = bpp;
	DispWidth = w;
	DispHeight = h;
	
	if (DispFrameBuffer == 0 || DispBackBuffer == 0) {
		DbgWriteFormated("PANIC! Couldn't init the display\r\n");																// Failed...
		ArchHalt();																												// Halt
	}
	
	for (UIntPtr i = 0; i < DispWidth * DispHeight * bpp; i += MM_PAGE_SIZE) {													// Let's map the frame buffer to the virtual memory!
		MmDereferencePage(MmGetPhys(DispFrameBuffer + i));																		// MemAAllocate allocated some phys addr as well, free it
		
		if (!MmMap(DispFrameBuffer + i, fb + i, MM_MAP_KDEF)) {
			DbgWriteFormated("PANIC! Couldn't init the display\r\n");
			ArchHalt();																											// Halt
		}
	}
	
	DispClearScreen(0xFF000000);																								// Clear the screen
	DispRefresh();
}
