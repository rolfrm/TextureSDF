#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iron/types.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iron/log.h>
#include <iron/mem.h> 
#include <iron/fileio.h>
#include <iron/time.h>
#include <iron/utils.h>
#include <iron/linmath.h>
#include <iron/math.h>
#include "gl_utils.h"
#include<dlfcn.h>
#include <sys/mman.h>
#include <icydb.h>
#include "string_vector.h"

void * module_open(const char * name){
  logd("Now opening '%s'\n", name);
  return (void *) 1;
  //void * module = dlopen(name, RTLD_NOW);
  //return module;
}

void * module_symbol(void * module, const char * sym){
  logd("Now loading sym '%s' from '%i'\n", sym, module);
  return (void *) 2;
  //return dlsym(module, sym);
}

char * module_error(){
  return dlerror();
}

typedef struct _module_data{
  icy_vector * vec;
}module_data;

typedef struct _engine_context{
  u32 context_id;
}engine_context;
engine_context current_context;

// get/set module data gets static data related to the module.
// uid is a unique ID that identifies the 
void * get_module_data(module_data * md){
  engine_context e = current_context;
  if(md->vec == NULL || icy_vector_count(md->vec) <= e.context_id)
    return NULL;
  
  return ((void **) icy_vector_lookup(md->vec, (icy_index){e.context_id}))[0];
}

void set_module_data(module_data * md, void * data){
  if(md->vec == NULL)
    md->vec = icy_vector_create(NULL, sizeof(void *));
  
  engine_context e = current_context;
  while(icy_vector_count(md->vec) <= e.context_id)
    icy_vector_alloc(md->vec);
  ((void **)icy_vector_lookup(md->vec, (icy_index){e.context_id}))[0] = data;
}




string_vector * get_loaded_modules(){
  static module_data _loaded_module;
  string_vector * vec = NULL;
  if(!(vec = get_module_data(&_loaded_module))){
    char buf[100];
    sprintf(buf, "imported_modules.%i", current_context.context_id);
    vec = string_vector_create(buf);
    set_module_data(&_loaded_module, vec);
  }
  return vec;
}


int load_module(const char * name){
  string_vector * loaded_modules = get_loaded_modules();
  static module_data _loaded_vec;
  string_vector * loaded_vec = NULL;
  if(!(loaded_vec = get_module_data(&_loaded_vec))){
    loaded_vec = string_vector_create(NULL);
    set_module_data(&_loaded_vec, loaded_vec);
  }
  
  size_t idx = 0;
  string_vector_index id;
  while(string_vector_iterate(loaded_vec, &id, 1, &idx)){
    char * _name = string_vector_lookup(loaded_vec, id);
    if(_name == NULL)continue;
    if(strcmp(_name, name) == 0){
      logd("Module '%s' is already loaded..\n", name);
      return -1;
    }
  }
  

  logd("Loading module '%s'\n", name);
  void * module = module_open(name);;
  if(module == NULL){
    loge("unable to load module '%s' %s\n", name, module_error());
    ASSERT(false);
    return -1;
  }

  void (* init_module)() = module_symbol(module, "init_module");
  if(init_module == NULL){
    loge("Unable to get 'init_module' from module '%s'\n", name);
    return -1;
  }

  var sidx = string_vector_alloc(loaded_vec, strlen(name) + 1);
  char * _name = string_vector_lookup(loaded_vec, sidx);
  strcpy(_name, name);
  
  _name = string_vector_lookup(loaded_modules, sidx);
  strcpy(_name, name);
  
  return 0;
}


void printError(const char * file, int line ){
  u32 err = glGetError();
  if(err != 0) logd("%s:%i : GL ERROR  %i\n", file, line, err);
}

#define PRINTERR() printError(__FILE__, __LINE__);

vec2 glfwGetNormalizedCursorPos(GLFWwindow * window){
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  int win_width, win_height;
  glfwGetWindowSize(window, &win_width, &win_height);
  return vec2_new((xpos / win_width * 2 - 1), -(ypos / win_height * 2 - 1));
}




