// Fox v0.5 
// by Jaroslaw Rozynski
//=== 
// TODO:
// - sprawdzanie jakie filtry sa dostepne sprzetowo
// - konwersje 
// - rozdzielenie wszystkiego na trojkaty
// - sprawdzenie obslugi alpha w dx 8.0
// - wypieprzyc wszystko co jest zwiazane z debugiem
// - 2320 calkowita exterminacja opengl
// - usuniecie wszystkich rzeczy zwiazanych z 8bpp

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>
#include "directx.h"

#include "FOX_error.h"
#include "FOX_timer.h"
#include "FOX_events.h"
#include "FOX_syswm.h"
#include "FOX_sysvideo.h"
#include "FOX_RLEaccel_c.h"
#include "FOX_blit.h"
#include "FOX_pixels_c.h"
#include "FOX_dx8video.h"
#include "FOX_syswm_c.h"
#include "FOX_sysmouse_c.h"
#include "FOX_dx8events_c.h"
#include "FOX_dx8yuv.h"
//#include "FOX_wingl_c.h"

// funkcje do stworzenia trybu graficznego dostepnego przez opengl - wtedy uzywamy dib a nie directx

//extern FOX_Surface *DIB_SetVideoMode(_THIS, FOX_Surface *current,
//				int width, int height, int bpp, Uint32 flags);

// funkcje directx  i ich wskazniki na struktury video i ich zdarzenia

