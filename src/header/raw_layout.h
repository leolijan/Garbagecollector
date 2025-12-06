#include <stdint.h>
#include <stdlib.h>

#include "header.h"

typedef u_int64_t raw_layout_t;

u_int64_t to_raw_layout(raw_layout_t size);
//returns size field
u_int64_t get_size(raw_layout_t raw);

header_t *create_raw_header(const size_t bytes);