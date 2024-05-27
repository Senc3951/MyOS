#pragma once

#include <mem/paging.h>

void vmm_init();

page_table_t *get_kernel_pd();