HRESULT (WINAPI *DDrawCreate)( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
HRESULT (WINAPI *DInputCreate)(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);

// to jest lista trybow enummodes2

struct DX8EnumRect {
	FOX_Rect r;
	struct DX8EnumRect* next;
};
static struct DX8EnumRect *enumlists[NUM_MODELISTS];

// experymentalnie


// klawiatura

static DIOBJECTDATAFORMAT KBD_fmt[] = {
	{ &GUID_Key, 0, 0x8000000C, 0x00000000 },
	{ &GUID_Key, 1, 0x8000010C, 0x00000000 },
	{ &GUID_Key, 2, 0x8000020C, 0x00000000 },
	{ &GUID_Key, 3, 0x8000030C, 0x00000000 },
	{ &GUID_Key, 4, 0x8000040C, 0x00000000 },
	{ &GUID_Key, 5, 0x8000050C, 0x00000000 },
	{ &GUID_Key, 6, 0x8000060C, 0x00000000 },
	{ &GUID_Key, 7, 0x8000070C, 0x00000000 },
	{ &GUID_Key, 8, 0x8000080C, 0x00000000 },
	{ &GUID_Key, 9, 0x8000090C, 0x00000000 },
	{ &GUID_Key, 10, 0x80000A0C, 0x00000000 },
	{ &GUID_Key, 11, 0x80000B0C, 0x00000000 },
	{ &GUID_Key, 12, 0x80000C0C, 0x00000000 },
	{ &GUID_Key, 13, 0x80000D0C, 0x00000000 },
	{ &GUID_Key, 14, 0x80000E0C, 0x00000000 },
	{ &GUID_Key, 15, 0x80000F0C, 0x00000000 },
	{ &GUID_Key, 16, 0x8000100C, 0x00000000 },
	{ &GUID_Key, 17, 0x8000110C, 0x00000000 },
	{ &GUID_Key, 18, 0x8000120C, 0x00000000 },
	{ &GUID_Key, 19, 0x8000130C, 0x00000000 },
	{ &GUID_Key, 20, 0x8000140C, 0x00000000 },
	{ &GUID_Key, 21, 0x8000150C, 0x00000000 },
	{ &GUID_Key, 22, 0x8000160C, 0x00000000 },
	{ &GUID_Key, 23, 0x8000170C, 0x00000000 },
	{ &GUID_Key, 24, 0x8000180C, 0x00000000 },
	{ &GUID_Key, 25, 0x8000190C, 0x00000000 },
	{ &GUID_Key, 26, 0x80001A0C, 0x00000000 },
	{ &GUID_Key, 27, 0x80001B0C, 0x00000000 },
	{ &GUID_Key, 28, 0x80001C0C, 0x00000000 },
	{ &GUID_Key, 29, 0x80001D0C, 0x00000000 },
	{ &GUID_Key, 30, 0x80001E0C, 0x00000000 },
	{ &GUID_Key, 31, 0x80001F0C, 0x00000000 },
	{ &GUID_Key, 32, 0x8000200C, 0x00000000 },
	{ &GUID_Key, 33, 0x8000210C, 0x00000000 },
	{ &GUID_Key, 34, 0x8000220C, 0x00000000 },
	{ &GUID_Key, 35, 0x8000230C, 0x00000000 },
	{ &GUID_Key, 36, 0x8000240C, 0x00000000 },
	{ &GUID_Key, 37, 0x8000250C, 0x00000000 },
	{ &GUID_Key, 38, 0x8000260C, 0x00000000 },
	{ &GUID_Key, 39, 0x8000270C, 0x00000000 },
	{ &GUID_Key, 40, 0x8000280C, 0x00000000 },
	{ &GUID_Key, 41, 0x8000290C, 0x00000000 },
	{ &GUID_Key, 42, 0x80002A0C, 0x00000000 },
	{ &GUID_Key, 43, 0x80002B0C, 0x00000000 },
	{ &GUID_Key, 44, 0x80002C0C, 0x00000000 },
	{ &GUID_Key, 45, 0x80002D0C, 0x00000000 },
	{ &GUID_Key, 46, 0x80002E0C, 0x00000000 },
	{ &GUID_Key, 47, 0x80002F0C, 0x00000000 },
	{ &GUID_Key, 48, 0x8000300C, 0x00000000 },
	{ &GUID_Key, 49, 0x8000310C, 0x00000000 },
	{ &GUID_Key, 50, 0x8000320C, 0x00000000 },
	{ &GUID_Key, 51, 0x8000330C, 0x00000000 },
	{ &GUID_Key, 52, 0x8000340C, 0x00000000 },
	{ &GUID_Key, 53, 0x8000350C, 0x00000000 },
	{ &GUID_Key, 54, 0x8000360C, 0x00000000 },
	{ &GUID_Key, 55, 0x8000370C, 0x00000000 },
	{ &GUID_Key, 56, 0x8000380C, 0x00000000 },
	{ &GUID_Key, 57, 0x8000390C, 0x00000000 },
	{ &GUID_Key, 58, 0x80003A0C, 0x00000000 },
	{ &GUID_Key, 59, 0x80003B0C, 0x00000000 },
	{ &GUID_Key, 60, 0x80003C0C, 0x00000000 },
	{ &GUID_Key, 61, 0x80003D0C, 0x00000000 },
	{ &GUID_Key, 62, 0x80003E0C, 0x00000000 },
	{ &GUID_Key, 63, 0x80003F0C, 0x00000000 },
	{ &GUID_Key, 64, 0x8000400C, 0x00000000 },
	{ &GUID_Key, 65, 0x8000410C, 0x00000000 },
	{ &GUID_Key, 66, 0x8000420C, 0x00000000 },
	{ &GUID_Key, 67, 0x8000430C, 0x00000000 },
	{ &GUID_Key, 68, 0x8000440C, 0x00000000 },
	{ &GUID_Key, 69, 0x8000450C, 0x00000000 },
	{ &GUID_Key, 70, 0x8000460C, 0x00000000 },
	{ &GUID_Key, 71, 0x8000470C, 0x00000000 },
	{ &GUID_Key, 72, 0x8000480C, 0x00000000 },
	{ &GUID_Key, 73, 0x8000490C, 0x00000000 },
	{ &GUID_Key, 74, 0x80004A0C, 0x00000000 },
	{ &GUID_Key, 75, 0x80004B0C, 0x00000000 },
	{ &GUID_Key, 76, 0x80004C0C, 0x00000000 },
	{ &GUID_Key, 77, 0x80004D0C, 0x00000000 },
	{ &GUID_Key, 78, 0x80004E0C, 0x00000000 },
	{ &GUID_Key, 79, 0x80004F0C, 0x00000000 },
	{ &GUID_Key, 80, 0x8000500C, 0x00000000 },
	{ &GUID_Key, 81, 0x8000510C, 0x00000000 },
	{ &GUID_Key, 82, 0x8000520C, 0x00000000 },
	{ &GUID_Key, 83, 0x8000530C, 0x00000000 },
	{ &GUID_Key, 84, 0x8000540C, 0x00000000 },
	{ &GUID_Key, 85, 0x8000550C, 0x00000000 },
	{ &GUID_Key, 86, 0x8000560C, 0x00000000 },
	{ &GUID_Key, 87, 0x8000570C, 0x00000000 },
	{ &GUID_Key, 88, 0x8000580C, 0x00000000 },
	{ &GUID_Key, 89, 0x8000590C, 0x00000000 },
	{ &GUID_Key, 90, 0x80005A0C, 0x00000000 },
	{ &GUID_Key, 91, 0x80005B0C, 0x00000000 },
	{ &GUID_Key, 92, 0x80005C0C, 0x00000000 },
	{ &GUID_Key, 93, 0x80005D0C, 0x00000000 },
	{ &GUID_Key, 94, 0x80005E0C, 0x00000000 },
	{ &GUID_Key, 95, 0x80005F0C, 0x00000000 },
	{ &GUID_Key, 96, 0x8000600C, 0x00000000 },
	{ &GUID_Key, 97, 0x8000610C, 0x00000000 },
	{ &GUID_Key, 98, 0x8000620C, 0x00000000 },
	{ &GUID_Key, 99, 0x8000630C, 0x00000000 },
	{ &GUID_Key, 100, 0x8000640C, 0x00000000 },
	{ &GUID_Key, 101, 0x8000650C, 0x00000000 },
	{ &GUID_Key, 102, 0x8000660C, 0x00000000 },
	{ &GUID_Key, 103, 0x8000670C, 0x00000000 },
	{ &GUID_Key, 104, 0x8000680C, 0x00000000 },
	{ &GUID_Key, 105, 0x8000690C, 0x00000000 },
	{ &GUID_Key, 106, 0x80006A0C, 0x00000000 },
	{ &GUID_Key, 107, 0x80006B0C, 0x00000000 },
	{ &GUID_Key, 108, 0x80006C0C, 0x00000000 },
	{ &GUID_Key, 109, 0x80006D0C, 0x00000000 },
	{ &GUID_Key, 110, 0x80006E0C, 0x00000000 },
	{ &GUID_Key, 111, 0x80006F0C, 0x00000000 },
	{ &GUID_Key, 112, 0x8000700C, 0x00000000 },
	{ &GUID_Key, 113, 0x8000710C, 0x00000000 },
	{ &GUID_Key, 114, 0x8000720C, 0x00000000 },
	{ &GUID_Key, 115, 0x8000730C, 0x00000000 },
	{ &GUID_Key, 116, 0x8000740C, 0x00000000 },
	{ &GUID_Key, 117, 0x8000750C, 0x00000000 },
	{ &GUID_Key, 118, 0x8000760C, 0x00000000 },
	{ &GUID_Key, 119, 0x8000770C, 0x00000000 },
	{ &GUID_Key, 120, 0x8000780C, 0x00000000 },
	{ &GUID_Key, 121, 0x8000790C, 0x00000000 },
	{ &GUID_Key, 122, 0x80007A0C, 0x00000000 },
	{ &GUID_Key, 123, 0x80007B0C, 0x00000000 },
	{ &GUID_Key, 124, 0x80007C0C, 0x00000000 },
	{ &GUID_Key, 125, 0x80007D0C, 0x00000000 },
	{ &GUID_Key, 126, 0x80007E0C, 0x00000000 },
	{ &GUID_Key, 127, 0x80007F0C, 0x00000000 },
	{ &GUID_Key, 128, 0x8000800C, 0x00000000 },
	{ &GUID_Key, 129, 0x8000810C, 0x00000000 },
	{ &GUID_Key, 130, 0x8000820C, 0x00000000 },
	{ &GUID_Key, 131, 0x8000830C, 0x00000000 },
	{ &GUID_Key, 132, 0x8000840C, 0x00000000 },
	{ &GUID_Key, 133, 0x8000850C, 0x00000000 },
	{ &GUID_Key, 134, 0x8000860C, 0x00000000 },
	{ &GUID_Key, 135, 0x8000870C, 0x00000000 },
	{ &GUID_Key, 136, 0x8000880C, 0x00000000 },
	{ &GUID_Key, 137, 0x8000890C, 0x00000000 },
	{ &GUID_Key, 138, 0x80008A0C, 0x00000000 },
	{ &GUID_Key, 139, 0x80008B0C, 0x00000000 },
	{ &GUID_Key, 140, 0x80008C0C, 0x00000000 },
	{ &GUID_Key, 141, 0x80008D0C, 0x00000000 },
	{ &GUID_Key, 142, 0x80008E0C, 0x00000000 },
	{ &GUID_Key, 143, 0x80008F0C, 0x00000000 },
	{ &GUID_Key, 144, 0x8000900C, 0x00000000 },
	{ &GUID_Key, 145, 0x8000910C, 0x00000000 },
	{ &GUID_Key, 146, 0x8000920C, 0x00000000 },
	{ &GUID_Key, 147, 0x8000930C, 0x00000000 },
	{ &GUID_Key, 148, 0x8000940C, 0x00000000 },
	{ &GUID_Key, 149, 0x8000950C, 0x00000000 },
	{ &GUID_Key, 150, 0x8000960C, 0x00000000 },
	{ &GUID_Key, 151, 0x8000970C, 0x00000000 },
	{ &GUID_Key, 152, 0x8000980C, 0x00000000 },
	{ &GUID_Key, 153, 0x8000990C, 0x00000000 },
	{ &GUID_Key, 154, 0x80009A0C, 0x00000000 },
	{ &GUID_Key, 155, 0x80009B0C, 0x00000000 },
	{ &GUID_Key, 156, 0x80009C0C, 0x00000000 },
	{ &GUID_Key, 157, 0x80009D0C, 0x00000000 },
	{ &GUID_Key, 158, 0x80009E0C, 0x00000000 },
	{ &GUID_Key, 159, 0x80009F0C, 0x00000000 },
	{ &GUID_Key, 160, 0x8000A00C, 0x00000000 },
	{ &GUID_Key, 161, 0x8000A10C, 0x00000000 },
	{ &GUID_Key, 162, 0x8000A20C, 0x00000000 },
	{ &GUID_Key, 163, 0x8000A30C, 0x00000000 },
	{ &GUID_Key, 164, 0x8000A40C, 0x00000000 },
	{ &GUID_Key, 165, 0x8000A50C, 0x00000000 },
	{ &GUID_Key, 166, 0x8000A60C, 0x00000000 },
	{ &GUID_Key, 167, 0x8000A70C, 0x00000000 },
	{ &GUID_Key, 168, 0x8000A80C, 0x00000000 },
	{ &GUID_Key, 169, 0x8000A90C, 0x00000000 },
	{ &GUID_Key, 170, 0x8000AA0C, 0x00000000 },
	{ &GUID_Key, 171, 0x8000AB0C, 0x00000000 },
	{ &GUID_Key, 172, 0x8000AC0C, 0x00000000 },
	{ &GUID_Key, 173, 0x8000AD0C, 0x00000000 },
	{ &GUID_Key, 174, 0x8000AE0C, 0x00000000 },
	{ &GUID_Key, 175, 0x8000AF0C, 0x00000000 },
	{ &GUID_Key, 176, 0x8000B00C, 0x00000000 },
	{ &GUID_Key, 177, 0x8000B10C, 0x00000000 },
	{ &GUID_Key, 178, 0x8000B20C, 0x00000000 },
	{ &GUID_Key, 179, 0x8000B30C, 0x00000000 },
	{ &GUID_Key, 180, 0x8000B40C, 0x00000000 },
	{ &GUID_Key, 181, 0x8000B50C, 0x00000000 },
	{ &GUID_Key, 182, 0x8000B60C, 0x00000000 },
	{ &GUID_Key, 183, 0x8000B70C, 0x00000000 },
	{ &GUID_Key, 184, 0x8000B80C, 0x00000000 },
	{ &GUID_Key, 185, 0x8000B90C, 0x00000000 },
	{ &GUID_Key, 186, 0x8000BA0C, 0x00000000 },
	{ &GUID_Key, 187, 0x8000BB0C, 0x00000000 },
	{ &GUID_Key, 188, 0x8000BC0C, 0x00000000 },
	{ &GUID_Key, 189, 0x8000BD0C, 0x00000000 },
	{ &GUID_Key, 190, 0x8000BE0C, 0x00000000 },
	{ &GUID_Key, 191, 0x8000BF0C, 0x00000000 },
	{ &GUID_Key, 192, 0x8000C00C, 0x00000000 },
	{ &GUID_Key, 193, 0x8000C10C, 0x00000000 },
	{ &GUID_Key, 194, 0x8000C20C, 0x00000000 },
	{ &GUID_Key, 195, 0x8000C30C, 0x00000000 },
	{ &GUID_Key, 196, 0x8000C40C, 0x00000000 },
	{ &GUID_Key, 197, 0x8000C50C, 0x00000000 },
	{ &GUID_Key, 198, 0x8000C60C, 0x00000000 },
	{ &GUID_Key, 199, 0x8000C70C, 0x00000000 },
	{ &GUID_Key, 200, 0x8000C80C, 0x00000000 },
	{ &GUID_Key, 201, 0x8000C90C, 0x00000000 },
	{ &GUID_Key, 202, 0x8000CA0C, 0x00000000 },
	{ &GUID_Key, 203, 0x8000CB0C, 0x00000000 },
	{ &GUID_Key, 204, 0x8000CC0C, 0x00000000 },
	{ &GUID_Key, 205, 0x8000CD0C, 0x00000000 },
	{ &GUID_Key, 206, 0x8000CE0C, 0x00000000 },
	{ &GUID_Key, 207, 0x8000CF0C, 0x00000000 },
	{ &GUID_Key, 208, 0x8000D00C, 0x00000000 },
	{ &GUID_Key, 209, 0x8000D10C, 0x00000000 },
	{ &GUID_Key, 210, 0x8000D20C, 0x00000000 },
	{ &GUID_Key, 211, 0x8000D30C, 0x00000000 },
	{ &GUID_Key, 212, 0x8000D40C, 0x00000000 },
	{ &GUID_Key, 213, 0x8000D50C, 0x00000000 },
	{ &GUID_Key, 214, 0x8000D60C, 0x00000000 },
	{ &GUID_Key, 215, 0x8000D70C, 0x00000000 },
	{ &GUID_Key, 216, 0x8000D80C, 0x00000000 },
	{ &GUID_Key, 217, 0x8000D90C, 0x00000000 },
	{ &GUID_Key, 218, 0x8000DA0C, 0x00000000 },
	{ &GUID_Key, 219, 0x8000DB0C, 0x00000000 },
	{ &GUID_Key, 220, 0x8000DC0C, 0x00000000 },
	{ &GUID_Key, 221, 0x8000DD0C, 0x00000000 },
	{ &GUID_Key, 222, 0x8000DE0C, 0x00000000 },
	{ &GUID_Key, 223, 0x8000DF0C, 0x00000000 },
	{ &GUID_Key, 224, 0x8000E00C, 0x00000000 },
	{ &GUID_Key, 225, 0x8000E10C, 0x00000000 },
	{ &GUID_Key, 226, 0x8000E20C, 0x00000000 },
	{ &GUID_Key, 227, 0x8000E30C, 0x00000000 },
	{ &GUID_Key, 228, 0x8000E40C, 0x00000000 },
	{ &GUID_Key, 229, 0x8000E50C, 0x00000000 },
	{ &GUID_Key, 230, 0x8000E60C, 0x00000000 },
	{ &GUID_Key, 231, 0x8000E70C, 0x00000000 },
	{ &GUID_Key, 232, 0x8000E80C, 0x00000000 },
	{ &GUID_Key, 233, 0x8000E90C, 0x00000000 },
	{ &GUID_Key, 234, 0x8000EA0C, 0x00000000 },
	{ &GUID_Key, 235, 0x8000EB0C, 0x00000000 },
	{ &GUID_Key, 236, 0x8000EC0C, 0x00000000 },
	{ &GUID_Key, 237, 0x8000ED0C, 0x00000000 },
	{ &GUID_Key, 238, 0x8000EE0C, 0x00000000 },
	{ &GUID_Key, 239, 0x8000EF0C, 0x00000000 },
	{ &GUID_Key, 240, 0x8000F00C, 0x00000000 },
	{ &GUID_Key, 241, 0x8000F10C, 0x00000000 },
	{ &GUID_Key, 242, 0x8000F20C, 0x00000000 },
	{ &GUID_Key, 243, 0x8000F30C, 0x00000000 },
	{ &GUID_Key, 244, 0x8000F40C, 0x00000000 },
	{ &GUID_Key, 245, 0x8000F50C, 0x00000000 },
	{ &GUID_Key, 246, 0x8000F60C, 0x00000000 },
	{ &GUID_Key, 247, 0x8000F70C, 0x00000000 },
	{ &GUID_Key, 248, 0x8000F80C, 0x00000000 },
	{ &GUID_Key, 249, 0x8000F90C, 0x00000000 },
	{ &GUID_Key, 250, 0x8000FA0C, 0x00000000 },
	{ &GUID_Key, 251, 0x8000FB0C, 0x00000000 },
	{ &GUID_Key, 252, 0x8000FC0C, 0x00000000 },
	{ &GUID_Key, 253, 0x8000FD0C, 0x00000000 },
	{ &GUID_Key, 254, 0x8000FE0C, 0x00000000 },
	{ &GUID_Key, 255, 0x8000FF0C, 0x00000000 },
};

const DIDATAFORMAT c_dfDIKeyboard = { 24, 16, 0x00000002, 256, 256, KBD_fmt };

// mysz

static DIOBJECTDATAFORMAT PTR_fmt[] = {
	{ &GUID_XAxis, 0, 0x00FFFF03, 0x00000000 },
	{ &GUID_YAxis, 4, 0x00FFFF03, 0x00000000 },
	{ &GUID_ZAxis, 8, 0x80FFFF03, 0x00000000 },
	{ NULL, 12, 0x00FFFF0C, 0x00000000 },
	{ NULL, 13, 0x00FFFF0C, 0x00000000 },
	{ NULL, 14, 0x80FFFF0C, 0x00000000 },
	{ NULL, 15, 0x80FFFF0C, 0x00000000 },
};

const DIDATAFORMAT c_dfDIMouse = { 24, 16, 0x00000002, 16, 7, PTR_fmt };

// funkcje inicjalizujace

static int DX8_VideoInit(_THIS, FOX_PixelFormat *vformat);

static FOX_Rect **DX8_ListModes(_THIS, FOX_PixelFormat *format, Uint32 flags);

static FOX_Surface *DX8_SetVideoMode(_THIS, FOX_Surface *current, int width, int height, int bpp, Uint32 flags);

static int DX8_SetColors(_THIS, int firstcolor, int ncolors,
			 FOX_Color *colors);

static void DX8_SwapGamma(_THIS);

#ifdef IID_IDirectDrawGammaControl
	static int DX8_SetGammaRamp(_THIS, Uint16 *ramp);
	static int DX8_GetGammaRamp(_THIS, Uint16 *ramp);
#endif
static void DX8_VideoQuit(_THIS);

// funkcje do obslugi hardware surface

static int DX8_AllocHWSurface(_THIS, FOX_Surface *surface);
static int DX8_CheckHWBlit(_THIS, FOX_Surface *src, FOX_Surface *dst);
static int DX8_FillHWRect(_THIS, FOX_Surface *dst, FOX_Rect *dstrect, Uint32 color);
static int DX8_SetHWColorKey(_THIS, FOX_Surface *surface, Uint32 key);
static int DX8_SetHWAlpha(_THIS, FOX_Surface *surface, Uint8 alpha);
static int DX8_LockHWSurface(_THIS, FOX_Surface *surface);
static void DX8_UnlockHWSurface(_THIS, FOX_Surface *surface);
static int DX8_FlipHWSurface(_THIS, FOX_Surface *surface);
static void DX8_FreeHWSurface(_THIS, FOX_Surface *surface);

static int DX8_AllocDDSurface(_THIS, FOX_Surface *surface, 
				LPDIRECTDRAWSURFACE3 requested, Uint32 flag);

// windows message

static void DX8_RealizePalette(_THIS);
static void DX8_PaletteChanged(_THIS, HWND window);
static void DX8_WinPAINT(_THIS, HDC hdc);

static int DX8_Available(void)
{
	HINSTANCE DInputDLL;
	HINSTANCE DDrawDLL;
	int dinput_ok;
	int ddraw_ok;

	// sprawdz wersje directx
	dinput_ok = 0;
	DInputDLL = LoadLibrary("DINPUT.DLL");
	
	if ( DInputDLL != NULL ) 
	{
		dinput_ok = 1;
	  	FreeLibrary(DInputDLL);
	}
	ddraw_ok = 0;
	DDrawDLL = LoadLibrary("DDRAW.DLL");
	
	if ( DDrawDLL != NULL ) 
	{
	  HRESULT (WINAPI *DDrawCreate)(GUID *,LPDIRECTDRAW *,IUnknown *);
	  LPDIRECTDRAW DDraw;
		
	  // sprobuj utworzyc obiekt directdraw
	  
	  DDrawCreate = (void *)GetProcAddress(DDrawDLL, "DirectDrawCreate");

	  if ( (DDrawCreate != NULL)
			&& !FAILED(DDrawCreate(NULL, &DDraw, NULL)) ) 
	  {
	    if ( !FAILED(IDirectDraw_SetCooperativeLevel(DDraw,
							NULL, DDSCL_NORMAL)) ) 
		{
	      DDSURFACEDESC desc;
	      LPDIRECTDRAWSURFACE  DDrawSurf;
	      LPDIRECTDRAWSURFACE3 DDrawSurf3;

		  // sprobuj stworzyc DirectDrawSurface3 

		  memset(&desc, 0, sizeof(desc));
	      desc.dwSize = sizeof(desc);
	      desc.dwFlags = DDSD_CAPS;
	      desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_VIDEOMEMORY;
	      
		  if ( !FAILED(IDirectDraw_CreateSurface(DDraw, &desc,
							&DDrawSurf, NULL)) ) 
		  {
	        if ( !FAILED(IDirectDrawSurface_QueryInterface(DDrawSurf,
			&IID_IDirectDrawSurface3, (LPVOID *)&DDrawSurf3)) ) 
			{
	        
			ddraw_ok = 1;
	        
	          IDirectDrawSurface3_Release(DDrawSurf3);
	        }
	        IDirectDrawSurface_Release(DDrawSurf);
	      }
	    }
	    IDirectDraw_Release(DDraw);
	  }
	  FreeLibrary(DDrawDLL);
	}
	return(dinput_ok && ddraw_ok);
}

// funkcje do ladowania funkcji directx dynamicznie

static HINSTANCE DDrawDLL = NULL;
static HINSTANCE DInputDLL = NULL;

static void DX8_Unload(void)
{
	if ( DDrawDLL != NULL ) 
	{
		FreeLibrary(DDrawDLL);
		DDrawCreate = NULL;
		DDrawDLL = NULL;
	}
	if ( DInputDLL != NULL ) 
	{
		FreeLibrary(DInputDLL);
		DInputCreate = NULL;
		DInputDLL = NULL;
	}
}

static int DX8_Load(void)
{
	int status;

	DX8_Unload();
	DDrawDLL = LoadLibrary("DDRAW.DLL");
	
	if ( DDrawDLL != NULL ) 
	{
		DDrawCreate = (void *)GetProcAddress(DDrawDLL,
					"DirectDrawCreate");
	}
	
	DInputDLL = LoadLibrary("DINPUT.DLL");
	
	if ( DInputDLL != NULL ) 
	{
		DInputCreate = (void *)GetProcAddress(DInputDLL,
					"DirectInputCreateA");
	}
	
	if ( DDrawDLL && DDrawCreate && DInputDLL && DInputCreate ) 
	{
		status = 0;
	} 
	else 
	{
		DX8_Unload();
		status = -1;
	}
	return status;
}

// usun urzadzenie 

static void DX8_DeleteDevice(FOX_VideoDevice *this)
{
	
	// zwolnij directdraw object

	if ( ddraw2 != NULL ) 
	{
		IDirectDraw2_Release(ddraw2);
	}
	DX8_Unload();
	
	if ( this ) 
	{
		if ( this->hidden ) 
		{
			free(this->hidden);
		}
		
		if ( this->gl_data ) 
		{
			free(this->gl_data);
		}
		free(this);
	}
}

static FOX_VideoDevice *DX8_CreateDevice(int devindex)
{
	FOX_VideoDevice *device;

	// zaladuj directx
	
	if ( DX8_Load() < 0 ) 
	{
		return(NULL);
	}

	// zainicjalizuj wszystkie zmienne, ktore czyscimy przy wyjsciu
	
	device = (FOX_VideoDevice *)malloc(sizeof(FOX_VideoDevice));

	if ( device ) 
	{
		memset(device, 0, (sizeof *device));
		device->hidden = (struct FOX_PrivateVideoData *)
				malloc((sizeof *device->hidden));
//		device->gl_data = (struct FOX_PrivateGLData *)
//				malloc((sizeof *device->gl_data));
	}
	
	if ( (device == NULL) || (device->hidden == NULL) ||
		                 (device->gl_data == NULL) ) 
	{
		FOX_OutOfMemory();
		DX8_DeleteDevice(device);
		return(NULL);
	}
	
	memset(device->hidden, 0, (sizeof *device->hidden));
//	memset(device->gl_data, 0, (sizeof *device->gl_data));

	// ustaw wskazniki funkcji
	
	device->VideoInit = DX8_VideoInit;
	device->ListModes = DX8_ListModes;
	device->SetVideoMode = DX8_SetVideoMode;
	device->UpdateMouse = WIN_UpdateMouse;
	device->CreateYUVOverlay = DX8_CreateYUVOverlay;
	device->SetColors = DX8_SetColors;
	device->UpdateRects = NULL;
	device->VideoQuit = DX8_VideoQuit;
	device->AllocHWSurface = DX8_AllocHWSurface;
	device->CheckHWBlit = DX8_CheckHWBlit;
	device->FillHWRect = DX8_FillHWRect;
	device->SetHWColorKey = DX8_SetHWColorKey;
	device->SetHWAlpha = DX8_SetHWAlpha;
	device->LockHWSurface = DX8_LockHWSurface;
	device->UnlockHWSurface = DX8_UnlockHWSurface;
	device->FlipHWSurface = DX8_FlipHWSurface;
	device->FreeHWSurface = DX8_FreeHWSurface;

#ifdef IID_IDirectDrawGammaControl
	device->SetGammaRamp = DX8_SetGammaRamp;
	device->GetGammaRamp = DX8_GetGammaRamp;
#endif

	device->SetCaption = WIN_SetWMCaption;
	device->SetIcon = WIN_SetWMIcon;
	device->IconifyWindow = WIN_IconifyWindow;
	device->GrabInput = WIN_GrabInput;
	device->GetWMInfo = WIN_GetWMInfo;
	device->FreeWMCursor = WIN_FreeWMCursor;
	device->CreateWMCursor = WIN_CreateWMCursor;
	device->ShowWMCursor = WIN_ShowWMCursor;
	device->WarpWMCursor = WIN_WarpWMCursor;
	device->CheckMouseMode = WIN_CheckMouseMode;
	device->InitOSKeymap = DX8_InitOSKeymap;
	device->PumpEvents = DX8_PumpEvents;

	// ustaw funkcje przechwytujace wiadomosci windows
	
	WIN_RealizePalette = DX8_RealizePalette;
	WIN_PaletteChanged = DX8_PaletteChanged;
	WIN_SwapGamma = DX8_SwapGamma;
	WIN_WinPAINT = DX8_WinPAINT;
	HandleMessage = DX8_HandleMessage;

	device->free = DX8_DeleteDevice;

	// gotowe
	
	return device;
}

VideoBootStrap DIRECTX_bootstrap = {
	"directx", "Win95/98/2000 DirectX",
	DX8_Available, DX8_CreateDevice
};

static HRESULT WINAPI EnumModes2(DDSURFACEDESC *desc, VOID *udata)
{
	FOX_VideoDevice *this = (FOX_VideoDevice *)udata;
	struct DX8EnumRect *enumrect;
#if defined(NONAMELESSUNION)
	int bpp = desc->ddpfPixelFormat.u1.dwRGBBitCount;
#else
	int bpp = desc->ddpfPixelFormat.dwRGBBitCount;
#endif

	switch (bpp)  {
		case 8:
		case 16:
		case 24:
		case 32:
			bpp /= 8; --bpp;
			++FOX_nummodes[bpp];
			enumrect = (struct DX8EnumRect*)malloc(sizeof(struct DX8EnumRect));
			if ( !enumrect ) 
			{
				FOX_OutOfMemory();
				return(DDENUMRET_CANCEL);
			}
			enumrect->r.x = 0;
			enumrect->r.y = 0;
			enumrect->r.w = (Uint16)desc->dwWidth;
			enumrect->r.h = (Uint16)desc->dwHeight;
			enumrect->next = enumlists[bpp];
			enumlists[bpp] = enumrect;
			break;
	}


	return(DDENUMRET_OK);
}

void SetDDerror(const char *function, int code)
{
	static char *error;
	static char  errbuf[BUFSIZ];

	errbuf[0] = 0;
	switch (code) {
		case DDERR_GENERIC:
			error = "nieznany blad!";
			break;
		case DDERR_EXCEPTION:
			error = "wystapil wyjatek directx";
			break;
		case DDERR_INVALIDOBJECT:
			error = "niewlasciwy obiekt";
			break;
		case DDERR_INVALIDPARAMS:
			error = "niewlasciwe parametry";
			break;
		case DDERR_NOTFOUND:
			error = "obiekt nie znaleziony";
			break;
		case DDERR_INVALIDRECT:
			error = "niewlasciwy rectangle";
			break;
		case DDERR_INVALIDCAPS:
			error = "niewlasciwy caps member";
			break;
		case DDERR_INVALIDPIXELFORMAT:
			error = "niewlasciwy format obrazu";
			break;
		case DDERR_OUTOFMEMORY:
			error = "za malo pamieci dla directx";
			break;
		case DDERR_OUTOFVIDEOMEMORY:
			error = "za malo pamieci na karcie graficznej";
			break;
		case DDERR_SURFACEBUSY:
			error = "directx surface zajety";
			break;
		case DDERR_SURFACELOST:
			error = "stracono surface";
			break;
		case DDERR_WASSTILLDRAWING:
			error = "DirectDraw nadal wykonuje funkcje";
			break;
		case DDERR_INVALIDSURFACETYPE:
			error = "niewlasciwy typ directx surface";
			break;
		case DDERR_NOEXCLUSIVEMODE:
			error = "nie mozna uzyskac dostepu";
			break;
		case DDERR_NOPALETTEATTACHED:
			error = "brak dolaczonej palety";
			break;
		case DDERR_NOPALETTEHW:
			error = "brak sprzetowej palety";
			break;
		case DDERR_NOT8BITCOLOR:
			error = "tryb graficzny bez colorkey";
			break;
		case DDERR_EXCLUSIVEMODEALREADYSET:
			error = "tryb ekskluzywny jest juz ustawiony";
			break;
		case DDERR_HWNDALREADYSET:
			error = "handler dla okna juz ustawiony";
			break;
		case DDERR_HWNDSUBCLASSED:
			error = "okno juz przydzielone do klasy";
			break;
		case DDERR_NOBLTHW:
			error = "nie ma sprzetowego wyswietlania (hardware blit)";
			break;
		case DDERR_IMPLICITLYCREATED:
			error = "surface zostal stworzony z bledem";
			break;
		case DDERR_INCOMPATIBLEPRIMARY:
			error = "nie prawidlowy pierwszoplanowy surface";
			break;
		case DDERR_NOCOOPERATIVELEVELSET:
			error = "brak ustawionego poziomu";
			break;
		case DDERR_NODIRECTDRAWHW:
			error = "brak sprzetowego DirectDraw";
			break;
		case DDERR_NOEMULATION:
			error = "nie ma dostepnej emulacji w directx";
			break;
		case DDERR_NOFLIPHW:
			error = "brak sprzetowego kopiowania w directx";
			break;
		case DDERR_NOTFLIPPABLE:
			error = "nie mozna kopiowac surface w directx !!!";
			break;
		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			error = "pierwszoplanowy surface w directx juz istnieje";
			break;
		case DDERR_UNSUPPORTEDMODE:
			error = "nie obslugiwany tryb";
			break;
		case DDERR_WRONGMODE:
			error = "directx surface stworzony w innym trybie";
			break;
		case DDERR_UNSUPPORTED:
			error = "operacja w directx nie obslugiwana";
			break;
		case E_NOINTERFACE:
			error = "interfejs w directx nie dostepny";
			break;
		default:
			sprintf(errbuf, "%s: nieznany blad DirectDraw: 0x%x",
								function, code);
			break;
	}
	if ( ! errbuf[0] ) {
		sprintf(errbuf, "%s: %s", function, error);
	}
	FOX_SetError("%s", errbuf);
	return;
}

static int DX8_UpdateVideoInfo(_THIS)
{
	
#ifndef IID_IDirectDrawGammaControl
	// gamma niezefiniowany ? 
	DDCAPS DDCaps;
#else
	DDCAPS_DX8 DDCaps;
#endif
	HRESULT result;

	// wypelnij sprzetowe 
	
	memset(&DDCaps, 0, sizeof(DDCaps));
	DDCaps.dwSize = sizeof(DDCaps);
	result = IDirectDraw2_GetCaps(ddraw2, (DDCAPS *)&DDCaps, NULL);

	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDraw2::GetCaps", result);
		return(-1);
	}
	this->info.hw_available = 1;
	
	if ( (DDCaps.dwCaps & DDCAPS_BLT) == DDCAPS_BLT ) 
	{
		this->info.blit_hw = 1;
	}
	
	if ( ((DDCaps.dwCaps & DDCAPS_COLORKEY) == DDCAPS_COLORKEY) &&
	     ((DDCaps.dwCKeyCaps & DDCKEYCAPS_SRCBLT) == DDCKEYCAPS_SRCBLT) ) 
	{
		this->info.blit_hw_CC = 1;
	}
	
	if ( (DDCaps.dwCaps & DDCAPS_ALPHA) == DDCAPS_ALPHA ) 
	{
		// to jest tylko alpha channel, trzeba poprawic
		
		this->info.blit_hw_A = 0;
	}

	if ( (DDCaps.dwCaps & DDCAPS_CANBLTSYSMEM) == DDCAPS_CANBLTSYSMEM ) 
	{
		this->info.blit_sw = 1;
		// hmmm? 
		this->info.blit_sw_CC = this->info.blit_hw_CC;
		this->info.blit_sw_A = this->info.blit_hw_A;
	}
	
	if ( (DDCaps.dwCaps & DDCAPS_BLTCOLORFILL) == DDCAPS_BLTCOLORFILL ) 
	{
		this->info.blit_fill = 1;
	}

	// sprawdz ile jest dostepnej pamieci na karcie
		
	{ DDSCAPS ddsCaps;
	  DWORD total_mem;
		ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
		result = IDirectDraw2_GetAvailableVidMem(ddraw2,
						&ddsCaps, &total_mem, NULL);
		
		if ( result != DD_OK ) 
		{
			total_mem = DDCaps.dwVidMemTotal; 
		}
		this->info.video_mem = total_mem/1024;
	}
	return(0);
}

