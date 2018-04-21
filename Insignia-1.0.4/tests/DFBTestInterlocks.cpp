#include <++dfb.h>

#include <InsigniaTest.h>


class DFBTestInterlocks : public Insignia::Test
{
     INSIGNIA_TEST_CLASS( DFBTestInterlocks );

private:
     IDirectFB m_dfb;

public:
     class ConfigInterlocks : public Config {
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          configs.push_back( new ConfigInterlocks() );
     }

     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestInterlocks";
     }

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual Result Run( const Config& config ) {
          int                   i;
          IDirectFBSurface      surface;
          DFBSurfaceDescription desc;
          DFBSurfacePixelFormat format;

          desc.flags       = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT );
          desc.width       = 500;
          desc.height      = 500;
//          desc.pixelformat = DSPF_UNKNOWN;

          surface = m_dfb.CreateSurface( desc );

          format = surface.GetPixelFormat();

          for (i=0; i<1000; i++) {
               DFBColor  expect = { 0xff, (u8) ((i & 1) ? 0xff : 0x00), (u8) ((i & 2) ? 0xff : 0x00), (u8) ((i & 4) ? 0xff : 0x00) };
               DFBColor  result = {0};
               void     *data;
               int       pitch;

               surface.Clear( expect.r, expect.g, expect.b, expect.a );
               
               surface.Lock( DSLF_READ, &data, &pitch );

               data = (char*)data + (desc.width  - 10) * DFB_BYTES_PER_PIXEL(format);
               data = (char*)data + (desc.height - 10) * pitch;

               dfb_pixel_to_color( format, *(unsigned long*)data, &result );

               surface.Unlock();

               if (!DFB_COLOR_EQUAL( expect, result )) {
                    direct_log_printf( NULL, "  %02x %02x %02x %02x != %02x %02x %02x %02x !!!!!\n",
                                       result.a, result.r, result.g, result.b,
                                       expect.a, expect.r, expect.g, expect.b );

                    return RESULT_FAILURE;
               }
          }

          return RESULT_SUCCESS;
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


INSIGNIA_TEST_STATICS( DFBTestInterlocks );

