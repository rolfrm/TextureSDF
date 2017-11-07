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
#include <iron/linmath.h>

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
  u32 shader2;
  
  u32 texture;
  u32 shader;
  image * img;
  float aspect;
  
  u32 offset_loc, size_loc;
  u32 texture2;
  image * img2;
  float x, y;

  u32 df_compute_shader;
  u32 df_compute_target;
  u32 df_vert_cnt;
}dist_field_context;
#define POINTS 1024 * 32
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
      ctx->x = -0.0;
      ctx->y = -0.0;
      ctx->offset_loc = 0;
      ctx->size_loc = 1;
    }
    
    {
      u32 vs = compileShaderFromFile(GL_VERTEX_SHADER, "s2.vert");
      u32 fs = compileShaderFromFile(GL_FRAGMENT_SHADER, "s2.frag");
      ctx->shader2 = linkGlProgram(2, vs, fs);
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
    {
      u32 shader = compileShaderFromFile(GL_COMPUTE_SHADER, "comp.cs");
      u32 prog = linkGlProgram(1, shader);
      ctx->df_compute_shader = prog;
      u32 verts_cnt = POINTS;
      ctx->df_vert_cnt = verts_cnt;

 
      u32 buffer[1];
      glGenBuffers(1, buffer);
      size_t vsize = verts_cnt * sizeof(f32);
      float * fbuffer = alloc0(vsize * 2);
      for(u32 i = 0; i < verts_cnt ; i++){
	fbuffer[i * 2] = 0;
	fbuffer[i * 2 + 1] = 0;
      }

      glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * verts_cnt * 2, fbuffer, GL_STREAM_DRAW);

      glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[0]);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer[0]);
      ctx->df_compute_target = buffer[0];
      
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

  vec2 pos = vec2_new(ctx->x, ctx->y);

  pos = vec2_mul(vec2_scale(vec2_add(pos, vec2_new(1, 1)), 0.5f), vec2_new(ctx->img->width, ctx->img->height));
  
  float d3 = distance_to_field(ctx->img, vec2_sub(pos, vec2_new(0.5, 0.5)));
  //d3 = 2;
  // todo: billinear interpolation. add y axis change.  

  glBindTexture(GL_TEXTURE_2D, ctx->texture2);
  glUniform2f(ctx->offset_loc, ctx->x, ctx->y);
  glUniform2f(ctx->size_loc, 4.0 / 50.0, 4.0 / 50.0);// d3 / 50, d3 / 50.0f);
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
  
  pos = vec2_new(ctx->x * ctx->img->width * 0.5, ctx->y* ctx->img->height  * 0.5);
  
  bool collides = distance_field_collide(ctx->img2, ctx->img, vec2_sub(pos, vec2_new(0.5f, 0.5f)), 12.5f);
  vec2_print(pos); logd("%f ", d3);
  d3 = 2;
  logd("Collides? %i\n", collides);
  
  {
      glUseProgram(ctx->df_compute_shader);
      
      glBindTexture(GL_TEXTURE_2D, ctx->texture);
      glUniform2f(1, ctx->x, ctx->y);
      glUniform2f(2, 2.0f / (float)ctx->img->width, 2.0f / (float)ctx->img->height);
      glUniform1i(3, POINTS);
      //glUniform2f(2, 1.23, 1.25);
      glBindBuffer(GL_ARRAY_BUFFER, ctx->df_compute_target);
      
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, ctx->df_compute_target);
      glDispatchCompute(ctx->df_vert_cnt,1,1);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      
      float * data = glMapBufferRange(GL_ARRAY_BUFFER, 0, ctx->df_vert_cnt * 2 * sizeof(float), GL_MAP_READ_BIT);
      //ASSERT(data != NULL);
      //for(u32 i =0 ; i < ctx->df_vert_cnt; i++)
      //	logd("%f %f\n", data[i * 2], data[i * 2 + 1]);
      logd("Render distance field (%f %f) %f   (%f %f %f)\n",pos.x, pos.y, d3 , data[0], d3,data[0]/ d3);
      glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  
  
  glUseProgram(ctx->shader2);
  glBindBuffer(GL_ARRAY_BUFFER, ctx->df_compute_target);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
  glUniform2f(0, ctx->x, ctx->y);
  glUniform2f(1, 1, 1);
  glDrawArrays(GL_LINE_LOOP, 0, ctx->df_vert_cnt);
  

}

void init_module(){
  distance_field_convert_test();
  //convert_file_to_distance_field("./distance field test.png", "distance field out.png", 20);
  convert_file_to_distance_field("./alien2.png", "alien out.png", 5);
  gl_render_distance_field = intern_string("dist/render");
  register_method(gl_render_distance_field, render_distance_field);
  register_event(gl_render_distance_field, gl_post_render, false);
}
