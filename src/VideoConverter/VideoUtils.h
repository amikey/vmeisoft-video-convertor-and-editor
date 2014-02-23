#pragma once

extern void copy_32bpp(uint8_t *dst, int32_t dst_step, uint8_t *src, int32_t src_step, int32_t width, int32_t height);
extern void border_32bpp(uint32_t *dst, int32_t width, int32_t height);
extern void bend_32bpp(uint8_t *dst, int32_t dest_step, uint8_t *src, int32_t width, int32_t height);
extern void gray_32bpp(uint8_t *dst, int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t width, int32_t height);
extern void rect_32bpp(uint8_t *dst, int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t width, int32_t height);
extern void GetResizeMethodRect(ResizeMethod ResizeMethod, int32_t DestWidth, int32_t DestHeight, int32_t SrcWidth, int32_t SrcHeight, Rect &srcRect, Rect &dstRect);

extern uint8_t* LoadFileToMemory(const wchar_t *file, int32_t width, int32_t height);
extern void ChangeAlpha(uint8_t *p, uint32_t a, int32_t pixels);

