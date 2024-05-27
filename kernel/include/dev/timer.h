#pragma once

#include <common.h>

void timer_init();

void timer_oneshot(const uint64_t ms);
void timer_periodic(const uint64_t ms);