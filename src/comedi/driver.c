#include <stdint.h>

#include "rtsyn/internal/comedi/driver.h"

enum {
    RTSYN_COMEDI_ERROR_NONE = 0,
    RTSYN_COMEDI_ERROR_OPEN = 1,
    RTSYN_COMEDI_ERROR_SUBDEVICE = 2,
    RTSYN_COMEDI_ERROR_RANGE = 3,
    RTSYN_COMEDI_ERROR_READ = 4,
    RTSYN_COMEDI_ERROR_WRITE = 5,
};

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
#include <comedi.h>
#include <comedilib.h>

static int rtsyn_comedi_find_subdevice(comedi_t *device, int preferred, int fallback) {
    int subdevice = comedi_find_subdevice_by_type(device, preferred, 0);
    if (subdevice >= 0) {
        return subdevice;
    }
    return fallback >= 0 ? comedi_find_subdevice_by_type(device, fallback, 0) : -1;
}

static int rtsyn_comedi_configure_dio(comedi_t *device, int subdevice, uint64_t channel,
                                      int direction) {
    if (subdevice < 0 ||
        comedi_get_subdevice_type(device, (unsigned int)subdevice) != COMEDI_SUBD_DIO) {
        return 0;
    }

    return comedi_dio_config(device, (unsigned int)subdevice, (unsigned int)channel, direction) < 0
               ? -1
               : 0;
}
#endif

int rtsyn_comedi_driver_discover(const char *device_path, uint32_t *analog_inputs,
                                 uint32_t *analog_outputs, uint32_t *digital_inputs,
                                 uint32_t *digital_outputs) {
    if (!device_path || !analog_inputs || !analog_outputs || !digital_inputs || !digital_outputs) {
        return -1;
    }
    *analog_inputs = *analog_outputs = *digital_inputs = *digital_outputs = 0;
#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    comedi_t *device = comedi_open(device_path);
    if (!device) {
        return -1;
    }
    int ai = rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_AI, -1);
    int ao = rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_AO, -1);
    int di = rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_DI, COMEDI_SUBD_DIO);
    int do_subdevice = rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_DO, COMEDI_SUBD_DIO);
    if (ai >= 0) *analog_inputs = (uint32_t)comedi_get_n_channels(device, (unsigned int)ai);
    if (ao >= 0) *analog_outputs = (uint32_t)comedi_get_n_channels(device, (unsigned int)ao);
    if (di >= 0) *digital_inputs = (uint32_t)comedi_get_n_channels(device, (unsigned int)di);
    if (do_subdevice >= 0)
        *digital_outputs = (uint32_t)comedi_get_n_channels(device, (unsigned int)do_subdevice);
    comedi_close(device);
    return 0;
#else
    return -1;
#endif
}

int rtsyn_comedi_driver_open(rtsyn_comedi_instance_t *comedi) {
    if (!comedi) {
        return -1;
    }

    rtsyn_comedi_driver_close(comedi);

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    comedi_t *device = comedi_open(comedi->device_path);
    if (!device) {
        comedi->last_error = RTSYN_COMEDI_ERROR_OPEN;
        return -1;
    }

    comedi->analog_input_subdevice = rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_AI, -1);
    comedi->analog_output_subdevice = rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_AO, -1);
    comedi->digital_input_subdevice =
        rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_DI, COMEDI_SUBD_DIO);
    comedi->digital_output_subdevice =
        rtsyn_comedi_find_subdevice(device, COMEDI_SUBD_DO, COMEDI_SUBD_DIO);

    if (comedi->analog_input_subdevice < 0 && comedi->analog_output_subdevice < 0 &&
        comedi->digital_input_subdevice < 0 && comedi->digital_output_subdevice < 0) {
        comedi_close(device);
        comedi->last_error = RTSYN_COMEDI_ERROR_SUBDEVICE;
        return -1;
    }

    comedi->device = device;
    comedi->hardware_ready = 1U;
    comedi->last_error = RTSYN_COMEDI_ERROR_NONE;
    return 0;
#else
    comedi->hardware_ready = 0U;
    comedi->last_error = RTSYN_COMEDI_ERROR_OPEN;
    return -1;
#endif
}

