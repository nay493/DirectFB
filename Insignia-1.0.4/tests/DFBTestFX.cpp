#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>

#include "DFBfx.h"




class DFBTestFX : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestFX );

private:
     IDirectFB m_dfb;

public:
     class ConfigFX : public Config {
     public:
          ConfigFX( DFBSurfacePixelFormat dst_format,
                    DFBSurfacePixelFormat src_format )
          {
               this->dst_format = dst_format;
               this->src_format = src_format;
          }

          DFBSurfacePixelFormat dst_format;
          DFBSurfacePixelFormat src_format;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          configs.push_back( new ConfigFX( DSPF_ARGB,     DSPF_ARGB ) );

          // Test needs to be fixed for non 8888 format
          // configs.push_back( new ConfigFX( DSPF_ARGB4444, DSPF_ARGB ) );
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestFX";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual Result Run( const Config& config ) {
          const ConfigFX& conf = dynamic_cast<const ConfigFX&>(config);

          return runFx( conf.dst_format, conf.src_format );
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }


private:
     Result runFx( DFBSurfacePixelFormat dst_format,
                   DFBSurfacePixelFormat src_format )
     {
          DFBSurfaceDescription   desc;
          IDirectFBSurface        dst_surface;
          IDirectFBSurface        src_surface;
          CardState               state;
          unsigned int            blittingflags[] = {
               DSBLIT_NOFX,
     
               DSBLIT_BLEND_ALPHACHANNEL,
               DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY,
               DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_BLEND_COLORALPHA,
     
               DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL,
               DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY,
               DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_BLEND_COLORALPHA,
          };
          unsigned int            drawingflags[] = {
               DSDRAW_NOFX,
     
               DSDRAW_BLEND,
               DSDRAW_BLEND | DSDRAW_SRC_PREMULTIPLY
          };
     
          desc.flags       = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH  |
                                                          DSDESC_HEIGHT |
                                                          DSDESC_PIXELFORMAT);
          desc.width       = 256;
          desc.height      = 256;
          desc.pixelformat = dst_format;
     
          dst_surface = m_dfb.CreateSurface( desc );
     
     
          desc.width       = 200;
          desc.height      = 200;
          desc.pixelformat = src_format;
     
          src_surface = m_dfb.CreateSurface( desc );
     
     
          direct_log_printf( NULL, "Performing Fx on %s, using an %s source...\n",
                             dfb_pixelformat_name(dst_format), dfb_pixelformat_name(src_format) );
     
          state.src_blend = DSBF_ONE;
          state.dst_blend = DSBF_INVSRCALPHA;
          state.color.a   = 0xc0;
          state.color.r   = 0xc0;
          state.color.g   = 0x20;
          state.color.b   = 0x40;
     
          for (int i=0; i<D_ARRAY_SIZE(blittingflags); i++) {
               DFBColor src_pixel = { 0x80, 0x80, 0x20, 0x00 };
               DFBColor dst_pixel = { 0xff, 0x40, 0x80, 0xff };
     
               state.blittingflags = (DFBSurfaceBlittingFlags) blittingflags[i];
     
               runFxTest( &state, src_surface, dst_surface, src_pixel, dst_pixel, DFXL_BLIT );
          }
     
          for (int i=0; i<D_ARRAY_SIZE(drawingflags); i++) {
               DFBColor src_pixel = { 0x80, 0x80, 0x20, 0x00 };
               DFBColor dst_pixel = { 0xff, 0x40, 0x80, 0xff };
     
               state.drawingflags = (DFBSurfaceDrawingFlags) drawingflags[i];
     
     
               FlagSet<DFBSurfaceBlittingFlags> blittingflags = DSBLIT_NOFX;
     
               if (state.drawingflags & DSDRAW_BLEND)
                    blittingflags |= DSBLIT_BLEND_ALPHACHANNEL;
     
               if (state.drawingflags & DSDRAW_DST_COLORKEY)
                    blittingflags |= DSBLIT_DST_COLORKEY;
     
               if (state.drawingflags & DSDRAW_SRC_PREMULTIPLY)
                    blittingflags |= DSBLIT_SRC_PREMULTIPLY;
     
               if (state.drawingflags & DSDRAW_DST_PREMULTIPLY)
                    blittingflags |= DSBLIT_DST_PREMULTIPLY;
     
               if (state.drawingflags & DSDRAW_DEMULTIPLY)
                    blittingflags |= DSBLIT_DEMULTIPLY;
     
               if (state.drawingflags & DSDRAW_XOR)
                    blittingflags |= DSBLIT_XOR;
     
     
               state.blittingflags = blittingflags;
     
               runFxTest( &state, src_surface, dst_surface, src_pixel, dst_pixel, DFXL_FILLRECTANGLE );
          }
     
     
          for (int i=0; i<0x800; i++) {
               DFBColor src_pixel = { 0x80, 0x80, 0x20, 0x00 };
               DFBColor dst_pixel = { 0xff, 0x40, 0x80, 0xff };
     
               state.blittingflags = (DFBSurfaceBlittingFlags) i;
     
               if (i == (i & (DSBLIT_BLEND_ALPHACHANNEL |
                              DSBLIT_BLEND_COLORALPHA   |
                              DSBLIT_COLORIZE           |
                              DSBLIT_SRC_PREMULTIPLY    |
                              DSBLIT_DST_PREMULTIPLY    |
                              DSBLIT_SRC_PREMULTCOLOR   |
                              DSBLIT_XOR)))
                    runFxTest( &state, src_surface, dst_surface, src_pixel, dst_pixel, DFXL_BLIT );
          }
     
          for (int i=0; i<11; i++) {
               for (int n=0; n<11; n++) {
                    DFBColor src_pixel = { 0x80, 0x80, 0x20, 0x00 };
                    DFBColor dst_pixel = { 0xff, 0x40, 0x80, 0xff };
          
                    state.blittingflags = (DFBSurfaceBlittingFlags) DSBLIT_BLEND_ALPHACHANNEL;
                    state.src_blend     = (DFBSurfaceBlendFunction) (i + 1);
                    state.dst_blend     = (DFBSurfaceBlendFunction) (n + 1);
          
                    runFxTest( &state, src_surface, dst_surface, src_pixel, dst_pixel, DFXL_BLIT );
               }
          }

          return RESULT_SUCCESS;
     }
     
     void runFxTest( const CardState     *state,
                     IDirectFBSurface     src,
                     IDirectFBSurface     dst,
                     DFBColor             src_pixel,
                     DFBColor             dst_pixel,
                     DFBAccelerationMask  func )
     {
          DFBColor            fx_result;
          DFBColor            hw_result = {0};
          DFBColor            sw_result;
          DFBAccelerationMask hw_mask;
          static int          test_counter = 1;
     
          direct_log_printf( NULL, "\n" );
          direct_log_printf( NULL, "Test #%d - %s\n", test_counter++, Insignia::Util::AccelerationMaskToString(func) );
     
          DFBfx::DumpState( state, DFB_BLITTING_FUNCTION( func ) );
     
          direct_log_printf( NULL, "\n" );
     
          /* Show original destination values. */
          direct_log_printf( NULL, "  dst_pixel:     %02x %02x %02x %02x\n", dst_pixel.a, dst_pixel.r, dst_pixel.g, dst_pixel.b );
     
          /* Show original source values. */
          direct_log_printf( NULL, "  src_pixel:     %02x %02x %02x %02x\n", src_pixel.a, src_pixel.r, src_pixel.g, src_pixel.b );
     
          direct_log_printf( NULL, "\n" );
     
          /* Do magic... */
          fx_result = DFBfx::BlitPixel( state, src_pixel, dst_pixel );
     
          dst.DisableAcceleration( DFXL_NONE );
     
          dst.SetBlittingFlags( state->blittingflags );
          dst.SetDrawingFlags( state->drawingflags );
          dst.SetColor( state->color.r, state->color.g, state->color.b, state->color.a );
          dst.SetDstBlendFunction( state->dst_blend );
          dst.SetSrcBlendFunction( state->src_blend );
          dst.SetDstColorKey( state->dst_colorkey >> 16, state->dst_colorkey >> 8, state->dst_colorkey );
          dst.SetSrcColorKey( state->src_colorkey >> 16, state->src_colorkey >> 8, state->src_colorkey );
     
          hw_mask = dst.GetAccelerationMask( src );
     
          if (DFB_BLITTING_FUNCTION( func )) {
               if (hw_mask & func) {
                    hw_result = fxBlitTest( src, dst, src_pixel, dst_pixel, func );
     
                    dst.DisableAcceleration( DFXL_ALL );
               }
     
               sw_result = fxBlitTest( src, dst, src_pixel, dst_pixel, func );
          }
          else {
               if (hw_mask & func) {
                    hw_result = fxDrawTest( dst, src_pixel, dst_pixel, func );
     
                    dst.DisableAcceleration( DFXL_ALL );
               }
     
               sw_result = fxDrawTest( dst, src_pixel, dst_pixel, func );
          }
     
          /* Show resulting values. */
          direct_log_printf( NULL, "  fx result:     %02x %02x %02x %02x\n", fx_result.a, fx_result.r, fx_result.g, fx_result.b );
          direct_log_printf( NULL, "%ssw result:     %02x %02x %02x %02x\n", DFB_COLOR_EQUAL( fx_result, sw_result ) ? "  " : "**",
                             sw_result.a, sw_result.r, sw_result.g, sw_result.b );
     
          if (hw_mask & func)
               direct_log_printf( NULL, "%shw result:     %02x %02x %02x %02x\n", DFB_COLOR_EQUAL( fx_result, hw_result ) ? "  " : "++",
                                  hw_result.a, hw_result.r, hw_result.g, hw_result.b );
     
          direct_log_printf( NULL, "\n" );
     }
     
     DFBColor fxBlitTest( IDirectFBSurface     src,
                          IDirectFBSurface     dst,
                          DFBColor             src_pixel,
                          DFBColor             dst_pixel,
                          DFBAccelerationMask  func )
     {
          DFBColor  result = {0};
          void     *data;
          int       pitch;
     
          src.Clear( src_pixel.r, src_pixel.g, src_pixel.b, src_pixel.a );
     
          dst.Clear( dst_pixel.r, dst_pixel.g, dst_pixel.b, dst_pixel.a );
     
          switch (func) {
               case DFXL_BLIT:
                    dst.Blit( src );
                    break;
     
               case DFXL_STRETCHBLIT:
                    dst.StretchBlit( src );
                    break;
     
               default:
                    direct_log_printf( NULL, "unsupported blitting function\n" );
          }

          dst.Lock( DSLF_READ, &data, &pitch );
     
          dfb_pixel_to_color( dst.GetPixelFormat(), *(unsigned long*)data, &result );
     
          dst.Unlock();
     
          return result;
     }
     
     DFBColor fxDrawTest( IDirectFBSurface     dst,
                          DFBColor             src_pixel,
                          DFBColor             dst_pixel,
                          DFBAccelerationMask  func )
     {
          DFBColor  result = {0};
          void     *data;
          int       pitch;
     
          dst.Clear( dst_pixel.r, dst_pixel.g, dst_pixel.b, dst_pixel.a );
     
          dst.SetColor( src_pixel.r, src_pixel.g, src_pixel.b, src_pixel.a );
     
          switch (func) {
               case DFXL_FILLRECTANGLE:
                    dst.FillRectangle( 0, 0, dst.GetWidth(), dst.GetHeight() );
                    break;
     
               default:
                    direct_log_printf( NULL, "unsupported drawing function\n" );
          }
     
          dst.Lock( DSLF_READ, &data, &pitch );
     
          dfb_pixel_to_color( dst.GetPixelFormat(), *(unsigned long*)data, &result );
     
          dst.Unlock();
     
          return result;
     }
};


INSIGNIA_TEST_STATICS( DFBTestFX );

