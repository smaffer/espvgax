#include <ESPVGAX.h>

ESPVGAX vga;

void loop() {
  while (1) { 
    //never return from loop(). never call delay() or yield() functions
  }
}
void setup() {
  vga.begin();
  vga.drawRect(0, 0, ESPVGAX_WIDTH-1, ESPVGAX_HEIGHT-1, 1);

  vga.drawCircle(ESPVGAX_WIDTH/2, ESPVGAX_HEIGHT/2, 240-1, 1, false);
  vga.drawCircle(ESPVGAX_WIDTH/2, ESPVGAX_HEIGHT/2, 120-1, 1, false);
  vga.drawCircle(ESPVGAX_WIDTH/2, ESPVGAX_HEIGHT/2, 60-1, 1, true);
  
  int sw=32, sh=28;
  vga.drawRect(10, 10, sw, sh, 1, true, ESPVGAX_OP_XOR);
  vga.drawRect(ESPVGAX_WIDTH-sw-10, 10, sw, sh, 1, true, ESPVGAX_OP_XOR);
  vga.drawRect(10, ESPVGAX_HEIGHT-sh-10, sw, sh, 1, true, ESPVGAX_OP_XOR);
  vga.drawRect(ESPVGAX_WIDTH-sw-10, ESPVGAX_HEIGHT-sh-10, sw, sh, 1, true, ESPVGAX_OP_XOR);

  vga.drawLine(0, 0, ESPVGAX_WIDTH, ESPVGAX_HEIGHT, 1, ESPVGAX_OP_XOR);
  vga.drawLine(0, ESPVGAX_HEIGHT, ESPVGAX_WIDTH, 0, 1, ESPVGAX_OP_XOR);
  vga.drawLine(ESPVGAX_WIDTH/2, 0, ESPVGAX_WIDTH/2, ESPVGAX_HEIGHT, 1, ESPVGAX_OP_XOR);
  vga.drawLine(0, ESPVGAX_HEIGHT/2, ESPVGAX_WIDTH, ESPVGAX_HEIGHT/2, 1, ESPVGAX_OP_XOR);
}
