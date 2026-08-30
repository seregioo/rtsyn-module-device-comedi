#include "rtsyn/comedi.h"
#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/create.h"
#include "rtsyn/internal/comedi/destroy.h"
#include "rtsyn/internal/comedi/process.h"
#include "rtsyn/internal/comedi/read_state.h"
#include "rtsyn/internal/comedi/set_param.h"
#include "rtsyn/internal/comedi/start.h"
#include "rtsyn/internal/comedi/stop.h"
#include "rtsyn/internal/comedi/driver.h"

#include <stdio.h>
#include <string.h>

uint32_t rtsyn_comedi_port_count = 0;
rtsyn_comedi_port_kind_t rtsyn_comedi_port_kinds[RTSYN_COMEDI_MAX_PORTS];
uint32_t rtsyn_comedi_port_channels[RTSYN_COMEDI_MAX_PORTS];
static rtsyn_abi_port_descriptor_t rtsyn_comedi_ports[RTSYN_COMEDI_MAX_PORTS];
static char rtsyn_comedi_port_names[RTSYN_COMEDI_MAX_PORTS][32];

static const rtsyn_abi_param_descriptor_t rtsyn_comedi_params[RTSYN_COMEDI_PARAM_COUNT] = {
    [RTSYN_COMEDI_PARAM_DEVICE_PATH] =
        {
            .name = "device_path",
            .description = "COMEDI device path.",
            .value_type = RTSYN_ABI_VALUE_STRING,
        },
};

static const rtsyn_abi_state_descriptor_t rtsyn_comedi_states[RTSYN_COMEDI_STATE_COUNT] = {
    [RTSYN_COMEDI_STATE_HARDWARE_READY] =
        {
            .name = "hardware_ready",
            .description = "Non-zero when the COMEDI device was opened and configured.",
            .value_type = RTSYN_ABI_VALUE_U64,
        },
    [RTSYN_COMEDI_STATE_LAST_ERROR] =
        {
            .name = "last_error",
            .description = "Last COMEDI device error code observed by the module.",
            .value_type = RTSYN_ABI_VALUE_U64,
        },
};

static rtsyn_abi_node_descriptor_t rtsyn_comedi_descriptor = {
    .name = "comedi",
    .node_type = RTSYN_ABI_NODE_DEVICE,
    .port_count = 0,
    .ports = rtsyn_comedi_ports,
    .param_count = RTSYN_COMEDI_PARAM_COUNT,
    .params = rtsyn_comedi_params,
    .state_count = RTSYN_COMEDI_STATE_COUNT,
    .states = rtsyn_comedi_states,
    .callbacks =
        {
            .create = rtsyn_comedi_create,
            .set_param = rtsyn_comedi_set_param,
            .read_state = rtsyn_comedi_read_state,
            .start = rtsyn_comedi_start,
            .process = rtsyn_comedi_process,
            .stop = rtsyn_comedi_stop,
            .destroy = rtsyn_comedi_destroy,
        },
};

static void rtsyn_comedi_add_ports(const char *prefix, rtsyn_comedi_port_kind_t kind,
                                   rtsyn_abi_value_type_t value_type,
                                   rtsyn_abi_port_direction_t direction, uint32_t channel_count) {
    for (uint32_t channel = 0;
         channel < channel_count && rtsyn_comedi_port_count < RTSYN_COMEDI_MAX_PORTS; ++channel) {
        uint32_t port = rtsyn_comedi_port_count++;
        (void)snprintf(rtsyn_comedi_port_names[port], sizeof(rtsyn_comedi_port_names[port]),
                       "%s_%u", prefix, channel);
        rtsyn_comedi_ports[port] = (rtsyn_abi_port_descriptor_t){
            .name = rtsyn_comedi_port_names[port],
            .value_type = value_type,
            .direction = direction,
        };
        rtsyn_comedi_port_kinds[port] = kind;
        rtsyn_comedi_port_channels[port] = channel;
    }
}

static void rtsyn_comedi_initialize_descriptor(void) {
    if (rtsyn_comedi_port_count != 0) {
        return;
    }
    uint32_t analog_inputs = 0, analog_outputs = 0, digital_inputs = 0, digital_outputs = 0;
    if (rtsyn_comedi_driver_discover("/dev/comedi0", &analog_inputs, &analog_outputs,
                                    &digital_inputs, &digital_outputs) != 0) {
        analog_inputs = analog_outputs = digital_inputs = digital_outputs = 1;
    }

    rtsyn_comedi_add_ports("analog_output", RTSYN_COMEDI_PORT_KIND_ANALOG_OUTPUT,
                           RTSYN_ABI_VALUE_F64, RTSYN_ABI_PORT_DIRECTION_IN, analog_outputs);
    rtsyn_comedi_add_ports("digital_output", RTSYN_COMEDI_PORT_KIND_DIGITAL_OUTPUT,
                           RTSYN_ABI_VALUE_U64, RTSYN_ABI_PORT_DIRECTION_IN, digital_outputs);
    rtsyn_comedi_add_ports("analog_input", RTSYN_COMEDI_PORT_KIND_ANALOG_INPUT,
                           RTSYN_ABI_VALUE_F64, RTSYN_ABI_PORT_DIRECTION_OUT, analog_inputs);
    rtsyn_comedi_add_ports("digital_input", RTSYN_COMEDI_PORT_KIND_DIGITAL_INPUT,
                           RTSYN_ABI_VALUE_U64, RTSYN_ABI_PORT_DIRECTION_OUT, digital_inputs);
    rtsyn_comedi_descriptor.port_count = rtsyn_comedi_port_count;
}

const rtsyn_abi_node_descriptor_t *RTSYN_ABI_CALL
rtsyn_comedi_get_descriptor(void)
{
    rtsyn_comedi_initialize_descriptor();
    return &rtsyn_comedi_descriptor;
}

const rtsyn_abi_node_descriptor_t *RTSYN_ABI_CALL
rtsyn_module_get_descriptor(void)
{
    return rtsyn_comedi_get_descriptor();
}
