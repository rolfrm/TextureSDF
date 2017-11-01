void * module_open(const char * name);
void * module_symbol(void * module, const char * sym);
char * module_error();
typedef struct _module_data{
  icy_vector * vec;
}module_data;

typedef struct _engine_context{
  u32 context_id;
}engine_context;
extern engine_context current_context;

void * get_module_data(module_data * md);
void set_module_data(module_data * md, void * data);
int load_module(const char * name);