// inicjalizacja 

int DX8_VideoInit(_THIS, FOX_PixelFormat *vformat)
{
	HRESULT result;
	LPDIRECTDRAW ddraw;
	int i, j;
	HDC hdc;

	// inicjalizuj wszystko 
	
	ddraw2 = NULL;
	FOX_primary = NULL;
	FOX_clipper = NULL;
	FOX_palette = NULL;

	for ( i=0; i<NUM_MODELISTS; ++i ) 
	{
		FOX_nummodes[i] = 0;
		FOX_modelist[i] = NULL;
		FOX_modeindex[i] = 0;
	}
	colorchange_expected = 0;

	// stworz okno
	
	if ( DX8_CreateWindow(this) < 0 ) 
	{
		return(-1);
	}
#ifndef DISABLE_AUDIO
	DX8_SoundFocus(FOX_Window);
#endif

	// stworz obiekt directdraw
	
	result = DDrawCreate(NULL, &ddraw, NULL);
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawCreate", result);
		return(-1);
	}
	result = IDirectDraw_QueryInterface(ddraw, &IID_IDirectDraw2,
							(LPVOID *)&ddraw2);
	IDirectDraw_Release(ddraw);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDraw::QueryInterface", result);
		return(-1);
	}

	// sprawdz glebokosc ;) 
	
	hdc = GetDC(FOX_Window);
	vformat->BitsPerPixel = GetDeviceCaps(hdc,PLANES) *
					GetDeviceCaps(hdc,BITSPIXEL);
	ReleaseDC(FOX_Window, hdc);

	// ponumeruj dostepne tryby
	
	for ( i=0; i<NUM_MODELISTS; ++i )
		enumlists[i] = NULL;

	result = IDirectDraw2_EnumDisplayModes(ddraw2,0,NULL,this,EnumModes2);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDraw2::EnumDisplayModes", result);
		return(-1);
	}
	
	for ( i=0; i<NUM_MODELISTS; ++i ) 
	{
		struct DX8EnumRect *rect;
		FOX_modelist[i] = (FOX_Rect **)
				malloc((FOX_nummodes[i]+1)*sizeof(FOX_Rect *));
		
		if ( FOX_modelist[i] == NULL ) 
		{
			FOX_OutOfMemory();
			return(-1);
		}
		
		for ( j = 0, rect = enumlists[i]; rect; ++j, rect = rect->next ) 
		{
			FOX_modelist[i][j]=(FOX_Rect *)rect;
		}
		FOX_modelist[i][j] = NULL;
	}
	
	// wypelnij 
	
	this->info.wm_available = 1;

	DX8_UpdateVideoInfo(this);

	return(0);
}

