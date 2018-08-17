//file included from ESPVGAX.cpp

/* 
 * porting of TVOut::draw_row method, writen by Myles Metzer, optimized for 
 * ESPVGAX 32bit framebuffer (super blazing fast??) 
 */
static inline void draw_row(int line, int x0, int x1, uint8_t c, int op) {
	if (x0==x1) {
	  ESPVGAX::putpixel(x0, line, c, op);
	} else {
	  if (x0 > x1) {
	    int tmp=x0;
	    x0=x1;
	    x1=tmp;
	  }
	  int sw=x1-x0;
    while (x0%32) {
      ESPVGAX::putpixel(x0, line, c, op);
      x0++;
      sw--;
    }
	  while (sw>32) {
	  	uint32_t c32=c ? 0xffffffff : 0;
  		ESPVGAX::putpixel32(x0/32, line, SWAP_UINT32(c32), op);
	  	x0+=32;
	  	sw-=32;
	  }
    while (sw>0) {
      ESPVGAX::putpixel(x0, line, c, op);
      x0++;
      sw--;
    }
	}
}
// porting of TVOut::draw_column method, writen by Myles Metzer
static inline void draw_column(int row, int y0, int y1, uint8_t c, int op) {
	if (y0==y1) {
	  ESPVGAX::putpixel(row, y0, c, op);
	} else {
	  if (y1<y0) {
	    int tmp=y0;
	    y0=y1;
	    y1=tmp;
	  }
	  while (y0<y1) {
	    ESPVGAX::putpixel(row, y0++, c, op);
	  }
	}
}
// porting of TVOut::draw_row method, writen by Myles Metzer
void ESPVGAX::drawRect(int x0, int y0, int w, int h, uint8_t c, bool fill, int op) {
	if (fill) {
    for (int i=y0; i<y0+h; i++) {
      draw_row(i, x0, x0+w, c, op);
    }
  } else {
	  drawLine(x0,   y0,   x0+w, y0,   c, op);
	  drawLine(x0,   y0,   x0,   y0+h, c, op);
	  drawLine(x0+w, y0,   x0+w, y0+h, c, op);
	  drawLine(x0,   y0+h, x0+w, y0+h, c, op);
	}
}
// Bresenham's line algorithm from https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void ESPVGAX::drawLine(int x0, int y0, int x1, int y1, uint8_t c, int op) {
  if (x0==x1)
    return draw_column(x0, y0, y1, c, op);
  else if (y0==y1)
    return draw_row(y0, x0, x1, c, op);

  int dx=abs(x1-x0), sx=x0<x1 ? 1 : -1;
  int dy=abs(y1-y0), sy=y0<y1 ? 1 : -1;
  
  if (dx==1 && dy==1)
    return putpixel(x0,y0,c, op);

  int err=(dx>dy ? dx : -dy)/2, e2;
  for (;;) {
    putpixel(x0,y0,c,op);
    if (x0==x1 && y0==y1) 
      break;
    e2=err;
    if (e2>-dx) { 
      err-=dy; 
      x0+=sx; 
    }
    if (e2<dy) { 
      err+=dx; 
      y0+=sy; 
    }
  }
}
// Midpoint circle algorithm from https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void ESPVGAX::drawCircle(int x0, int y0, int radius, uint8_t c, bool fill, int op) {
	if (radius<0)
		return;
  int x=radius-1;
  int y=0;
  int dx=1;
  int dy=1;
  int err=dx-(radius<<1);
  while (x>=y) {
    if (fill) {
      draw_row(y0+y, x0-x, x0+x, c, op);
      draw_row(y0+x, x0-y, x0+y, c, op);
      draw_row(y0-y, x0-x, x0+x, c, op);
      draw_row(y0-x, x0-y, x0+y, c, op);
    } else {
      putpixel(x0-x, y0+y, c, op);
      putpixel(x0+x, y0+y, c, op);
      putpixel(x0-y, y0+x, c, op);
      putpixel(x0+y, y0+x, c, op);
      putpixel(x0-x, y0-y, c, op);
      putpixel(x0+x, y0-y, c, op);
      putpixel(x0-y, y0-x, c, op);
      putpixel(x0+y, y0-x, c, op);
    }
    if (err<=0) {
      y++;
      err+=dy;
      dy+=2;
    }
    if (err>0) {
      x--;
      dx+=2;
      err+=dx-(radius<<1);
    }
  }
}