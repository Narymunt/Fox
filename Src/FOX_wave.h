// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:


// grabbed z msdn 
// wave format 

#define RIFF			0x46464952		// RIFF
#define WAVE			0x45564157		// WAVE
#define FACT			0x74636166		// fact
#define LIST			0x5453494c		// LIST
#define FMT				0x20746D66		// fmt
#define DATA			0x61746164		// data
#define PCM_CODE		0x0001
#define MS_ADPCM_CODE	0x0002
#define IMA_ADPCM_CODE	0x0011
#define WAVE_MONO		1
#define WAVE_STEREO		2

// 3x chunks 

typedef struct WaveFMT {

/* nie zapisane
	Uint32	FMTchunk;
	Uint32	fmtlen;
*/
	Uint16	encoding;	
	Uint16	channels;			// 1 = mono, 2 = stereo
	Uint32	frequency;			// 11025, 22050, 44100
	Uint32	byterate;			// bpsecond
	Uint16	blockalign;			// bytes per sample 
	Uint16	bitspersample;		// 8,12,16,4 ADPCM
} WaveFMT;

// glowna struktura 

typedef struct Chunk {
	Uint32 magic;
	Uint32 length;
	Uint8 *data;			// dane zawieraja dlugosc i magic 
} Chunk;

// end
