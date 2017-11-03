#version 430

layout(location = 0) in vec2 loc;
layout(location = 0) uniform vec2 position;
layout(location = 1) uniform vec2 size;
void main(){
  gl_Position = vec4(position + loc * size, 0 ,1);  
}
