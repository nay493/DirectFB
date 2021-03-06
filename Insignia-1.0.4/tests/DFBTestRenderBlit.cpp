#include "DFBTestRender.h"


class DFBTestRenderBlit : public DFBTestRender
{
     INSIGNIA_TEST_CLASS( DFBTestRenderBlit );

public:
     virtual void GetInfo( Info &info ) {
          info.name = "DFBTestRenderBlit";
     }

     virtual void Prepare( const Config& configuration ) {
     }

     virtual Result Run( const Config& configuration ) {

          const ConfigRender& config = dynamic_cast<const ConfigRender&>(configuration);

          DFBSurfaceDescription desc;

          desc.flags       = (DFBSurfaceDescriptionFlags)( DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS );
          desc.width       = config.width;
          desc.height      = config.height;
          desc.pixelformat = config.format;
          desc.caps        = config.caps;

          IDirectFBSurface temps[16];

          for (int n=0; !stopped(); n++) {
               IDirectFBSurface surface = m_dfb.CreateSurface( desc );

               if (config.clear)
                    surface.Clear();

               DFBRectangle rects[1000];

               for (int n=0; n<1000; n++) {
                    rects[n].x = n % (config.width - 20);
                    rects[n].y = n % (config.height - 20);
                    rects[n].w = 40;
                    rects[n].h = 40;
               }

               for (int i=0; i<config.fills; i++)
                    surface.FillRectangles( rects, 1000 );

               {
                    int n = rand() & 0xf;

                    if (temps[n])
                         surface.Blit( temps[n] );

                    temps[n] = surface;
               }
          }

          return RESULT_SUCCESS;
     }

     virtual void Cleanup( const Config& config ) {
     }
};


INSIGNIA_TEST_STATICS( DFBTestRenderBlit );

