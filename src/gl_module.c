#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include <iron/types.h>
#include <iron/log.h>
#include <iron/mem.h>
#include <iron/utils.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <icydb.h>

#include "neon.h"
#include "module.h"

#include "gl_module.h"

u32 gl_pre_render;
u32 gl_post_render;

typedef struct{
  GLFWwindow * window;

}gl_context_data;

void glfwError(int x, const char * err){
  ERROR("%i %s", x, err);
}



gl_context_data * get_or_init_context(){
  static module_data ctx_holder;
  static bool glfw_inited = false;
  gl_context_data * ctx = get_module_data(&ctx_holder);
  if(!ctx){
    logd("Creating a new context\n");
    ctx = alloc0(sizeof(ctx[0]));
    set_module_data(&ctx_holder, ctx);
    if(!glfw_inited){
      logd("Initialized glfw\n");
      if(!glfwInit())
	ERROR("Not able to initialize glfw.\n");
      
      glfwSetErrorCallback(glfwError);
    }
    glfw_inited = true;
      
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true);
    GLFWwindow * window = glfwCreateWindow(512, 512, "GL MODULE", NULL, NULL);
    logd("Created context : %i    %i    %i\n", ctx, ctx->window, window);
    if(window == NULL)
      ERROR("Unable to create window!\n");
    ASSERT(window != NULL);
    ctx->window = window;
    glfwMakeContextCurrent(window);
    glewInit();

  }
  return ctx;
}

void pre_render_scene(){
  gl_context_data * ctx = get_or_init_context();
  glfwMakeContextCurrent(ctx->window);
  int width, height;
  glfwGetWindowSize(ctx->window, &width, &height);
  glViewport(0, 0, width, height);
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  logd("Pre render\n");
}

void post_render_scene(){
  gl_context_data * ctx = get_or_init_context();
  glfwPollEvents();
  glfwSwapBuffers(ctx->window);
  logd("Post render\n");
}

void init_module(){
  gl_pre_render = intern_string("gl/pre_render");
  gl_post_render = intern_string("gl/post_render");

  register_event(gl_pre_render, 0, false);
  register_event(gl_post_render, gl_pre_render, true);

  register_method(gl_pre_render, pre_render_scene);
  register_method(gl_post_render, post_render_scene);
  
}

bool isKeyDown(int key){
  var ctx = get_or_init_context();
  int keystate = glfwGetKey(ctx->window, key);
  return keystate == GLFW_PRESS;
  
}
