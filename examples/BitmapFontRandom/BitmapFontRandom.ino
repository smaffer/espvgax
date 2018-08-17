#include <ESPVGAX.h>
#include <fonts/monodos8.h>

ESPVGAX vga;
char matrix[60][64];

void loop() {
  while (1) { 
    //never return from loop(). never call delay() or yield() functions
    for (int i=0; i!=32; i++) {
      int x=vga.rand() % 64;
      int y=vga.rand() % 60;
      uint8_t c=(uint8_t)(vga.rand()%256);
      if (c==0)
        c=1;
      else if (c=='\n')
        c=' ';
      matrix[y][x]=(char)c;
    }
    vga.print((char*)matrix, 0, 0, true, sizeof matrix);
  }
}
void setup() {      
  vga.begin();
  vga.setBitmapFont((uint8_t*)img_monodos8_data, 8);
  memset(matrix, ' ', sizeof matrix);
}