module_data test_mod;
int main(){

  string_vector_test();
  engine_context ctx = {1};
  current_context = ctx;

  
  ASSERT(!get_module_data(&test_mod));
  void *test_data = alloc0(10);
  set_module_data(&test_mod, test_data);
  var newv = get_module_data(&test_mod);
  ASSERT(newv == test_data);

  current_context = (engine_context){2};

  {
    ASSERT(!get_module_data(&test_mod));
    void *test_data = alloc0(10);
    set_module_data(&test_mod, test_data);
    var newv = get_module_data(&test_mod);
    ASSERT(newv == test_data);
  }
  
  current_context = ctx;
  
  ASSERT(get_module_data(&test_mod) == test_data);
  ASSERT(0 == load_module("my_module.so"));
  ASSERT(-1 == load_module("my_module.so"));
  ASSERT(-1 == load_module("my_module.so"));
  ASSERT(0 == load_module("my_module2.so"));
  ASSERT(-1 == load_module("my_module2.so"));
  ASSERT(-1 == load_module("my_module.so"));
  
  return 0;
  /*
  vec3 light_source_center = vec3_new(0,0,0);
  
  glfwInit();
  
  glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true);

  GLFWwindow * win = glfwCreateWindow(512, 512, "Octree Rendering", NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwSwapInterval(2);  
  ASSERT(glewInit() == GLEW_OK);
  
  gl_init_debug_calls();
  glClearColor(0.0, 0.0, 0.0, 0.0);

  u32 verts_cnt = 32;
  
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

  
  u32 shader = compileShaderFromFile(GL_COMPUTE_SHADER, "comp.cs");
  
  u32 sar[] = {shader};
  u32 prog = linkGlProgram(sar, 1);

  glUseProgram(prog);
  
  glDispatchCompute(4,4,2);
  //glFlush();
  float * data = glMapBufferRange(GL_ARRAY_BUFFER, 0, verts_cnt * 2 * sizeof(float), GL_MAP_READ_BIT);
  ASSERT(data != NULL);
  for(u32 i =0 ; i < 32; i++)
    logd("%f\n", data[i * 2]);
  
  return 0;
  
  vec2 cursorPos = vec2_zero;
  void cursorMoved(GLFWwindow * win, double x, double y){

  }
  void keyfun(GLFWwindow* w,int k,int s,int a,int m){
    UNUSED(w);UNUSED(k);UNUSED(s);UNUSED(m);
    UNUSED(a);
  }
  
  void mbfun(GLFWwindow * w, int button, int action, int mods){

  }

  void scrollfun(GLFWwindow * w, double xscroll, double yscroll){

  }
  glfwSetScrollCallback(win, scrollfun);
  glfwSetKeyCallback(win, keyfun);
  glfwSetCursorPosCallback(win, cursorMoved);
  glfwSetMouseButtonCallback(win, mbfun);

  float t = 0;
  f128 current_time = timestampf();
  while(glfwWindowShouldClose(win) == false){
    u64 ts = timestamp();
    //render_zoom *= 1.01;
    t += 0.1;
    //t = 0;
    UNUSED(t);
    //render_zoom = 2;
    int up = glfwGetKey(win, GLFW_KEY_UP);
    int down = glfwGetKey(win, GLFW_KEY_DOWN);
    int right = glfwGetKey(win, GLFW_KEY_RIGHT);
    int left = glfwGetKey(win, GLFW_KEY_LEFT);
    int w = glfwGetKey(win, GLFW_KEY_W);
    int s = glfwGetKey(win, GLFW_KEY_S);
        
    int width = 0, height = 0;

    glfwGetWindowSize(win,&width, &height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glfwSwapBuffers(win);
    u64 ts2 = timestamp();
    var seconds_spent = ((double)(ts2 - ts) * 1e-6);
    
    logd("%f s \n", seconds_spent);
    if(seconds_spent < 0.016){
      iron_sleep(0.016 - seconds_spent);
    }

    glfwPollEvents();
    }*/
}

