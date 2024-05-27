#pragma once

#include <common.h>

__MALLOC__ void *kmalloc(size_t size);
__MALLOC__ void *kcalloc(size_t nmemb, size_t size);
__MALLOC__ void *krealloc(void *ptr, size_t size);

void kfree(void *ptr);