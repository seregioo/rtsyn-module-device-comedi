#include <string.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/driver.h"
#include "rtsyn/internal/comedi/start.h"

rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_start(void *instance) {
    rtsyn_comedi_instance_t *comedi = (rtsyn_comedi_instance_t *)instance;

    if (!comedi) {
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }

    memset(comedi->analog_values, 0, sizeof(comedi->analog_values));
    memset(comedi->digital_values, 0, sizeof(comedi->digital_values));
    comedi->hardware_ready = 0U;
    comedi->last_error = 0U;
    (void)rtsyn_comedi_driver_open(comedi);

    return RTSYN_ABI_STATUS_OK;
}
