// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_stretch_c.h"
#include "FOX_yuvfuncs.h"
#include "FOX_yuv_sw_c.h"

// flagi dla procka 

#define MMX_CPU		0x800000
	
#ifdef USE_ASMBLIT
	#define CPU_Flags()	Hermes_X86_CPU()
#else
	#define CPU_Flags()	0L
#endif

#ifdef USE_ASMBLIT
	#define X86_ASSEMBLER
	#define HermesConverterInterface	void
	#define HermesClearInterface		void
	#define STACKCALL
typedef Uint32 int32;

#include "HeadX86.h"
#endif

// struktura 

static struct private_yuvhwfuncs sw_yuvfuncs = 
{
	FOX_LockYUV_SW,
	FOX_UnlockYUV_SW,
	FOX_DisplayYUV_SW,
	FOX_FreeYUV_SW
};

// do konwersji rgb

struct private_yuvhwdata 
{
	FOX_Surface *stretch;
	FOX_Surface *display;

	Uint8 *pixels;
	
	int *colortab;
	
	Uint32 *rgb_2_pix;
	
	void (*Display1X)(int *colortab, Uint32 *rgb_2_pix,
                          unsigned char *lum, unsigned char *cr,
                          unsigned char *cb, unsigned char *out,
                          int rows, int cols, int mod );
	
	void (*Display2X)(int *colortab, Uint32 *rgb_2_pix,
	                  unsigned char *lum, unsigned char *cr,
                          unsigned char *cb, unsigned char *out,
                          int rows, int cols, int mod );

	// osobno 
	Uint16 pitches[3];
	Uint8 *planes[3];
};


// konwersja

extern void Color565DitherYV12MMX1X( int *colortab, Uint32 *rgb_2_pix,
                                     unsigned char *lum, unsigned char *cr,
                                     unsigned char *cb, unsigned char *out,
                                     int rows, int cols, int mod );

extern void ColorRGBDitherYV12MMX1X( int *colortab, Uint32 *rgb_2_pix,
                                     unsigned char *lum, unsigned char *cr,
                                     unsigned char *cb, unsigned char *out,
                                     int rows, int cols, int mod );

static void Color16DitherYV12Mod1X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{

    unsigned short* row1;
    unsigned short* row2;
    unsigned char* lum2;
    
	int x, y;
    
	int cr_r;
    int crb_g;
    
	int cb_b;
    
	int cols_2 = cols / 2;

    row1 = (unsigned short*) out;
    row2 = row1 + cols + mod;
    lum2 = lum + cols;

    mod += cols + mod;

    y = rows / 2;
    
	while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            ++cr; ++cb;

            L = *lum++;
            *row1++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

            L = *lum++;
            *row1++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);


			// drugi rzad

            L = *lum2++;
            *row2++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

            L = *lum2++;
            *row2++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);
        }
                
        lum  += cols;
        lum2 += cols;
        row1 += mod;
        row2 += mod;
    }
}

// dither

static void Color24DitherYV12Mod1X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int value;
    unsigned char* row1;
    unsigned char* row2;
    unsigned char* lum2;
    
	int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    row1 = out;
    row2 = row1 + cols*3 + mod*3;
    lum2 = lum + cols;

    mod += cols + mod;
    mod *= 3;

    y = rows / 2;
    
	while( y-- )
    {
        x = cols_2;
    
		while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            ++cr; ++cb;

            L = *lum++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            *row1++ = (value      ) & 0xFF;
            *row1++ = (value >>  8) & 0xFF;
            *row1++ = (value >> 16) & 0xFF;

            L = *lum++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            *row1++ = (value      ) & 0xFF;
            *row1++ = (value >>  8) & 0xFF;
            *row1++ = (value >> 16) & 0xFF;

	
			// drugi rzad

            L = *lum2++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            *row2++ = (value      ) & 0xFF;
            *row2++ = (value >>  8) & 0xFF;
            *row2++ = (value >> 16) & 0xFF;

            L = *lum2++;
            
			value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            
			*row2++ = (value      ) & 0xFF;
            *row2++ = (value >>  8) & 0xFF;
            *row2++ = (value >> 16) & 0xFF;
        }
        
        lum  += cols;
        lum2 += cols;
        row1 += mod;
        row2 += mod;
    }
}

