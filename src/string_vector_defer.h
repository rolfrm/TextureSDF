// This file is auto generated by icy-vector
#include "icydb.h"
typedef struct _string_vector_defer{
  char ** column_names;
  char ** column_types;
  size_t * count;
  size_t * capacity;
  size_t * free_index_count;
  const size_t column_count;
  icy_mem * free_indexes;
  icy_mem * header;
  const size_t column_sizes[1];
  
  string_vector_internal_indexes * int_id;
  icy_mem * int_id_area;
}string_vector_defer;

// a vector index.
typedef struct{
  size_t index;
}string_vector_defer_index;

typedef struct{
  size_t index;
  size_t count;
}string_vector_defer_indexes;

string_vector_defer * string_vector_defer_create(const char * optional_name);
string_vector_defer_index string_vector_defer_alloc(string_vector_defer * table);
string_vector_defer_indexes string_vector_defer_alloc_sequence(string_vector_defer * table, size_t count);
void string_vector_defer_remove(string_vector_defer * table, string_vector_defer_index index);
void string_vector_defer_remove_sequence(string_vector_defer * table, string_vector_defer_indexes * indexes);
void string_vector_defer_clear(string_vector_defer * table);
void string_vector_defer_optimize(string_vector_defer * table);
void string_vector_defer_destroy(string_vector_defer ** table);
