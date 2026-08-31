#include <math.h>
#include <string.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/set_param.h"

rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_set_param(void *instance, uint32_t param_index,
                                                         const void *value) {
    rtsyn_comedi_instance_t *comedi = (rtsyn_comedi_instance_t *)instance;

    if (!comedi || !value) {
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }

    switch (param_index) {
    case RTSYN_COMEDI_PARAM_DEVICE_PATH:
        strncpy(comedi->device_path, (const char *)value, sizeof(comedi->device_path) - 1);
        comedi->device_path[sizeof(comedi->device_path) - 1] = '\0';
        return RTSYN_ABI_STATUS_OK;
    default: {
        uint32_t channel = param_index - RTSYN_COMEDI_PARAM_ANALOG_OUTPUT_GAIN_BASE;
        double gain = *(const double *)value;
        if (channel >= RTSYN_COMEDI_MAX_PORTS || !isfinite(gain)) {
            return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
        }
        comedi->analog_output_gains[channel] = gain;
        return RTSYN_ABI_STATUS_OK;
    }
    }
}
