#version 410

uniform sampler2D tex;

out vec4 fragcolor;
in vec2 uv;
void main(){
   vec4 col = vec4(1);
   vec4 c = texture(tex, uv);
   float distAlphaMask = c.x;
   float offset = 0.003;
  // col = vec4(vec3(distAlphaMask < 0.01 ? 0.0 : 1.0), 1.0)
   col = vec4(0, 0, 0, 1 - smoothstep(0.0 + offset, 0.02 + offset, distAlphaMask) * 5.0);
   fragcolor = col;
}
