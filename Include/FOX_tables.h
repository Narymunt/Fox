// Fox v0.5
// by Jaroslaw Rozynski
//====
// TODO:
// - uporzadkowac
// - sprawdzic

#ifndef _FOX_tables_h
#define _FOX_tables_h

#include <math.h>
#include <FOX.h>

// funkcje przeliczajace 

#ifdef __cplusplus
extern "C" {
#endif

void make_sin(void)
{
    int i;
    float a,c;

    c=M_PI/128.0;
    for(i=0,a=0.0; i<256;i++,a+=c)
    {
        sin256[i]=(int)((sin(a)*256.0)+0.5);
        cos256[i]=(int)((cos(a)*256.0)+0.5);
    }
    c=M_PI/512.0;
    for(i=0,a=0.0; i<1024;i++,a+=c)
    {
        sin1024[i]=(int)((sin(a)*1024.0)+0.5);
        cos1024[i]=(int)((cos(a)*1024.0)+0.5);
    }
}

long RotateX(int x,int y,int angle)
{
    return(((sin1024[angle]*y)-(cos1024[angle]*x))/1024);
}

long RotateY(int x,int y,int angle)
{
    return(((sin1024[angle]*x)-(cos1024[angle]*y))/1024);
}




// koniec jako c

#ifdef __cplusplus
};
#endif

#endif

// end
