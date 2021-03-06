// This file is auto generated by icy-table.
#ifndef TABLE_COMPILER_INDEX
#define TABLE_COMPILER_INDEX
#define array_element_size(array) sizeof(array[0])
#define array_count(array) (sizeof(array)/array_element_size(array))
#include "icydb.h"
#include <stdlib.h>
#endif


event_table_weight * event_table_weight_create(const char * optional_name){
  static const char * const column_names[] = {(char *)"key", (char *)"method"};
  static const char * const column_types[] = {"f64", "u32"};
  event_table_weight * instance = calloc(sizeof(event_table_weight), 1);
  instance->column_names = (char **)column_names;
  instance->column_types = (char **)column_types;
  
  icy_table_init((icy_table * )instance, optional_name, 2, (unsigned int[]){sizeof(f64), sizeof(u32)}, (char *[]){(char *)"key", (char *)"method"});

  return instance;
}

void event_table_weight_insert(event_table_weight * table, f64 * key, u32 * method, size_t count){
  void * array[] = {(void* )key, (void* )method};
  icy_table_inserts((icy_table *) table, array, count);
}

void event_table_weight_set(event_table_weight * table, f64 key, u32 method){
  void * array[] = {(void* )&key, (void* )&method};
  icy_table_inserts((icy_table *) table, array, 1);
}

void event_table_weight_lookup(event_table_weight * table, f64 * keys, size_t * out_indexes, size_t count){
  icy_table_finds((icy_table *) table, keys, out_indexes, count);
}

void event_table_weight_remove(event_table_weight * table, f64 * keys, size_t key_count){
  size_t indexes[key_count];
  size_t index = 0;
  size_t cnt = 0;
  while(0 < (cnt = icy_table_iter((icy_table *) table, keys, key_count, NULL, indexes, array_count(indexes), &index))){
    icy_table_remove_indexes((icy_table *) table, indexes, cnt);
    index = 0;
  }
}

void event_table_weight_clear(event_table_weight * table){
  icy_table_clear((icy_table *) table);
}

void event_table_weight_unset(event_table_weight * table, f64 key){
  event_table_weight_remove(table, &key, 1);
}

bool event_table_weight_try_get(event_table_weight * table, f64 * key, u32 * method){
  void * array[] = {(void* )key, (void* )method};
  void * column_pointers[] = {(void *)table->key, (void *)table->method};
  size_t __index = 0;
  icy_table_finds((icy_table *) table, array[0], &__index, 1);
  if(__index == 0) return false;
  unsigned int sizes[] = {sizeof(f64), sizeof(u32)};
  for(int i = 1; i < 2; i++){
    if(array[i] != NULL)
      memcpy(array[i], column_pointers[i] + __index * sizes[i], sizes[i]); 
  }
  return true;
}

void event_table_weight_print(event_table_weight * table){
  icy_table_print((icy_table *) table);
}

size_t event_table_weight_iter(event_table_weight * table, f64 * keys, size_t keycnt, f64 * optional_keys_out, size_t * indexes, size_t cnt, size_t * iterator){
  return icy_table_iter((icy_table *) table, keys, keycnt, optional_keys_out, indexes, cnt, iterator);

}