FOX_Rect **DX8_ListModes(_THIS, FOX_PixelFormat *format, Uint32 flags)
{
	int bpp;

	bpp = format->BitsPerPixel;
	
	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		switch (bpp) {  
			case 8:
			case 16:
			case 24:
			case 32:
				bpp = (bpp/8)-1;
				if ( FOX_nummodes[bpp] > 0 )
					return(FOX_modelist[bpp]);
			default:
				return((FOX_Rect **)0);
		}
	} 
	else 
	{
		if ( this->screen->format->BitsPerPixel == bpp ) 
		{
			return((FOX_Rect **)-1);
		} 
		else 
		{
			return((FOX_Rect **)0);
		}
	}
}

// funkcje aktualizujace screen 

static void DX8_WindowUpdate(_THIS, int numrects, FOX_Rect *rects);
static void DX8_DirectUpdate(_THIS, int numrects, FOX_Rect *rects);

FOX_Surface *DX8_SetVideoMode(_THIS, FOX_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	FOX_Surface *video;
	HRESULT result;
	DWORD sharemode;
	DWORD style;
	const DWORD directstyle =
			(WS_POPUP);
	const DWORD windowstyle = 
			(WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX);
	const DWORD resizestyle =
			(WS_THICKFRAME|WS_MAXIMIZEBOX);
	DDSURFACEDESC ddsd;
	LPDIRECTDRAWSURFACE  dd_surface1;
	LPDIRECTDRAWSURFACE3 dd_surface3;
	BOOL was_visible;

#ifdef DDRAW_DEBUG
 fprintf(stderr, "Setting %dx%dx%d video mode\n", width, height, bpp);
#endif
	
	// wycentrowac okno ? 
 
	was_visible = IsWindowVisible(FOX_Window);

	// wyczysc poprzednie directdraw 
		
	if ( current->hwdata ) 
	{
		this->FreeHWSurface(this, current);
		current->hwdata = NULL;
	}
	
	if ( FOX_primary != NULL ) 
	{
		IDirectDrawSurface3_Release(FOX_primary);
		FOX_primary = NULL;
	}
	
	if ( (current->flags & (FOX_OPENGL|FOX_FULLSCREEN)) ==
	                       (FOX_OPENGL|FOX_FULLSCREEN) ) 
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// ustaw wlasciwy styl dla okna
	
	style = GetWindowLong(FOX_Window, GWL_STYLE);
	style &= ~(resizestyle|WS_MAXIMIZE);

	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		style &= ~windowstyle;
		style |= directstyle;
	} 
	else 
	{
		if ( flags & FOX_NOFRAME ) 
		{
			style &= ~windowstyle;
			style |= directstyle;
		} 
		else 
		{
			style &= ~directstyle;
			style |= windowstyle;
			
			if ( flags & FOX_RESIZABLE ) 
			{
				style |= resizestyle;
			}
		}
		if (IsZoomed(FOX_Window)) style |= WS_MAXIMIZE;
	}
	SetWindowLong(FOX_Window, GWL_STYLE, style);

	// ustaw wspoldzielony tryb directdraw
	
	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		sharemode = DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|DDSCL_ALLOWREBOOT;
	} 
	else 
	{
		sharemode = DDSCL_NORMAL;
	}
	
	result = IDirectDraw2_SetCooperativeLevel(ddraw2,FOX_Window,sharemode);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDraw2::SetCooperativeLevel", result);
		return(NULL);
	}

	// ustaw tryb wyswietlania
	
	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		FOX_resizing = 1;
		SetWindowPos(FOX_Window, NULL, 0, 0, 
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			(SWP_NOCOPYBITS | SWP_NOZORDER));
		FOX_resizing = 0;
		ShowWindow(FOX_Window, SW_SHOW);

		while ( GetForegroundWindow() != FOX_Window ) 
		{
			SetForegroundWindow(FOX_Window);
			FOX_Delay(100);
		}
		result = IDirectDraw2_SetDisplayMode(ddraw2, width, height,
								bpp, 0, 0);
		
		if ( result != DD_OK ) 
		{
			// nie mozna ustawic trybu caloekranowego
			// inicjalizuj w oknie
			return(DX8_SetVideoMode(this, current,
				width, height, bpp, flags & ~FOX_FULLSCREEN)); 
		}
		DX8_DInputReset(this, 1);
	} 
	else 
	{
		DX8_DInputReset(this, 0);
	}
	DX8_UpdateVideoInfo(this);

	// stworz podstawowy directdraw surface
	
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = (DDSCAPS_PRIMARYSURFACE|DDSCAPS_VIDEOMEMORY);

	if ( (flags & FOX_FULLSCREEN) != FOX_FULLSCREEN ) 
	{
		// nie ma w oknie double bufora
		flags &= ~FOX_DOUBLEBUF;
	}
	
	if ( (flags & FOX_DOUBLEBUF) == FOX_DOUBLEBUF ) 
	{
		ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps |= (DDSCAPS_COMPLEX|DDSCAPS_FLIP);
		ddsd.dwBackBufferCount = 1;
	}
	
	result = IDirectDraw2_CreateSurface(ddraw2, &ddsd, &dd_surface1, NULL); 
	
	if ( (result != DD_OK) && ((flags & FOX_DOUBLEBUF) == FOX_DOUBLEBUF) ) 
	{
		ddsd.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_COMPLEX|DDSCAPS_FLIP);
		ddsd.dwBackBufferCount = 0;
		result = IDirectDraw2_CreateSurface(ddraw2,
						&ddsd, &dd_surface1, NULL); 
	}
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDraw2::CreateSurface(PRIMARY)", result);
		return(NULL);
	}
	
	result = IDirectDrawSurface_QueryInterface(dd_surface1,
			&IID_IDirectDrawSurface3, (LPVOID *)&FOX_primary);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface::QueryInterface", result);
		return(NULL);
	}
	
	result = IDirectDrawSurface_Release(dd_surface1);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface::Release", result);
		return(NULL);
	}

	// pobierz format 
	
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PIXELFORMAT|DDSD_CAPS;
	result = IDirectDrawSurface3_GetSurfaceDesc(FOX_primary, &ddsd);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface::Release", result);
		return(NULL);
	}
	
	if ( ! (ddsd.ddpfPixelFormat.dwFlags&DDPF_RGB) ) 
	{
		FOX_SetError("[FOX]: podstawowy surface ddraw nie jest rgb");
		return(NULL);
	}

	// zwolnij stara palete i ustaw nowa 
	
	if ( FOX_palette != NULL ) 
	{
		IDirectDrawPalette_Release(FOX_palette);
		FOX_palette = NULL;
	}
