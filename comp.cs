#version 430

layout(std430, binding = 0) buffer dest
{
  float data[];
};

layout(local_size_x = 16, local_size_y = 1) in;

float trace_distance(vec2 p, vec2 direction){
  float distance = 0.0;
  while(distance < 500){
      float d = 10 - length(p);
    p = p + direction * d;
    distance += d;
    if(d < 1)
      return distance;
  }
  return 500;
}
#define M_PI 3.14
void main(){
  int index = int(gl_GlobalInvocationID.x);
  uint points = 32;
  float angle = 2.0 * M_PI * float(index) / float(points);
  data[index * 2 + 0] = angle;
  data[index * 2 + 1] = trace_distance(vec2(0,0), vec2(sin(angle),cos(angle)));
}