void rtsyn_comedi_driver_close(rtsyn_comedi_instance_t *comedi) {
    if (!comedi) {
        return;
    }

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    if (comedi->device) {
        comedi_close((comedi_t *)comedi->device);
    }
#endif

    comedi->device = 0;
    comedi->analog_input_subdevice = -1;
    comedi->analog_output_subdevice = -1;
    comedi->digital_input_subdevice = -1;
    comedi->digital_output_subdevice = -1;
    comedi->hardware_ready = 0U;
}

int rtsyn_comedi_driver_read_analog(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                    double *out_value) {
    if (!comedi || !out_value || !comedi->device || comedi->analog_input_subdevice < 0) {
        return -1;
    }

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    comedi_t *device = (comedi_t *)comedi->device;
    const unsigned int subdevice = (unsigned int)comedi->analog_input_subdevice;
    const unsigned int range_index = 0;
    lsampl_t raw = 0;

    if (comedi_data_read(device, subdevice, channel, range_index, AREF_GROUND, &raw) < 0) {
        comedi->last_error = RTSYN_COMEDI_ERROR_READ;
        return -1;
    }

    const comedi_range *range = comedi_get_range(device, subdevice, channel, range_index);
    if (!range) {
        comedi->last_error = RTSYN_COMEDI_ERROR_RANGE;
        return -1;
    }

    *out_value = comedi_to_phys(raw, range, comedi_get_maxdata(device, subdevice, channel));
    comedi->last_error = RTSYN_COMEDI_ERROR_NONE;
    return 0;
#else
    return -1;
#endif
}

int rtsyn_comedi_driver_write_analog(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                     double value) {
    if (!comedi || !comedi->device || comedi->analog_output_subdevice < 0) {
        return -1;
    }

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    comedi_t *device = (comedi_t *)comedi->device;
    const unsigned int subdevice = (unsigned int)comedi->analog_output_subdevice;
    const unsigned int range_index = 0;
    const comedi_range *range = comedi_get_range(device, subdevice, channel, range_index);
    if (!range) {
        comedi->last_error = RTSYN_COMEDI_ERROR_RANGE;
        return -1;
    }

    const lsampl_t raw =
        comedi_from_phys(value, range, comedi_get_maxdata(device, subdevice, channel));
    if (comedi_data_write(device, subdevice, channel, range_index, AREF_GROUND, raw) < 0) {
        comedi->last_error = RTSYN_COMEDI_ERROR_WRITE;
        return -1;
    }

    comedi->last_error = RTSYN_COMEDI_ERROR_NONE;
    return 0;
#else
    (void)value;
    return -1;
#endif
}

int rtsyn_comedi_driver_read_digital(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                     uint64_t *out_value) {
    if (!comedi || !out_value || !comedi->device || comedi->digital_input_subdevice < 0) {
        return -1;
    }

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    if (rtsyn_comedi_configure_dio((comedi_t *)comedi->device, comedi->digital_input_subdevice,
                                   channel, COMEDI_INPUT) < 0) {
        comedi->last_error = RTSYN_COMEDI_ERROR_SUBDEVICE;
        return -1;
    }
    unsigned int bit = 0;
    if (comedi_dio_read((comedi_t *)comedi->device, (unsigned int)comedi->digital_input_subdevice,
                        channel, &bit) < 0) {
        comedi->last_error = RTSYN_COMEDI_ERROR_READ;
        return -1;
    }

    *out_value = bit != 0U ? 1U : 0U;
    comedi->last_error = RTSYN_COMEDI_ERROR_NONE;
    return 0;
#else
    return -1;
#endif
}

int rtsyn_comedi_driver_write_digital(rtsyn_comedi_instance_t *comedi, uint32_t channel,
                                      uint64_t value) {
    if (!comedi || !comedi->device || comedi->digital_output_subdevice < 0) {
        return -1;
    }

#if defined(RTSYN_COMEDI_ENABLE_DRIVER)
    if (rtsyn_comedi_configure_dio((comedi_t *)comedi->device, comedi->digital_output_subdevice,
                                   channel, COMEDI_OUTPUT) < 0) {
        comedi->last_error = RTSYN_COMEDI_ERROR_SUBDEVICE;
        return -1;
    }
    if (comedi_dio_write((comedi_t *)comedi->device, (unsigned int)comedi->digital_output_subdevice,
                         channel, value != 0U ? 1U : 0U) < 0) {
        comedi->last_error = RTSYN_COMEDI_ERROR_WRITE;
        return -1;
    }

    comedi->last_error = RTSYN_COMEDI_ERROR_NONE;
    return 0;
#else
    (void)value;
    return -1;
#endif
}
