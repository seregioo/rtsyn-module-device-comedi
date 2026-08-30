/**
 * @file rtsyn/internal/comedi/driver.h
 * @author Sergio Hidalgo
 * @brief Internal COMEDI driver boundary for the RTSyn COMEDI device.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_DRIVER_H
#define RTSYN_INTERNAL_COMEDI_DRIVER_H

#include "rtsyn/internal/comedi.h"

#ifdef __cplusplus
extern "C" {
#endif

int rtsyn_comedi_driver_open(rtsyn_comedi_instance_t *comedi);

int rtsyn_comedi_driver_discover(const char *device_path, uint32_t *analog_inputs,
                                 uint32_t *analog_outputs, uint32_t *digital_inputs,
                                 uint32_t *digital_outputs);

void rtsyn_comedi_driver_close(rtsyn_comedi_instance_t *comedi);

int rtsyn_comedi_driver_read_analog(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                    double *out_value);

int rtsyn_comedi_driver_write_analog(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                     double value);

int rtsyn_comedi_driver_read_digital(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                     uint64_t *out_value);

int rtsyn_comedi_driver_write_digital(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                      uint64_t value);

#ifdef __cplusplus
}
#endif

#endif /* RTSYN_INTERNAL_COMEDI_DRIVER_H */
