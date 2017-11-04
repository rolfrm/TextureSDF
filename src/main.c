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
#include <dlfcn.h>
#include <sys/mman.h>
#include <icydb.h>
#include "string_vector.h"
#include "module.h"
#include "event_table.h"
#include "event_table.c"
#include "event_table_weight.h"
#include "event_table_weight.c"

#include "intern_string_vector_1.h"
#include "intern_string_vector_2.h"

typedef struct{
  u128 X;
  u128 Y;
}u256;

#include "intern_string_vector_3.h"
#include "intern_string_vector_1.c"
#include "intern_string_vector_2.c"
#include "intern_string_vector_3.c"

#include "neon.h"
#include "method_table.h"
#include "method_table.c"

event_table_weight * get_event_table_weight(){
  static module_data event_table_weight_data;
  event_table_weight * table_weight;
  if(!(table_weight = get_module_data(&event_table_weight_data))){
    char buf[100];
    sprintf(buf, "registered_events.weights.%i", current_context.context_id);
    table_weight = event_table_weight_create(buf);
    set_module_data(&event_table_weight_data, table_weight);
  }
  return table_weight;
}
u32 new_unique_id(){
  static module_data id_data;
  icy_mem * data_area;
  if(NULL == (data_area = get_module_data(&id_data))){
    char buf[100];
    sprintf(buf, "intern_string_ids.%i", current_context.context_id);
    data_area = icy_mem_create(buf);
    set_module_data(&id_data, data_area);
  }
  u32 * ptr = data_area->ptr;
  *ptr += 1;
  return *ptr;
}

u32 intern_string(const char * string){
  static module_data isvd_1, isvd_2, isvd_3;
  intern_string_vector_1 * isv_1 = NULL;
  intern_string_vector_2 * isv_2 = NULL;
  intern_string_vector_3 * isv_3 = NULL;
  if(!(isv_1 = get_module_data(&isvd_1))){
    char buf[100];
    sprintf(buf, "intern_string_1.%i", current_context.context_id);
    isv_1 = intern_string_vector_1_create(buf);
    set_module_data(&isvd_1, isv_1);
    
    sprintf(buf, "intern_string_2.%i", current_context.context_id);
    isv_2 = intern_string_vector_2_create(buf);
    set_module_data(&isvd_2, isv_2);
    
    sprintf(buf, "intern_string_3.%i", current_context.context_id);
    isv_3 = intern_string_vector_3_create(buf);
    set_module_data(&isvd_3, isv_3);
  }else{
    isv_2 = get_module_data(&isvd_2);
    isv_3 = get_module_data(&isvd_3);
  }
  u32 length = strlen(string);
  if(length < sizeof(u64)){
    u64 v = 0;
    memcpy(&v, string, length);
    u32 key;
    if(!intern_string_vector_1_try_get(isv_1, &v, &key)){
      key = new_unique_id();
      intern_string_vector_1_set(isv_1, v, key);
    }
    return key;
  }else if(length < sizeof(u128)){
    u128 v = 0;
    memcpy(&v, string, length);
    u32 key;
    if(!intern_string_vector_2_try_get(isv_2, &v, &key)){
      key = new_unique_id();
      intern_string_vector_2_set(isv_2, v, key);
    }
    return key;
  }else if(length < sizeof(u256)){
    u256 v = {0};
    memcpy(&v, string, length);
    u32 key;
    if(!intern_string_vector_3_try_get(isv_3, &v, &key)){
      key = new_unique_id();
      intern_string_vector_3_set(isv_3, v, key);
    }
    return key;
  }else{
    ERROR("Unable to create key for %s, length %i", string, length);
    return 0;
  }
}

method_table * get_method_table(){
  static module_data method_table_data;
  method_table * table = get_module_data(&method_table_data);
  if(table == NULL){
    table = method_table_create(NULL);
    set_module_data(&method_table_data, table);
  }
  return table;
}

void register_method(u32 method, method_ptr f){
  let table = get_method_table();
  method_table_set(table, method, f);
}

method_ptr get_registered_method(u32 method){
  let table = get_method_table();
  method_ptr out = NULL;
  if(!method_table_try_get(table, &method, &out))
    return NULL;
  return out;
}

