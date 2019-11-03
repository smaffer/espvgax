/*
ESPVGAX Library for Arduino ESP8266
Sourcecode url: https://github.com/smaffer/espvgax

512x480px VGA framebuffer with 1bpp, +2 optionals line colors.

COPYRIGHT (C) 2018 Sandro Maffiodo
smaffer@gmail.com
http://www.sandromaffiodo.com

VGA signal generation based on https://github.com/hchunhui/esp-vga (thanks for 
your awesome works).

See https://github.com/smaffer/espvgax for the library description, for the
hardware wiring and library usage.
*/
#ifndef __ESPVGAX_LIBRARY__
#define __ESPVGAX_LIBRARY__

#define ESPVGAX_VERSION "1.0.0"

#include <ESP8266WiFi.h>
#include <Arduino.h>

#define ESPVGAX_WIDTH 512
#define ESPVGAX_BWIDTH (ESPVGAX_WIDTH/8)
#define ESPVGAX_WWIDTH (ESPVGAX_WIDTH/32)
#define ESPVGAX_HEIGHT 480
#define ESPVGAX_FBBSIZE (ESPVGAX_HEIGHT*ESPVGAX_BWIDTH)

#define ESPVGAX_HSYNC_PIN D2 
#define ESPVGAX_VSYNC_PIN D1 
#define ESPVGAX_COLOR_PIN D7 //cannot be changed. D7=GPIO13, used by HSPI
/* 
 * A NICE TRICK: connect a wire to D5 if you want a background color: this PIN 
 * is HIGH when PIXELDATA is sent to VGA. You can choose one of the two VGA RGB 
 * PINs not connected to D7 
 */
#define ESPVGAX_A_NICE_TRICK //:-)
/* 
 * enable EXTRA COLORS generation. Two additional PINs are used. These colors 
 * will cover one or more lines 
 */
//#define ESPVGAX_EXTRA_COLORS
/*
 * optional PIN for line coloring, used only if ESPVGAX_EXTRA_COLORS is defined
 * 
 * WARNING: If you change this constant you NEED to change sources where GP16O
 *  hardware address is used) 
 */
#define ESPVGAX_EXTRA_COLOR1_PIN D0 
/* 
 * optional PIN for line coloring, used only if ESPVGAX_EXTRA_COLORS is defined
 * 
 * WARNING: This PIN must be disconnected before upload sketches) 
 */
#define ESPVGAX_EXTRA_COLOR2_PIN D4 
// line property bits
#define ESPVGAX_PROP_COLOR1 1
#define ESPVGAX_PROP_COLOR2 2
/* 
 * choose ESP8266 hardware timer used for VGA signal gneeration. 1 is TIMER1 
 * and 0 is TIMER0. In both cases, Arduino libraries will generate some 
 * conflicts so you need to keep your program as simple as possible, using only 
 * other libraries that DO NOT use hardware timers or background tasks, like 
 * Wifi or SPI. You can use SoftwareSerial or Wire library to communicate with 
 * the external world, or if you really need Wifi, you can pause VGA signal 
 * temporarely (pause/resume methods) 
 *
 * WARNING: TIMER0 works only at 80MHz
 */
#define ESPVGAX_TIMER 1

// BITWISE operations, used by drawing primitives
#define ESPVGAX_OP_OR 1
#define ESPVGAX_OP_XOR 2
#define ESPVGAX_OP_SET 3

// only for documentation: track pointers that MUST BE on FLASH(PROGMEM)
#define ESPVGAX_PROGMEM 

// data aligned on 32bit memory. needed to use pgm_read_dword
#define ESPVGAX_ALIGN32 __attribute__ ((aligned(4)))

