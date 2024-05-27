#pragma once

#include <common.h>

void ioapic_init();
void ioapic_map_irq(const uint8_t irq, const uint8_t dest);