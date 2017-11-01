#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <iron/types.h>
#include <iron/log.h>
#include <iron/mem.h>
#include <iron/image.h>
#include <iron/utils.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <icydb.h>

#include "neon.h"
#include "module.h"

#include "gl_module.h"
#include "gl_utils.h"
#include "distance_field.h"
u32 gl_render_distance_field;

u32 loadImagedf(i16 * pixels, u32 width, u32 height){
  
  GLuint tex = 0;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_LUMINANCE, GL_SHORT, pixels);
  return tex;
}

typedef struct{
  u32 texture;
  u32 shader;
  image * img;
  
  u32 offset_loc, size_loc;
  u32 texture2;
  image * img2;
  float x, y;  
}dist_field_context;

void render_distance_field(){
  static module_data cx;
  dist_field_context * ctx = get_module_data(&cx);
  if(ctx == NULL){
    ctx = alloc0(sizeof(ctx[0]));
    set_module_data(&cx, ctx);

    { // load distance field texture
      image * img = image_load("./distance field out.png");
      //i16 * px = img->buffer;
      //ERROR(":: %i %i \n", px[0], px[1]);
      u32 texture = loadImagedf(img->buffer, img->width, img->height);
      //image_delete(&img);
      ctx->texture = texture;
      ctx->img = img;
      ctx->x = -0.5;
      ctx->y = -0.5;
      ctx->offset_loc = 0;
      ctx->size_loc = 1;
    }
    { // load distance field texture
      image * img = image_load("./alien out.png");
      u32 texture = loadImagedf(img->buffer, img->width, img->height);
      ctx->texture2 = texture;
      ctx->img2 = img;
    }
        
    {
      u32 vs = compileShaderFromFile(GL_VERTEX_SHADER, "s.vert");
      u32 fs = compileShaderFromFile(GL_FRAGMENT_SHADER, "s.frag");
      ctx->shader = linkGlProgram(2, vs, fs);
    }
  }
  ///ERROR("FINISHED HERE\n");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glClearColor(1,0,0,1);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(ctx->shader);
  glUniform2f(ctx->offset_loc, 0, 0);
  glUniform2f(ctx->size_loc, 1.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D, ctx->texture);
  glDrawArrays(GL_TRIANGLE_STRIP,0, 4);

  


  f32 locx = (ctx->x + 1) * ctx->img->width * 0.5;
  f32 locy = (ctx->y + 1) * ctx->img->height * 0.5;
  
  int idx = (((int)locx) + ((int)locy) * ctx->img->width);

  float d1x, d2x;
  {
    i16 d = ((i16 *) ctx->img->buffer)[idx];
    i16 d2 = ((i16 *) ctx->img->buffer)[idx + 1];
    float w = locx - floorf(locx);
    float d3 =(float)(d * (1 - w) + d2 * w);
    d1x = d3;
  }

  {
    i16 d = ((i16 *) ctx->img->buffer)[idx + ctx->img->width];
    i16 d2 = ((i16 *) ctx->img->buffer)[idx + 1 + ctx->img->width];
    float w = locx - floorf(locx);
    float d3 = (float)(d * (1 - w) + d2 * w);
    d2x = d3;    
  }
  
  float w = locy - floorf(locy);
  float d3 =((float)(d1x * (1 - w) + d2x * w) * 0.01) * 0.2;
  
  // todo: billinear interpolation. add y axis change.  

  glBindTexture(GL_TEXTURE_2D, ctx->texture2);
  glUniform2f(ctx->offset_loc, ctx->x, ctx->y);
  glUniform2f(ctx->size_loc, d3 / 160.0f, d3 / 160.0f);
  glBindTexture(GL_TEXTURE_2D, ctx->texture2);
  glDrawArrays(GL_TRIANGLE_STRIP,0, 4);


  if(isKeyDown(GLFW_KEY_UP)){
    ctx->y += 0.001;
  }
  if(isKeyDown(GLFW_KEY_DOWN)){
    ctx->y -= 0.001;
  }

  if(isKeyDown(GLFW_KEY_LEFT)){
    ctx->x -= 0.001;
  }
  if(isKeyDown(GLFW_KEY_RIGHT)){
    ctx->x += 0.001;
  }  
  
  logd("Render distance field (%f %f) %f\n",locx, locy, d3);
}

void init_module(){
  //convert_file_to_distance_field("./distance field test.png", "distance field out.png");
  //convert_file_to_distance_field("./alien.png", "alien out.png", 3);
  //distance_field_convert_test();
  gl_render_distance_field = intern_string("dist/render");
  register_method(gl_render_distance_field, render_distance_field);
  register_event(gl_render_distance_field, gl_post_render, false);
}
