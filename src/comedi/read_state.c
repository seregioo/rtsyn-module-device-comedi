#include <string.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/read_state.h"

rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_read_state(const void *instance,
                                                          uint32_t state_index, void *out_value) {
    const rtsyn_comedi_instance_t *comedi = (const rtsyn_comedi_instance_t *)instance;

    if (!comedi || !out_value) {
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }

    switch (state_index) {
    case RTSYN_COMEDI_STATE_HARDWARE_READY:
        *(uint64_t *)out_value = comedi->hardware_ready;
        return RTSYN_ABI_STATUS_OK;
    case RTSYN_COMEDI_STATE_LAST_ERROR:
        *(uint64_t *)out_value = comedi->last_error;
        return RTSYN_ABI_STATUS_OK;
    default:
        return RTSYN_ABI_STATUS_INVALID_ARGUMENT;
    }
}
