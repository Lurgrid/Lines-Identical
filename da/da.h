extern da *da_empty(size_t size);

extern void da_dispose(da **dptr);

extern void *da_add(da *d, const void *ref);

extern void *da_cond_left_search(da * d, bool (*cond)(const void *));

extern void *da_nth(da * d, size_t n);

extern size_t da_length(da * d);

extern int da_add_input_fct(da *d, int (get) (FILE * stream, size_t size), FILE * stream);

