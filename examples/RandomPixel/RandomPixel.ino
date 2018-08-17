#include <ESPVGAX.h>

ESPVGAX vga;

//set one pixel at a time
void random_setpixel() {
  int x=vga.rand()%ESPVGAX_WIDTH;
  int y=vga.rand()%ESPVGAX_HEIGHT;
  vga.setpixel(x, y, vga.rand()%2);
}
//set 8 pixels at a time
void random_setpixel32() {
  int x=vga.rand()%ESPVGAX_WWIDTH;
  int y=vga.rand()%ESPVGAX_HEIGHT;
  vga.setpixel32(x, y, vga.rand());
}
//set 32 pixels at a time
void random_setpixel8() {
  int x=vga.rand()%ESPVGAX_BWIDTH;
  int y=vga.rand()%ESPVGAX_HEIGHT;
  vga.setpixel8(x, y, vga.rand()%256); 
}
void loop() {
  while (1) { 
    //never return from loop(). never call delay() or yield() functions
    random_setpixel();
    random_setpixel8();
    random_setpixel32();
  }
}
void setup() {
  vga.begin();
}