// ESPVGAX static class
class ESPVGAX {
public:
  /* 
   * begin()
   * end()
   *    install/uninstall VGA signal generation. Depending on ESPVGAX_TIMER
   *    hardware timer1 or timer2 will be used.
   *    begin method will setup HSPI data transmission on PINS D5,D6,D7,D8 
   */
  static void begin();
  static void end();
  /* 
   * pause()
   * resume()
   *    temporarely blackout VGA pixels trasfert. The VGA signal is keep ON but
   *    zero pixels will be drawed. This is usefull if you need to use some
   *    Arduino functions that generate too many noises on the VGA signal
   *    generation.
   *
   *    NOTE: from my tests pause/resume does not work with Wifi. If you look
   *      at ./examples/Wifi/Wifi.ino i have used begin/end instead... pause/
   *      resume will cause an ESP8266 runtime exception
   */
  static void pause();
  static void resume();
  /*
   * setLineProp(y, prop)
   * setLinesProp(start, end, prop)
   * getLineProp(y)
   *    get/set orizzontal line properties. prop parameter must be a bitwise OR
   *    of ESPVGAX_PROP_* constants. For example, ESPVGAX_PROP_COLOR1 will turn
   *    ON the output pin ESPVGAX_EXTRA_COLOR1_PIN for the selected line (y)
   */
  static void setLineProp(int y, uint8_t prop);
  static void setLinesProp(int start, int end, uint8_t prop);
  static uint8_t getLineProp(int y);
  /*
   * delay(msec)
   * rand()
   * srand(seed)
   *    replacement for some Arduino functions that will generate noise on the
   *    VGA signal generation. Please use these functions instead of the normal
   *    ones.
   */
  static void delay(uint32_t msec);
  static uint32_t rand();
  static void srand(uint32_t seed);
  /*
   * clear(c8)
   *    fast clear of VGA framebuffer. c8 parameter is used to fill 8 pixels 
   *    with cleared value. For example 0xff will turn 8 pixel on, 0xf0 will 
   *    turn 4 pixels on and 4 off
   */
  static inline void clear(uint8_t c8=0) { 
    memset((void*)fbw, c8, ESPVGAX_FBBSIZE); }
  /*
   * copy_P(from)
   * copy  (from)
   *    fast copy from a buffer in RAM or in FLASH to VGA framebuffer. Use the
   *    copy_P function is your buffer is stored in FLASH.
   */
  static inline void copy_P(ESPVGAX_PROGMEM uint8_t *from) { 
    memcpy_P((void*)fbb, (void*)from, ESPVGAX_FBBSIZE); }

  static inline void copy(uint8_t *from) { 
    memcpy((void*)fbw, (void*)from, ESPVGAX_FBBSIZE); }
  /*
   * isYOutside  (y  )
   * isXOutside  (x  )
   * isXOutside8 (x8 )
   * isXOutside32(x32)
   *    test if a coordinate (x or y) lies outside of the framebuffer.
   *
   *    WARNING: isXOutside8 require an x coordinate in bytes instead of pixels.
   *      For example pixel 9 must be converted to 1, pixel 11 also to 1.
   *    WARNING: isXOutside32 require an x coordinate in 32bit words instead of 
   *      pixels. For example pixel 32 must be converted to 1, 64 to 2, etc..
   */
  static inline bool isYOutside(int y) { 
    return y<0 || y>=ESPVGAX_HEIGHT; }

  static inline bool isXOutside(int x) { 
    return x<0 || x>=ESPVGAX_WIDTH; }

  static inline bool isXOutside8(int x8) { 
    return x8<0 || x8>=ESPVGAX_BWIDTH; }