#if defined(NONAMELESSUNION)
	if ( ddsd.ddpfPixelFormat.u1.dwRGBBitCount == 8 ) 
	{
#else
	if ( ddsd.ddpfPixelFormat.dwRGBBitCount == 8 ) 
	{
#endif
		int i;

		if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
		{
			// dostep do calej palety
			for ( i=0; i<256; ++i ) 
			{
				FOX_colors[i].peFlags =
						(PC_NOCOLLAPSE|PC_RESERVED);
				FOX_colors[i].peRed = 0;
				FOX_colors[i].peGreen = 0;
				FOX_colors[i].peBlue = 0;
			}
		} 
		else 
		{
			// pierwsze 10 kolorow zarezerwowane dla windows
			for ( i=0; i<10; ++i ) 
			{
				FOX_colors[i].peFlags = PC_EXPLICIT;
				FOX_colors[i].peRed = i;
				FOX_colors[i].peGreen = 0;
				FOX_colors[i].peBlue = 0;
			}
			
			for ( i=10; i<(10+236); ++i ) 
			{
				FOX_colors[i].peFlags = PC_NOCOLLAPSE;
				FOX_colors[i].peRed = 0;
				FOX_colors[i].peGreen = 0;
				FOX_colors[i].peBlue = 0;
			}
			
			// ostatnie dziesiec kolorow zarezerwowane dla windows
			
			for ( i=246; i<256; ++i ) 
			{
				FOX_colors[i].peFlags = PC_EXPLICIT;
				FOX_colors[i].peRed = i;
				FOX_colors[i].peGreen = 0;
				FOX_colors[i].peBlue = 0;
			}
		}
		result = IDirectDraw2_CreatePalette(ddraw2,
		     			(DDPCAPS_8BIT|DDPCAPS_ALLOW256),
						FOX_colors, &FOX_palette, NULL);
		
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDraw2::CreatePalette", result);
			return(NULL);
		}
		
		result = IDirectDrawSurface3_SetPalette(FOX_primary,
								FOX_palette);
		
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDrawSurface3::SetPalette", result);
			return(NULL);
		}
	}

	// stworz nasz surface
	
	video = current;
	if ( (width != video->w) || (height != video->h)
			|| (video->format->BitsPerPixel != 
#if defined(NONAMELESSUNION)
				ddsd.ddpfPixelFormat.u1.dwRGBBitCount) ) 
	{
#else
				ddsd.ddpfPixelFormat.dwRGBBitCount) ) 
				{
#endif
		FOX_FreeSurface(video);
		video = FOX_CreateRGBSurface(FOX_SWSURFACE, 0, 0,
#if defined(NONAMELESSUNION)
				ddsd.ddpfPixelFormat.u1.dwRGBBitCount,
					ddsd.ddpfPixelFormat.u2.dwRBitMask,
					ddsd.ddpfPixelFormat.u3.dwGBitMask,
					ddsd.ddpfPixelFormat.u4.dwBBitMask,
#else
				ddsd.ddpfPixelFormat.dwRGBBitCount,
					ddsd.ddpfPixelFormat.dwRBitMask,
					ddsd.ddpfPixelFormat.dwGBitMask,
					ddsd.ddpfPixelFormat.dwBBitMask,
#endif
								0);
		if ( video == NULL ) 
		{
			FOX_OutOfMemory();
			return(NULL);
		}
		video->w = width;
		video->h = height;
		video->pitch = 0;
	}
	video->flags = 0;	// wyczysc flagi

	if ( (flags & FOX_FULLSCREEN) != FOX_FULLSCREEN ) 
	{
		// potrzebne jezeli przelaczamy sie z fullscreen do okna
		
		if ( video->pixels == NULL ) 
		{
			video->pitch = (width*video->format->BytesPerPixel);
			
			video->pitch = (video->pitch + 7) & ~7;
			video->pixels = (void *)malloc(video->h*video->pitch);
			
			if ( video->pixels == NULL ) 
			{
				if ( video != current ) 
				{
					FOX_FreeSurface(video);
				}
				FOX_OutOfMemory();
				return(NULL);
			}
		}
		dd_surface3 = NULL;
		video->flags |= FOX_SWSURFACE;
		
		if ( (flags & FOX_RESIZABLE) && !(flags & FOX_NOFRAME) ) 
		{
			video->flags |= FOX_RESIZABLE;
		}
		
		if ( flags & FOX_NOFRAME ) 
		{
			video->flags |= FOX_NOFRAME;
		}
	} 
	else 
	{
		// potrzebne jezeli przechodzimy z okna do fullscreen
		if ( video->pixels != NULL ) 
		{
			free(video->pixels);
			video->pixels = NULL;
		}
		dd_surface3 = FOX_primary;
		video->flags |= FOX_HWSURFACE;
	}

	// sprawdz czy podstawowy surface ma double buffor
	
	if ( (ddsd.ddsCaps.dwCaps & DDSCAPS_FLIP) == DDSCAPS_FLIP ) 
	{
		video->flags |= FOX_DOUBLEBUF;
	}

	// zalokuj surface
	
	if ( DX8_AllocDDSurface(this, video, dd_surface3,
	                        video->flags&FOX_HWSURFACE) < 0 ) 
	{
		if ( video != current ) 
		{
			FOX_FreeSurface(video);
		}
		return(NULL);
	}

	// ustaw funkcje blitujace
	
	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		video->flags |= FOX_FULLSCREEN;
	
		if ( video->format->palette != NULL ) 
		{
			video->flags |= FOX_HWPALETTE;
		}
		this->UpdateRects = DX8_DirectUpdate;
	} 
	else 
	{
		this->UpdateRects = DX8_WindowUpdate;
	}

	// ustaw rozmiar dla okna
	
	if ( (flags & FOX_FULLSCREEN) != FOX_FULLSCREEN ) 
	{
		RECT bounds;
		int  x, y;
		UINT swp_flags;
		
		if ( FOX_clipper == NULL ) 
		{
			result = IDirectDraw2_CreateClipper(ddraw2,
							0, &FOX_clipper, NULL);
			
			if ( result != DD_OK ) 
			{
				if ( video != current ) 
				{
					FOX_FreeSurface(video);
				}
				SetDDerror("DirectDraw2::CreateClipper",result);
				return(NULL);
			}
		}
		
		result = IDirectDrawClipper_SetHWnd(FOX_clipper, 0, FOX_Window);
		
		if ( result != DD_OK ) 
		{
			if ( video != current ) 
			{
				FOX_FreeSurface(video);
			}
			SetDDerror("DirectDrawClipper::SetHWnd", result);
			return(NULL);
		}
		
		result = IDirectDrawSurface3_SetClipper(FOX_primary,
								FOX_clipper);
		
		if ( result != DD_OK ) 
		{
			if ( video != current ) 
			{
				FOX_FreeSurface(video);
			}
			SetDDerror("DirectDrawSurface3::SetClipper", result);
			return(NULL);
		}

		// ustaw rozmiar okna
		
		FOX_resizing = 1;
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = video->w;
		bounds.bottom = video->h;
		AdjustWindowRect(&bounds, GetWindowLong(FOX_Window, GWL_STYLE),
									FALSE);
		width = bounds.right-bounds.left;
		height = bounds.bottom-bounds.top;
		x = (GetSystemMetrics(SM_CXSCREEN)-width)/2;
		y = (GetSystemMetrics(SM_CYSCREEN)-height)/2;
		
		if ( y < 0 ) 
		{ 
			y -= GetSystemMetrics(SM_CYCAPTION)/2;
		}
		
		swp_flags = (SWP_NOCOPYBITS | SWP_NOZORDER);
		
		if ( was_visible ) 
		{
			swp_flags |= SWP_NOMOVE;
		}
		SetWindowPos(FOX_Window, NULL, x, y, width, height, swp_flags);
		FOX_resizing = 0;
	}
	ShowWindow(FOX_Window, SW_SHOW);
	SetForegroundWindow(FOX_Window);
	
	return(video);
}

