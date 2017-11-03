#version 410

uniform vec2 offset;
uniform vec2 size;
out vec2 uv;
void main(){
  int x = gl_VertexID % 2;
  int y = gl_VertexID / 2;
  vec2 p = offset + size * vec2(x * 2 - 1, y * 2 - 1);
  gl_Position = vec4(p.x, p.y , 0, 1);
  uv = vec2(x,y);  
}