  static inline bool isXOutside32(int x32) { 
    return x32<0 || x32>=ESPVGAX_WWIDTH; }
  /*
   * putpixel  (x,   y, c,   op)
   * putpixel8 (x8,  y, c8,  op)
   * putpixel32(x32, y, c32, op)
   *    slow put pixel. draw a single pixel on the framebuffer. This method is
   *    slow compared to methods that draws multiple pixels at a time, but in
   *    some drawing primitives this is the simplest way to draw a single pixel.
   *
   *    parameter c is the color of the pixel to be set (1 on, 0 off).
   *    parameter op can be one of the ESPVGAX_OP_* constants and will select
   *      the bitwise operation used to put the pixel.
   *
   *    WARNING: putpixel8 and putpixel32 are optimized for write 8 pixels and
   *      32 pixels at a time. For these methods you must use x coordinates 
   *      expressed in bytes (putpixel8) or 32bit words (putpixel32)
   *
   *    WARNING(2): in *32 methods c32 must bytes must be in big endian order!
   */
  static inline void putpixel(int x, int y, uint8_t c, int op=ESPVGAX_OP_SET) {
    #define PUTPIXEL_BASE(x, c, suffix) \
      if (isXOutside(x) || isYOutside(y)) \
        return; \
      if (op==ESPVGAX_OP_SET) \
        set##suffix(x, y, c); \
      else if (op==ESPVGAX_OP_XOR) \
        xor##suffix(x, y, c); \
      else \
        or##suffix(x, y, c);
    PUTPIXEL_BASE(x, c, pixel);
  }
  static inline void putpixel8(int x8, int y, uint8_t c8, int op=ESPVGAX_OP_SET) {
    PUTPIXEL_BASE(x8, c8, pixel8);
  }
  static inline void putpixel32(int x32, int y, uint32_t c32, int op=ESPVGAX_OP_SET) {
    PUTPIXEL_BASE(x32, c32, pixel32);
    #undef PUTPIXEL_BASE
  }
  /*
   * setpixel32(x32, y, c32)
   * orpixel32 (x32, y, c32)
   * xorpixel32(x32, y, c32)
   *    fast put pixels optimized to write 32 pixels at a time.
   *
   *    WARNING: x32 parameter require an x coordinate in 32bit words instead of
   *      pixels, so, if you want to address pixels 32..63 you need to use an
   *      x32 coordinate equal to 1, if you want to address 64..95 use 2
   *
   *    WARNING(2): in *32 methods c32 must bytes must be in big endian order!
   */
  static inline void setpixel32(int x32, int y, uint32_t c32) {
    #define WRITE_PIXEL32_BASE(op) \
      if (isXOutside32(x32) || isYOutside(y)) \
        return; \
      fbw[y][x32] op c32;
    WRITE_PIXEL32_BASE(=);
  }
  static inline void orpixel32(int x32, int y, uint32_t c32) {
    WRITE_PIXEL32_BASE(|=);
  }
  static inline void xorpixel32(int x32, int y, uint32_t c32) {
    WRITE_PIXEL32_BASE(^=);
    #undef WRITE_PIXEL32_BASE
  }
  /*
   * setpixel8(x8, y, c8)
   * orpixel8 (x8, y, c8)
   * xorpixel8(x8, y, c8)
   *    fast put pixels optimized to write 8 pixels at a time.
   *
   *    WARNING: x8 parameter require an x coordinate in bytes instead of 
   *      pixels, so, if you want to address pixels 8..15 you need to use an
   *      x8 coordinate equal to 1, if you want to address 16..23 use 2
   */
  static inline void setpixel8(int x8, int y, uint8_t c8) {
    #define WRITE_PIXEL8_BASE(op) \
      if (isXOutside8(x8) || isYOutside(y)) \
        return; \
      fbb[y*ESPVGAX_BWIDTH+x8] op c8;
    WRITE_PIXEL8_BASE(=);
  }  
  static inline void orpixel8(int x8, int y, uint8_t c8) {
    WRITE_PIXEL8_BASE(|=);
  }  
  static inline void xorpixel8(int x8, int y, uint8_t c8) {
    WRITE_PIXEL8_BASE(^=);
    #undef WRITE_PIXEL8_BASE
  }  
  /*
   * setpixel(x, y, c)
   * orpixel (x, y, c)
   * xorpixel(x, y, c)
   *    write single pixels. If you can, use optimized version for writing 8 or
   *    32 pixels at a time. write speed will be more higher!
   */
  static inline void setpixel(int x, int y, uint8_t c) {
    #define WRITE_PIXEL_BASE \
      uint8_t *p=(uint8_t*)&fbb[y*ESPVGAX_BWIDTH + (x>>3)]; \
      uint8_t shift=7-(x & 7); \
      c=c>0?1:0;
    WRITE_PIXEL_BASE;
    *p&=~(1<<shift);
    *p|= (c<<shift);
  }
  static inline void orpixel(int x, int y, uint8_t c) {
    WRITE_PIXEL_BASE;
    c=c>0?1:0;
    *p|=(c<<shift);
  }
  static inline void xorpixel(int x, int y, uint8_t c) {
    WRITE_PIXEL_BASE;
    #undef WRITE_PIXEL_BASE
    c=c>0?1:0;
    *p^=(c<<shift);
    #undef WRITE_PIXEL_BASE
  }
  /*
   * getpixel(x, y)
   *    x: horizontal pixel coordinate. Must be less than ESPVGAX_WIDTH
   *    y: vertical pixel coordinate. Must be less than ESPVGAX_HEIGHT
   *    return: 1bit color at <x,y> coordinate
   */
  static inline uint8_t getpixel(int x, int y) {
      if (isXOutside(x) || isYOutside(y))
        return 0;
      uint8_t *p=(uint8_t*)&fbb[y*ESPVGAX_BWIDTH + (x>>3)];
      uint8_t shift=7-(x & 7);
      return ( (*p) >> shift ) & 1;
  }
  /*
   * blit_P(src, dx, dy, srcw, scrh, op, scrwstride)
   * blit  (src, dx, dy, srcw, scrh, op, scrwstride)
   *    draw an image at a given coordinate.
   *
   *    parameter src point to the image to be drawed. blit_P method require
   *      that src data is stored inside FLASH ROM
   *    parameters dx and dy specify the top left coordinate where the src image 
   *      will be drawed
   *    parameters srcw and srch specify the width and height, in pixels, of the
   *      src image
   *    parameter op set the bitwise operator used to draw the src image. One of
   *      the ESPVGAX_OP_* constants must be used
   *    parameter srcwstride is the width, in pixels, of the src image. This
   *      parameter can seem to be the same thing of parameter srcw but can be
   *      a different value. For example, if src image is a subimage of a 
   *      bigger image, srcw will be the width of the subimage, but srcwstride
   *      MUST be the width of the bigger image. This value will be used to 
   *      jump from a line of pixels, inside src, to the next line of pixels
   *
   *    REMARKS: op=ESPVGAX_OP_XOR will work fine ONLY for blit operations that
   *      has dx and srcw multiples of 32 pixels
   *
   *    REMARKS(2): if you need to draw a big image (bigger than half the 
   *      framebuffer size) some flicker can appear. I have not figured out why
   *      this event appen but it appens. You can choose between the following
   *      solutions to limit screen flickering:
   *        1) draw the big image once. Do not redraw the image if you do not
   *          need to redraw it
   *        2) split the image in multiple subimage and draw them in sequence,
   *          maybe using some delay calling ESPVGAX::delay method
   */
  static void blit_P(ESPVGAX_PROGMEM uint8_t *src, int dx, int dy, 
    int srcw, int srch, int op=ESPVGAX_OP_SET, int srcwstride=0);

