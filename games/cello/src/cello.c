#include <stdio.h>

#define GLEW_STATIC
#include "GL/glew.h"

#define NO_SDL_GLEXT
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_image.h"

#include "asset_manager.h"
#include "geometry.h"
#include "material.h"
#include "text_renderer.h"
#include "forward_renderer.h"
#include "deferred_renderer.h"
#include "font.h"
#include "timing.h"
#include "scripting.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

static render_model* cello;
static material* cello_mat;

static font* console_font;
static render_text* rt_framerate;
static render_text* rt_test_text;

static camera* cam;

void cello_init() {
  
  printf("Cello game init!\n");
  
  /* New Camera */
  
  cam = camera_new( v3(20.0, 0.0, 0.0) , v3_zero() );
  
  /* Renderer Setup */

#ifdef DEFERRED_RENDER
  deferred_renderer_init(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  deferred_renderer_set_camera(cam);
#else
  forward_renderer_init(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  forward_renderer_set_camera(cam);
#endif
  
  /* Script stuff */
  
  script* s = asset_get("./engine/scripts/hello_world.lua");
  scripting_run_script(s);  
  
  /* Get reference to the Cello */
  
  load_folder("/resources/cello/");
  
  cello = asset_get("/resources/cello/cello.obj");
  cello_mat = asset_get("/resources/cello/cello.mat");
  
  printf("Cello name: %s\n", cello->name);
    
  /* Put some text on the screen */
  
  console_font = asset_get("./engine/fonts/console_font.fnt");
  
  rt_framerate = render_text_new("hello", 10, console_font);
  rt_framerate->position = v2(-1.0,-1.0);
  rt_framerate->scale = v2(1.0,1.0);
  rt_framerate->color = v4(0,0,0,1);
  render_text_update(rt_framerate);
  
  rt_test_text = render_text_new("Corange v0.1", 512, console_font);
  rt_test_text->position = v2(-1.0,-0.95);
  rt_test_text->color = v4(0,0,1,1);
  render_text_update(rt_test_text);
  
  
}

void cello_update() {

}

void cello_event(SDL_Event event) {

  switch(event.type){
  case SDL_KEYUP:
        
    if (event.key.keysym.sym == SDLK_UP) { cam->position.y += 1; }
    if (event.key.keysym.sym == SDLK_DOWN) { cam->position.y -= 1; }
    
    if (event.key.keysym.sym == SDLK_LEFT) { cam->position.z += 1; }
    if (event.key.keysym.sym == SDLK_RIGHT) { cam->position.z -= 1; }

    if (event.key.keysym.sym == SDLK_LEFTBRACKET) { cam->position.x += 1; }
    if (event.key.keysym.sym == SDLK_RIGHTBRACKET) { cam->position.x -= 1;}
    
    break;
  }
    
}

void cello_render() {

#ifdef DEFERRED_RENDER
  deferred_renderer_begin();
  deferred_renderer_render_model(cello, cello_mat);
  deferred_renderer_end();
#else
  forward_renderer_begin();
  forward_renderer_render_model(cello, cello_mat);
  forward_renderer_end();
#endif

  /* Render text */
  
  render_text_update_string(rt_framerate, frame_rate_string());
  
  render_text_render(rt_framerate);
  render_text_render(rt_test_text);

}

void cello_finish() {

  camera_delete(cam);

  render_text_delete(rt_framerate);
  render_text_delete(rt_test_text);
  
  #ifdef DEFERRED_RENDER
    deferred_renderer_finish();
  #else
    forward_renderer_finish();
  #endif

  printf("Cello game finish!\n");

}