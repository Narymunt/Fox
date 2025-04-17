// do konwersji pomiedzy formatami 
// 16bpp->32bpp itepe

//=== konwersja z 32bpp na 16bpp

int Conv32to16(unsigned char ucR, unsigned char ucG, unsigned char ucB)
{
	return ((ucR>>3)<<11)+((ucG>>2)<<5)+((ucB>>3));
}

