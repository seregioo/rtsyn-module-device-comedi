/**
 * @file rtsyn/internal/comedi.h
 * @author Sergio Hidalgo
 * @brief Internal state declarations for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_H
#define RTSYN_INTERNAL_COMEDI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { RTSYN_COMEDI_MAX_PORTS = 64 };

typedef enum rtsyn_comedi_port_kind_e {
    RTSYN_COMEDI_PORT_KIND_ANALOG_OUTPUT,
    RTSYN_COMEDI_PORT_KIND_DIGITAL_OUTPUT,
    RTSYN_COMEDI_PORT_KIND_ANALOG_INPUT,
    RTSYN_COMEDI_PORT_KIND_DIGITAL_INPUT,
} rtsyn_comedi_port_kind_t;

extern uint32_t rtsyn_comedi_port_count;
extern rtsyn_comedi_port_kind_t rtsyn_comedi_port_kinds[RTSYN_COMEDI_MAX_PORTS];
extern uint32_t rtsyn_comedi_port_channels[RTSYN_COMEDI_MAX_PORTS];

/**
 * @brief Per-node runtime instance owned by RTSyn through the ABI callbacks.
 */
typedef struct rtsyn_comedi_instance_s {
    double analog_values[RTSYN_COMEDI_MAX_PORTS];
    double analog_output_gains[RTSYN_COMEDI_MAX_PORTS];
    uint64_t digital_values[RTSYN_COMEDI_MAX_PORTS];
    char device_path[16];
    uint64_t hardware_ready;
    uint64_t last_error;
    void *device;
    int analog_input_subdevice;
    int analog_output_subdevice;
    int digital_input_subdevice;
    int digital_output_subdevice;
} rtsyn_comedi_instance_t;

/**
 * @brief Port indexes exposed by the module descriptor.
 */
typedef enum rtsyn_comedi_instance_params_e : uint32_t {
    RTSYN_COMEDI_PARAM_DEVICE_PATH = 0,
    RTSYN_COMEDI_PARAM_ANALOG_OUTPUT_GAIN_BASE = 1,
} rtsyn_comedi_instance_params_t;

/**
 * @brief State indexes accepted by the read-state callback.
 */
typedef enum rtsyn_comedi_instance_states_e : uint32_t {
    RTSYN_COMEDI_STATE_HARDWARE_READY = 0,
    RTSYN_COMEDI_STATE_LAST_ERROR = 1,
    RTSYN_COMEDI_STATE_COUNT = 2,
} rtsyn_comedi_instance_states_t;

#ifdef __cplusplus
}
#endif

#endif /* RTSYN_INTERNAL_COMEDI_H */