// dither

static void Color32DitherYV12Mod1X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int* row1;
    unsigned int* row2;
    unsigned char* lum2;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    row1 = (unsigned int*) out;
    row2 = row1 + cols + mod;
    lum2 = lum + cols;

    mod += cols + mod;

    y = rows / 2;
    
	while( y-- )
    {
        x = cols_2;
        
		while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            ++cr; ++cb;

            L = *lum++;
            *row1++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

            L = *lum++;
            *row1++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);


            /* Now, do second row.  */

            L = *lum2++;
            *row2++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

            L = *lum2++;
            *row2++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);
        }

        /*
         * These values are at the start of the next line, (due
         * to the ++'s above),but they need to be at the start
         * of the line after that.
         */
        lum  += cols;
        lum2 += cols;
        row1 += mod;
        row2 += mod;
    }
}

/*
 * In this function I make use of a nasty trick. The tables have the lower
 * 16 bits replicated in the upper 16. This means I can write ints and get
 * the horisontal doubling for free (almost).
 */
static void Color16DitherYV12Mod2X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int* row1 = (unsigned int*) out;
    const int next_row = cols+(mod/2);
    unsigned int* row2 = row1 + 2*next_row;
    unsigned char* lum2;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    lum2 = lum + cols;

    mod = (next_row * 3) + (mod/2);

    y = rows / 2;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            ++cr; ++cb;

            L = *lum++;
            row1[0] = row1[next_row] = (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row1++;

            L = *lum++;
            row1[0] = row1[next_row] = (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row1++;


            /* Now, do second row. */

            L = *lum2++;
            row2[0] = row2[next_row] = (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row2++;

            L = *lum2++;
            row2[0] = row2[next_row] = (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row2++;
        }

        /*
         * These values are at the start of the next line, (due
         * to the ++'s above),but they need to be at the start
         * of the line after that.
         */
        lum  += cols;
        lum2 += cols;
        row1 += mod;
        row2 += mod;
    }
}

static void Color24DitherYV12Mod2X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int value;
    unsigned char* row1 = out;
    const int next_row = (cols*2 + mod) * 3;
    unsigned char* row2 = row1 + 2*next_row;
    unsigned char* lum2;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    lum2 = lum + cols;

    mod = next_row*3 + mod*3;

    y = rows / 2;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            ++cr; ++cb;

            L = *lum++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            row1[0+0] = row1[3+0] = row1[next_row+0] = row1[next_row+3+0] =
                     (value      ) & 0xFF;
            row1[0+1] = row1[3+1] = row1[next_row+1] = row1[next_row+3+1] =
                     (value >>  8) & 0xFF;
            row1[0+2] = row1[3+2] = row1[next_row+2] = row1[next_row+3+2] =
                     (value >> 16) & 0xFF;
            row1 += 2*3;

            L = *lum++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            row1[0+0] = row1[3+0] = row1[next_row+0] = row1[next_row+3+0] =
                     (value      ) & 0xFF;
            row1[0+1] = row1[3+1] = row1[next_row+1] = row1[next_row+3+1] =
                     (value >>  8) & 0xFF;
            row1[0+2] = row1[3+2] = row1[next_row+2] = row1[next_row+3+2] =
                     (value >> 16) & 0xFF;
            row1 += 2*3;


            /* Now, do second row. */

            L = *lum2++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            row2[0+0] = row2[3+0] = row2[next_row+0] = row2[next_row+3+0] =
                     (value      ) & 0xFF;
            row2[0+1] = row2[3+1] = row2[next_row+1] = row2[next_row+3+1] =
                     (value >>  8) & 0xFF;
            row2[0+2] = row2[3+2] = row2[next_row+2] = row2[next_row+3+2] =
                     (value >> 16) & 0xFF;
            row2 += 2*3;

            L = *lum2++;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            row2[0+0] = row2[3+0] = row2[next_row+0] = row2[next_row+3+0] =
                     (value      ) & 0xFF;
            row2[0+1] = row2[3+1] = row2[next_row+1] = row2[next_row+3+1] =
                     (value >>  8) & 0xFF;
            row2[0+2] = row2[3+2] = row2[next_row+2] = row2[next_row+3+2] =
                     (value >> 16) & 0xFF;
            row2 += 2*3;
        }

        /*
         * These values are at the start of the next line, (due
         * to the ++'s above),but they need to be at the start
         * of the line after that.
         */
        lum  += cols;
        lum2 += cols;
        row1 += mod;
        row2 += mod;
    }
}

