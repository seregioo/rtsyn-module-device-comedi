#include <stdlib.h>

#include "rtsyn/internal/comedi.h"
#include "rtsyn/internal/comedi/destroy.h"
#include "rtsyn/internal/comedi/driver.h"

void RTSYN_ABI_CALL rtsyn_comedi_destroy(void *instance) {
    rtsyn_comedi_instance_t *comedi = (rtsyn_comedi_instance_t *)instance;
    rtsyn_comedi_driver_close(comedi);
    free(comedi);
}
