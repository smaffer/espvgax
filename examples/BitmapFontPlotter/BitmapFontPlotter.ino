#include <ESPVGAX.h>
#include <fonts/monodos12.h>
#include <fonts/monodos8.h>

ESPVGAX vga;

static const ESPVGAX_ALIGN32 char str0[] PROGMEM="\
ESPVGAX Version 1.0\n\
\n\
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor \
incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis \
nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \n\
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu \
fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in \
culpa qui officia deserunt mollit anim id est laborum.\n\
Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium \
doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore \
veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim \
ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia \
consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque \
porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, \
adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et \
dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis \
nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex \
ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea \
voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem \
eum fugiat quo voluptas nulla pariatur?\n\
At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis \
praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias \
excepturi sint occaecati cupiditate non provident, similique sunt in culpa \
qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum \
quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum \
soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime \
placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus.\n\
Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus \
saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae.\n\
Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis \
voluptatibus maiores alias consequatur aut perferendis doloribus asperiores \
repellat.";

void loop() {
  while (1) { 
    //never return from loop(). never call delay() or yield() functions
    vga.clear(0);
    ESPVGAX::PrintInfo end;
    //plot with MONODOS12 font, height 12
    vga.setBitmapFont((uint8_t*)img_monodos12_data, 12);
    end.x=end.y=0;
    for (int i=0; i!=2000; i++) {
      //plot single character, with automatic text wrap 
      end=vga.print_P(str0+i, end.x, end.y, true, 1, ESPVGAX_OP_SET, false, 0);
      vga.delay(5);
    }
    vga.delay(2000);
    vga.clear(0);
    //plot with MONODOS8 font, height 8
    vga.setBitmapFont((uint8_t*)img_monodos8_data, 8);
    end.x=end.y=0;
    for (int i=0; i!=2000; i++) {
      //plot single character, with automatic text wrap 
      end=vga.print_P(str0+i, end.x, end.y, true, 1, ESPVGAX_OP_SET, false, 0);
      vga.delay(5);
    }
    //print another string after the plotted string (endy+8)
    vga.print_P(str0, 0, end.y+8, true);
  }
}
void setup() {
  vga.begin();
}