static void Color32DitherYV12Mod2X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int* row1 = (unsigned int*) out;
    const int next_row = cols*2+mod;
    unsigned int* row2 = row1 + 2*next_row;
    unsigned char* lum2;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    lum2 = lum + cols;

    mod = (next_row * 3) + mod;

    y = rows / 2;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            ++cr; ++cb;

            L = *lum++;
            row1[0] = row1[1] = row1[next_row] = row1[next_row+1] =
                                       (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row1 += 2;

            L = *lum++;
            row1[0] = row1[1] = row1[next_row] = row1[next_row+1] =
                                       (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row1 += 2;


            /* Now, do second row. */

            L = *lum2++;
            row2[0] = row2[1] = row2[next_row] = row2[next_row+1] =
                                       (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row2 += 2;

            L = *lum2++;
            row2[0] = row2[1] = row2[next_row] = row2[next_row+1] =
                                       (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row2 += 2;
        }

        /*
         * These values are at the start of the next line, (due
         * to the ++'s above),but they need to be at the start
         * of the line after that.
         */
        lum  += cols;
        lum2 += cols;
        row1 += mod;
        row2 += mod;
    }
}

static void Color16DitherYUY2Mod1X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned short* row;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    row = (unsigned short*) out;

    y = rows;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            cr += 4; cb += 4;

            L = *lum; lum += 2;
            *row++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

            L = *lum; lum += 2;
            *row++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

        }

        row += mod;
    }
}

static void Color24DitherYUY2Mod1X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int value;
    unsigned char* row;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    row = (unsigned char*) out;
    mod *= 3;
    y = rows;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            cr += 4; cb += 4;

            L = *lum; lum += 2;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            *row++ = (value      ) & 0xFF;
            *row++ = (value >>  8) & 0xFF;
            *row++ = (value >> 16) & 0xFF;

            L = *lum; lum += 2;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            *row++ = (value      ) & 0xFF;
            *row++ = (value >>  8) & 0xFF;
            *row++ = (value >> 16) & 0xFF;

        }
        row += mod;
    }
}

static void Color32DitherYUY2Mod1X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int* row;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    row = (unsigned int*) out;
    y = rows;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            cr += 4; cb += 4;

            L = *lum; lum += 2;
            *row++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);

            L = *lum; lum += 2;
            *row++ = (rgb_2_pix[ L + cr_r ] |
                       rgb_2_pix[ L + crb_g ] |
                       rgb_2_pix[ L + cb_b ]);


        }
        row += mod;
    }
}

/*
 * In this function I make use of a nasty trick. The tables have the lower
 * 16 bits replicated in the upper 16. This means I can write ints and get
 * the horisontal doubling for free (almost).
 */
static void Color16DitherYUY2Mod2X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int* row = (unsigned int*) out;
    const int next_row = cols+(mod/2);
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;

    y = rows;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            cr += 4; cb += 4;

            L = *lum; lum += 2;
            row[0] = row[next_row] = (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row++;

            L = *lum; lum += 2;
            row[0] = row[next_row] = (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row++;

        }
        row += next_row;
    }
}

static void Color24DitherYUY2Mod2X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int value;
    unsigned char* row = out;
    const int next_row = (cols*2 + mod) * 3;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;
    y = rows;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            cr += 4; cb += 4;

            L = *lum; lum += 2;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            row[0+0] = row[3+0] = row[next_row+0] = row[next_row+3+0] =
                     (value      ) & 0xFF;
            row[0+1] = row[3+1] = row[next_row+1] = row[next_row+3+1] =
                     (value >>  8) & 0xFF;
            row[0+2] = row[3+2] = row[next_row+2] = row[next_row+3+2] =
                     (value >> 16) & 0xFF;
            row += 2*3;

            L = *lum; lum += 2;
            value = (rgb_2_pix[ L + cr_r ] |
                     rgb_2_pix[ L + crb_g ] |
                     rgb_2_pix[ L + cb_b ]);
            row[0+0] = row[3+0] = row[next_row+0] = row[next_row+3+0] =
                     (value      ) & 0xFF;
            row[0+1] = row[3+1] = row[next_row+1] = row[next_row+3+1] =
                     (value >>  8) & 0xFF;
            row[0+2] = row[3+2] = row[next_row+2] = row[next_row+3+2] =
                     (value >> 16) & 0xFF;
            row += 2*3;

        }
        row += next_row;
    }
}