  static void blit(uint8_t *src, int dx, int dy, int srcw, int srch, 
    int op=ESPVGAX_OP_SET, int srcwstride=0);
  /*
   * setFont(fnt, glyphscount, fntheight, glyphbwidth, hspace, vspace)
   *    set current font for print methods. this method will set a dynamic 
   *    width font (not monospaced). you can generate a compatible font by
   *    using the 1bitfont.html tool included inside the ./tools/ folder.
   *
   *    parameter fnt must point to an array of bytes where the first byte is 
   *      the width of the glyph, the following 3 bytes are unused (needed for
   *      alignments) and the following N*M bytes are the bitmap of the glyph.
   *      For example:
   *        uint8_t ESPVGAX_ALIGN32 fnt0[2][4+6] PROGMEM={
   *          { 1,0,0,0, 128, 128, 128, 0, 128, 0, },
   *          { 1,0,0,0, 128, 128, 128, 0, 128, 0, },
   *        }
   *      is a list of two glyphs (character !) with font height equal to 6 
   *    parameter glyphscount is the number of glyphs inside fnt array
   *    parameter fntheight is the height of glyphs (must be the same for all
   *      glyphs inside fnt array)
   *    parameter glyphbwidth is the number of bytes used for each glyph. For
   *      example, if your font has a maximum width of 6px you need only 1 byte 
   *      width glyphs, if the masimum is 14, you need 2 bytes for each glyph
   *      orizzontal data.
   *    parameter hspace and vspace specify the space in pixels used to draw
   *      text strings. hspace is for orizzontal and vspace for vertical space
   *      (like a sort of text interline)
   */
  static void setFont(ESPVGAX_PROGMEM uint8_t *fnt, uint8_t glyphscount, 
    uint8_t fntheight, uint8_t glyphbwidth=1, uint8_t hspace=2, 
    uint8_t vspace=1);
  /*
   * setBitmapFont(bitmap, fntheight, glyphbwidth)
   *    set current font for print methods. this method will set a fixed width
   *    font (monospaced) where all 256 font glyphs have the same width and
   *    height.
   *
   *    parameter bitmap must point to an image that contains all 256 font 
   *      glyphs. The image must contain 16 glyphs for line and a total of
   *      16 lines of glyphs
   *    parameter fntheight set the height of one of the 256 glyphs. All glyphs
   *      must have the same height
   *    parameter glyphbwidth set the width, in bytes, of one of the 256 glyphs
   */
  static void setBitmapFont(ESPVGAX_PROGMEM uint8_t *bitmap, uint8_t fntheight,
                            int glyphbwidth=1); 
  /*
   * print_P(str, dx, dy, wrap, len, op, bold)
   * print  (str, dx, dy, wrap, len, op, bold) 
   *    draw a string of characters on framebuffer. print_P method requires a
   *    pointer to a string stored in FLASH ROM.
   *    input string will be drawed using the current selected font (bitmap
   *    font or font with dynamic widths).
   *
   *    parameter str point to the string to be drawed
   *    parameters dx and dy are the (x,y) coordinate where the string will be
   *      drawed
   *    parameter wrap enable automatic text wrapping. When a character of the
   *      string lies outside of the framebuffer width, an automatic carriage
   *      return is generated (end of line)
   *    parameter len, if different than -1, is the number of characters to be
   *      drawed on screen. NOTE: if the string str include a ZERO character
   *      the rendering will stop despite len require more characters to be
   *      rendered
   *    parameter op specify the bitwise operation used to draw characters. One
   *      of the ESPVGAX_OP_* constants MUST be used
   *    parameter bold enable bold style rendering. If true, two characters will
   *      be drawed over, with a distance of 1 pixel
   *    parameter dx0override will be used, instead of dx, when a carriege 
   *      return is found, or when a wrap occour (only when wrap=true)
   *    parameter calc, if true no pixels will be drawed. All wrap and cursor
   *      move operations will be executed. You can use this parameter to
   *      measure the text before rendering it, for example you can call this
   *      method with calc=true and, after, center the text horizzontally
   *    return the screen coordinate where the print process has stopped, plus
   *      the width of the widest printed line
   *
   *    REMARKS: bold=true do not work well with op=ESPVGAX_OP_XOR. The only
   *      case where bold=true and op=ESPVGAX_OP_XOR work well is when dx and
   *      srcw are multiples of 32 pixels
   *
   *    REMARKS(2): if you print a long string keep in mind that reading from
   *      current font's glyphs (bitmap in FLASH ROM) can cause some flicker if
   *      the number of read op is high. I have not figured out why this appen
   *      but if you need to draw a long string whitout too many flicker you
   *      can choose one of these solutions:
   *        1) draw the string once. Do not redraw the long string if you do not
   *          need to redraw it. In this case, if flicker appear, it appear only
   *          once.
   *        2) draw substrings instead of the full string. If you are using the
   *          wrap option (wrap=true) you can use the dx0ovveride parameter to
   *          render a subset of the string characters (N at a time instead of
   *          all characters at one time)
   */
  class PrintInfo {
  public:
    PrintInfo(int x=0, int y=0, int w=0) : x(x), y(y), w(w) {}
    int x, y, w;
  };
  static PrintInfo print_P(ESPVGAX_PROGMEM const char *str, int dx, int dy, 
    bool wrap=false, int len=-1, int op=ESPVGAX_OP_SET, bool bold=false, 
    int dx0override=-1, bool calc=false); 