void register_event(u32 method, u32 relative, bool after){
  static module_data event_table_data;
  
  event_table * table;
  event_table_weight * table_weight = get_event_table_weight();
  if(!(table = get_module_data(&event_table_data))){
    char buf[100];
    sprintf(buf, "registered_events.%i", current_context.context_id);
    table = event_table_create(buf);
    set_module_data(&event_table_data, table);
  }


  f64 key, prevkey;
  f64 scale, _scale;
  if(method == 0){
    event_table_set(table, 0, 0.0, 1.0);
    event_table_weight_set(table_weight, 0.0, 0);
    return;
  }
  
  ASSERT(event_table_try_get(table, &relative, &prevkey, &scale));
  if(event_table_try_get(table, &method, &key, &_scale)){
    return; // already registered.
  }

  while(event_table_weight_try_get(table_weight, &prevkey, NULL)){
    if(after){

      prevkey = prevkey + scale;
      scale *= 0.5;
      after = false;
    }else{
      prevkey = prevkey - scale;
      scale *= 0.5;
      after = true;
    }
  }

  event_table_weight_set(table_weight, prevkey, method);
  event_table_set(table, method, prevkey, scale);
}

void test_register_event(){

  register_event(0,0,false);
  register_event(1,0,false);
  register_event(2,0,false);
  register_event(3,0, true);
  register_event(4,1, true);
  register_event(5,1, false);
  register_event(6,0, true);
  register_event(7,0, true);
  register_event(8,0, true);
  bool m1_called = false, m2_called = false, m3_called = false;
  void m1(){
    logd("M1 Called\n");
    m1_called = true;
  }

  void m2(){
    logd("M2 Called\n");
    m2_called = true;
  }

  void m3(){
    logd("M3 Called\n");
    m3_called = true;
  }
  
  register_method(3, m1);
  register_method(5, m2);
  register_method(6, m3);
  
  event_table_weight * table_weight = get_event_table_weight();
  event_table_weight_print(table_weight);
  logd("\n");
  for(u32 i = 0; i < table_weight->count; i++){
    logd("%f %i\n", table_weight->key[i + 1], table_weight->method[i + 1]);
    method_ptr p = get_registered_method(table_weight->method[i + 1]);
    if(p == NULL) continue;
    p();
  }
  logd("\n");
  ASSERT(m1_called && m2_called && m3_called);
}

void intern_string_test(){
  u32 x1 = intern_string("X");
  u32 x2 = intern_string("X");
  u32 x3 = intern_string("hello world");
  u32 x4 = intern_string("hello world");
  u32 x5 = intern_string("hello world2 Hello?");
  u32 x6 = intern_string("hello world2");
  ASSERT(x1 == x2);
  ASSERT(x3 == x4);
  ASSERT(x1 != x3);
  ASSERT(x5 != x1);
  ASSERT(x5 != x6);
  ASSERT(x5 != x1);
  ASSERT(x5 != x3); 
}

void neon_engine_test(){
  static module_data test_mod;
  engine_context ctx = {3};
  current_context = ctx;
  
  string_vector_test();
  intern_string_test();


  ASSERT(!get_module_data(&test_mod));
  void *test_data = alloc0(10);
  set_module_data(&test_mod, test_data);
  var newv = get_module_data(&test_mod);
  ASSERT(newv == test_data);

  current_context = (engine_context){2};
  {
    ASSERT(!get_module_data(&test_mod));
    void *test_data = alloc0(10);
    set_module_data(&test_mod, test_data);
    var newv = get_module_data(&test_mod);
    ASSERT(newv == test_data);
  }
  
  current_context = ctx;

  ASSERT(get_module_data(&test_mod) == test_data);
  //ASSERT(0 == load_module("./game_module1.so"));
  //ASSERT(-1 == load_module("./game_module1.so"));

  test_register_event();
}

void neon_engine_main(){
  //distance_field_convert_test();
  neon_engine_test();

  engine_context ctx = {1};
  current_context = ctx;
  register_event(0,0,false);  
  ASSERT(0 == load_module("./game_module1.so"));
  ASSERT(0 == load_module("./dist_module.so"));

  while(true){
    u64 ts = timestamp();

    event_table_weight * table_weight = get_event_table_weight();
    //event_table_weight_print(table_weight);
    for(u32 i = 0; i < table_weight->count; i++){
      method_ptr p = get_registered_method(table_weight->method[i + 1]);
      if(p == NULL) continue;
      p();
    }
    
    u64 ts2 = timestamp();
    var seconds_spent = ((double)(ts2 - ts) * 1e-6);
    
    logd("%f s \n", seconds_spent);
    if(seconds_spent < 0.016){
      iron_sleep(0.016 - seconds_spent);
    }
  }
}

int main(){
  neon_engine_main();
  
  return 0;
}
