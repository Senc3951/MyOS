#pragma once

#define NMI_CONTROL_PORT_A 0x92
#define NMI_CONTROL_PORT_B 0x61

/**
 * @brief Enable NMI.
*/
void nmi_enable();

/**
 * @brief Disable NMI.
*/
void nmi_disable();