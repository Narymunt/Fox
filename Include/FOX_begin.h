// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:
// - sprawdziæ alignacjê
// - sprawdziæ DECLSPEC
// - poprawiæ, ¿eby nie wyrzuca³ warning

// Kilka niezbednych rzeczy dla ustawienia funkcji dynamicznych itd.
// Ciekawe jest to, ze wszyscy zalecaja stosowanie alignacji na 4 bajtach, bo
// zawsze mi sie wydawalo, ze Visual sam sobie alignuje

// sprawdz czy juz to dolaczone ? 

#ifdef	_begin_code_h	
	#error FOX: W zlym miejscu dolaczony begin_code.h 
#endif

	// teraz definiujemy, ze to co znajduje sie dalej

	#define _begin_code_h

// w jaki sposob generujemy dll?
// !!! nie mam za cholere pojecia co o znaczy, ale bez tego nie chce dzialac
// !!! jako multithreaded dll

	#define DECLSPEC __declspec(dllexport)


// kolejna mala zagadka - alignacja zmiennych na granicy 4 bajtow
// !!! jezeli beda jakies problemy do wywalic to 
// !!! poza tym tutaj jest wstawka, ze ma omijac warning wlasnie o alignacje

#pragma warning(disable: 4103)
#pragma pack(push,4)

// taki sobie skrot, ale generalnie, jezeli bedzie problem z funkcjami inline, to
// trzeba bedzie to wszystko przeklepac jeszcze raz

#define __inline__	__inline


// end