struct private_hwdata {
	LPDIRECTDRAWSURFACE3 dd_surface;
	LPDIRECTDRAWSURFACE3 dd_writebuf;
};

static int DX8_AllocDDSurface(_THIS, FOX_Surface *surface, 
				LPDIRECTDRAWSURFACE3 requested, Uint32 flag)
{
	LPDIRECTDRAWSURFACE  dd_surface1;
	LPDIRECTDRAWSURFACE3 dd_surface3;
	DDSURFACEDESC ddsd;
	HRESULT result;

	// wyczysc sprzetowe flagi
	
	surface->flags &= ~flag;

	// sprzetowa akceleracja
	
	surface->hwdata = (struct private_hwdata *)
					malloc(sizeof(*surface->hwdata));
	
	if ( surface->hwdata == NULL ) 
	{
		FOX_OutOfMemory();
		return(-1);
	}
	dd_surface3 = NULL;

	// ustaw surface
	
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = (DDSD_WIDTH|DDSD_HEIGHT|DDSD_CAPS|
					DDSD_PITCH|DDSD_PIXELFORMAT);
	ddsd.dwWidth = surface->w;
	ddsd.dwHeight= surface->h;
#if defined(NONAMELESSUNION)
	ddsd.u1.lPitch = surface->pitch;
#else
	ddsd.lPitch = surface->pitch;
#endif
	if ( (flag & FOX_HWSURFACE) == FOX_HWSURFACE ) 
	{
		ddsd.ddsCaps.dwCaps =
				(DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY);
	} 
	else 
	{
		ddsd.ddsCaps.dwCaps =
				(DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY);
	}
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	
	if ( surface->format->palette ) 
	{
		ddsd.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
	}
#if defined(NONAMELESSUNION)
	ddsd.ddpfPixelFormat.u1.dwRGBBitCount = surface->format->BitsPerPixel;
	ddsd.ddpfPixelFormat.u2.dwRBitMask = surface->format->Rmask;
	ddsd.ddpfPixelFormat.u3.dwGBitMask = surface->format->Gmask;
	ddsd.ddpfPixelFormat.u4.dwBBitMask = surface->format->Bmask;
#else
	ddsd.ddpfPixelFormat.dwRGBBitCount = surface->format->BitsPerPixel;
	ddsd.ddpfPixelFormat.dwRBitMask = surface->format->Rmask;
	ddsd.ddpfPixelFormat.dwGBitMask = surface->format->Gmask;
	ddsd.ddpfPixelFormat.dwBBitMask = surface->format->Bmask;
#endif

	// stworz surface dla directxdraw 
	
	if ( requested != NULL ) 
	{
		dd_surface3 = requested;
	} 
	else 
	{
		result = IDirectDraw2_CreateSurface(ddraw2,
						&ddsd, &dd_surface1, NULL); 
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDraw2::CreateSurface", result);
			goto error_end;
		}
		result = IDirectDrawSurface_QueryInterface(dd_surface1,
			&IID_IDirectDrawSurface3, (LPVOID *)&dd_surface3);
		IDirectDrawSurface_Release(dd_surface1);
		
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDrawSurface::QueryInterface", result);
			goto error_end;
		}
	}

	if ( (flag & FOX_HWSURFACE) == FOX_HWSURFACE ) 
	{
		
		result = IDirectDrawSurface3_GetCaps(dd_surface3,&ddsd.ddsCaps);
		
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDrawSurface3::GetCaps", result);
			goto error_end;
		}
		
		if ( (ddsd.ddsCaps.dwCaps&DDSCAPS_VIDEOMEMORY) !=
							DDSCAPS_VIDEOMEMORY ) 
		{
			FOX_SetError("[FOX]: za malo pamieci na karcie graficznej");
			goto error_end;
		}
	} 
	else 
	{
		
		ddsd.dwFlags = DDSD_LPSURFACE;
		ddsd.lpSurface = surface->pixels;
		result = IDirectDrawSurface3_SetSurfaceDesc(dd_surface3,
								&ddsd, 0);
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDraw2::SetSurfaceDesc", result);
			goto error_end;
		}
	
	}

	// sprawdz czy format surface byl dobrze ustawiony
	
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	result = IDirectDrawSurface3_Lock(dd_surface3, NULL,
					&ddsd, DDLOCK_NOSYSLOCK, NULL);

	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::Lock", result);
		goto error_end;
	}
	
	IDirectDrawSurface3_Unlock(dd_surface3, NULL);

	
	if ( (flag & FOX_HWSURFACE) == FOX_SWSURFACE ) 
	{
		if ( ddsd.lpSurface != surface->pixels ) 
		{
			FOX_SetError("[FOX]: DDraw nie uzywa pamieci FOX surface");
			goto error_end;
		}
		if (
#if defined(NONAMELESSUNION)
			ddsd.u1.lPitch
#else
			ddsd.lPitch
#endif
				 != (LONG)surface->pitch ) 
		{
			FOX_SetError("[FOX]: DDraw stworzyl surface w zlym formacie");
			goto error_end;
		}
	} 
	else 
	{
#if defined(NONAMELESSUNION)
		surface->pitch = (Uint16)ddsd.u1.lPitch;
#else
		surface->pitch = (Uint16)ddsd.lPitch;
#endif
	}
#if defined(NONAMELESSUNION)
	if ( (ddsd.ddpfPixelFormat.u1.dwRGBBitCount != 
					surface->format->BitsPerPixel) ||
	     (ddsd.ddpfPixelFormat.u2.dwRBitMask != surface->format->Rmask) ||
	     (ddsd.ddpfPixelFormat.u3.dwGBitMask != surface->format->Gmask) ||
	     (ddsd.ddpfPixelFormat.u4.dwBBitMask != surface->format->Bmask) )
	{
#else
	if ( (ddsd.ddpfPixelFormat.dwRGBBitCount != 
					surface->format->BitsPerPixel) ||
	     (ddsd.ddpfPixelFormat.dwRBitMask != surface->format->Rmask) ||
	     (ddsd.ddpfPixelFormat.dwGBitMask != surface->format->Gmask) ||
	     (ddsd.ddpfPixelFormat.dwBBitMask != surface->format->Bmask) )
	{
#endif
		FOX_SetError("[FOX]: DDraw nie uzywa FOX surface");
		goto error_end;
	}
	if ( (ddsd.dwWidth != (DWORD)surface->w) ||
		(ddsd.dwHeight != (DWORD)surface->h) ) 
	{
		FOX_SetError("[FOX]: DDraw stworzyl surface o zlym rozmiarze");
		goto error_end;
	}

	// ustaw surface
	
	surface->flags |= flag;
	surface->hwdata->dd_surface = dd_surface3;
	
	if ( (surface->flags & FOX_DOUBLEBUF) == FOX_DOUBLEBUF ) 
	{
		LPDIRECTDRAWSURFACE3 dd_writebuf;

		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		result = IDirectDrawSurface3_GetAttachedSurface(dd_surface3,
						&ddsd.ddsCaps, &dd_writebuf);
		if ( result != DD_OK ) 
		{
			SetDDerror("DirectDrawSurface3::GetAttachedSurface",
								result);
		} 
		else 
		{
			dd_surface3 = dd_writebuf;
		}
	}
	surface->hwdata->dd_writebuf = dd_surface3;
	
	return(0);

error_end:
	if ( (dd_surface3 != NULL) && (dd_surface3 != requested) ) 
	{
		IDirectDrawSurface_Release(dd_surface3);
	}
	free(surface->hwdata);
	surface->hwdata = NULL;
	return(-1);
}

// alokacja sprzetowego surface

static int DX8_AllocHWSurface(_THIS, FOX_Surface *surface)
{
	
	if ( FOX_primary == NULL ) 
	{
		FOX_SetError("[FOX]: musisz najpierw ustawic tryb bez opengl");
		return(-1);
	}
	return(DX8_AllocDDSurface(this, surface, NULL, FOX_HWSURFACE));
}

// to wlasciwie jest juz nie potrzebne

#ifdef DDRAW_DEBUG

void PrintSurface(char *title, LPDIRECTDRAWSURFACE3 surface, Uint32 flags)
{
	DDSURFACEDESC ddsd;

	// zablokuj i wczytaj 
	
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	
	if ( IDirectDrawSurface3_Lock(surface, NULL, &ddsd,
			(DDLOCK_NOSYSLOCK|DDLOCK_WAIT), NULL) != DD_OK ) 
	{
		return;
	}
	IDirectDrawSurface3_Unlock(surface, NULL);
	
	fprintf(stderr, "%s:\n", title);
	fprintf(stderr, "\tSize: %dx%d in %s at %ld bpp (pitch = %ld)\n",
		ddsd.dwWidth, ddsd.dwHeight,
		(flags & FOX_HWSURFACE) ? "hardware" : "software",
#if defined(NONAMELESSUNION)
		ddsd.ddpfPixelFormat.u1.dwRGBBitCount, ddsd.u1.lPitch);
#else
		ddsd.ddpfPixelFormat.dwRGBBitCount, ddsd.lPitch);