static void Color32DitherYUY2Mod2X( int *colortab, Uint32 *rgb_2_pix,
                                    unsigned char *lum, unsigned char *cr,
                                    unsigned char *cb, unsigned char *out,
                                    int rows, int cols, int mod )
{
    unsigned int* row = (unsigned int*) out;
    const int next_row = cols*2+mod;
    int x, y;
    int cr_r;
    int crb_g;
    int cb_b;
    int cols_2 = cols / 2;
    mod+=mod;
    y = rows;
    while( y-- )
    {
        x = cols_2;
        while( x-- )
        {
            register int L;

            cr_r   = 0*768+256 + colortab[ *cr + 0*256 ];
            crb_g  = 1*768+256 + colortab[ *cr + 1*256 ]
                               + colortab[ *cb + 2*256 ];
            cb_b   = 2*768+256 + colortab[ *cb + 3*256 ];
            cr += 4; cb += 4;

            L = *lum; lum += 2;
            row[0] = row[1] = row[next_row] = row[next_row+1] =
                                       (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row += 2;

            L = *lum; lum += 2;
            row[0] = row[1] = row[next_row] = row[next_row+1] =
                                       (rgb_2_pix[ L + cr_r ] |
                                        rgb_2_pix[ L + crb_g ] |
                                        rgb_2_pix[ L + cb_b ]);
            row += 2;


        }

        row += next_row;
    }
}

/*
 * How many 1 bits are there in the Uint32.
 * Low performance, do not call often.
 */
static int number_of_bits_set( Uint32 a )
{
    if(!a) return 0;
    if(a & 1) return 1 + number_of_bits_set(a >> 1);
    return(number_of_bits_set(a >> 1));
}

/*
 * How many 0 bits are there at least significant end of Uint32.
 * Low performance, do not call often.
 */
static int free_bits_at_bottom( Uint32 a )
{
      /* assume char is 8 bits */
    if(!a) return sizeof(Uint32) * 8;
    if(((Sint32)a) & 1l) return 0;
    return 1 + free_bits_at_bottom ( a >> 1);
}


FOX_Overlay *FOX_CreateYUV_SW(_THIS, int width, int height, Uint32 format, FOX_Surface *display)
{
	FOX_Overlay *overlay;
	struct private_yuvhwdata *swdata;
	int *Cr_r_tab;
	int *Cr_g_tab;
	int *Cb_g_tab;
	int *Cb_b_tab;
	Uint32 *r_2_pix_alloc;
	Uint32 *g_2_pix_alloc;
	Uint32 *b_2_pix_alloc;
	int i, cpu_mmx;
	int CR, CB;
	Uint32 Rmask, Gmask, Bmask;

	/* Only RGB packed pixel conversion supported */
	if ( (display->format->BytesPerPixel != 2) &&
	     (display->format->BytesPerPixel != 3) &&
	     (display->format->BytesPerPixel != 4) ) {
		FOX_SetError("Can't use YUV data on non 16/24/32 bit surfaces");
		return(NULL);
	}

	/* Verify that we support the format */
	switch (format) {
	    case FOX_YV12_OVERLAY:
	    case FOX_IYUV_OVERLAY:
	    case FOX_YUY2_OVERLAY:
	    case FOX_UYVY_OVERLAY:
	    case FOX_YVYU_OVERLAY:
		break;
	    default:
		FOX_SetError("Unsupported YUV format");
		return(NULL);
	}

	/* Create the overlay structure */
	overlay = (FOX_Overlay *)malloc(sizeof *overlay);
	if ( overlay == NULL ) {
		FOX_OutOfMemory();
		return(NULL);
	}
	memset(overlay, 0, (sizeof *overlay));

	/* Fill in the basic members */
	overlay->format = format;
	overlay->w = width;
	overlay->h = height;

	/* Set up the YUV surface function structure */
	overlay->hwfuncs = &sw_yuvfuncs;

	/* Create the pixel data and lookup tables */
	swdata = (struct private_yuvhwdata *)malloc(sizeof *swdata);
	overlay->hwdata = swdata;
	if ( swdata == NULL ) {
		FOX_OutOfMemory();
		FOX_FreeYUVOverlay(overlay);
		return(NULL);
	}
	swdata->stretch = NULL;
	swdata->display = display;
	swdata->pixels = (Uint8 *) malloc(width*height*2);
	swdata->colortab = (int *)malloc(4*256*sizeof(int));
	Cr_r_tab = &swdata->colortab[0*256];
	Cr_g_tab = &swdata->colortab[1*256];
	Cb_g_tab = &swdata->colortab[2*256];
	Cb_b_tab = &swdata->colortab[3*256];
	swdata->rgb_2_pix = (Uint32 *)malloc(3*768*sizeof(Uint32));
	r_2_pix_alloc = &swdata->rgb_2_pix[0*768];
	g_2_pix_alloc = &swdata->rgb_2_pix[1*768];
	b_2_pix_alloc = &swdata->rgb_2_pix[2*768];
	if ( ! swdata->pixels || ! swdata->colortab || ! swdata->rgb_2_pix ) {
		FOX_OutOfMemory();
		FOX_FreeYUVOverlay(overlay);
		return(NULL);
	}

	/* Generate the tables for the display surface */
	for (i=0; i<256; i++) {
		/* Gamma correction (luminescence table) and chroma correction
		   would be done here.  See the Berkeley mpeg_play sources.
		*/
		CB = CR = (i-128);
		Cr_r_tab[i] = (int) ( (0.419/0.299) * CR);
		Cr_g_tab[i] = (int) (-(0.299/0.419) * CR);
		Cb_g_tab[i] = (int) (-(0.114/0.331) * CB); 
		Cb_b_tab[i] = (int) ( (0.587/0.331) * CB);
	}

	/* 
	 * Set up entries 0-255 in rgb-to-pixel value tables.
	 */
	Rmask = display->format->Rmask;
	Gmask = display->format->Gmask;
	Bmask = display->format->Bmask;
	for ( i=0; i<256; ++i ) {
		r_2_pix_alloc[i+256] = i >> (8 - number_of_bits_set(Rmask));
		r_2_pix_alloc[i+256] <<= free_bits_at_bottom(Rmask);
		g_2_pix_alloc[i+256] = i >> (8 - number_of_bits_set(Gmask));
		g_2_pix_alloc[i+256] <<= free_bits_at_bottom(Gmask);
		b_2_pix_alloc[i+256] = i >> (8 - number_of_bits_set(Bmask));
		b_2_pix_alloc[i+256] <<= free_bits_at_bottom(Bmask);
	}

	/*
	 * If we have 16-bit output depth, then we double the value
	 * in the top word. This means that we can write out both
	 * pixels in the pixel doubling mode with one op. It is 
	 * harmless in the normal case as storing a 32-bit value
	 * through a short pointer will lose the top bits anyway.
	 */
	if( display->format->BytesPerPixel == 2 ) {
		for ( i=0; i<256; ++i ) {
			r_2_pix_alloc[i+256] |= (r_2_pix_alloc[i+256]) << 16;
			g_2_pix_alloc[i+256] |= (g_2_pix_alloc[i+256]) << 16;
			b_2_pix_alloc[i+256] |= (b_2_pix_alloc[i+256]) << 16;
		}
	}

	/*
	 * Spread out the values we have to the rest of the array so that
	 * we do not need to check for overflow.
	 */
	for ( i=0; i<256; ++i ) {
		r_2_pix_alloc[i] = r_2_pix_alloc[256];
		r_2_pix_alloc[i+512] = r_2_pix_alloc[511];
		g_2_pix_alloc[i] = g_2_pix_alloc[256];
		g_2_pix_alloc[i+512] = g_2_pix_alloc[511];
		b_2_pix_alloc[i] = b_2_pix_alloc[256];
		b_2_pix_alloc[i+512] = b_2_pix_alloc[511];
	}

	/* You have chosen wisely... */
	switch (format) {
	    case FOX_YV12_OVERLAY:
	    case FOX_IYUV_OVERLAY:
		cpu_mmx = CPU_Flags() & MMX_CPU;
		if ( display->format->BytesPerPixel == 2 ) {
#if defined(i386) && defined(__GNUC__) && defined(USE_ASMBLIT)
			/* inline assembly functions */
			if ( cpu_mmx && (Rmask == 0xF800) &&
			                (Gmask == 0x07E0) &&
				        (Bmask == 0x001F) &&
			                (width & 15) == 0) {
/*printf("Using MMX 16-bit 565 dither\n");*/
				swdata->Display1X = Color565DitherYV12MMX1X;
			} else {
/*printf("Using C 16-bit dither\n");*/
				swdata->Display1X = Color16DitherYV12Mod1X;
			}
#else
			swdata->Display1X = Color16DitherYV12Mod1X;
#endif
			swdata->Display2X = Color16DitherYV12Mod2X;
		}
		if ( display->format->BytesPerPixel == 3 ) {
			swdata->Display1X = Color24DitherYV12Mod1X;
			swdata->Display2X = Color24DitherYV12Mod2X;
		}
		if ( display->format->BytesPerPixel == 4 ) {
#if defined(i386) && defined(__GNUC__) && defined(USE_ASMBLIT)
			/* inline assembly functions */
			if ( cpu_mmx && (Rmask == 0x00FF0000) &&
			                (Gmask == 0x0000FF00) &&
				        (Bmask == 0x000000FF) && 
			                (width & 15) == 0) {
/*printf("Using MMX 32-bit dither\n");*/
				swdata->Display1X = ColorRGBDitherYV12MMX1X;
			} else {
/*printf("Using C 32-bit dither\n");*/
				swdata->Display1X = Color32DitherYV12Mod1X;
			}
#else
			swdata->Display1X = Color32DitherYV12Mod1X;
#endif
			swdata->Display2X = Color32DitherYV12Mod2X;
		}
		break;
	    case FOX_YUY2_OVERLAY:
	    case FOX_UYVY_OVERLAY:
	    case FOX_YVYU_OVERLAY:
		if ( display->format->BytesPerPixel == 2 ) {
			swdata->Display1X = Color16DitherYUY2Mod1X;
			swdata->Display2X = Color16DitherYUY2Mod2X;
		}
		if ( display->format->BytesPerPixel == 3 ) {
			swdata->Display1X = Color24DitherYUY2Mod1X;
			swdata->Display2X = Color24DitherYUY2Mod2X;
		}
		if ( display->format->BytesPerPixel == 4 ) {
			swdata->Display1X = Color32DitherYUY2Mod1X;
			swdata->Display2X = Color32DitherYUY2Mod2X;
		}
		break;
	    default:
		/* We should never get here (caught above) */
		break;
	}

	/* Find the pitch and offset values for the overlay */
	overlay->pitches = swdata->pitches;
	overlay->pixels = swdata->planes;
	switch (format) {
	    case FOX_YV12_OVERLAY:
	    case FOX_IYUV_OVERLAY:
		overlay->pitches[0] = overlay->w;
		overlay->pitches[1] = overlay->pitches[0] / 2;
		overlay->pitches[2] = overlay->pitches[0] / 2;
	        overlay->pixels[0] = swdata->pixels;
	        overlay->pixels[1] = overlay->pixels[0] +
		                     overlay->pitches[0] * overlay->h;
	        overlay->pixels[2] = overlay->pixels[1] +
		                     overlay->pitches[1] * overlay->h / 2;
		overlay->planes = 3;
		break;
	    case FOX_YUY2_OVERLAY:
	    case FOX_UYVY_OVERLAY:
	    case FOX_YVYU_OVERLAY:
		overlay->pitches[0] = overlay->w*2;
	        overlay->pixels[0] = swdata->pixels;
		overlay->planes = 1;
		break;
	    default:
		/* We should never get here (caught above) */
		break;
	}

	/* We're all done.. */
	return(overlay);
}

int FOX_LockYUV_SW(_THIS, FOX_Overlay *overlay)
{
	return(0);
}

void FOX_UnlockYUV_SW(_THIS, FOX_Overlay *overlay)
{
	return;
}

int FOX_DisplayYUV_SW(_THIS, FOX_Overlay *overlay, FOX_Rect *dstrect)
{
	struct private_yuvhwdata *swdata;
	FOX_Surface *stretch;
	FOX_Surface *display;
	int scale_2x;
	Uint8 *lum, *Cr, *Cb;
	Uint8 *dst;
	int mod;

	swdata = overlay->hwdata;
	scale_2x = 0;
	stretch = 0;
	if ( (overlay->w != dstrect->w) || (overlay->h != dstrect->h) ) {
		if ( (dstrect->w == 2*overlay->w) &&
		     (dstrect->h == 2*overlay->h) ) {
			scale_2x = 1;
		} else {
			if ( ! swdata->stretch ) {
				display = swdata->display;
				swdata->stretch = FOX_CreateRGBSurface(
					FOX_SWSURFACE,
					overlay->w, overlay->h,
					display->format->BitsPerPixel,
					display->format->Rmask,
					display->format->Gmask,
					display->format->Bmask, 0);
				if ( ! swdata->stretch ) {
					return(-1);
				}
			}
			stretch = swdata->stretch;
		}
	}

	if ( stretch ) {
		display = stretch;
	} else {
		display = swdata->display;
	}
	switch (overlay->format) {
	    case FOX_YV12_OVERLAY:
		lum = overlay->pixels[0];
		Cr =  overlay->pixels[1];
		Cb =  overlay->pixels[2];
		break;
	    case FOX_IYUV_OVERLAY:
		lum = overlay->pixels[0];
		Cr =  overlay->pixels[2];
		Cb =  overlay->pixels[1];
		break;
	    case FOX_YUY2_OVERLAY:
		lum = overlay->pixels[0];
		Cr = lum + 3;
		Cb = lum + 1;
		break;
	    case FOX_UYVY_OVERLAY:
		lum = overlay->pixels[0]+1;
		Cr = lum + 1;
		Cb = lum - 1;
		break;
	    case FOX_YVYU_OVERLAY:
		lum = overlay->pixels[0];
		Cr = lum + 1;
		Cb = lum + 3;
		break;
	    default:
		FOX_SetError("Unsupported YUV format in blit (??)");
		return(-1);
	}
	if ( FOX_MUSTLOCK(display) ) {
        	if ( FOX_LockSurface(display) < 0 ) {
			return(-1);
		}
	}
	if ( stretch ) {
		dst = (Uint8 *)stretch->pixels;
	} else {
		dst = (Uint8 *)display->pixels
			+ dstrect->x * display->format->BytesPerPixel
			+ dstrect->y * display->pitch;
	}
	mod = (display->pitch / display->format->BytesPerPixel);

	if ( scale_2x ) {
		mod -= (overlay->w * 2);
		swdata->Display2X(swdata->colortab, swdata->rgb_2_pix,
		                  lum, Cr, Cb, dst, overlay->h, overlay->w,mod);
	} else {
		mod -= overlay->w;
		swdata->Display1X(swdata->colortab, swdata->rgb_2_pix,
		                  lum, Cr, Cb, dst, overlay->h, overlay->w,mod);
	}
	if ( FOX_MUSTLOCK(display) ) {
		FOX_UnlockSurface(display);
	}
	if ( stretch ) {
		display = swdata->display;
		FOX_SoftStretch(stretch, NULL, display, dstrect);
	}
	FOX_UpdateRects(display, 1, dstrect);

	return(0);
}

void FOX_FreeYUV_SW(_THIS, FOX_Overlay *overlay)
{
	struct private_yuvhwdata *swdata;

	swdata = overlay->hwdata;
	if ( swdata ) {
		if ( swdata->stretch ) {
			FOX_FreeSurface(swdata->stretch);
		}
		if ( swdata->pixels ) {
			free(swdata->pixels);
		}
		if ( swdata->colortab ) {
			free(swdata->colortab);
		}
		if ( swdata->rgb_2_pix ) {
			free(swdata->rgb_2_pix);
		}
		free(swdata);
	}
}

// end
