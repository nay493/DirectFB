
//#define DIRECT_ENABLE_DEBUG


#include <++dfb.h>
#include "InsigniaUtil.h"

extern "C" {
#include <direct/debug.h>

#include <gfx/convert.h>
}

#include "DFBfx.h"


D_DEBUG_DOMAIN( Insignia_DFBfx, "Insignia/DFBfx", "Insignia DFBfx" );

/**********************************************************************************************************************/

DFBColor
DFBfx::BlitPixel( const CardState *state, DFBColor src, DFBColor dst )
{
     /* Scratch for blending stage. */
     DFBColor x;

     D_DEBUG_AT( Insignia_DFBfx, "%s( %02x %02x %02x %02x -> %02x %02x %02x %02x )\n", __FUNCTION__,
                 src.a, src.r, src.g, src.b,
                 dst.a, dst.r, dst.g, dst.b );

     /*
      * Input => short circuit to Output? (simple blits)
      */

     /* Without any flag the source is simply copied. */
     if (!state->blittingflags) {
          D_DEBUG_AT( Insignia_DFBfx, "  => NOFX = %02x %02x %02x %02x\n", src.a, src.r, src.g, src.b );
          return src;
     }

     /* Source color keying is the 2nd simplest operation. */
     if (state->blittingflags & DSBLIT_SRC_COLORKEY) {
          /* If the source matches the color key, keep the destination. */
          if (PIXEL_RGB32(src.r,src.g,src.b) == (int)state->src_colorkey)
               return dst;
     }

     /* Destination color keying already requires reading the destination. */
     if (state->blittingflags & DSBLIT_DST_COLORKEY) {
          /* If the destination does not match the color key, keep the destination. */
          if (PIXEL_RGB32(dst.r,dst.g,dst.b) != (int)state->dst_colorkey)
               return dst;
     }

     /*
      * Modulation stage
      */
     #define MODULATE(a,b)    do { (a) = (((int)(a) * ((int)(b) + 1)) >> 8); } while (0)

     /* Modulate source alpha value with global alpha factor? */
     if (state->blittingflags & DSBLIT_BLEND_COLORALPHA) {
          /* Combine with source alpha value... */
          if (state->blittingflags & DSBLIT_BLEND_ALPHACHANNEL)
               MODULATE( src.a, state->color.a );
          else
               /* ...or replace it. */
               src.a = state->color.a;
     }

     /* Modulate source colors with global color factors? */
     if (state->blittingflags & DSBLIT_COLORIZE) {
          MODULATE( src.r, state->color.r );
          MODULATE( src.g, state->color.g );
          MODULATE( src.b, state->color.b );
     }

     /*
      * Premultiplication stage
      */

     /* Premultiply source colors with (modulated) source alpha value? */
     if (state->blittingflags & DSBLIT_SRC_PREMULTIPLY) {
          MODULATE( src.r, src.a );
          MODULATE( src.g, src.a );
          MODULATE( src.b, src.a );
     }

     /* Premultiply source colors with global alpha factor only? */
     if (state->blittingflags & DSBLIT_SRC_PREMULTCOLOR) {
          MODULATE( src.r, state->color.a );
          MODULATE( src.g, state->color.a );
          MODULATE( src.b, state->color.a );
     }

     /* Premultiply destination colors with destination alpha value? */
     if (state->blittingflags & DSBLIT_DST_PREMULTIPLY) {
          MODULATE( dst.r, dst.a );
          MODULATE( dst.g, dst.a );
          MODULATE( dst.b, dst.a );
     }

     /*
      * XOR comes right before blending, after load, modulate and premultiply.
      */
     if (state->blittingflags & DSBLIT_XOR) {
          src.a ^= dst.a;
          src.r ^= dst.r;
          src.g ^= dst.g;
          src.b ^= dst.b;
     }

     /*
      * Blending stage
      */

     /* Initialize scratch with source values, modify the copy according to the source blend function.
        Could be done better by writing to the scratch only once after the calculation. */
     x = src;

     /* Blend scratch (source copy) and destination values accordingly. */
     if (state->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) {
          /* Apply the source blend function to the scratch. */
          switch (state->src_blend) {
               /* Sargb *= 0.0 */
               case DSBF_ZERO:
                    x.a = x.r = x.g = x.b = 0;
                    break;

               /* Sargb *= 1.0 */
               case DSBF_ONE:
                    break;

               /* Sargb *= Sargb */
               case DSBF_SRCCOLOR:
                    MODULATE( x.a, src.a );
                    MODULATE( x.r, src.r );
                    MODULATE( x.g, src.g );
                    MODULATE( x.b, src.b );
                    break;

               /* Sargb *= 1.0 - Sargb */
               case DSBF_INVSRCCOLOR:
                    MODULATE( x.a, src.a ^ 0xff );
                    MODULATE( x.r, src.r ^ 0xff );
                    MODULATE( x.g, src.g ^ 0xff );
                    MODULATE( x.b, src.b ^ 0xff );
                    break;

               /* Sargb *= Saaaa */
               case DSBF_SRCALPHA:
                    MODULATE( x.a, src.a );
                    MODULATE( x.r, src.a );
                    MODULATE( x.g, src.a );
                    MODULATE( x.b, src.a );
                    break;

               /* Sargb *= 1.0 - Saaaa */
               case DSBF_INVSRCALPHA:
                    MODULATE( x.a, src.a ^ 0xff );
                    MODULATE( x.r, src.a ^ 0xff );
                    MODULATE( x.g, src.a ^ 0xff );
                    MODULATE( x.b, src.a ^ 0xff );
                    break;

               /* Sargb *= Daaaa */
               case DSBF_DESTALPHA:
                    MODULATE( x.a, dst.a );
                    MODULATE( x.r, dst.a );
                    MODULATE( x.g, dst.a );
                    MODULATE( x.b, dst.a );
                    break;

               /* Sargb *= 1.0 - Daaaa */
               case DSBF_INVDESTALPHA:
                    MODULATE( x.a, dst.a ^ 0xff );
                    MODULATE( x.r, dst.a ^ 0xff );
                    MODULATE( x.g, dst.a ^ 0xff );
                    MODULATE( x.b, dst.a ^ 0xff );
                    break;

               /* Sargb *= Dargb */
               case DSBF_DESTCOLOR:
                    MODULATE( x.a, dst.a );
                    MODULATE( x.r, dst.r );
                    MODULATE( x.g, dst.g );
                    MODULATE( x.b, dst.b );
                    break;

               /* Sargb *= 1.0 - Dargb */
               case DSBF_INVDESTCOLOR:
                    MODULATE( x.a, dst.a ^ 0xff );
                    MODULATE( x.r, dst.r ^ 0xff );
                    MODULATE( x.g, dst.g ^ 0xff );
                    MODULATE( x.b, dst.b ^ 0xff );
                    break;

               /* Srgb *= min(Sa, 1-Da) */
               case DSBF_SRCALPHASAT:
                    MODULATE( x.r, MIN( src.a, dst.a ^ 0xff ) );
                    MODULATE( x.g, MIN( src.a, dst.a ^ 0xff ) );
                    MODULATE( x.b, MIN( src.a, dst.a ^ 0xff ) );
                    break;

               default:
                    D_BUG( "unknown blend function %d", state->src_blend );
                    break;
          }

          /* Apply the destination blend function. */
          switch (state->dst_blend) {
               /* Dargb *= 0.0 */
               case DSBF_ZERO:
                    dst.a = dst.r = dst.g = dst.b = 0;
                    break;

               /* Dargb *= 1.0 */
               case DSBF_ONE:
                    break;

               /* Dargb *= Sargb */
               case DSBF_SRCCOLOR:
                    MODULATE( dst.a, src.a );
                    MODULATE( dst.r, src.r );
                    MODULATE( dst.g, src.g );
                    MODULATE( dst.b, src.b );
                    break;

               /* Dargb *= 1.0 - Sargb */
               case DSBF_INVSRCCOLOR:
                    MODULATE( dst.a, src.a ^ 0xff );
                    MODULATE( dst.r, src.r ^ 0xff );
                    MODULATE( dst.g, src.g ^ 0xff );
                    MODULATE( dst.b, src.b ^ 0xff );
                    break;

               /* Dargb *= Saaaa */
               case DSBF_SRCALPHA:
                    MODULATE( dst.a, src.a );
                    MODULATE( dst.r, src.a );
                    MODULATE( dst.g, src.a );
                    MODULATE( dst.b, src.a );
                    break;

               /* Dargb *= 1.0 - Saaaa */
               case DSBF_INVSRCALPHA:
                    MODULATE( dst.a, src.a ^ 0xff );
                    MODULATE( dst.r, src.a ^ 0xff );
                    MODULATE( dst.g, src.a ^ 0xff );
                    MODULATE( dst.b, src.a ^ 0xff );
                    break;

               /* Dargb *= Daaaa */
               case DSBF_DESTALPHA:
                    MODULATE( dst.r, dst.a );
                    MODULATE( dst.g, dst.a );
                    MODULATE( dst.b, dst.a );
                    MODULATE( dst.a, dst.a ); //
                    break;

               /* Dargb *= 1.0 - Daaaa */
               case DSBF_INVDESTALPHA:
                    MODULATE( dst.r, dst.a ^ 0xff );
                    MODULATE( dst.g, dst.a ^ 0xff );
                    MODULATE( dst.b, dst.a ^ 0xff );
                    MODULATE( dst.a, dst.a ^ 0xff ); //
                    break;

               /* Dargb *= Dargb */
               case DSBF_DESTCOLOR:
                    MODULATE( dst.r, dst.r );
                    MODULATE( dst.g, dst.g );
                    MODULATE( dst.b, dst.b );
                    MODULATE( dst.a, dst.a ); //
                    break;

               /* Dargb *= 1.0 - Dargb */
               case DSBF_INVDESTCOLOR:
                    MODULATE( dst.r, dst.r ^ 0xff );
                    MODULATE( dst.g, dst.g ^ 0xff );
                    MODULATE( dst.b, dst.b ^ 0xff );
                    MODULATE( dst.a, dst.a ^ 0xff ); //
                    break;

               /* Drgb *= min(Sa, 1-Da) */
               case DSBF_SRCALPHASAT:
                    MODULATE( dst.r, MIN( src.a, dst.a ^ 0xff ) );
                    MODULATE( dst.g, MIN( src.a, dst.a ^ 0xff ) );
                    MODULATE( dst.b, MIN( src.a, dst.a ^ 0xff ) );
                    break;

               default:
                    D_BUG( "unknown blend function %d", state->dst_blend );
                    break;
          }

          /*
           * Add blended destination values to the scratch.
           */
          x.a = CLAMP( x.a + dst.a, 0x00, 0xff );
          x.r = CLAMP( x.r + dst.r, 0x00, 0xff );
          x.g = CLAMP( x.g + dst.g, 0x00, 0xff );
          x.b = CLAMP( x.b + dst.b, 0x00, 0xff );
     }

     /* Better not use the conversion from premultiplied to non-premultiplied! */
     if (state->blittingflags & DSBLIT_DEMULTIPLY) {
          x.r = ((int)x.r << 8) / ((int)x.a + 1);
          x.g = ((int)x.g << 8) / ((int)x.a + 1);
          x.b = ((int)x.b << 8) / ((int)x.a + 1);
     }

     #undef MODULATE

     /*
      * Output
      */
     return x;
}

