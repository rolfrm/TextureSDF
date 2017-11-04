#version 430

layout(std430, binding = 0) buffer dest
{
  float data[];
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform vec2 start;
layout(location = 2) uniform vec2 scale;
layout(location = 3) uniform int points;
#define M_PI 3.14159265359
void main(){
    int index = int(gl_GlobalInvocationID.x);// + int(gl_GlobalInvocationID.y) * 8 + int(gl_GlobalInvocationID.z) * 64;

  float angle = 2.0 * M_PI * float(index) / float(points);
  
  float distance = 0.0;
  vec2 p = start;
  vec2 direction = vec2(sin(angle),cos(angle));
  
  while(distance < 10){
      float x = texture(tex, p * 0.5 - vec2(0.5)).x * 0.05;
    
    x = x / scale.x;
    p = p + direction * x;
    distance += x;
    
    if(x <= 0.0015)
      break;
  }
  //data[0] = texture(tex, start).x;
  //data[1] = texture(tex, start).x;
  direction = direction * distance;
  data[index * 2 + 0] = direction.x;
  data[index * 2 + 1] = direction.y;
  
}
