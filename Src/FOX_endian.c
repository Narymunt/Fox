// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - dobrac sie do wyzszych bajtow w int64 bez rolowania

// dynamiczne wczytywanie i zapisywanie 

// swap byte, long long zapis itd. 

#include "FOX_endian.h"


Uint16 FOX_ReadLE16 (FOX_RWops *src)
{
	Uint16 value;

	FOX_RWread(src, &value, (sizeof value), 1);
	return(FOX_SwapLE16(value));
}

Uint16 FOX_ReadBE16 (FOX_RWops *src)
{
	Uint16 value;

	FOX_RWread(src, &value, (sizeof value), 1);
	return(FOX_SwapBE16(value));
}

Uint32 FOX_ReadLE32 (FOX_RWops *src)
{
	Uint32 value;

	FOX_RWread(src, &value, (sizeof value), 1);
	return(FOX_SwapLE32(value));
}

Uint32 FOX_ReadBE32 (FOX_RWops *src)
{
	Uint32 value;

	FOX_RWread(src, &value, (sizeof value), 1);
	return(FOX_SwapBE32(value));
}

Uint64 FOX_ReadLE64 (FOX_RWops *src)
{
	Uint64 value;

	FOX_RWread(src, &value, (sizeof value), 1);
	return(FOX_SwapLE64(value));
}

Uint64 FOX_ReadBE64 (FOX_RWops *src)
{
	Uint64 value;

	FOX_RWread(src, &value, (sizeof value), 1);
	return(FOX_SwapBE64(value));
}

int FOX_WriteLE16 (FOX_RWops *dst, Uint16 value)
{
	value = FOX_SwapLE16(value);
	return(FOX_RWwrite(dst, &value, (sizeof value), 1));
}

int FOX_WriteBE16 (FOX_RWops *dst, Uint16 value)
{
	value = FOX_SwapBE16(value);
	return(FOX_RWwrite(dst, &value, (sizeof value), 1));
}

int FOX_WriteLE32 (FOX_RWops *dst, Uint32 value)
{
	value = FOX_SwapLE32(value);
	return(FOX_RWwrite(dst, &value, (sizeof value), 1));
}

int FOX_WriteBE32 (FOX_RWops *dst, Uint32 value)
{
	value = FOX_SwapBE32(value);
	return(FOX_RWwrite(dst, &value, (sizeof value), 1));
}

int FOX_WriteLE64 (FOX_RWops *dst, Uint64 value)
{
	value = FOX_SwapLE64(value);
	return(FOX_RWwrite(dst, &value, (sizeof value), 1));
}

int FOX_WriteBE64 (FOX_RWops *dst, Uint64 value)
{
	value = FOX_SwapBE64(value);
	return(FOX_RWwrite(dst, &value, (sizeof value), 1));
}

// end
