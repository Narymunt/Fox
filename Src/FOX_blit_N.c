// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:
// - poprawic mapowanie pixeli na 8bpp (jest zaznaczone) 01


#include <stdio.h>

#include "FOX_types.h"
#include "FOX_video.h"
#include "blit.h"
#include "FOX_byteorder.h"

// sprawdzanie flag procesora

#define MMX_CPU		0x800000

#define CPU_Flags()	0L


// blitowanie pomiedzy surface


// zalezy od endian

#if ( FOX_BYTEORDER == FOX_LIL_ENDIAN )
	#define HI	1
	#define LO	0
#else /* ( FOX_BYTEORDER == FOX_BIG_ENDIAN ) */
	#define HI	0
	#define LO	1
#endif

// rgb888 => 332

#define RGB888_RGB332(dst, src) { \
	dst = (((src)&0x00E00000)>>16)| \
	      (((src)&0x0000E000)>>11)| \
	      (((src)&0x000000C0)>>6); \
}

static void Blit_RGB888_index8(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	int width, height;
	Uint32 *src;
	const Uint8 *map;
	Uint8 *dst;
	int srcskip, dstskip;

	// ustaw podstawowe zmienne
	
	width = info->d_width;
	height = info->d_height;
	src = (Uint32 *)info->s_pixels;
	srcskip = info->s_skip/4;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = info->table;

	if ( map == NULL ) 
	{
		while ( height-- ) 
		{
#ifdef USE_DUFFS_LOOP
			DUFFS_LOOP(
				RGB888_RGB332(*dst++, *src);
			, width);
#else
			for ( c=width/4; c; --c ) 
			{
				
				++src;
				RGB888_RGB332(*dst++, *src);
				++src;
				RGB888_RGB332(*dst++, *src);
				++src;
				RGB888_RGB332(*dst++, *src);
				++src;
			}
			switch ( width & 3 ) {
				case 3:
					RGB888_RGB332(*dst++, *src);
					++src;
				case 2:
					RGB888_RGB332(*dst++, *src);
					++src;
				case 1:
					RGB888_RGB332(*dst++, *src);
					++src;
			}
#endif /* USE_DUFFS_LOOP */
			src += srcskip;
			dst += dstskip;
		}
	} else {
		int pixel;

		while ( height-- ) {
#ifdef USE_DUFFS_LOOP
			DUFFS_LOOP(
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
			, width);
#else
			for ( c=width/4; c; --c ) 
			{
			
				// skracaj pixel

				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
			}
			switch ( width & 3 ) {
				case 3:
					RGB888_RGB332(pixel, *src);
					*dst++ = map[pixel];
					++src;
				case 2:
					RGB888_RGB332(pixel, *src);
					*dst++ = map[pixel];
					++src;
				case 1:
					RGB888_RGB332(pixel, *src);
					*dst++ = map[pixel];
					++src;
			}
#endif /* USE_DUFFS_LOOP */
			src += srcskip;
			dst += dstskip;
		}
	}
}

// rgb 888=>555

#define RGB888_RGB555(dst, src) { \
	*(Uint16 *)(dst) = (((*src)&0x00F80000)>>9)| \
	                   (((*src)&0x0000F800)>>6)| \
	                   (((*src)&0x000000F8)>>3); \
}

#define RGB888_RGB555_TWO(dst, src) { \
	*(Uint32 *)(dst) = (((((src[HI])&0x00F80000)>>9)| \
	                     (((src[HI])&0x0000F800)>>6)| \
	                     (((src[HI])&0x000000F8)>>3))<<16)| \
	                     (((src[LO])&0x00F80000)>>9)| \
	                     (((src[LO])&0x0000F800)>>6)| \
	                     (((src[LO])&0x000000F8)>>3); \
}

static void Blit_RGB888_RGB555(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	int width, height;
	Uint32 *src;
	Uint16 *dst;
	int srcskip, dstskip;

	// ustaw podstawowe zmienne

	width = info->d_width;
	height = info->d_height;
	src = (Uint32 *)info->s_pixels;
	srcskip = info->s_skip/4;
	dst = (Uint16 *)info->d_pixels;
	dstskip = info->d_skip/2;

#ifdef USE_DUFFS_LOOP
	while ( height-- ) {
		DUFFS_LOOP(
			RGB888_RGB555(dst, src);
			++src;
			++dst;
		, width);
		src += srcskip;
		dst += dstskip;
	}
#else
	
	// alignacja na 4 bajtach

	if ( (long)dst & 0x03 ) 
	{
		
		if ( width == 0 ) 
		{
			return;
		}
		--width;

		while ( height-- ) 
		{
			
			RGB888_RGB555(dst, src);
			++src;
			++dst;

			// 4 pixele w chunk
			
			for ( c=width/4; c; --c ) 
			{
				RGB888_RGB555_TWO(dst, src);
				src += 2;
				dst += 2;
				RGB888_RGB555_TWO(dst, src);
				src += 2;
				dst += 2;
			}
			
			switch (width & 3) {
				case 3:
					RGB888_RGB555(dst, src);
					++src;
					++dst;
				case 2:
					RGB888_RGB555_TWO(dst, src);
					src += 2;
					dst += 2;
					break;
				case 1:
					RGB888_RGB555(dst, src);
					++src;
					++dst;
					break;
			}
			src += srcskip;
			dst += dstskip;
		}
	} 
	else 
	{ 
		while ( height-- ) 
		{
			// kopiuj w 4 pixelowe chunki
			
			for ( c=width/4; c; --c ) 
			{
				RGB888_RGB555_TWO(dst, src);
				src += 2;
				dst += 2;
				RGB888_RGB555_TWO(dst, src);
				src += 2;
				dst += 2;
			}
			
			switch (width & 3) {
				case 3:
					RGB888_RGB555(dst, src);
					++src;
					++dst;
				case 2:
					RGB888_RGB555_TWO(dst, src);
					src += 2;
					dst += 2;
					break;
				case 1:
					RGB888_RGB555(dst, src);
					++src;
					++dst;
					break;
			}
			src += srcskip;
			dst += dstskip;
		}
	}
#endif /* USE_DUFFS_LOOP */
}

// rozklad 8bpp

#define RGB888_RGB565(dst, src) { \
	*(Uint16 *)(dst) = (((*src)&0x00F80000)>>8)| \
	                   (((*src)&0x0000FC00)>>5)| \
	                   (((*src)&0x000000F8)>>3); \
}
#define RGB888_RGB565_TWO(dst, src) { \
	*(Uint32 *)(dst) = (((((src[HI])&0x00F80000)>>8)| \
	                     (((src[HI])&0x0000FC00)>>5)| \
	                     (((src[HI])&0x000000F8)>>3))<<16)| \
	                     (((src[LO])&0x00F80000)>>8)| \
	                     (((src[LO])&0x0000FC00)>>5)| \
	                     (((src[LO])&0x000000F8)>>3); \
}

static void Blit_RGB888_RGB565(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	int width, height;
	Uint32 *src;
	Uint16 *dst;
	int srcskip, dstskip;
	
	// poczatkowe ustawienia

	width = info->d_width;
	height = info->d_height;
	src = (Uint32 *)info->s_pixels;
	srcskip = info->s_skip/4;
	dst = (Uint16 *)info->d_pixels;
	dstskip = info->d_skip/2;

#ifdef USE_DUFFS_LOOP
	while ( height-- ) {
		DUFFS_LOOP(
			RGB888_RGB565(dst, src);
			++src;
			++dst;
		, width);
		src += srcskip;
		dst += dstskip;
	}
#else
	
	// alignuj na granicy 4 bajtow
	
	if ( (long)dst & 0x03 ) 
	{
		// nie rob
		if ( width == 0 ) 
		{
			return;
		}
		--width;

		while ( height-- ) 
		{
			
			RGB888_RGB565(dst, src);
			++src;
			++dst;

			// 4 pixelowe chunki
			
			for ( c=width/4; c; --c ) 
			{
				RGB888_RGB565_TWO(dst, src);
				src += 2;
				dst += 2;
				RGB888_RGB565_TWO(dst, src);
				src += 2;
				dst += 2;
			}
			
			// cos pozostalo ? 

			switch (width & 3) {
				case 3:
					RGB888_RGB565(dst, src);
					++src;
					++dst;
				case 2:
					RGB888_RGB565_TWO(dst, src);
					src += 2;
					dst += 2;
					break;
				case 1:
					RGB888_RGB565(dst, src);
					++src;
					++dst;
					break;
			}
			src += srcskip;
			dst += dstskip;
		}
	}
	else 
	{ 
		while ( height-- ) 
		{
			
			// kopiuj 4 pixelowe chunki 

			for ( c=width/4; c; --c ) 
			{
				RGB888_RGB565_TWO(dst, src);
				src += 2;
				dst += 2;
				RGB888_RGB565_TWO(dst, src);
				src += 2;
				dst += 2;
			}
			
			// cos pozostalo ?	

			switch (width & 3) {
				case 3:
					RGB888_RGB565(dst, src);
					++src;
					++dst;
				case 2:
					RGB888_RGB565_TWO(dst, src);
					src += 2;
					dst += 2;
					break;
				case 1:
					RGB888_RGB565(dst, src);
					++src;
					++dst;
					break;
			}
			src += srcskip;
			dst += dstskip;
		}
	}
#endif /* USE_DUFFS_LOOP */
}

