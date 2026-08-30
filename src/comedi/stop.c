#include <string.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/driver.h"
#include "rtsyn/internal/comedi/stop.h"

rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_stop(void *instance) {
    rtsyn_comedi_instance_t *comedi = (rtsyn_comedi_instance_t *)instance;

    if (!comedi) {
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }

    rtsyn_comedi_driver_close(comedi);
    memset(comedi->analog_values, 0, sizeof(comedi->analog_values));
    memset(comedi->digital_values, 0, sizeof(comedi->digital_values));
    comedi->hardware_ready = 0U;
    comedi->last_error = 0U;

    return RTSYN_ABI_STATUS_OK;
}