#endif
	fprintf(stderr, "\tR = 0x%X, G = 0x%X, B = 0x%X\n", 
#if defined(NONAMELESSUNION)
	     		ddsd.ddpfPixelFormat.u2.dwRBitMask,
	     		ddsd.ddpfPixelFormat.u3.dwGBitMask,
	     		ddsd.ddpfPixelFormat.u4.dwBBitMask);
#else
	     		ddsd.ddpfPixelFormat.dwRBitMask,
	     		ddsd.ddpfPixelFormat.dwGBitMask,
	     		ddsd.ddpfPixelFormat.dwBBitMask);
#endif
}
#endif /* DDRAW_DEBUG */

static int DX8_HWAccelBlit(FOX_Surface *src, FOX_Rect *srcrect,
					FOX_Surface *dst, FOX_Rect *dstrect)
{
	LPDIRECTDRAWSURFACE3 src_surface;
	LPDIRECTDRAWSURFACE3 dst_surface;
	DWORD flags;
	RECT rect;
	HRESULT result;

	// ustaw
	
	src_surface = src->hwdata->dd_writebuf;
	dst_surface = dst->hwdata->dd_writebuf;
	rect.top    = srcrect->y;
	rect.bottom = srcrect->y+srcrect->h;
	rect.left   = srcrect->x;
	rect.right  = srcrect->x+srcrect->w;
	if ( (src->flags & FOX_SRCCOLORKEY) == FOX_SRCCOLORKEY )
		flags = DDBLTFAST_SRCCOLORKEY;
	else
		flags = DDBLTFAST_NOCOLORKEY;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// usuniecie tej flagi da duzego kopa, ale moze sie wieszac system
	
	flags |= DDBLTFAST_WAIT;

	// blit
	
	result = IDirectDrawSurface3_BltFast(dst_surface,
			dstrect->x, dstrect->y, src_surface, &rect, flags);

	if ( result != DD_OK ) 
	{
		if ( result == DDERR_SURFACELOST ) 
		{
			result = IDirectDrawSurface3_Restore(src_surface);
			result = IDirectDrawSurface3_Restore(dst_surface);
			
			FOX_SetError("[FOX]: surface stracony");
			return(-2);
		}
		SetDDerror("IDirectDrawSurface3::BltFast", result);
#ifdef DDRAW_DEBUG
 fprintf(stderr, "Original dest rect: %dx%d at %d,%d\n", dstrect->w, dstrect->h, dstrect->x, dstrect->y);
 fprintf(stderr, "HW accelerated %sblit to from 0x%p to 0x%p at (%d,%d)\n",
		(src->flags & FOX_SRCCOLORKEY) ? "colorkey " : "", src, dst,
					dstrect->x, dstrect->y);
  PrintSurface("SRC", src_surface, src->flags);
  PrintSurface("DST", dst_surface, dst->flags);
 fprintf(stderr, "Source rectangle: (%d,%d) - (%d,%d)\n",
		rect.left, rect.top, rect.right, rect.bottom);
#endif
		// niespodziewany blad
		return(src->map->sw_blit(src, srcrect, dst, dstrect));
	}
	return(0);
}

// sprawdz ;)

static int DX8_CheckHWBlit(_THIS, FOX_Surface *src, FOX_Surface *dst)
{
	int accelerated;
	
	// musimy ustawic
	
	if ( (src->flags & FOX_HWSURFACE) == FOX_SWSURFACE ) 
	{
		// zaalokuj surface do blitowania		

		if ( src->hwdata == NULL ) 
		{
			DX8_AllocDDSurface(this, src, NULL, FOX_SWSURFACE);
		}
	}
	
	if ( src->hwdata == NULL ) 
	{
		return(0);
	}

	// ustaw init akceleracji
	
	src->flags |= FOX_HWACCEL;

	// ustaw atrybuty
	
	if ( (src->flags & FOX_SRCCOLORKEY) == FOX_SRCCOLORKEY ) 
	{
		if ( DX8_SetHWColorKey(this, src, src->format->colorkey) < 0 ) 
		{
			src->flags &= ~FOX_HWACCEL;
		}
	}
	
	if ( (src->flags & FOX_SRCALPHA) == FOX_SRCALPHA ) 
	{
		if ( DX8_SetHWAlpha(this, src, src->format->alpha) < 0 ) 
		{
			src->flags &= ~FOX_HWACCEL;
		}
	}
	
	accelerated = !!(src->flags & FOX_HWACCEL);
	if ( accelerated ) {
#ifdef DDRAW_DEBUG
  fprintf(stderr, "Setting accelerated blit on 0x%p\n", src);
#endif
		src->map->hw_blit = DX8_HWAccelBlit;
	}
	return(accelerated);
}

static int DX8_FillHWRect(_THIS, FOX_Surface *dst, FOX_Rect *dstrect, Uint32 color)
{
	LPDIRECTDRAWSURFACE3 dst_surface;
	RECT area;
	DDBLTFX bltfx;
	HRESULT result;

#ifdef DDRAW_DEBUG
 fprintf(stderr, "HW accelerated fill at (%d,%d)\n", dstrect->x, dstrect->y);
#endif
	dst_surface = dst->hwdata->dd_writebuf;
	area.top = dstrect->y;
	area.bottom = dstrect->y+dstrect->h;
	area.left = dstrect->x;
	area.right = dstrect->x+dstrect->w;
	bltfx.dwSize = sizeof(bltfx);
#if defined(NONAMELESSUNION)
	bltfx.u5.dwFillColor = color;
#else
	bltfx.dwFillColor = color;
#endif
	result = IDirectDrawSurface3_Blt(dst_surface,
			&area, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &bltfx);
	if ( result == DDERR_SURFACELOST ) 
	{
		IDirectDrawSurface3_Restore(dst_surface);
		result = IDirectDrawSurface3_Blt(dst_surface,
			&area, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &bltfx);
	}
	
	if ( result != DD_OK ) 
	{
		SetDDerror("IDirectDrawSurface3::Blt", result);
		return(-1);
	}
	return(0);
}

// ustaw kolor kluczowy dla sprzetowego surface

static int DX8_SetHWColorKey(_THIS, FOX_Surface *surface, Uint32 key)
{
	DDCOLORKEY colorkey;
	HRESULT result;

	// ustaw ck dla surface
	
	colorkey.dwColorSpaceLowValue = key;
	colorkey.dwColorSpaceHighValue = key;
	result = IDirectDrawSurface3_SetColorKey(
			surface->hwdata->dd_surface, DDCKEY_SRCBLT, &colorkey);

	if ( result != DD_OK ) 
	{
		SetDDerror("IDirectDrawSurface3::SetColorKey", result);
		return(-1);
	}
	return(0);
}

// ustawienie alfy

static int DX8_SetHWAlpha(_THIS, FOX_Surface *surface, Uint8 alpha)
{
	return(-1);
}

// zablokuj surface

static int DX8_LockHWSurface(_THIS, FOX_Surface *surface)
{
	HRESULT result;
	LPDIRECTDRAWSURFACE3 dd_surface;
	DDSURFACEDESC ddsd;

	// zablokuj i wczytaj
	dd_surface = surface->hwdata->dd_writebuf;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	result = IDirectDrawSurface3_Lock(dd_surface, NULL, &ddsd,
					(DDLOCK_NOSYSLOCK|DDLOCK_WAIT), NULL);
	
	if ( result == DDERR_SURFACELOST ) 
	{
		result = IDirectDrawSurface3_Restore(
						surface->hwdata->dd_surface);
		result = IDirectDrawSurface3_Lock(dd_surface, NULL, &ddsd, 
					(DDLOCK_NOSYSLOCK|DDLOCK_WAIT), NULL);
	}
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::Lock", result);
		return(-1);
	}
	
#if defined(NONAMELESSUNION)
	if ( surface->pitch != ddsd.u1.lPitch ) 
	{
		surface->pitch = ddsd.u1.lPitch;
#else
	if ( surface->pitch != ddsd.lPitch ) 
	{
		surface->pitch = (Uint16)ddsd.lPitch;
#endif
		surface->offset =
			((ddsd.dwHeight-surface->h)/2)*surface->pitch +
			((ddsd.dwWidth-surface->w)/2)*
					surface->format->BytesPerPixel;
	}
	surface->pixels = ddsd.lpSurface;
	return(0);
}

// odblokuj sprzetowy surface

static void DX8_UnlockHWSurface(_THIS, FOX_Surface *surface)
{
	IDirectDrawSurface3_Unlock(surface->hwdata->dd_writebuf, NULL);
	surface->pixels = NULL;
}

// kopiuj sprzetowy surface

static int DX8_FlipHWSurface(_THIS, FOX_Surface *surface)
{
	HRESULT result;
	LPDIRECTDRAWSURFACE3 dd_surface;

	dd_surface = surface->hwdata->dd_surface;
	result = IDirectDrawSurface3_Flip(dd_surface, NULL, DDFLIP_WAIT);
	
	if ( result == DDERR_SURFACELOST ) 
	{
		result = IDirectDrawSurface3_Restore(
						surface->hwdata->dd_surface);
		result = IDirectDrawSurface3_Flip(dd_surface,NULL,DDFLIP_WAIT);
	}
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::Flip", result);
		return(-1);
	}
	return(0);
}

// zwolnij sprzetowy surface, chyba az tak glupi nie jestes zeby zapomniec 

static void DX8_FreeHWSurface(_THIS, FOX_Surface *surface)
{
	if ( surface->hwdata ) 
	{
		if ( surface->hwdata->dd_surface != FOX_primary ) 
		{
			IDirectDrawSurface3_Release(surface->hwdata->dd_surface);
		}
		free(surface->hwdata);
		surface->hwdata = NULL;
	}
}

// aktualizuj 

void DX8_WindowUpdate(_THIS, int numrects, FOX_Rect *rects)
{
	HRESULT result;
	int i;
	RECT src, dst;

	for ( i=0; i<numrects; ++i ) 
	{
		src.top = rects[i].y;
		src.bottom = rects[i].y+rects[i].h;
		src.left = rects[i].x;
		src.right = rects[i].x+rects[i].w;
		dst.top = FOX_bounds.top+src.top;
		dst.left = FOX_bounds.left+src.left;
		dst.bottom = FOX_bounds.top+src.bottom;
		dst.right = FOX_bounds.left+src.right;
		result = IDirectDrawSurface3_Blt(FOX_primary, &dst, 
					this->screen->hwdata->dd_surface, &src,
							DDBLT_WAIT, NULL);
		// sprawdz 
		
		if ( result == DDERR_SURFACELOST ) 
		{
			IDirectDrawSurface3_Restore(FOX_primary);
			IDirectDrawSurface3_Blt(FOX_primary, &dst, 
					this->screen->hwdata->dd_surface, &src,
							DDBLT_WAIT, NULL);
		}
	}
}

void DX8_DirectUpdate(_THIS, int numrects, FOX_Rect *rects)
{
}

#define SIMPLE_COMPRESSION
#define CS_CS_DIST(A, B) ({						\
	int r = (A.r - B.r);						\
	int g = (A.g - B.g);						\
	int b = (A.b - B.b);						\
	(r*r + g*g + b*b);						\
})