// pixel 565=>888

#if ( FOX_BYTEORDER == FOX_LIL_ENDIAN )
	#define RGB565_32(dst, src, map) (map[src[0]*2] + map[src[1]*2+1])
#else /* ( FOX_BYTEORDER == FOX_BIG_ENDIAN ) */
	#define RGB565_32(dst, src, map) (map[src[1]*2] + map[src[0]*2+1])
#endif

static void Blit_RGB565_32(FOX_BlitInfo *info, const Uint32 *map)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	int width, height;
	Uint8 *src;
	Uint32 *dst;
	int srcskip, dstskip;

	// ustaw podstawowe zmienne
	
	width = info->d_width;
	height = info->d_height;
	src = (Uint8 *)info->s_pixels;
	srcskip = info->s_skip;
	dst = (Uint32 *)info->d_pixels;
	dstskip = info->d_skip/4;

#ifdef USE_DUFFS_LOOP
	while ( height-- ) {
		DUFFS_LOOP(
		{
			*dst++ = RGB565_32(dst, src, map);
			src += 2;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
#else
	while ( height-- ) 
	{
		
		// ustaw w 4 pixelowe chunki
		
		for ( c=width/4; c; --c ) 
		{
			*dst++ = RGB565_32(dst, src, map);
			src += 2;
			*dst++ = RGB565_32(dst, src, map);
			src += 2;
			*dst++ = RGB565_32(dst, src, map);
			src += 2;
			*dst++ = RGB565_32(dst, src, map);
			src += 2;
		}

		// cos pozostalo		

		switch (width & 3) {
			case 3:
				*dst++ = RGB565_32(dst, src, map);
				src += 2;
			case 2:
				*dst++ = RGB565_32(dst, src, map);
				src += 2;
			case 1:
				*dst++ = RGB565_32(dst, src, map);
				src += 2;
				break;
		}
		src += srcskip;
		dst += dstskip;
	}
#endif /* USE_DUFFS_LOOP */
}

// przeliczone tablice 

static const Uint32 RGB565_ARGB8888_LUT[512] = {
		0x00000000, 0xff000000, 0x00000008, 0xff002000,
		0x00000010, 0xff004000, 0x00000018, 0xff006100,
		0x00000020, 0xff008100, 0x00000029, 0xff00a100,
		0x00000031, 0xff00c200, 0x00000039, 0xff00e200,
		0x00000041, 0xff080000, 0x0000004a, 0xff082000,
		0x00000052, 0xff084000, 0x0000005a, 0xff086100,
		0x00000062, 0xff088100, 0x0000006a, 0xff08a100,
		0x00000073, 0xff08c200, 0x0000007b, 0xff08e200,
		0x00000083, 0xff100000, 0x0000008b, 0xff102000,
		0x00000094, 0xff104000, 0x0000009c, 0xff106100,
		0x000000a4, 0xff108100, 0x000000ac, 0xff10a100,
		0x000000b4, 0xff10c200, 0x000000bd, 0xff10e200,
		0x000000c5, 0xff180000, 0x000000cd, 0xff182000,
		0x000000d5, 0xff184000, 0x000000de, 0xff186100,
		0x000000e6, 0xff188100, 0x000000ee, 0xff18a100,
		0x000000f6, 0xff18c200, 0x000000ff, 0xff18e200,
		0x00000400, 0xff200000, 0x00000408, 0xff202000,
		0x00000410, 0xff204000, 0x00000418, 0xff206100,
		0x00000420, 0xff208100, 0x00000429, 0xff20a100,
		0x00000431, 0xff20c200, 0x00000439, 0xff20e200,
		0x00000441, 0xff290000, 0x0000044a, 0xff292000,
		0x00000452, 0xff294000, 0x0000045a, 0xff296100,
		0x00000462, 0xff298100, 0x0000046a, 0xff29a100,
		0x00000473, 0xff29c200, 0x0000047b, 0xff29e200,
		0x00000483, 0xff310000, 0x0000048b, 0xff312000,
		0x00000494, 0xff314000, 0x0000049c, 0xff316100,
		0x000004a4, 0xff318100, 0x000004ac, 0xff31a100,
		0x000004b4, 0xff31c200, 0x000004bd, 0xff31e200,
		0x000004c5, 0xff390000, 0x000004cd, 0xff392000,
		0x000004d5, 0xff394000, 0x000004de, 0xff396100,
		0x000004e6, 0xff398100, 0x000004ee, 0xff39a100,
		0x000004f6, 0xff39c200, 0x000004ff, 0xff39e200,
		0x00000800, 0xff410000, 0x00000808, 0xff412000,
		0x00000810, 0xff414000, 0x00000818, 0xff416100,
		0x00000820, 0xff418100, 0x00000829, 0xff41a100,
		0x00000831, 0xff41c200, 0x00000839, 0xff41e200,
		0x00000841, 0xff4a0000, 0x0000084a, 0xff4a2000,
		0x00000852, 0xff4a4000, 0x0000085a, 0xff4a6100,
		0x00000862, 0xff4a8100, 0x0000086a, 0xff4aa100,
		0x00000873, 0xff4ac200, 0x0000087b, 0xff4ae200,
		0x00000883, 0xff520000, 0x0000088b, 0xff522000,
		0x00000894, 0xff524000, 0x0000089c, 0xff526100,
		0x000008a4, 0xff528100, 0x000008ac, 0xff52a100,
		0x000008b4, 0xff52c200, 0x000008bd, 0xff52e200,
		0x000008c5, 0xff5a0000, 0x000008cd, 0xff5a2000,
		0x000008d5, 0xff5a4000, 0x000008de, 0xff5a6100,
		0x000008e6, 0xff5a8100, 0x000008ee, 0xff5aa100,
		0x000008f6, 0xff5ac200, 0x000008ff, 0xff5ae200,
		0x00000c00, 0xff620000, 0x00000c08, 0xff622000,
		0x00000c10, 0xff624000, 0x00000c18, 0xff626100,
		0x00000c20, 0xff628100, 0x00000c29, 0xff62a100,
		0x00000c31, 0xff62c200, 0x00000c39, 0xff62e200,
		0x00000c41, 0xff6a0000, 0x00000c4a, 0xff6a2000,
		0x00000c52, 0xff6a4000, 0x00000c5a, 0xff6a6100,
		0x00000c62, 0xff6a8100, 0x00000c6a, 0xff6aa100,
		0x00000c73, 0xff6ac200, 0x00000c7b, 0xff6ae200,
		0x00000c83, 0xff730000, 0x00000c8b, 0xff732000,
		0x00000c94, 0xff734000, 0x00000c9c, 0xff736100,
		0x00000ca4, 0xff738100, 0x00000cac, 0xff73a100,
		0x00000cb4, 0xff73c200, 0x00000cbd, 0xff73e200,
		0x00000cc5, 0xff7b0000, 0x00000ccd, 0xff7b2000,
		0x00000cd5, 0xff7b4000, 0x00000cde, 0xff7b6100,
		0x00000ce6, 0xff7b8100, 0x00000cee, 0xff7ba100,
		0x00000cf6, 0xff7bc200, 0x00000cff, 0xff7be200,
		0x00001000, 0xff830000, 0x00001008, 0xff832000,
		0x00001010, 0xff834000, 0x00001018, 0xff836100,
		0x00001020, 0xff838100, 0x00001029, 0xff83a100,
		0x00001031, 0xff83c200, 0x00001039, 0xff83e200,
		0x00001041, 0xff8b0000, 0x0000104a, 0xff8b2000,
		0x00001052, 0xff8b4000, 0x0000105a, 0xff8b6100,
		0x00001062, 0xff8b8100, 0x0000106a, 0xff8ba100,
		0x00001073, 0xff8bc200, 0x0000107b, 0xff8be200,
		0x00001083, 0xff940000, 0x0000108b, 0xff942000,
		0x00001094, 0xff944000, 0x0000109c, 0xff946100,
		0x000010a4, 0xff948100, 0x000010ac, 0xff94a100,
		0x000010b4, 0xff94c200, 0x000010bd, 0xff94e200,
		0x000010c5, 0xff9c0000, 0x000010cd, 0xff9c2000,
		0x000010d5, 0xff9c4000, 0x000010de, 0xff9c6100,
		0x000010e6, 0xff9c8100, 0x000010ee, 0xff9ca100,
		0x000010f6, 0xff9cc200, 0x000010ff, 0xff9ce200,
		0x00001400, 0xffa40000, 0x00001408, 0xffa42000,
		0x00001410, 0xffa44000, 0x00001418, 0xffa46100,
		0x00001420, 0xffa48100, 0x00001429, 0xffa4a100,
		0x00001431, 0xffa4c200, 0x00001439, 0xffa4e200,
		0x00001441, 0xffac0000, 0x0000144a, 0xffac2000,
		0x00001452, 0xffac4000, 0x0000145a, 0xffac6100,
		0x00001462, 0xffac8100, 0x0000146a, 0xffaca100,
		0x00001473, 0xffacc200, 0x0000147b, 0xfface200,
		0x00001483, 0xffb40000, 0x0000148b, 0xffb42000,
		0x00001494, 0xffb44000, 0x0000149c, 0xffb46100,
		0x000014a4, 0xffb48100, 0x000014ac, 0xffb4a100,
		0x000014b4, 0xffb4c200, 0x000014bd, 0xffb4e200,
		0x000014c5, 0xffbd0000, 0x000014cd, 0xffbd2000,
		0x000014d5, 0xffbd4000, 0x000014de, 0xffbd6100,
		0x000014e6, 0xffbd8100, 0x000014ee, 0xffbda100,
		0x000014f6, 0xffbdc200, 0x000014ff, 0xffbde200,
		0x00001800, 0xffc50000, 0x00001808, 0xffc52000,
		0x00001810, 0xffc54000, 0x00001818, 0xffc56100,
		0x00001820, 0xffc58100, 0x00001829, 0xffc5a100,
		0x00001831, 0xffc5c200, 0x00001839, 0xffc5e200,
		0x00001841, 0xffcd0000, 0x0000184a, 0xffcd2000,
		0x00001852, 0xffcd4000, 0x0000185a, 0xffcd6100,
		0x00001862, 0xffcd8100, 0x0000186a, 0xffcda100,
		0x00001873, 0xffcdc200, 0x0000187b, 0xffcde200,
		0x00001883, 0xffd50000, 0x0000188b, 0xffd52000,
		0x00001894, 0xffd54000, 0x0000189c, 0xffd56100,
		0x000018a4, 0xffd58100, 0x000018ac, 0xffd5a100,
		0x000018b4, 0xffd5c200, 0x000018bd, 0xffd5e200,
		0x000018c5, 0xffde0000, 0x000018cd, 0xffde2000,
		0x000018d5, 0xffde4000, 0x000018de, 0xffde6100,
		0x000018e6, 0xffde8100, 0x000018ee, 0xffdea100,
		0x000018f6, 0xffdec200, 0x000018ff, 0xffdee200,
		0x00001c00, 0xffe60000, 0x00001c08, 0xffe62000,
		0x00001c10, 0xffe64000, 0x00001c18, 0xffe66100,
		0x00001c20, 0xffe68100, 0x00001c29, 0xffe6a100,
		0x00001c31, 0xffe6c200, 0x00001c39, 0xffe6e200,
		0x00001c41, 0xffee0000, 0x00001c4a, 0xffee2000,
		0x00001c52, 0xffee4000, 0x00001c5a, 0xffee6100,
		0x00001c62, 0xffee8100, 0x00001c6a, 0xffeea100,
		0x00001c73, 0xffeec200, 0x00001c7b, 0xffeee200,
		0x00001c83, 0xfff60000, 0x00001c8b, 0xfff62000,
		0x00001c94, 0xfff64000, 0x00001c9c, 0xfff66100,
		0x00001ca4, 0xfff68100, 0x00001cac, 0xfff6a100,
		0x00001cb4, 0xfff6c200, 0x00001cbd, 0xfff6e200,
		0x00001cc5, 0xffff0000, 0x00001ccd, 0xffff2000,
		0x00001cd5, 0xffff4000, 0x00001cde, 0xffff6100,
		0x00001ce6, 0xffff8100, 0x00001cee, 0xffffa100,
		0x00001cf6, 0xffffc200, 0x00001cff, 0xffffe200
};

static void Blit_RGB565_ARGB8888(FOX_BlitInfo *info)
{
    Blit_RGB565_32(info, RGB565_ARGB8888_LUT);
}

// przeliczone wartosci

static const Uint32 RGB565_ABGR8888_LUT[512] = {
		0xff000000, 0x00000000, 0xff080000, 0x00002000,
		0xff100000, 0x00004000, 0xff180000, 0x00006100,
		0xff200000, 0x00008100, 0xff290000, 0x0000a100,
		0xff310000, 0x0000c200, 0xff390000, 0x0000e200,
		0xff410000, 0x00000008, 0xff4a0000, 0x00002008,
		0xff520000, 0x00004008, 0xff5a0000, 0x00006108,
		0xff620000, 0x00008108, 0xff6a0000, 0x0000a108,
		0xff730000, 0x0000c208, 0xff7b0000, 0x0000e208,
		0xff830000, 0x00000010, 0xff8b0000, 0x00002010,
		0xff940000, 0x00004010, 0xff9c0000, 0x00006110,
		0xffa40000, 0x00008110, 0xffac0000, 0x0000a110,
		0xffb40000, 0x0000c210, 0xffbd0000, 0x0000e210,
		0xffc50000, 0x00000018, 0xffcd0000, 0x00002018,
		0xffd50000, 0x00004018, 0xffde0000, 0x00006118,
		0xffe60000, 0x00008118, 0xffee0000, 0x0000a118,
		0xfff60000, 0x0000c218, 0xffff0000, 0x0000e218,
		0xff000400, 0x00000020, 0xff080400, 0x00002020,
		0xff100400, 0x00004020, 0xff180400, 0x00006120,
		0xff200400, 0x00008120, 0xff290400, 0x0000a120,
		0xff310400, 0x0000c220, 0xff390400, 0x0000e220,
		0xff410400, 0x00000029, 0xff4a0400, 0x00002029,
		0xff520400, 0x00004029, 0xff5a0400, 0x00006129,
		0xff620400, 0x00008129, 0xff6a0400, 0x0000a129,
		0xff730400, 0x0000c229, 0xff7b0400, 0x0000e229,
		0xff830400, 0x00000031, 0xff8b0400, 0x00002031,
		0xff940400, 0x00004031, 0xff9c0400, 0x00006131,
		0xffa40400, 0x00008131, 0xffac0400, 0x0000a131,
		0xffb40400, 0x0000c231, 0xffbd0400, 0x0000e231,
		0xffc50400, 0x00000039, 0xffcd0400, 0x00002039,
		0xffd50400, 0x00004039, 0xffde0400, 0x00006139,
		0xffe60400, 0x00008139, 0xffee0400, 0x0000a139,
		0xfff60400, 0x0000c239, 0xffff0400, 0x0000e239,
		0xff000800, 0x00000041, 0xff080800, 0x00002041,
		0xff100800, 0x00004041, 0xff180800, 0x00006141,
		0xff200800, 0x00008141, 0xff290800, 0x0000a141,
		0xff310800, 0x0000c241, 0xff390800, 0x0000e241,
		0xff410800, 0x0000004a, 0xff4a0800, 0x0000204a,
		0xff520800, 0x0000404a, 0xff5a0800, 0x0000614a,
		0xff620800, 0x0000814a, 0xff6a0800, 0x0000a14a,
		0xff730800, 0x0000c24a, 0xff7b0800, 0x0000e24a,
		0xff830800, 0x00000052, 0xff8b0800, 0x00002052,
		0xff940800, 0x00004052, 0xff9c0800, 0x00006152,
		0xffa40800, 0x00008152, 0xffac0800, 0x0000a152,
		0xffb40800, 0x0000c252, 0xffbd0800, 0x0000e252,
		0xffc50800, 0x0000005a, 0xffcd0800, 0x0000205a,
		0xffd50800, 0x0000405a, 0xffde0800, 0x0000615a,
		0xffe60800, 0x0000815a, 0xffee0800, 0x0000a15a,
		0xfff60800, 0x0000c25a, 0xffff0800, 0x0000e25a,
		0xff000c00, 0x00000062, 0xff080c00, 0x00002062,
		0xff100c00, 0x00004062, 0xff180c00, 0x00006162,
		0xff200c00, 0x00008162, 0xff290c00, 0x0000a162,
		0xff310c00, 0x0000c262, 0xff390c00, 0x0000e262,
		0xff410c00, 0x0000006a, 0xff4a0c00, 0x0000206a,
		0xff520c00, 0x0000406a, 0xff5a0c00, 0x0000616a,
		0xff620c00, 0x0000816a, 0xff6a0c00, 0x0000a16a,
		0xff730c00, 0x0000c26a, 0xff7b0c00, 0x0000e26a,
		0xff830c00, 0x00000073, 0xff8b0c00, 0x00002073,
		0xff940c00, 0x00004073, 0xff9c0c00, 0x00006173,
		0xffa40c00, 0x00008173, 0xffac0c00, 0x0000a173,
		0xffb40c00, 0x0000c273, 0xffbd0c00, 0x0000e273,
		0xffc50c00, 0x0000007b, 0xffcd0c00, 0x0000207b,
		0xffd50c00, 0x0000407b, 0xffde0c00, 0x0000617b,
		0xffe60c00, 0x0000817b, 0xffee0c00, 0x0000a17b,
		0xfff60c00, 0x0000c27b, 0xffff0c00, 0x0000e27b,
		0xff001000, 0x00000083, 0xff081000, 0x00002083,
		0xff101000, 0x00004083, 0xff181000, 0x00006183,
		0xff201000, 0x00008183, 0xff291000, 0x0000a183,
		0xff311000, 0x0000c283, 0xff391000, 0x0000e283,
		0xff411000, 0x0000008b, 0xff4a1000, 0x0000208b,
		0xff521000, 0x0000408b, 0xff5a1000, 0x0000618b,
		0xff621000, 0x0000818b, 0xff6a1000, 0x0000a18b,
		0xff731000, 0x0000c28b, 0xff7b1000, 0x0000e28b,
		0xff831000, 0x00000094, 0xff8b1000, 0x00002094,
		0xff941000, 0x00004094, 0xff9c1000, 0x00006194,
		0xffa41000, 0x00008194, 0xffac1000, 0x0000a194,
		0xffb41000, 0x0000c294, 0xffbd1000, 0x0000e294,
		0xffc51000, 0x0000009c, 0xffcd1000, 0x0000209c,
		0xffd51000, 0x0000409c, 0xffde1000, 0x0000619c,
		0xffe61000, 0x0000819c, 0xffee1000, 0x0000a19c,
		0xfff61000, 0x0000c29c, 0xffff1000, 0x0000e29c,
		0xff001400, 0x000000a4, 0xff081400, 0x000020a4,
		0xff101400, 0x000040a4, 0xff181400, 0x000061a4,
		0xff201400, 0x000081a4, 0xff291400, 0x0000a1a4,
		0xff311400, 0x0000c2a4, 0xff391400, 0x0000e2a4,
		0xff411400, 0x000000ac, 0xff4a1400, 0x000020ac,
		0xff521400, 0x000040ac, 0xff5a1400, 0x000061ac,
		0xff621400, 0x000081ac, 0xff6a1400, 0x0000a1ac,
		0xff731400, 0x0000c2ac, 0xff7b1400, 0x0000e2ac,
		0xff831400, 0x000000b4, 0xff8b1400, 0x000020b4,
		0xff941400, 0x000040b4, 0xff9c1400, 0x000061b4,
		0xffa41400, 0x000081b4, 0xffac1400, 0x0000a1b4,
		0xffb41400, 0x0000c2b4, 0xffbd1400, 0x0000e2b4,
		0xffc51400, 0x000000bd, 0xffcd1400, 0x000020bd,
		0xffd51400, 0x000040bd, 0xffde1400, 0x000061bd,
		0xffe61400, 0x000081bd, 0xffee1400, 0x0000a1bd,
		0xfff61400, 0x0000c2bd, 0xffff1400, 0x0000e2bd,
		0xff001800, 0x000000c5, 0xff081800, 0x000020c5,
		0xff101800, 0x000040c5, 0xff181800, 0x000061c5,
		0xff201800, 0x000081c5, 0xff291800, 0x0000a1c5,
		0xff311800, 0x0000c2c5, 0xff391800, 0x0000e2c5,
		0xff411800, 0x000000cd, 0xff4a1800, 0x000020cd,
		0xff521800, 0x000040cd, 0xff5a1800, 0x000061cd,
		0xff621800, 0x000081cd, 0xff6a1800, 0x0000a1cd,
		0xff731800, 0x0000c2cd, 0xff7b1800, 0x0000e2cd,
		0xff831800, 0x000000d5, 0xff8b1800, 0x000020d5,
		0xff941800, 0x000040d5, 0xff9c1800, 0x000061d5,
		0xffa41800, 0x000081d5, 0xffac1800, 0x0000a1d5,
		0xffb41800, 0x0000c2d5, 0xffbd1800, 0x0000e2d5,
		0xffc51800, 0x000000de, 0xffcd1800, 0x000020de,
		0xffd51800, 0x000040de, 0xffde1800, 0x000061de,
		0xffe61800, 0x000081de, 0xffee1800, 0x0000a1de,
		0xfff61800, 0x0000c2de, 0xffff1800, 0x0000e2de,
		0xff001c00, 0x000000e6, 0xff081c00, 0x000020e6,
		0xff101c00, 0x000040e6, 0xff181c00, 0x000061e6,
		0xff201c00, 0x000081e6, 0xff291c00, 0x0000a1e6,
		0xff311c00, 0x0000c2e6, 0xff391c00, 0x0000e2e6,
		0xff411c00, 0x000000ee, 0xff4a1c00, 0x000020ee,
		0xff521c00, 0x000040ee, 0xff5a1c00, 0x000061ee,
		0xff621c00, 0x000081ee, 0xff6a1c00, 0x0000a1ee,
		0xff731c00, 0x0000c2ee, 0xff7b1c00, 0x0000e2ee,
		0xff831c00, 0x000000f6, 0xff8b1c00, 0x000020f6,
		0xff941c00, 0x000040f6, 0xff9c1c00, 0x000061f6,
		0xffa41c00, 0x000081f6, 0xffac1c00, 0x0000a1f6,
		0xffb41c00, 0x0000c2f6, 0xffbd1c00, 0x0000e2f6,
		0xffc51c00, 0x000000ff, 0xffcd1c00, 0x000020ff,
		0xffd51c00, 0x000040ff, 0xffde1c00, 0x000061ff,
		0xffe61c00, 0x000081ff, 0xffee1c00, 0x0000a1ff,
		0xfff61c00, 0x0000c2ff, 0xffff1c00, 0x0000e2ff
};
static void Blit_RGB565_ABGR8888(FOX_BlitInfo *info)
{
    Blit_RGB565_32(info, RGB565_ABGR8888_LUT);
}

// przeliczone tablice 

static const Uint32 RGB565_RGBA8888_LUT[512] = {
		0x000000ff, 0x00000000, 0x000008ff, 0x00200000,
		0x000010ff, 0x00400000, 0x000018ff, 0x00610000,
		0x000020ff, 0x00810000, 0x000029ff, 0x00a10000,
		0x000031ff, 0x00c20000, 0x000039ff, 0x00e20000,
		0x000041ff, 0x08000000, 0x00004aff, 0x08200000,
		0x000052ff, 0x08400000, 0x00005aff, 0x08610000,
		0x000062ff, 0x08810000, 0x00006aff, 0x08a10000,
		0x000073ff, 0x08c20000, 0x00007bff, 0x08e20000,
		0x000083ff, 0x10000000, 0x00008bff, 0x10200000,
		0x000094ff, 0x10400000, 0x00009cff, 0x10610000,
		0x0000a4ff, 0x10810000, 0x0000acff, 0x10a10000,
		0x0000b4ff, 0x10c20000, 0x0000bdff, 0x10e20000,
		0x0000c5ff, 0x18000000, 0x0000cdff, 0x18200000,
		0x0000d5ff, 0x18400000, 0x0000deff, 0x18610000,
		0x0000e6ff, 0x18810000, 0x0000eeff, 0x18a10000,
		0x0000f6ff, 0x18c20000, 0x0000ffff, 0x18e20000,
		0x000400ff, 0x20000000, 0x000408ff, 0x20200000,
		0x000410ff, 0x20400000, 0x000418ff, 0x20610000,
		0x000420ff, 0x20810000, 0x000429ff, 0x20a10000,
		0x000431ff, 0x20c20000, 0x000439ff, 0x20e20000,
		0x000441ff, 0x29000000, 0x00044aff, 0x29200000,
		0x000452ff, 0x29400000, 0x00045aff, 0x29610000,
		0x000462ff, 0x29810000, 0x00046aff, 0x29a10000,
		0x000473ff, 0x29c20000, 0x00047bff, 0x29e20000,
		0x000483ff, 0x31000000, 0x00048bff, 0x31200000,
		0x000494ff, 0x31400000, 0x00049cff, 0x31610000,
		0x0004a4ff, 0x31810000, 0x0004acff, 0x31a10000,
		0x0004b4ff, 0x31c20000, 0x0004bdff, 0x31e20000,
		0x0004c5ff, 0x39000000, 0x0004cdff, 0x39200000,
		0x0004d5ff, 0x39400000, 0x0004deff, 0x39610000,
		0x0004e6ff, 0x39810000, 0x0004eeff, 0x39a10000,
		0x0004f6ff, 0x39c20000, 0x0004ffff, 0x39e20000,
		0x000800ff, 0x41000000, 0x000808ff, 0x41200000,
		0x000810ff, 0x41400000, 0x000818ff, 0x41610000,
		0x000820ff, 0x41810000, 0x000829ff, 0x41a10000,
		0x000831ff, 0x41c20000, 0x000839ff, 0x41e20000,
		0x000841ff, 0x4a000000, 0x00084aff, 0x4a200000,
		0x000852ff, 0x4a400000, 0x00085aff, 0x4a610000,
		0x000862ff, 0x4a810000, 0x00086aff, 0x4aa10000,
		0x000873ff, 0x4ac20000, 0x00087bff, 0x4ae20000,
		0x000883ff, 0x52000000, 0x00088bff, 0x52200000,
		0x000894ff, 0x52400000, 0x00089cff, 0x52610000,
		0x0008a4ff, 0x52810000, 0x0008acff, 0x52a10000,
		0x0008b4ff, 0x52c20000, 0x0008bdff, 0x52e20000,
		0x0008c5ff, 0x5a000000, 0x0008cdff, 0x5a200000,
		0x0008d5ff, 0x5a400000, 0x0008deff, 0x5a610000,
		0x0008e6ff, 0x5a810000, 0x0008eeff, 0x5aa10000,
		0x0008f6ff, 0x5ac20000, 0x0008ffff, 0x5ae20000,
		0x000c00ff, 0x62000000, 0x000c08ff, 0x62200000,
		0x000c10ff, 0x62400000, 0x000c18ff, 0x62610000,
		0x000c20ff, 0x62810000, 0x000c29ff, 0x62a10000,
		0x000c31ff, 0x62c20000, 0x000c39ff, 0x62e20000,
		0x000c41ff, 0x6a000000, 0x000c4aff, 0x6a200000,
		0x000c52ff, 0x6a400000, 0x000c5aff, 0x6a610000,
		0x000c62ff, 0x6a810000, 0x000c6aff, 0x6aa10000,
		0x000c73ff, 0x6ac20000, 0x000c7bff, 0x6ae20000,
		0x000c83ff, 0x73000000, 0x000c8bff, 0x73200000,
		0x000c94ff, 0x73400000, 0x000c9cff, 0x73610000,
		0x000ca4ff, 0x73810000, 0x000cacff, 0x73a10000,
		0x000cb4ff, 0x73c20000, 0x000cbdff, 0x73e20000,
		0x000cc5ff, 0x7b000000, 0x000ccdff, 0x7b200000,
		0x000cd5ff, 0x7b400000, 0x000cdeff, 0x7b610000,
		0x000ce6ff, 0x7b810000, 0x000ceeff, 0x7ba10000,
		0x000cf6ff, 0x7bc20000, 0x000cffff, 0x7be20000,
		0x001000ff, 0x83000000, 0x001008ff, 0x83200000,
		0x001010ff, 0x83400000, 0x001018ff, 0x83610000,
		0x001020ff, 0x83810000, 0x001029ff, 0x83a10000,
		0x001031ff, 0x83c20000, 0x001039ff, 0x83e20000,
		0x001041ff, 0x8b000000, 0x00104aff, 0x8b200000,
		0x001052ff, 0x8b400000, 0x00105aff, 0x8b610000,
		0x001062ff, 0x8b810000, 0x00106aff, 0x8ba10000,
		0x001073ff, 0x8bc20000, 0x00107bff, 0x8be20000,
		0x001083ff, 0x94000000, 0x00108bff, 0x94200000,
		0x001094ff, 0x94400000, 0x00109cff, 0x94610000,
		0x0010a4ff, 0x94810000, 0x0010acff, 0x94a10000,
		0x0010b4ff, 0x94c20000, 0x0010bdff, 0x94e20000,
		0x0010c5ff, 0x9c000000, 0x0010cdff, 0x9c200000,
		0x0010d5ff, 0x9c400000, 0x0010deff, 0x9c610000,
		0x0010e6ff, 0x9c810000, 0x0010eeff, 0x9ca10000,
		0x0010f6ff, 0x9cc20000, 0x0010ffff, 0x9ce20000,
		0x001400ff, 0xa4000000, 0x001408ff, 0xa4200000,
		0x001410ff, 0xa4400000, 0x001418ff, 0xa4610000,
		0x001420ff, 0xa4810000, 0x001429ff, 0xa4a10000,
		0x001431ff, 0xa4c20000, 0x001439ff, 0xa4e20000,
		0x001441ff, 0xac000000, 0x00144aff, 0xac200000,
		0x001452ff, 0xac400000, 0x00145aff, 0xac610000,
		0x001462ff, 0xac810000, 0x00146aff, 0xaca10000,
		0x001473ff, 0xacc20000, 0x00147bff, 0xace20000,
		0x001483ff, 0xb4000000, 0x00148bff, 0xb4200000,
		0x001494ff, 0xb4400000, 0x00149cff, 0xb4610000,
		0x0014a4ff, 0xb4810000, 0x0014acff, 0xb4a10000,
		0x0014b4ff, 0xb4c20000, 0x0014bdff, 0xb4e20000,
		0x0014c5ff, 0xbd000000, 0x0014cdff, 0xbd200000,
		0x0014d5ff, 0xbd400000, 0x0014deff, 0xbd610000,
		0x0014e6ff, 0xbd810000, 0x0014eeff, 0xbda10000,
		0x0014f6ff, 0xbdc20000, 0x0014ffff, 0xbde20000,
		0x001800ff, 0xc5000000, 0x001808ff, 0xc5200000,
		0x001810ff, 0xc5400000, 0x001818ff, 0xc5610000,
		0x001820ff, 0xc5810000, 0x001829ff, 0xc5a10000,
		0x001831ff, 0xc5c20000, 0x001839ff, 0xc5e20000,
		0x001841ff, 0xcd000000, 0x00184aff, 0xcd200000,
		0x001852ff, 0xcd400000, 0x00185aff, 0xcd610000,
		0x001862ff, 0xcd810000, 0x00186aff, 0xcda10000,
		0x001873ff, 0xcdc20000, 0x00187bff, 0xcde20000,
		0x001883ff, 0xd5000000, 0x00188bff, 0xd5200000,
		0x001894ff, 0xd5400000, 0x00189cff, 0xd5610000,
		0x0018a4ff, 0xd5810000, 0x0018acff, 0xd5a10000,
		0x0018b4ff, 0xd5c20000, 0x0018bdff, 0xd5e20000,
		0x0018c5ff, 0xde000000, 0x0018cdff, 0xde200000,
		0x0018d5ff, 0xde400000, 0x0018deff, 0xde610000,
		0x0018e6ff, 0xde810000, 0x0018eeff, 0xdea10000,
		0x0018f6ff, 0xdec20000, 0x0018ffff, 0xdee20000,
		0x001c00ff, 0xe6000000, 0x001c08ff, 0xe6200000,
		0x001c10ff, 0xe6400000, 0x001c18ff, 0xe6610000,
		0x001c20ff, 0xe6810000, 0x001c29ff, 0xe6a10000,
		0x001c31ff, 0xe6c20000, 0x001c39ff, 0xe6e20000,
		0x001c41ff, 0xee000000, 0x001c4aff, 0xee200000,
		0x001c52ff, 0xee400000, 0x001c5aff, 0xee610000,
		0x001c62ff, 0xee810000, 0x001c6aff, 0xeea10000,
		0x001c73ff, 0xeec20000, 0x001c7bff, 0xeee20000,
		0x001c83ff, 0xf6000000, 0x001c8bff, 0xf6200000,
		0x001c94ff, 0xf6400000, 0x001c9cff, 0xf6610000,
		0x001ca4ff, 0xf6810000, 0x001cacff, 0xf6a10000,
		0x001cb4ff, 0xf6c20000, 0x001cbdff, 0xf6e20000,
		0x001cc5ff, 0xff000000, 0x001ccdff, 0xff200000,
		0x001cd5ff, 0xff400000, 0x001cdeff, 0xff610000,
		0x001ce6ff, 0xff810000, 0x001ceeff, 0xffa10000,
		0x001cf6ff, 0xffc20000, 0x001cffff, 0xffe20000,
};

static void Blit_RGB565_RGBA8888(FOX_BlitInfo *info)
{
    Blit_RGB565_32(info, RGB565_RGBA8888_LUT);
}

// przeliczone wartosci

static const Uint32 RGB565_BGRA8888_LUT[512] = {
		0x00000000, 0x000000ff, 0x08000000, 0x002000ff,
		0x10000000, 0x004000ff, 0x18000000, 0x006100ff,
		0x20000000, 0x008100ff, 0x29000000, 0x00a100ff,
		0x31000000, 0x00c200ff, 0x39000000, 0x00e200ff,
		0x41000000, 0x000008ff, 0x4a000000, 0x002008ff,
		0x52000000, 0x004008ff, 0x5a000000, 0x006108ff,
		0x62000000, 0x008108ff, 0x6a000000, 0x00a108ff,
		0x73000000, 0x00c208ff, 0x7b000000, 0x00e208ff,
		0x83000000, 0x000010ff, 0x8b000000, 0x002010ff,
		0x94000000, 0x004010ff, 0x9c000000, 0x006110ff,
		0xa4000000, 0x008110ff, 0xac000000, 0x00a110ff,
		0xb4000000, 0x00c210ff, 0xbd000000, 0x00e210ff,
		0xc5000000, 0x000018ff, 0xcd000000, 0x002018ff,
		0xd5000000, 0x004018ff, 0xde000000, 0x006118ff,
		0xe6000000, 0x008118ff, 0xee000000, 0x00a118ff,
		0xf6000000, 0x00c218ff, 0xff000000, 0x00e218ff,
		0x00040000, 0x000020ff, 0x08040000, 0x002020ff,
		0x10040000, 0x004020ff, 0x18040000, 0x006120ff,
		0x20040000, 0x008120ff, 0x29040000, 0x00a120ff,
		0x31040000, 0x00c220ff, 0x39040000, 0x00e220ff,
		0x41040000, 0x000029ff, 0x4a040000, 0x002029ff,
		0x52040000, 0x004029ff, 0x5a040000, 0x006129ff,
		0x62040000, 0x008129ff, 0x6a040000, 0x00a129ff,
		0x73040000, 0x00c229ff, 0x7b040000, 0x00e229ff,
		0x83040000, 0x000031ff, 0x8b040000, 0x002031ff,
		0x94040000, 0x004031ff, 0x9c040000, 0x006131ff,
		0xa4040000, 0x008131ff, 0xac040000, 0x00a131ff,
		0xb4040000, 0x00c231ff, 0xbd040000, 0x00e231ff,
		0xc5040000, 0x000039ff, 0xcd040000, 0x002039ff,
		0xd5040000, 0x004039ff, 0xde040000, 0x006139ff,
		0xe6040000, 0x008139ff, 0xee040000, 0x00a139ff,
		0xf6040000, 0x00c239ff, 0xff040000, 0x00e239ff,
		0x00080000, 0x000041ff, 0x08080000, 0x002041ff,
		0x10080000, 0x004041ff, 0x18080000, 0x006141ff,
		0x20080000, 0x008141ff, 0x29080000, 0x00a141ff,
		0x31080000, 0x00c241ff, 0x39080000, 0x00e241ff,
		0x41080000, 0x00004aff, 0x4a080000, 0x00204aff,
		0x52080000, 0x00404aff, 0x5a080000, 0x00614aff,
		0x62080000, 0x00814aff, 0x6a080000, 0x00a14aff,
		0x73080000, 0x00c24aff, 0x7b080000, 0x00e24aff,
		0x83080000, 0x000052ff, 0x8b080000, 0x002052ff,
		0x94080000, 0x004052ff, 0x9c080000, 0x006152ff,
		0xa4080000, 0x008152ff, 0xac080000, 0x00a152ff,
		0xb4080000, 0x00c252ff, 0xbd080000, 0x00e252ff,
		0xc5080000, 0x00005aff, 0xcd080000, 0x00205aff,
		0xd5080000, 0x00405aff, 0xde080000, 0x00615aff,
		0xe6080000, 0x00815aff, 0xee080000, 0x00a15aff,
		0xf6080000, 0x00c25aff, 0xff080000, 0x00e25aff,
		0x000c0000, 0x000062ff, 0x080c0000, 0x002062ff,
		0x100c0000, 0x004062ff, 0x180c0000, 0x006162ff,
		0x200c0000, 0x008162ff, 0x290c0000, 0x00a162ff,
		0x310c0000, 0x00c262ff, 0x390c0000, 0x00e262ff,
		0x410c0000, 0x00006aff, 0x4a0c0000, 0x00206aff,
		0x520c0000, 0x00406aff, 0x5a0c0000, 0x00616aff,
		0x620c0000, 0x00816aff, 0x6a0c0000, 0x00a16aff,
		0x730c0000, 0x00c26aff, 0x7b0c0000, 0x00e26aff,
		0x830c0000, 0x000073ff, 0x8b0c0000, 0x002073ff,
		0x940c0000, 0x004073ff, 0x9c0c0000, 0x006173ff,
		0xa40c0000, 0x008173ff, 0xac0c0000, 0x00a173ff,
		0xb40c0000, 0x00c273ff, 0xbd0c0000, 0x00e273ff,
		0xc50c0000, 0x00007bff, 0xcd0c0000, 0x00207bff,
		0xd50c0000, 0x00407bff, 0xde0c0000, 0x00617bff,
		0xe60c0000, 0x00817bff, 0xee0c0000, 0x00a17bff,
		0xf60c0000, 0x00c27bff, 0xff0c0000, 0x00e27bff,
		0x00100000, 0x000083ff, 0x08100000, 0x002083ff,
		0x10100000, 0x004083ff, 0x18100000, 0x006183ff,
		0x20100000, 0x008183ff, 0x29100000, 0x00a183ff,
		0x31100000, 0x00c283ff, 0x39100000, 0x00e283ff,
		0x41100000, 0x00008bff, 0x4a100000, 0x00208bff,
		0x52100000, 0x00408bff, 0x5a100000, 0x00618bff,
		0x62100000, 0x00818bff, 0x6a100000, 0x00a18bff,
		0x73100000, 0x00c28bff, 0x7b100000, 0x00e28bff,
		0x83100000, 0x000094ff, 0x8b100000, 0x002094ff,
		0x94100000, 0x004094ff, 0x9c100000, 0x006194ff,
		0xa4100000, 0x008194ff, 0xac100000, 0x00a194ff,
		0xb4100000, 0x00c294ff, 0xbd100000, 0x00e294ff,
		0xc5100000, 0x00009cff, 0xcd100000, 0x00209cff,
		0xd5100000, 0x00409cff, 0xde100000, 0x00619cff,
		0xe6100000, 0x00819cff, 0xee100000, 0x00a19cff,
		0xf6100000, 0x00c29cff, 0xff100000, 0x00e29cff,
		0x00140000, 0x0000a4ff, 0x08140000, 0x0020a4ff,
		0x10140000, 0x0040a4ff, 0x18140000, 0x0061a4ff,
		0x20140000, 0x0081a4ff, 0x29140000, 0x00a1a4ff,
		0x31140000, 0x00c2a4ff, 0x39140000, 0x00e2a4ff,
		0x41140000, 0x0000acff, 0x4a140000, 0x0020acff,
		0x52140000, 0x0040acff, 0x5a140000, 0x0061acff,
		0x62140000, 0x0081acff, 0x6a140000, 0x00a1acff,
		0x73140000, 0x00c2acff, 0x7b140000, 0x00e2acff,
		0x83140000, 0x0000b4ff, 0x8b140000, 0x0020b4ff,
		0x94140000, 0x0040b4ff, 0x9c140000, 0x0061b4ff,
		0xa4140000, 0x0081b4ff, 0xac140000, 0x00a1b4ff,
		0xb4140000, 0x00c2b4ff, 0xbd140000, 0x00e2b4ff,
		0xc5140000, 0x0000bdff, 0xcd140000, 0x0020bdff,
		0xd5140000, 0x0040bdff, 0xde140000, 0x0061bdff,
		0xe6140000, 0x0081bdff, 0xee140000, 0x00a1bdff,
		0xf6140000, 0x00c2bdff, 0xff140000, 0x00e2bdff,
		0x00180000, 0x0000c5ff, 0x08180000, 0x0020c5ff,
		0x10180000, 0x0040c5ff, 0x18180000, 0x0061c5ff,
		0x20180000, 0x0081c5ff, 0x29180000, 0x00a1c5ff,
		0x31180000, 0x00c2c5ff, 0x39180000, 0x00e2c5ff,
		0x41180000, 0x0000cdff, 0x4a180000, 0x0020cdff,
		0x52180000, 0x0040cdff, 0x5a180000, 0x0061cdff,
		0x62180000, 0x0081cdff, 0x6a180000, 0x00a1cdff,
		0x73180000, 0x00c2cdff, 0x7b180000, 0x00e2cdff,
		0x83180000, 0x0000d5ff, 0x8b180000, 0x0020d5ff,
		0x94180000, 0x0040d5ff, 0x9c180000, 0x0061d5ff,
		0xa4180000, 0x0081d5ff, 0xac180000, 0x00a1d5ff,
		0xb4180000, 0x00c2d5ff, 0xbd180000, 0x00e2d5ff,
		0xc5180000, 0x0000deff, 0xcd180000, 0x0020deff,
		0xd5180000, 0x0040deff, 0xde180000, 0x0061deff,
		0xe6180000, 0x0081deff, 0xee180000, 0x00a1deff,
		0xf6180000, 0x00c2deff, 0xff180000, 0x00e2deff,
		0x001c0000, 0x0000e6ff, 0x081c0000, 0x0020e6ff,
		0x101c0000, 0x0040e6ff, 0x181c0000, 0x0061e6ff,
		0x201c0000, 0x0081e6ff, 0x291c0000, 0x00a1e6ff,
		0x311c0000, 0x00c2e6ff, 0x391c0000, 0x00e2e6ff,
		0x411c0000, 0x0000eeff, 0x4a1c0000, 0x0020eeff,
		0x521c0000, 0x0040eeff, 0x5a1c0000, 0x0061eeff,
		0x621c0000, 0x0081eeff, 0x6a1c0000, 0x00a1eeff,
		0x731c0000, 0x00c2eeff, 0x7b1c0000, 0x00e2eeff,
		0x831c0000, 0x0000f6ff, 0x8b1c0000, 0x0020f6ff,
		0x941c0000, 0x0040f6ff, 0x9c1c0000, 0x0061f6ff,
		0xa41c0000, 0x0081f6ff, 0xac1c0000, 0x00a1f6ff,
		0xb41c0000, 0x00c2f6ff, 0xbd1c0000, 0x00e2f6ff,
		0xc51c0000, 0x0000ffff, 0xcd1c0000, 0x0020ffff,
		0xd51c0000, 0x0040ffff, 0xde1c0000, 0x0061ffff,
		0xe61c0000, 0x0081ffff, 0xee1c0000, 0x00a1ffff,
		0xf61c0000, 0x00c2ffff, 0xff1c0000, 0x00e2ffff
};

static void Blit_RGB565_BGRA8888(FOX_BlitInfo *info)
{
    Blit_RGB565_32(info, RGB565_BGRA8888_LUT);
}

// moze sie nie zgadzac

#ifndef RGB888_RGB332
#define RGB888_RGB332(dst, src) { \
	dst = (((src)&0x00E00000)>>16)| \
	      (((src)&0x0000E000)>>11)| \
	      (((src)&0x000000C0)>>6); \
}
#endif

static void Blit_RGB888_index8_map(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	int pixel;
	int width, height;
	Uint32 *src;
	const Uint8 *map;
	Uint8 *dst;
	int srcskip, dstskip;

	// ustaw podstawowe info
	
	width = info->d_width;
	height = info->d_height;
	src = (Uint32 *)info->s_pixels;
	srcskip = info->s_skip/4;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = info->table;

#ifdef USE_DUFFS_LOOP
	while ( height-- ) {
		DUFFS_LOOP(
			RGB888_RGB332(pixel, *src);
			*dst++ = map[pixel];
			++src;
		, width);
		src += srcskip;
		dst += dstskip;
	}
#else
	while ( height-- ) {
		for ( c=width/4; c; --c ) 
		{
			// pakuj w 8 pixelowe chunki
			
			RGB888_RGB332(pixel, *src);
			*dst++ = map[pixel];
			++src;
			RGB888_RGB332(pixel, *src);
			*dst++ = map[pixel];
			++src;
			RGB888_RGB332(pixel, *src);
			*dst++ = map[pixel];
			++src;
			RGB888_RGB332(pixel, *src);
			*dst++ = map[pixel];
			++src;
		}
		switch ( width & 3 ) {
			case 3:
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
			case 2:
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
			case 1:
				RGB888_RGB332(pixel, *src);
				*dst++ = map[pixel];
				++src;
		}
		src += srcskip;
		dst += dstskip;
	}
#endif /* USE_DUFFS_LOOP */
}
static void BlitNto1(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	int width, height;
	Uint8 *src;
	const Uint8 *map;
	Uint8 *dst;
	int srcskip, dstskip;
	int srcbpp;
	Uint32 pixel;
	int  sR, sG, sB;
	FOX_PixelFormat *srcfmt;

	// podstawowe info
	
	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = info->table;
	srcfmt = info->src;
	srcbpp = srcfmt->BytesPerPixel;

	if ( map == NULL ) 
	{
		while ( height-- ) {
#ifdef USE_DUFFS_LOOP
			DUFFS_LOOP(
				DISEMBLE_RGB(src, srcbpp, srcfmt, pixel,
								sR, sG, sB);
				if ( 1 ) 
				{
				  	// pakuj pixel rgb w 8bpp
				  	*dst = ((sR>>5)<<(3+2))|
					        ((sG>>5)<<(2)) |
					        ((sB>>6)<<(0)) ;
				}
				dst++;
				src += srcbpp;
			, width);
#else
			for ( c=width; c; --c ) 
			{
				DISEMBLE_RGB(src, srcbpp, srcfmt, pixel,
								sR, sG, sB);
				if ( 1 ) 
				{
					// pakuj w 8bpp
				  	*dst = ((sR>>5)<<(3+2))|
					        ((sG>>5)<<(2)) |
					        ((sB>>6)<<(0)) ;
				}
				dst++;
				src += srcbpp;
			}
#endif
			src += srcskip;
			dst += dstskip;
		}
	} else {
		while ( height-- ) {
#ifdef USE_DUFFS_LOOP
			DUFFS_LOOP(
				DISEMBLE_RGB(src, srcbpp, srcfmt, pixel,
								sR, sG, sB);
				if ( 1 ) 
				{
				  	// pakuj w 8bpp
				  	*dst = map[((sR>>5)<<(3+2))|
						   ((sG>>5)<<(2))  |
						   ((sB>>6)<<(0))  ];
				}
				dst++;
				src += srcbpp;
			,width);
#else
			for ( c=width; c; --c ) 
			{
				DISEMBLE_RGB(src, srcbpp, srcfmt, pixel,
								sR, sG, sB);
				if ( 1 ) 
				{
					// pakuj rgb w 8bpp  	
				  	*dst = map[((sR>>5)<<(3+2))|
						   ((sG>>5)<<(2))  |
						   ((sB>>6)<<(0))  ];
				}
				dst++;
				src += srcbpp;
			}
#endif /* USE_DUFFS_LOOP */
			src += srcskip;
			dst += dstskip;
		}
	}
}
static void BlitNtoN(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	FOX_PixelFormat *srcfmt = info->src;
	int srcbpp = srcfmt->BytesPerPixel;
	FOX_PixelFormat *dstfmt = info->dst;
	int dstbpp = dstfmt->BytesPerPixel;
	unsigned alpha = dstfmt->Amask ? FOX_ALPHA_OPAQUE : 0;

	while ( height-- ) {
		DUFFS_LOOP(
		{
		        Uint32 pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
			ASSEMBLE_RGBA(dst, dstbpp, dstfmt, sR, sG, sB, alpha);
			dst += dstbpp;
			src += srcbpp;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitNtoNCopyAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	FOX_PixelFormat *srcfmt = info->src;
	int srcbpp = srcfmt->BytesPerPixel;
	FOX_PixelFormat *dstfmt = info->dst;
	int dstbpp = dstfmt->BytesPerPixel;
	int c;

	// 01 !!! tutaj do poprawienia na poprawne mapowanie do 0..255
	
	while ( height-- ) 
	{
		for ( c=width; c; --c ) 
		{
		        Uint32 pixel;
			unsigned sR, sG, sB, sA;
			DISEMBLE_RGBA(src, srcbpp, srcfmt, pixel,
				      sR, sG, sB, sA);
			ASSEMBLE_RGBA(dst, dstbpp, dstfmt,
				      sR, sG, sB, sA);
			dst += dstbpp;
			src += srcbpp;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitNto1Key(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	FOX_PixelFormat *srcfmt = info->src;
	const Uint8 *palmap = info->table;
	Uint32 ckey = srcfmt->colorkey;
	Uint32 rgbmask = ~srcfmt->Amask;
	int srcbpp;
	Uint32 pixel;
	Uint8  sR, sG, sB;

	// ustaw podstawowe zmienne
	
	srcbpp = srcfmt->BytesPerPixel;
	ckey &= rgbmask;

	if ( palmap == NULL ) 
	{
		while ( height-- ) 
		{
			DUFFS_LOOP(
			{
				DISEMBLE_RGB(src, srcbpp, srcfmt, pixel,
								sR, sG, sB);
				if ( (pixel & rgbmask) != ckey ) 
				{
					// pakuj pixel w 8bpp
				  	*dst = ((sR>>5)<<(3+2))|
						((sG>>5)<<(2)) |
						((sB>>6)<<(0)) ;
				}
				dst++;
				src += srcbpp;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	} 
	else 
	{
		while ( height-- ) 
		{
			DUFFS_LOOP(
			{
				DISEMBLE_RGB(src, srcbpp, srcfmt, pixel,
								sR, sG, sB);
				if ( (pixel & rgbmask) != ckey ) 
				{
					// pakuj w 8bpp	
				  	*dst = palmap[((sR>>5)<<(3+2))|
							((sG>>5)<<(2))  |
							((sB>>6)<<(0))  ];
				}
				dst++;
				src += srcbpp;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	}
}

static void Blit2to2Key(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint16 *srcp = (Uint16 *)info->s_pixels;
	int srcskip = info->s_skip;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip;
	Uint32 ckey = info->src->colorkey;
	Uint32 rgbmask = ~info->src->Amask;

	// ustaw jakies podstawowe info

        srcskip /= 2;
        dstskip /= 2;
	ckey &= rgbmask;

	while ( height-- ) {
		DUFFS_LOOP(
		{
			if ( (*srcp & rgbmask) != ckey ) 
			{
				*dstp = *srcp;
			}
			dstp++;
			srcp++;
		},
		width);
		srcp += srcskip;
		dstp += dstskip;
	}
}

// bez klucza

static void BlitNtoNKey(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	Uint32 ckey = info->src->colorkey;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;
	int dstbpp = dstfmt->BytesPerPixel;
	unsigned alpha = dstfmt->Amask ? FOX_ALPHA_OPAQUE : 0;

	while ( height-- ) {
		DUFFS_LOOP(
		{
		        Uint32 pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			RETRIEVE_RGB_PIXEL(src, srcbpp, pixel);
			
			if ( pixel != ckey ) 
			{
			    RGB_FROM_PIXEL(pixel, srcfmt, sR, sG, sB);
				ASSEMBLE_RGBA(dst, dstbpp, dstfmt,
					      sR, sG, sB, alpha);
			}
			dst += dstbpp;
			src += srcbpp;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitNtoNKeyCopyAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	Uint32 ckey = info->src->colorkey;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	Uint32 rgbmask = ~srcfmt->Amask;

	Uint8 srcbpp;
	Uint8 dstbpp;
	Uint32 pixel;
	Uint8  sR, sG, sB, sA;

	// ustaw podstawowe info
	
	srcbpp = srcfmt->BytesPerPixel;
	dstbpp = dstfmt->BytesPerPixel;
	ckey &= rgbmask;

	// !!! 01 do poprawienia

	while ( height-- ) 
	{
		DUFFS_LOOP(
		{
			DISEMBLE_RGBA(src, srcbpp, srcfmt, pixel,
				      sR, sG, sB, sA);
			if ( (pixel & rgbmask) != ckey ) 
			{
				  ASSEMBLE_RGBA(dst, dstbpp, dstfmt,
						sR, sG, sB, sA);
			}
			dst += dstbpp;
			src += srcbpp;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

// standardowy blitter

struct blit_table {
	Uint32 srcR, srcG, srcB;
	int dstbpp;
	Uint32 dstR, dstG, dstB;
	Uint32 cpu_flags;
	void *aux_data;
	FOX_loblit blitfunc;
        enum { NO_ALPHA, SET_ALPHA, COPY_ALPHA } alpha;
};

static const struct blit_table normal_blit_1[] = {
	{ 0,0,0, 0, 0,0,0, 0, NULL, NULL },
};

static const struct blit_table normal_blit_2[] = {
    { 0x0000F800,0x000007E0,0x0000001F, 4, 0x00FF0000,0x0000FF00,0x000000FF,
      0, NULL, Blit_RGB565_ARGB8888, SET_ALPHA },
    { 0x0000F800,0x000007E0,0x0000001F, 4, 0x000000FF,0x0000FF00,0x00FF0000,
      0, NULL, Blit_RGB565_ABGR8888, SET_ALPHA },
    { 0x0000F800,0x000007E0,0x0000001F, 4, 0xFF000000,0x00FF0000,0x0000FF00,
      0, NULL, Blit_RGB565_RGBA8888, SET_ALPHA },
    { 0x0000F800,0x000007E0,0x0000001F, 4, 0x0000FF00,0x00FF0000,0xFF000000,
      0, NULL, Blit_RGB565_BGRA8888, SET_ALPHA },

	  // domyslny dla 16bpp 
    
    { 0,0,0, 0, 0,0,0, 0, NULL, BlitNtoN, 0 }
};

// domyslny dla 24bpp

static const struct blit_table normal_blit_3[] = {
    { 0,0,0, 0, 0,0,0, 0, NULL, BlitNtoN, 0 }
};
static const struct blit_table normal_blit_4[] = {
    { 0x00FF0000,0x0000FF00,0x000000FF, 2, 0x0000F800,0x000007E0,0x0000001F,
      0, NULL, Blit_RGB888_RGB565, NO_ALPHA },
    { 0x00FF0000,0x0000FF00,0x000000FF, 2, 0x00007C00,0x000003E0,0x0000001F,
      0, NULL, Blit_RGB888_RGB555, NO_ALPHA },
	// domyslne dla 32bpp
	{ 0,0,0, 0, 0,0,0, 0, NULL, BlitNtoN, 0 }
};
static const struct blit_table *normal_blit[] = {
	normal_blit_1, normal_blit_2, normal_blit_3, normal_blit_4
};

FOX_loblit FOX_CalculateBlitN(FOX_Surface *surface, int blit_index)
{
	struct private_swaccel *sdata;
	FOX_PixelFormat *srcfmt;
	FOX_PixelFormat *dstfmt;
	const struct blit_table *table;
	int which;
	FOX_loblit blitfun;

	// ustaw i wybierz blit

	sdata = surface->map->sw_data;
	srcfmt = surface->format;
	dstfmt = surface->map->dst->format;

	if ( blit_index & 2 ) 
	{
		// alpha lub alpha +ck
	        return FOX_CalculateAlphaBlit(surface, blit_index);
	}
	
	if ( dstfmt->BitsPerPixel < 8 ) 
	{
		return(NULL);
	}
	
	if(blit_index == 1) 
	{
	    if(srcfmt->BytesPerPixel == 2
	       && surface->map->identity)
		return Blit2to2Key;
	    else if(dstfmt->BytesPerPixel == 1)
		return BlitNto1Key;
	    else 
		{
		if(srcfmt->Amask && dstfmt->Amask)
		    return BlitNtoNKeyCopyAlpha;
		else
		    return BlitNtoNKey;
	    }
	}

	blitfun = NULL;
	if 	( dstfmt->BitsPerPixel == 8 ) 
	{
		/// 8bpp jest ck
		
		if ( (srcfmt->BytesPerPixel == 4) &&
		     (srcfmt->Rmask == 0x00FF0000) &&
		     (srcfmt->Gmask == 0x0000FF00) &&
		     (srcfmt->Bmask == 0x000000FF) ) 
		{
			if ( surface->map->table ) 
			{
				blitfun = Blit_RGB888_index8_map;
			} 
			else 
			{
				blitfun = Blit_RGB888_index8;
			}
		} 
		else 
		{
			blitfun = BlitNto1;
		}
	} 
	else 
	{
	
	        int a_need = 0;
		if(dstfmt->Amask)
		    a_need = srcfmt->Amask ? COPY_ALPHA : SET_ALPHA;
		table = normal_blit[srcfmt->BytesPerPixel-1];
		for ( which=0; table[which].srcR; ++which ) 
		{
			if ( srcfmt->Rmask == table[which].srcR &&
			     srcfmt->Gmask == table[which].srcG &&
			     srcfmt->Bmask == table[which].srcB &&
			     dstfmt->BytesPerPixel == table[which].dstbpp &&
			     dstfmt->Rmask == table[which].dstR &&
			     dstfmt->Gmask == table[which].dstG &&
			     dstfmt->Bmask == table[which].dstB &&
			     (a_need & table[which].alpha) == a_need &&
			     (CPU_Flags()&table[which].cpu_flags) ==
			     table[which].cpu_flags )
				break;
		}
		sdata->aux_data = table[which].aux_data;
		blitfun = table[which].blitfunc;
		if(a_need == COPY_ALPHA && blitfun == BlitNtoN)
		    blitfun = BlitNtoNCopyAlpha;
	}

// usunac 

#ifdef DEBUG_ASM
	if ( (blitfun == FOX_BlitNtoN) || (blitfun == FOX_BlitNto1) )
		fprintf(stderr, "[FOX]: C blit\n");
	else
		fprintf(stderr, "[FOX]: C- blit\n");
#endif /* DEBUG_ASM */

// az dotad

	return(blitfun);
}

// end
