#include <string.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/driver.h"
#include "rtsyn/internal/comedi/process.h"

static bool rtsyn_comedi_port_is_connected(const rtsyn_abi_runtime_context_t *context,
                                           uint32_t port_index) {
    return !context->api->is_port_connected || context->api->is_port_connected(context, port_index);
}

static rtsyn_abi_status_t
rtsyn_comedi_process_immediate(rtsyn_comedi_instance_t *comedi,
                               const rtsyn_abi_runtime_context_t *context) {
    for (uint32_t port = 0; port < rtsyn_comedi_port_count; ++port) {
        if (!rtsyn_comedi_port_is_connected(context, port)) {
            continue;
        }
        rtsyn_abi_status_t status = RTSYN_ABI_STATUS_OK;
        uint32_t channel = rtsyn_comedi_port_channels[port];
        if (rtsyn_comedi_port_kinds[port] == RTSYN_COMEDI_PORT_KIND_ANALOG_INPUT) {
            (void)rtsyn_comedi_driver_read_analog(comedi, channel, &comedi->analog_values[port]);
            status = context->api->write(context, port, &comedi->analog_values[port]);
        } else if (rtsyn_comedi_port_kinds[port] == RTSYN_COMEDI_PORT_KIND_DIGITAL_INPUT) {
            (void)rtsyn_comedi_driver_read_digital(comedi, channel, &comedi->digital_values[port]);
            status = context->api->write(context, port, &comedi->digital_values[port]);
        }
        if (status < 0) {
            return status;
        }
    }
    return RTSYN_ABI_STATUS_OK;
}

static rtsyn_abi_status_t
rtsyn_comedi_process_terminal(rtsyn_comedi_instance_t *comedi,
                              const rtsyn_abi_runtime_context_t *context) {
    for (uint32_t port = 0; port < rtsyn_comedi_port_count; ++port) {
        if (!rtsyn_comedi_port_is_connected(context, port)) {
            continue;
        }
        rtsyn_abi_status_t status = RTSYN_ABI_STATUS_OK;
        uint32_t channel = rtsyn_comedi_port_channels[port];
        if (rtsyn_comedi_port_kinds[port] == RTSYN_COMEDI_PORT_KIND_ANALOG_OUTPUT) {
            status = context->api->read(context, port, &comedi->analog_values[port]);
            if (status >= 0) {
                (void)rtsyn_comedi_driver_write_analog(
                    comedi, channel,
                    comedi->analog_values[port] * comedi->analog_output_gains[channel]);
            }
        } else if (rtsyn_comedi_port_kinds[port] == RTSYN_COMEDI_PORT_KIND_DIGITAL_OUTPUT) {
            status = context->api->read(context, port, &comedi->digital_values[port]);
            if (status >= 0) {
                (void)rtsyn_comedi_driver_write_digital(comedi, channel,
                                                        comedi->digital_values[port]);
            }
        }
        if (status < 0) {
            return status;
        }
    }
    return RTSYN_ABI_STATUS_OK;
}

rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_process(void *instance,
                                                       const rtsyn_abi_runtime_context_t *context) {
    rtsyn_comedi_instance_t *comedi = (rtsyn_comedi_instance_t *)instance;

    if (!comedi || !context || !context->api || !context->api->read || !context->api->write) {
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }

    switch (context->cycle_stage) {
    case RTSYN_RUNTIME_CYCLE_STAGE_IMMEDIATE:
        return rtsyn_comedi_process_immediate(comedi, context);
    case RTSYN_RUNTIME_CYCLE_STAGE_TERMINAL:
        return rtsyn_comedi_process_terminal(comedi, context);
    default:
        return RTSYN_ABI_STATUS_OK;
    }
}
