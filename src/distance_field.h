void convert_to_distance_field(const u8 * g_image, int in_width, int in_height, i16 * o_image, int out_width, int out_height);
void convert_file_to_distance_field(const char * in_path, const char * out_path, int downscale);
void distance_field_convert_test();
float distance_to_field(image * df, vec2 p);
