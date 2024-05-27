#pragma once

#include <common.h>

#define FRAME_SIZE 4096

void pmm_init();

void *pmm_getFrame();
void pmm_releaseFrame(void *ptr);

uint64_t pmm_memend();