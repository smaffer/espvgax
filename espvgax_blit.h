//file included by ESPVGAX.cpp

/*
 * copy 32bits at a time. src and dst regions are aligned to 32 pixels. FAST
 */
#define BLIT32(op, reader) \
  int srcw32=srcw/32; \
  int srcwstride32=srcwstride/32; \
  int dx32=dx/32; \
  uint32_t *src32=(uint32_t*)src; \
  volatile uint32_t *dst32=(volatile uint32_t*)&fbw[dy][dx32]; \
  for (int y=0; y!=srch; y++) { \
    if (!isYOutside(dy+y)) { \
      for (int x32=0; x32!=srcw32; x32++) { \
        if (isXOutside32(dx32+x32)) \
          continue; \
        dst32[x32] op reader(src32+x32); \
      } \
    } \
    src32+=srcwstride32; \
    dst32+=ESPVGAX_WWIDTH; \
  } 
/*
 * write 32bits at a time reading from unaligned src. src width can be aligned
 * to 32,16 or 8 pixels.
 * dst (dx,dy) can be unaligned too. in this case, each write generate a carry
 * that will be writen from the next 32bits write.
 */
#define BLITUNALIGNED(loop_code, final_code, reader32, reader16, reader8) \
  volatile uint32_t *dst=(volatile uint32_t*)&fbw[dy][dx/32]; \
  uint32_t dshift=dx % 32; \
  uint8_t pre=0; \
  if (dx<0 && dx%32) { \
    dst--; \
    pre=32; \
  } \
  while (srch>0) { \
    if (!isYOutside(dy)) { \
      uint32_t sx=0; \
      int sw=srcw; \
      int ldx=dx; \
      volatile uint32_t *d=dst;   \
      uint32_t carry=0, cmask=0; \
      while (sw>0) { \
        /* load from src */ \
        uint32_t s=0; \
        uint8_t *psrc=src+sx/8; \
        if (sw>16) { \
          s=reader32(psrc); \
        } else if (sw>8) { \
          s=reader16(psrc); \
        } else { \
          s=reader8(psrc); \
        } \
        s=SWAP_UINT32(s); \
        /* write s to dst 32bit at a time */ \
        uint32_t dv=*d; \
        /* swap framebuffer bigendian 32bit word to littleendian 32bit word */ \
        dv=SWAP_UINT32(dv); \
        uint32_t dmask=0xffffffff; \
        if (sw<32)  \
          dmask<<=32-sw; \
        dmask>>=dshift; \
        if (!isXOutside(ldx)) { \
          loop_code \
          /* reswap bytes endianess to write back */ \
          *d=SWAP_UINT32(dv); \
        } \
        if (dshift && sw+pre+dshift>32) { \
          /* save carried bits not writen inside the current 32bit word */ \
          carry=s<<(32-dshift); \
          cmask=0xffffffff<<(32-dshift); \
          if (sw<32)  \
            cmask<<=sw; \
        } else { \
          carry=0; \
          cmask=0; \
        } \
        sw-=32; \
        sx+=32; \
        ldx+=32; \
        d++; \
      } \
      if (cmask) {  \
        /* write carried bits, remaining of the last 32bit write of this line */ \
        if (!isXOutside(ldx) && (uint8_t*)d<fbb+ESPVGAX_FBBSIZE) { \
          uint32_t dv=*d; \
          dv=SWAP_UINT32(dv); \
          final_code \
          *d=SWAP_UINT32(dv); \
        } \
      } \
    } \
    src+=srcwstride/8; \
    dst+=ESPVGAX_WWIDTH; \
    srch--; \
    dy++; \
  } 
/*
 * implementation of BLIT with memory reading functions parametrized. these
 * functions will be pgm_read_* functions in the case of reading data from 
 * FLASH storage, and will be RAM reading deferencing operator (*) in case of
 * reading data from RAM.
 */
#define BLITMETHOD(reader32, reader16, reader8) \
  if (dx>=ESPVGAX_WIDTH || dy>=ESPVGAX_HEIGHT) \
    return; \
  if (srcwstride==0) \
    srcwstride=srcw+(srcw % 8 ? 8-(srcw%8) : 0); \
  if (dx%32==0 && srcw%32==0) { \
    /* dx and srcw are aligned to 32bits. move dword instead of bits */ \
    if (op==ESPVGAX_OP_OR) { \
      BLIT32(|=, reader32); \
    } else if (op==ESPVGAX_OP_XOR) { \
      BLIT32(^=, reader32); \
    } else { \
      BLIT32(=, reader32); \
    } \
  } else { \
    /* blit is unaligned, use 32bit framebuffer with a bunch of shifts */ \
    if (op==ESPVGAX_OP_OR) { \
      BLITUNALIGNED( \
        { dv|=(s>>dshift)|carry; }, \
        { dv|=carry; },  \
        reader32, reader16, reader8); \
    } else if (op==ESPVGAX_OP_XOR) { \
      BLITUNALIGNED( \
        { dv^=(s>>dshift)|carry; }, \
        { dv^=carry; },  \
        reader32, reader16, reader8); \
    } else { /*OP_SET*/ \
      BLITUNALIGNED( \
        { dv&=~(dmask|cmask); dv|=(s>>dshift)|carry; }, \
        { dv&=~cmask; dv|=carry; },  \
        reader32, reader16, reader8); \
    } \
  } 

void ESPVGAX::blit_P(ESPVGAX_PROGMEM uint8_t *src, int dx, int dy, int srcw, 
  int srch, int op, int srcwstride) {

  BLITMETHOD(pgm_read_dword, pgm_read_word, pgm_read_byte);
}
void ESPVGAX::blit(uint8_t *src, int dx, int dy, int srcw, int srch, int op, 
  int srcwstride) {
  
  BLITMETHOD(*(uint32_t*), *(uint16_t*), *(uint8_t*));
}
