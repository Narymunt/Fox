// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

extern int FOX_RLESurface(FOX_Surface *surface);
extern int FOX_RLEBlit(FOX_Surface *src, FOX_Rect *srcrect,
                       FOX_Surface *dst, FOX_Rect *dstrect);
extern int FOX_RLEAlphaBlit(FOX_Surface *src, FOX_Rect *srcrect,
			    FOX_Surface *dst, FOX_Rect *dstrect);
extern void FOX_UnRLESurface(FOX_Surface *surface, int recode);

// end
