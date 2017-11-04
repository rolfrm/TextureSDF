#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iron/types.h>
#include <iron/log.h>
#include <iron/mem.h> 
#include <iron/fileio.h>
#include <iron/time.h>
#include <iron/utils.h>
#include <iron/math.h>
#include <iron/image.h>
#include <iron/linmath.h>
#include "distance_field.h"
void convert_to_distance_field(const u8 * g_image, int in_width, int in_height, i16 * o_image, int out_width, int out_height){
  u8 lookup(int x, int y){
    return g_image[x + y * in_width];
  }
  
  vec2 scale = vec2_new((float) out_width / (float)in_width, (float) out_height / (float)in_height);
  //vec2 scale2 = vec2_new(in_width, in_height);
  for(int _y = 0; _y < out_height; _y++){
    //    logd("Y: %i\n", _y);
    for(int _x = 0; _x < out_width; _x++){
      // lets just brute force
      
      vec2 p = vec2_new(_x, _y);
      vec2 p2 = vec2_div(p, scale);

      u8 value = lookup((int)p2.x, (int)p2.y);
      
      float mind = 10000;
      for( int __y = 0; __y < in_height; __y++){
	for( int __x = 0; __x < in_width; __x++){
	  u8 _value = lookup(__x, __y);
	  if(_value != value){

	    vec2 p3 = vec2_new(__x, __y);

	    float d = vec2_len(vec2_mul(vec2_sub(p3, p2), scale));
	    mind = MIN(d, mind);
	  }	  
	}
      }

      mind *= 100;
      if(mind > 30000)
	mind = 30000;
      o_image[_x + _y * out_width] = (value ? 1 : -1) * mind;
      logd("%i %i %i\n", _x, _y, o_image[_x + _y * out_width]);
    }
  }  
}

void convert_file_to_distance_field(const char * in_path, const char * out_path, int downscale){
  image * img = image_load(in_path);
  image_remove_alpha(img);
  logd("Image: %i %i %i\n", img->width, img->height, img->type);
  image * out_img = image_new(img->width / downscale, img->height / downscale, PIXEL_GRAY_ALPHA);
  convert_to_distance_field(img->buffer, img->width, img->height, (i16 *) out_img->buffer, out_img->width, out_img->height);
  i16 * px = out_img->buffer;
  //UN
  logd("%i %i \n", px[0], px[1]);
  image_save(out_img, out_path);
  out_img = image_load(out_path);
  i16 * px2 = out_img->buffer;
  if(px[0] != px2[0])
    ERROR("Invalid result\n");  
}

/*
float distance_to_field_nearest(image * df, vec2 p){
  vec2 c = p;
  if(c.x < 0)
    c.x = 0;
  
  if(c.x >= df->width)
    c.x = df->width - 1;
  
  if(c.y < 0)
    c.y = 0;

  if(c.y > df->height - 1)
    c.y = df->height - 1;
  
  i16 * img = df->buffer;
  int index = ((int)c.x) + ((int) c.y) * df->width;
  float d = img[index];
  d = d * 0.01;

  return vec2_len(vec2_sub(p, c)) + d;
  }*/

float distance_to_field(image * df, vec2 p){
  vec2 c = p;
  bool onex = false; // skip interpolate x-axis?
  bool oney = false; // skip interpolate y-axis?
  if(c.x < 0){
    c.x = 0;
    onex = true;
  }
  
  if(c.x >= df->width){
    c.x = df->width - 1;
    onex = true;
  }
  
  if(c.y < 0){
    oney = true;
    c.y = 0;
  }

  if(c.y > df->height - 1){
    c.y = df->height - 1;
    oney = true;
  }
  
  i16 * buffer = df->buffer;

  int idx_x = (int)c.x;
  int idx_y = (int)c.y;
  if(idx_x == df->width - 1)
    onex = true;
  
  if(idx_y == df->height -1)
    oney = true;
  

  int idx = idx_x + idx_y * df->width;
  float d1x, d2x;
  
  i16 d1 = buffer[idx];

  if(onex){
    d1x = d1;
  }else{
    i16 d2 = buffer[idx + 1];
    float w = c.x - floorf(c.x);
    float d3 =(float)(d1 * (1 - w) + d2 * w);
    d1x = d3;
  }
  
  float d = 0.0;
  if(oney) {
    d = d1x;
  }else{
    float d1 = (float) buffer[idx + df->width];
    if(onex){
      d2x = d1;
    }else{
      
      float d2 = (float) buffer[idx + 1 + df->width];
      float w = c.x - floorf(c.x);
      float d3 = d1 * (1 - w) + d2 * w;
      d2x = d3;
    }
    float w = c.y - floorf(c.y);
    d =((float)(d1x * (1 - w) + d2x * w));    
  }
  return vec2_len(vec2_sub(p, c)) + d * 0.01;
}

void distance_field_convert_test()
{
  image * img = image_load("./alien out.png");

  vec2 positions[] = {
    vec2_new(7.703,7.703),
    vec2_new(7.7,7.7),
    vec2_new(7.5,7.5),vec2_new(7,7),
    vec2_new(6,6), vec2_new(5,5),
    vec2_new(0,0), vec2_new(-1,-1),
    vec2_new(-5, -5), vec2_new(-5, 0),
    vec2_new(64, 64)
  };
  logd("Image: (%i %i)\n", img->width, img->height);
  for(u32 i = 0; i < array_count(positions); i++){
    float f = distance_to_field(img, positions[i]);
    vec2_print(positions[i]);
    logd("   %f\n", f);
  }

}
