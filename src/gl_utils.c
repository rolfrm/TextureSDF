#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <iron/types.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iron/log.h>
#include <iron/mem.h>
#include <iron/fileio.h>
#include <iron/utils.h>
#include "stb_image.h"
#include "gl_utils.h"

u32 loadImage(u8 * pixels, u32 width, u32 height, u32 channels){
  
  GLuint tex = 0;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  u32 intype = 0;
  switch(channels){
  case 1:
    intype = GL_RED;
    break;
  case 2:
    intype = GL_RG;
    break;
  case 3:
    intype = GL_RGB;
    break;
  case 4:
    intype = GL_RGBA;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }
u32 pixels_channels = 0;
  switch(channels){
  case 1:
    pixels_channels = GL_RED;
    break;
  case 2:
    pixels_channels = GL_RG;
    break;
  case 3:
    pixels_channels = GL_RGB;
    break;
  case 4:
    pixels_channels = GL_RGBA;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }  
  
  glTexImage2D(GL_TEXTURE_2D, 0, intype, width, height, 0, pixels_channels, GL_UNSIGNED_BYTE, pixels);
  return tex;
}
/*
u32 loadImagef(float * pixels, u32 width, u32 height, u32 channels){
  return loadImagefx(pixels, width, height, channels, GL_LINEAR);
}

u32 loadImagefx(float * pixels, u32 width, u32 height, u32 channels, int interp){
  
  GLuint tex = 0;
  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp);
  u32 intype = 0;
  switch(channels){
  case 1:
    intype = GL_R32F;
    break;
  case 2:
    intype = GL_RG32F;
    break;
  case 3:
    intype = GL_RGB32F;
    break;
  case 4:
    intype = GL_RGBA32F;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }
  u32 pixels_channels = 0;
  switch(channels){
  case 1:
    pixels_channels = GL_RED;
    break;
  case 2:
    pixels_channels = GL_RG;
    break;
  case 3:
    pixels_channels = GL_RGB;
    break;
  case 4:
    pixels_channels = GL_RGBA;
    break;
  default:
    ERROR("Invalid number of channels %i", channels);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, intype, width, height, 0, pixels_channels, GL_FLOAT, pixels);
  //glGenerateMipmap(GL_TEXTURE_2D);
  return tex;
}
*/


u32 compileShader(int program, const char * code){
  u32 ss = glCreateShader(program);
  i32 l = strlen(code);
  glShaderSource(ss, 1, (void *) &code, &l); 
  glCompileShader(ss);
  int compileStatus = 0;	
  glGetShaderiv(ss, GL_COMPILE_STATUS, &compileStatus);
  if(compileStatus == 0){
    logd("Error during shader compilation:");
    int loglen = 0;
    glGetShaderiv(ss, GL_INFO_LOG_LENGTH, &loglen);
    char * buffer = alloc0(loglen);
    glGetShaderInfoLog(ss, loglen, NULL, buffer);

    logd("%s", buffer);
    dealloc(buffer);
  } else{
    logd("--- Success\n");
  }
  return ss;
}

u32 compileShaderFromFile(u32 gl_prog_type, const char * filepath){
  logd("Compiling shader '%s'\n", filepath);
  char * vcode = read_file_to_string(filepath);
  u32 vs = compileShader(gl_prog_type, vcode);
  dealloc(vcode);
  return vs;
}

u32 createShaderFromFiles(const char * vs_path, const char * fs_path){
  u32 vs = compileShaderFromFile(GL_VERTEX_SHADER, vs_path);
  u32 fs = compileShaderFromFile(GL_FRAGMENT_SHADER, fs_path);
  u32 result = linkGlProgram(2, vs, fs);
  return result;
}

u32 linkGlProgram(u32 shader_cnt, ...){
  u32 shaders[shader_cnt];

  { // read arglist
    va_list arglist;
    va_start (arglist, shader_cnt);
    for(u32 i = 0; i < shader_cnt; i++)
      shaders[i] = va_arg(arglist, u32);
    
    va_end(arglist);
  }


  u32 prog = glCreateProgram();
  for(u32 i = 0; i < shader_cnt; i++){
    glAttachShader(prog, shaders[i]);
  }
  glLinkProgram(prog);

  int program_linked;
 
  glGetProgramiv(prog, GL_LINK_STATUS, &program_linked);

  if (program_linked != GL_TRUE)
    {
      i32 loglen;
      glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &loglen);      
      char * buffer = alloc0(loglen * sizeof(char) + 1);
      glGetProgramInfoLog(prog, 1024, &loglen, buffer);
      logd("%s", buffer);
      dealloc(buffer);
    // Write the error to a log
    }
  
  return prog;
}

void debugglcalls(GLenum source,
		  GLenum type,
		  GLuint id,
		  GLenum severity,
		  GLsizei length,
		  const GLchar *message,
		  const void *userParam){
  UNUSED(length);
  UNUSED(userParam);

  switch(type){
  case GL_DEBUG_TYPE_ERROR:
    logd("%i %i %i i\n", source, type, id, severity);
    ERROR("%s\n", message);
    ASSERT(false);
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
  case GL_DEBUG_TYPE_PORTABILITY:
  case GL_DEBUG_TYPE_OTHER:
    return;
  case GL_DEBUG_TYPE_PERFORMANCE:
    break;
  default:
    break;
  }
  logd("%i %i %i i\n", source, type, id, severity);
  logd("%s\n", message);
}

void gl_init_debug_calls(){
  glDebugMessageCallback(debugglcalls, NULL);
}
