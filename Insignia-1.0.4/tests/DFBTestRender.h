#ifndef __DFBTEST_RENDER___H__
#define __DFBTEST_RENDER___H__


#include <++dfb.h>

#include <InsigniaTest.h>
#include <InsigniaUtil.h>


class DFBTestRender : public Insignia::Test
{
protected:
     IDirectFB m_dfb;

public:
     struct ConfigRender : public Config {
          ConfigRender( unsigned int            num_processes,
                        int                     width,
                        int                     height,
                        DFBSurfacePixelFormat   format,
                        DFBSurfaceCapabilities  caps,
                        bool                    clear,
                        int                     fills,
                        const DFBDimension     &tsize )
               :
               width( width ),
               height( height ),
               format( format ),
               caps( caps ),
               clear( clear ),
               fills( fills ),
               tsize( tsize )
          {
               this->num_processes = num_processes;
          }

          /*

          develop basic scripting for tests 

          */

          virtual void Describe( Direct::String &string ) const {
               Config::Describe( string );

               string.PrintF( "  || Size: %4dx%4d  Format: %-8s  Caps: 0x%08x  Clear: %-3s  Fills: %3d  TSize: %4dx%4d",
                              width, height, dfb_pixelformat_name(format),
                              caps, clear ? "yes" : "no", fills, tsize.w, tsize.h );
          }

          int                    width;
          int                    height;
          DFBSurfacePixelFormat  format;
          DFBSurfaceCapabilities caps;
          bool                   clear;
          int                    fills;
          DFBDimension           tsize;
     };

     virtual void GetConfigs( vector<Config*> &configs ) {
          int                   fills[]   = { 1,/* 2, 5, 10,*/ 20, 50};//, 150, 500 };
          DFBSurfacePixelFormat formats[] = { DSPF_ARGB/*, DSPF_ARGB4444, DSPF_AYUV, DSPF_LUT8*/ };
          DFBDimension_C        sizes[]   = {
               { 100, 100 },
               { 400, 400 },
//               { 800, 800 },
          };
          DFBDimension_C        tsizes[]  = {
               {   1,   1 },
               {   5,   5 },
//               {  10,  10 },
               {  20,  20 },
//               {  40,  40 },
               {  70,  70 }
          };

          for (bool clear=false; ; clear=true) {
               for (unsigned int num_processes=1; num_processes<32; num_processes+=5) {
                    for (int d=0; d<D_ARRAY_SIZE(fills); d++) {
                         for (int i=0; i<D_ARRAY_SIZE(formats); i++) {
                              for (int s=0; s<D_ARRAY_SIZE(sizes); s++) {
                                   for (int t=0; t<D_ARRAY_SIZE(tsizes); t++) {
                                        configs.push_back( new ConfigRender( num_processes,
                                                                             sizes[s].w, sizes[s].h, formats[i], DSCAPS_NONE,
                                                                             clear, fills[d], tsizes[t] ));
                                   }
                              }
                         }
                    }
               }

               if (clear)
                    break;
          }
     }

     virtual void GetInfo( Info &info ) = 0;

     virtual void Initialize( unsigned int process_index ) {
          m_dfb = DirectFB::Create();
     }

     virtual void Prepare( const Config& configuration ) {
     }

     virtual Result Run( const Config& configuration ) = 0;

     virtual void Cleanup( const Config& config ) {
     }

     virtual void Finalize() {
          m_dfb = NULL;
     }
};


#endif

