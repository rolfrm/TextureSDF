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