static void DX8_CompressPalette(_THIS, FOX_Color *colors, int ncolors, int maxcolors)
{
#ifdef SIMPLE_COMPRESSION
	int i, j;
#else
	static FOX_Color zero = { 0, 0, 0, 0 };
	int i, j;
	int max, dist;
	int prev, next;
	int *pool;
	int *seen, *order;
#endif
	
	if ( maxcolors > ncolors ) 
	{
		maxcolors = ncolors;
	}

#ifdef SIMPLE_COMPRESSION
	
	for ( j=10, i=0; i<maxcolors; ++i, ++j ) 
	{
		FOX_colors[j].peRed = colors[i].r;
		FOX_colors[j].peGreen = colors[i].g;
		FOX_colors[j].peBlue = colors[i].b;
	}
#else
	
	// alokuj pamiec dla tablic
	
	pool = (int *)alloca(2*ncolors*sizeof(int));
	if ( pool == NULL ) 
	{
		return;
	}
	seen = pool;
	memset(seen, 0, ncolors*sizeof(int));
	order = pool+ncolors;

	// start z najjasniejszym kolorem
	max = 0;
	
	for ( i=0; i<ncolors; ++i ) 
	{
		dist = CS_CS_DIST(zero, colors[i]);
		
		if ( dist >= max ) 
		{
			max = dist;
			next = i;
		}
	}
	j = 0;
	order[j++] = next;
	seen[next] = 1;
	prev = next;

	// kontynuuj
	
	while ( j < maxcolors ) 
	{
		max = 0;
		for ( i=0; i<ncolors; ++i ) 
		{
			if ( seen[i] ) 
			{
				continue;
			}
			dist = CS_CS_DIST(colors[i], colors[prev]);
			if ( dist >= max ) 
			{
				max = dist;
				next = i;
			}
		}
		order[j++] = next;
		seen[next] = 1;
		prev = next;
	}
	
	for ( j=10, i=0; i<maxcolors; ++i, ++j ) 
	{
		FOX_colors[j].peRed = colors[order[i]].r;
		FOX_colors[j].peGreen = colors[order[i]].g;
		FOX_colors[j].peBlue = colors[order[i]].b;
	}
#endif /* SIMPLE_COMPRESSION */
}

// ustaw palete 

int DX8_SetColors(_THIS, int firstcolor, int ncolors, FOX_Color *colors)
{
	int i;
	int alloct_all;

	// kopiuj palete kolorow
	alloct_all = 0;
	
	if ( FOX_palette != NULL ) 
	{
		if ( (this->screen->flags&FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
		{
			
			for ( i=0; i< ncolors; ++i ) 
			{
			        int j = firstcolor + i;
				FOX_colors[j].peRed = colors[i].r;
				FOX_colors[j].peGreen = colors[i].g;
				FOX_colors[j].peBlue = colors[i].b;
			}
			IDirectDrawPalette_SetEntries(FOX_palette, 0,
				firstcolor, ncolors, &FOX_colors[firstcolor]);
			alloct_all = 1;
		} 
		else 
		{
			DX8_CompressPalette(this, colors, ncolors, 236);
			
			colorchange_expected = 1;
			IDirectDrawPalette_SetEntries(FOX_palette, 0,
							0, 256, FOX_colors);
		}
	}
	return(alloct_all);
}

static void DX8_SwapGamma(_THIS)
{
	return;
}

// gamma tylko w dx>=7

#ifdef IID_IDirectDrawGammaControl

static int DX8_SetGammaRamp(_THIS, Uint16 *ramp)
{
	LPDIRECTDRAWGAMMACONTROL gamma;
	DDGAMMARAMP gamma_ramp;
	HRESULT result;

	// sprawdz tryb video
	
	if ( ! FOX_primary ) 
	{
		FOX_SetError("[FOX]: tryb video musi byc ustawiony dla gammy");
		return(-1);
	}

	// pobierz controle gammy
	
	result = IDirectDrawSurface3_QueryInterface(FOX_primary,
			&IID_IDirectDrawGammaControl, (LPVOID *)&gamma);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::QueryInterface(GAMMA)", result);
		return(-1);
	}

	// ustaw rampe dla gammy
	
	memcpy(gamma_ramp.red, &ramp[0*256], 256*sizeof(*ramp));
	memcpy(gamma_ramp.green, &ramp[1*256], 256*sizeof(*ramp));
	memcpy(gamma_ramp.blue, &ramp[2*256], 256*sizeof(*ramp));
	result = IDirectDrawGammaControl_SetGammaRamp(gamma, 0, &gamma_ramp);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawGammaControl::SetGammaRamp()", result);
	}

	// zwolnij interface i zwroc
	IDirectDrawGammaControl_Release(gamma);
	return (result == DD_OK) ? 0 : -1;
}

// wygenerowanie rampy 

static int DX8_GetGammaRamp(_THIS, Uint16 *ramp)
{
	LPDIRECTDRAWGAMMACONTROL gamma;
	DDGAMMARAMP gamma_ramp;
	HRESULT result;

	// sprawdz tryb video
	
	if ( ! FOX_primary ) 
	{
		FOX_SetError("[FOX]: tryb video musi byc ustawiony dla korekty gammy");
		return(-1);
	}

	// pobierz game
	
	result = IDirectDrawSurface3_QueryInterface(FOX_primary,
			&IID_IDirectDrawGammaControl, (LPVOID *)&gamma);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::QueryInterface(GAMMA)", result);
		return(-1);
	}

	// ustaw rampe dla gammy
	
	result = IDirectDrawGammaControl_GetGammaRamp(gamma, 0, &gamma_ramp);
	
	if ( result == DD_OK ) 
	{
		memcpy(&ramp[0*256], gamma_ramp.red, 256*sizeof(*ramp));
		memcpy(&ramp[1*256], gamma_ramp.green, 256*sizeof(*ramp));
		memcpy(&ramp[2*256], gamma_ramp.blue, 256*sizeof(*ramp));
	} 
	else 
	{
		SetDDerror("DirectDrawGammaControl::GetGammaRamp()", result);
	}

	// zwolnij interface i wroc
	
	IDirectDrawGammaControl_Release(gamma);
	return (result == DD_OK) ? 0 : -1;
}

#endif /* IID_IDirectDrawGammaControl */

// wyjscie, zwalnianie surface itd

void DX8_VideoQuit(_THIS)
{
	int i, j;

	// jezeli fullscreen GL , reset 

	if ( this->screen != NULL ) 
	{
		if ( (this->screen->flags & (FOX_OPENGL|FOX_FULLSCREEN)) ==
		                            (FOX_OPENGL|FOX_FULLSCREEN) ) 
		{
			ChangeDisplaySettings(NULL, 0);
		}

	// powinno byc jakies sprawdzenie - jaki stan opengl ? zainicjalizowany

//		if ( this->screen->flags & FOX_OPENGL ) 
//		{
//			WIN_GL_ShutDown(this);
//		}
	}

	// zwolnij palete jezeli jakakolwiek uzywana
	
	if ( FOX_palette != NULL ) 
	{
		IDirectDrawPalette_Release(FOX_palette);
		FOX_palette = NULL;
	}

	// zwolnij podstawowy surface
	
	if ( FOX_primary != NULL ) 
	{
		FOX_primary = NULL;
	}

	// zwolnij liste dostepnych trybow video ;) albo graficznych :>
	
	for ( i=0; i<NUM_MODELISTS; ++i ) 
	{
		if ( FOX_modelist[i] != NULL ) 
		{
			for ( j=0; FOX_modelist[i][j]; ++j )
				free(FOX_modelist[i][j]);
			free(FOX_modelist[i]);
			FOX_modelist[i] = NULL;
		}
	}

	// zwolnij video 
	
	if ( FOX_Window ) 
	{
		DX8_DestroyWindow(this);
	}

	// zwolnij ikonke
	
	if ( screen_icn ) 
	{
		DestroyIcon(screen_icn);
		screen_icn = NULL;
	}
}

// tylko dla windows

void DX8_RealizePalette(_THIS)
{
	if ( FOX_palette ) 
	{
		IDirectDrawSurface3_SetPalette(FOX_primary, FOX_palette);
	}
}

// ponowne ustawienie dla 8bpp

static void DX8_Recolor8Bit(_THIS, FOX_Surface *surface, Uint8 *mapping)
{
	int row, col;
	Uint8 *pixels;

	if ( surface->w && surface->h ) 
	{
		if ( (surface->flags & FOX_HWSURFACE) == FOX_HWSURFACE ) 
		{
			if ( this->LockHWSurface(this, surface) < 0 ) 
			{
				return;
			}
		}
		
		for ( row=0; row<surface->h; ++row ) 
		{
			pixels = (Uint8 *)surface->pixels+row*surface->pitch;
			
			for ( col=0; col<surface->w; ++col, ++pixels ) 
			{
				*pixels = mapping[*pixels];
			}
		}
		
		if ( (surface->flags & FOX_HWSURFACE) == FOX_HWSURFACE ) 
		{
			this->UnlockHWSurface(this, surface);
		}
		FOX_UpdateRect(surface, 0, 0, 0, 0);
	}
}

// zmiana palety ? 

void DX8_PaletteChanged(_THIS, HWND window)
{
	FOX_Palette *palette;
	FOX_Color *saved = NULL;
	HDC hdc;
	int i;
	PALETTEENTRY *entries;

	// true jezeli okno wlasnie sie zamyka
	
	if ( (FOX_primary == NULL) || (FOX_VideoSurface == NULL) )
		return;

	
	palette = this->physpal;
	
	if(!palette)
	        palette = FOX_VideoSurface->format->palette;
	
	if ( palette == NULL ) 
	{ 
		return;
	}
	
	entries = (PALETTEENTRY *)alloca(palette->ncolors*sizeof(*entries));
	hdc = GetDC(window);
	GetSystemPaletteEntries(hdc, 0, palette->ncolors, entries);
	ReleaseDC(window, hdc);
	
	if ( ! colorchange_expected ) 
	{
		saved = (FOX_Color *)alloca(palette->ncolors*sizeof(FOX_Color));
		memcpy(saved, palette->colors, 
					palette->ncolors*sizeof(FOX_Color));
	}
	
	for ( i=0; i<palette->ncolors; ++i ) 
	{
		palette->colors[i].r = entries[i].peRed;
		palette->colors[i].g = entries[i].peGreen;
		palette->colors[i].b = entries[i].peBlue;
	}
	
	if ( ! colorchange_expected ) 
	{
		Uint8 mapping[256];

		memset(mapping, 0, sizeof(mapping));
		
		for ( i=0; i<palette->ncolors; ++i ) 
		{
			mapping[i] = FOX_FindColor(palette,
					saved[i].r, saved[i].g, saved[i].b);
		}
		DX8_Recolor8Bit(this, FOX_VideoSurface, mapping);
	}
	colorchange_expected = 0;

	FOX_FormatChanged(FOX_VideoSurface);
}

// exportowane 

void DX8_WinPAINT(_THIS, HDC hdc)
{
	FOX_UpdateRect(FOX_PublicSurface, 0, 0, 0, 0);
}

//end
