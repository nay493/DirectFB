#include "DFBTestSimple.h"



class ConfigTemplateWindow : public Insignia::ConfigTemplate<Insignia::ConfigSimple<ConfigTemplateWindow> > {
public:
     DFBDimension size;

     ConfigTemplateWindow()
     {
          addOption( new Insignia::ConfigOptionMinMax<DFBDimension>( "size", size, DFBDimension( 1, 1 ), DFBDimension( 512, 512 ) ) );
     }
};


class DFBTestSimpleWindow : public DFBTestSimple<ConfigTemplateWindow>
{
     INSIGNIA_TEST_CLASS( DFBTestSimpleWindow );

public:
     DFBTestSimpleWindow() : DFBTestSimple( "Window" ) {}

     virtual Result Run( const Insignia::ConfigSimple<ConfigTemplateWindow>& config ) {
          IDirectFBDisplayLayer layer = dfb.GetDisplayLayer( DLID_PRIMARY );

          DFBWindowDescription desc;

          desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_WIDTH | DWDESC_HEIGHT);
          desc.width  = config.size.w;
          desc.height = config.size.h;

          IDirectFBWindow window = layer.CreateWindow( desc );

          return RESULT_SUCCESS;
     }
};


INSIGNIA_TEST_STATICS( DFBTestSimpleWindow );

