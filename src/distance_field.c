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

void convert_to_distance_field(const u8 * g_image, int in_width, int in_height, i16 * o_image, int out_width, int out_height){
  u8 lookup(int x, int y){
    return g_image[x + y * in_width];
  }
  
  vec2 scale = vec2_new((float) out_width / (float)in_width, (float) out_height / (float)in_height);
  
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

	    float d = vec2_len(vec2_sub(p3, p2));
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

void distance_field_convert_test()
{

}
