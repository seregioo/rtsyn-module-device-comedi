#include <stdlib.h>
#include <string.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/create.h"

rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_create(void **out_instance) {
    if (!out_instance) {
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }

    rtsyn_comedi_instance_t *comedi =
        (rtsyn_comedi_instance_t *)malloc(sizeof(rtsyn_comedi_instance_t));

    if (!comedi) {
        return RTSYN_ABI_STATUS_ERROR;
    }

    memset(comedi->analog_values, 0, sizeof(comedi->analog_values));
    memset(comedi->digital_values, 0, sizeof(comedi->digital_values));
    strncpy(comedi->device_path, "/dev/comedi0", sizeof(comedi->device_path) - 1);
    comedi->device_path[sizeof(comedi->device_path) - 1] = '\0';
    comedi->hardware_ready = 0U;
    comedi->last_error = 0U;
    comedi->device = nullptr;
    comedi->analog_input_subdevice = -1;
    comedi->analog_output_subdevice = -1;
    comedi->digital_input_subdevice = -1;
    comedi->digital_output_subdevice = -1;
    *out_instance = comedi;

    return RTSYN_ABI_STATUS_OK;
}