/**********************************************************************************************************************/

void
DFBfx::DumpState( const CardState *state, bool blitting )
{
     if (blitting) {
          /* Show blitting flags being used. */
          Insignia::Util::DumpBlittingFlags( state->blittingflags );
     }
     else {
          /* Show drawing flags being used. */
          Insignia::Util::DumpDrawingFlags( state->drawingflags );
     }

     /* Blending needs source and destination blend function. */
     if (state->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) {
          Insignia::Util::DumpBlendFunctions( state->src_blend, state->dst_blend );
     }

     /* These require one or more global values via the color (for modulation). */
     if (state->blittingflags & (DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE))
          direct_log_printf( NULL, "  color:      %02x %02x %02x %02x\n",
                             state->color.a, state->color.r, state->color.g, state->color.b );

     /* Show source color key. */
     if (state->blittingflags & DSBLIT_SRC_COLORKEY)
          direct_log_printf( NULL, "  src_key:       %02x %02x %02x\n",
                             state->src_colorkey >> 16, (state->src_colorkey >> 8) & 0xff, state->src_colorkey & 0xff );

     /* Show destination color key. */
     if (state->blittingflags & DSBLIT_DST_COLORKEY)
          direct_log_printf( NULL, "  dst_key:       %02x %02x %02x\n",
                             state->dst_colorkey >> 16, (state->dst_colorkey >> 8) & 0xff, state->dst_colorkey & 0xff );
}