  static PrintInfo print(const char *str, int dx, int dy, bool wrap=false, 
    int len=-1, int op=ESPVGAX_OP_SET, bool bold=false, int dx0override=-1,
    bool calc=false);
  /*
   * drawRect(x, y, w, h, c, fill, op) 
   *    draw a rectangle, filled or edges only
   *
   *    parameters x,y,w,h specify the rectangle dimension
   *    parameter c is the value of the pixel to be set
   *    parameter fill enable fill of the rectangular area
   *    parameter op is one of the ESPVGAX_OP_* constant and specify the bitwise
   *      operation that will be used to plot pixels
   */
  static void drawRect(int x, int y, int w, int h, uint8_t c, bool fill=false,
    int op=ESPVGAX_OP_SET);
  /*
   * drawLine(x0, y0, x1, y1, c, op)
   *    draw a line
   *
   *    parameters x0,y0,x1,y1 specify the two points of the line
   *    parameter c is the value of the pixel to be set
   *    parameter op is one of the ESPVGAX_OP_* constant and specify the bitwise
   *      operation that will be used to plot pixels
   */
  static void drawLine(int x0, int y0, int x1, int y1, uint8_t c, 
    int op=ESPVGAX_OP_SET);
  /*
   * drawCircle(x, y, radius, c, fill, op)
   *    draw a circle, filled or outline only
   *
   *    parameters x,y specify the center of the circle
   *    parameter radius is the radius of the circle (cannot be negative)
   *    parameter c is the value of the pixel to be set
   *    parameter fill enable fill of the circle's area
   *    parameter op is one of the ESPVGAX_OP_* constant and specify the bitwise
   *      operation that will be used to plot pixels
   */
  static void drawCircle(int x, int y, int radius, uint8_t c, bool fill=false, 
    int op=ESPVGAX_OP_SET);
  /*
   * fbw[HEIGHT][WWIDTH]
   *    this is the VGA framebuffer! you can write directly to this matrix if
   *    you needed it.
   *
   *    WARNING: the number of columns in this matrix is WWIDTH and not WIDTH!
   *      WWIDTH (aka ESPVGAX_WWIDTH) is the number of 32bits words in a line
   *      and not the number of bytes or the number of pixels in a line!
   *
   *    WARNING(2): this is an array of 32bit words in big endian byte order!
   *      While ESP8266 is little endian, this mean that if you want to write 
   *      all pixels with 0 except the most significant 32nd bit to 1 you must 
   *      write
   *        fbw[Y][X]=0x00000080;
   *      instead of
   *        fbw[Y][X]=0x80000000;
   *      If you want you can work in little endian and swap easily to big
   *      endian you can use the SWAP_UINT32 macro. keep in mind that this has
   *      a costs in terms of CPU cycles
   */
  static volatile uint32_t ESPVGAX_ALIGN32 fbw[ESPVGAX_HEIGHT][ESPVGAX_WWIDTH];
  /*
   * fbb[HEIGHT*BWIDTH]
   *    this is the VGA framebuffer too! points to the same memory address of
   *    fbw and act as an alias. If you prefer to access 8 pixels at a time,
   *    instead of 32 pixels at a time, you can use this pointer.
   *
   *    WARNING: the number of columns in this array is BWIDTH and not WIDTH!
   *      BWIDTH (aka ESPVGAX_BWIDTH) is the number of bytes in a line and not 
   *      the number of pixels in a line!
   */
  static volatile uint8_t *fbb;
  /*
   * tone(uint8_t t)
   * noTone()
   *    Yet unimplemented.
   */
  static inline void tone(uint8_t t) {}
  static inline void noTone() {}
};
#ifndef SWAP_UINT16
#define SWAP_UINT16(x) (((x)>>8) | ((x)<<8))
#endif

#ifndef SWAP_UINT32
#define SWAP_UINT32(x) \
  (((x)>>24) | (((x)&0x00ff0000)>>8) | (((x)&0x0000ff00)<<8) | ((x)<<24))
#endif

#endif
