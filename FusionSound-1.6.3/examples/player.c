#include <config.h>

#include <stdio.h>
#include <math.h>
#include <libgen.h>
#include <pthread.h>

#include <leck/label.h>
#include <lite/lite.h>
#include <leck/slider.h>
#include <leck/button.h>
#include <lite/window.h>

#include <fusionsound.h>

#include "loader.h"


static float values[5] = { 0.25f, 0.5f, 0.5f, 0, 0.5f };

static const char *channels[5] = { "Pitch", "Volume", "Pan", "Position", "Direction" };

static IFusionSound         *sound;
static IFusionSoundBuffer   *buffer;
static IFusionSoundPlayback *playback;
static int                   sample_length;


static DirectResult
create_playback( const char *filename )
{
     DirectResult ret;

     ret = FusionSoundCreate( &sound );
     if (ret) {
          FusionSoundError( "FusionSoundCreate() failed", ret );
          return ret;
     }

     buffer = load_sample( sound, filename );
     if (buffer) {
          FSBufferDescription desc;

          buffer->GetDescription( buffer, &desc );

          sample_length = desc.length;

          ret = buffer->CreatePlayback( buffer, &playback );
          if (ret) {
               FusionSoundError( "CreatePlayback() failed", ret );

               buffer->Release( buffer );
          }
          else {
               playback->Start( playback, 0, -1 );
               return DR_OK;
          }
     }

     sound->Release( sound );

     return DR_FAILURE;
}

static void
destroy_playback( void )
{
     playback->Release( playback );
     buffer->Release( buffer );
     sound->Release( sound );
}

static void
slider_update( LiteSlider *slider, float pos, void *ctx )
{
     long i = (long) ctx;

     values[i] = pos;

     switch (i) {
          case 0:
               playback->SetPitch( playback, pos * 4.0f );
               break;
          case 1:
               playback->SetVolume( playback, pos * 2.0f );
               break;
          case 2:
               playback->SetPan( playback, pos * 2.0f - 1.0f );
               break;
          case 3:
               playback->Start( playback, pos * sample_length, -1 );
               break;
          case 4:
               playback->SetDirection( playback, (pos < 0.5f) ? FSPD_BACKWARD : FSPD_FORWARD );
               break;
          default:
               break;
     }
}

static void
button_pressed( LiteButton *button, void *ctx )
{
     static bool stopped;

     if (stopped) {
          playback->Continue( playback );

          stopped = false;
     }
     else {
          playback->Stop( playback );

          stopped = true;
     }
}

int
main (int argc, char *argv[])
{
     int           i;
     DirectResult  ret;
     LiteLabel    *label[5];
     LiteSlider   *slider[5];
     LiteButton   *playbutton;
     LiteWindow   *window;
     DFBRectangle  rect;

     ret = DirectFBInit( &argc, &argv );
     if (ret)
          DirectFBErrorFatal( "DirectFBInit() failed", ret );

     ret = FusionSoundInit( &argc, &argv );
     if (ret)
          FusionSoundErrorFatal( "FusionSoundInit() failed", ret );

     if (argc != 2) {
          fprintf (stderr, "\nUsage: %s <filename>\n", argv[0]);
          return -1;
     }

     /* initialize */
     if (lite_open( &argc, &argv ))
          return 1;

     /* init sound */
     if (create_playback( argv[1] )) {
          lite_close();
          return 2;
     }

     rect.x = LITE_CENTER_HORIZONTALLY;
     rect.y = LITE_CENTER_VERTICALLY;
     rect.w = 330;
     rect.h = 195;

     /* create a window */
     ret = lite_new_window( NULL,
                            &rect,
                            DWCAPS_ALPHACHANNEL,
                            liteDefaultWindowTheme,
                            basename(argv[1]), &window );

     /* setup the labels */
     for (i=0; i<5; i++) {
          ret = lite_new_label( LITE_BOX(window),
                                &(DFBRectangle){ 10, 10 + i * 25, 85, 18 },
                                liteDefaultLabelTheme, 18, &label[i] );

          lite_set_label_text( label[i], channels[i] );
     }

     /* setup the sliders */
     for (i=0; i<5; i++) {
          ret = lite_new_slider( LITE_BOX(window),
                                 &(DFBRectangle){ 100, 10 + i * 25, 220, 20 },
                                 liteDefaultSliderTheme, &slider[i] );

          lite_set_slider_pos( slider[i], values[i] );

          lite_on_slider_update( slider[i], slider_update, (void*) (long)i );
     }

     /* setup the play/pause button */
     ret = lite_new_button( LITE_BOX(window),
                            &(DFBRectangle){ 150, 135, 50, 50 },
                            liteDefaultButtonTheme, &playbutton );
     lite_set_button_image( playbutton, LITE_BS_NORMAL, "stop.png" );
     lite_set_button_image( playbutton, LITE_BS_DISABLED, "stop_disabled.png" );
     lite_set_button_image( playbutton, LITE_BS_HILITE, "stop_highlighted.png" );
     lite_set_button_image( playbutton, LITE_BS_PRESSED, "stop_pressed.png" );
     lite_on_button_press( playbutton, button_pressed, window );

     /* show the window */
     lite_set_window_opacity( window, 0xff );

     /* run the event loop with a timeout */
     while (lite_window_event_loop( window, 20 ) == DR_TIMEOUT) {
          int position = 0;

          playback->GetStatus( playback, NULL, &position );

          lite_set_slider_pos( slider[3], position / (float) sample_length );
     }

     /* destroy the window with all this children and resources */
     lite_destroy_window( window );

     /* deinit sound */
     destroy_playback();

     /* deinitialize */
     lite_close();

     return 0;
}
