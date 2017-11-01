typedef void (*method_ptr)();
u32 intern_string(const char * string);
void register_method(u32 method, method_ptr f);
method_ptr get_registered_method(u32 method);
void register_event(u32 method, u32 relative, bool after);
