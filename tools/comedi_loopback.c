#define _POSIX_C_SOURCE 200809L

#include <comedilib.h>

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *subdevice_type_name(int type) {
    switch (type) {
    case COMEDI_SUBD_AI: return "AI";
    case COMEDI_SUBD_AO: return "AO";
    case COMEDI_SUBD_DI: return "DI";
    case COMEDI_SUBD_DO: return "DO";
    case COMEDI_SUBD_DIO: return "DIO";
    case COMEDI_SUBD_COUNTER: return "COUNTER";
    case COMEDI_SUBD_TIMER: return "TIMER";
    case COMEDI_SUBD_MEMORY: return "MEMORY";
    case COMEDI_SUBD_CALIB: return "CALIB";
    case COMEDI_SUBD_PROC: return "PROC";
    case COMEDI_SUBD_SERIAL: return "SERIAL";
    case COMEDI_SUBD_PWM: return "PWM";
    default: return "UNUSED/UNKNOWN";
    }
}

static void print_comedi_error(const char *operation) {
    fprintf(stderr, "%s failed: %s\n", operation, comedi_strerror(comedi_errno()));
}

static void inspect_device(comedi_t *device) {
    int count = comedi_get_n_subdevices(device);
    printf("driver=%s board=%s subdevices=%d\n",
           comedi_get_driver_name(device), comedi_get_board_name(device), count);

    for (int subdevice = 0; subdevice < count; ++subdevice) {
        int type = comedi_get_subdevice_type(device, (unsigned int)subdevice);
        int channels = comedi_get_n_channels(device, (unsigned int)subdevice);
        int flags = comedi_get_subdevice_flags(device, (unsigned int)subdevice);
        printf("subdevice %d: type=%s(%d) channels=%d flags=0x%x%s%s\n",
               subdevice, subdevice_type_name(type), type, channels, flags,
               (flags & SDF_READABLE) ? " readable" : "",
               (flags & SDF_WRITABLE) ? " writable" : "");

        if ((type != COMEDI_SUBD_AI && type != COMEDI_SUBD_AO) || channels <= 0) {
            continue;
        }
        int ranges = comedi_get_n_ranges(device, (unsigned int)subdevice, 0);
        lsampl_t maxdata = comedi_get_maxdata(device, (unsigned int)subdevice, 0);
        printf("  channel 0: maxdata=%u ranges=%d\n", (unsigned int)maxdata, ranges);
        for (int range_index = 0; range_index < ranges; ++range_index) {
            comedi_range *range = comedi_get_range(device, (unsigned int)subdevice, 0,
                                                   (unsigned int)range_index);
            if (range) {
                printf("    range %d: [%.9g, %.9g] %s\n", range_index, range->min,
                       range->max, range->unit == UNIT_volt ? "V" : "(non-voltage unit)");
            }
        }
    }
}

static int select_voltage_range(comedi_t *device, unsigned int subdevice,
                                unsigned int channel, double voltage) {
    int count = comedi_get_n_ranges(device, subdevice, channel);
    for (int index = 0; index < count; ++index) {
        comedi_range *range = comedi_get_range(device, subdevice, channel,
                                               (unsigned int)index);
        if (range && range->unit == UNIT_volt && voltage >= range->min &&
            voltage <= range->max) {
            return index;
        }
    }
    return -1;
}

static void settle(void) {
    const struct timespec delay = {.tv_sec = 0, .tv_nsec = 10 * 1000 * 1000};
    nanosleep(&delay, NULL);
}

static double linear_to_phys(lsampl_t raw, const comedi_range *range, lsampl_t maxdata) {
    return range->min + ((double)raw / (double)maxdata) * (range->max - range->min);
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "/dev/comedi0";
    double requested_voltage = argc > 2 ? strtod(argv[2], NULL) : 1.0;
    unsigned int ai_channel = argc > 3 ? (unsigned int)strtoul(argv[3], NULL, 10) : 0;
    unsigned int ao_channel = argc > 4 ? (unsigned int)strtoul(argv[4], NULL, 10) : 0;
    const char *reference_name = argc > 5 ? argv[5] : "ground";
    unsigned int ai_reference = AREF_GROUND;
    if (strcmp(reference_name, "diff") == 0) {
        ai_reference = AREF_DIFF;
    } else if (strcmp(reference_name, "common") == 0) {
        ai_reference = AREF_COMMON;
    } else if (strcmp(reference_name, "ground") != 0) {
        fprintf(stderr, "Reference must be one of: ground, diff, common.\n");
        return EXIT_FAILURE;
    }
    comedi_t *device = comedi_open(path);
    if (!device) {
        fprintf(stderr, "Cannot open %s: %s\n", path, comedi_strerror(comedi_errno()));
        fprintf(stderr, "Check: ls -l %s; id; comedi_board_info %s\n", path, path);
        return EXIT_FAILURE;
    }

    printf("RTSyn Comedi loopback diagnostic revision 3\n");
    inspect_device(device);

    int ai = comedi_find_subdevice_by_type(device, COMEDI_SUBD_AI, 0);
    int ao = comedi_find_subdevice_by_type(device, COMEDI_SUBD_AO, 0);
    printf("selected: AI subdevice=%d channel=%u reference=%s, "
           "AO subdevice=%d channel=%u\n",
           ai, ai_channel, reference_name, ao, ao_channel);
    if (ai < 0 || ao < 0) {
        fprintf(stderr, "Both an AI and AO subdevice are required.\n");
        comedi_close(device);
        return EXIT_FAILURE;
    }
    if (ai_channel >= (unsigned int)comedi_get_n_channels(device, (unsigned int)ai) ||
        ao_channel >= (unsigned int)comedi_get_n_channels(device, (unsigned int)ao)) {
        fprintf(stderr, "Requested AI or AO channel is out of range.\n");
        comedi_close(device);
        return EXIT_FAILURE;
    }

    int ao_range_index = select_voltage_range(device, (unsigned int)ao, ao_channel,
                                              requested_voltage);
    int ai_range_index = select_voltage_range(device, (unsigned int)ai, ai_channel,
                                              requested_voltage);
    if (ao_range_index < 0 || ai_range_index < 0) {
        fprintf(stderr, "No voltage range contains %.9g V (AI range=%d, AO range=%d).\n",
                requested_voltage, ai_range_index, ao_range_index);
        comedi_close(device);
        return EXIT_FAILURE;
    }

    comedi_range *ao_range_ptr = comedi_get_range(device, (unsigned int)ao, ao_channel,
                                                  (unsigned int)ao_range_index);
    if (!ao_range_ptr) {
        print_comedi_error("comedi_get_range(AO0)");
        comedi_close(device);
        return EXIT_FAILURE;
    }
    comedi_range ao_range = *ao_range_ptr;
    lsampl_t ao_maxdata = comedi_get_maxdata(device, (unsigned int)ao, ao_channel);
    lsampl_t ao_raw = comedi_from_phys(requested_voltage, &ao_range, ao_maxdata);
    double quantized_voltage = comedi_to_phys(ao_raw, &ao_range, ao_maxdata);
    printf("AO conversion: requested=%.9g V range=%d maxdata=%u raw=%u quantized=%.9g V\n",
           requested_voltage, ao_range_index, (unsigned int)ao_maxdata,
           (unsigned int)ao_raw, quantized_voltage);

    if (comedi_data_write(device, (unsigned int)ao, ao_channel,
                          (unsigned int)ao_range_index,
                          AREF_GROUND, ao_raw) < 0) {
        print_comedi_error("comedi_data_write(AO)");
        comedi_close(device);
        return EXIT_FAILURE;
    }
    printf("AO%u write succeeded. Reading AI%u ten times after 10 ms settling intervals...\n",
           ao_channel, ai_channel);

    comedi_range *ai_range_ptr = comedi_get_range(device, (unsigned int)ai, ai_channel,
                                                  (unsigned int)ai_range_index);
    if (!ai_range_ptr) {
        print_comedi_error("comedi_get_range(AI0)");
        comedi_close(device);
        return EXIT_FAILURE;
    }
    comedi_range ai_range = *ai_range_ptr;
    lsampl_t ai_maxdata = comedi_get_maxdata(device, (unsigned int)ai, ai_channel);
    printf("AI conversion setup: range=%d [%.9g, %.9g] V maxdata=%u\n",
           ai_range_index, ai_range.min, ai_range.max, (unsigned int)ai_maxdata);
    for (int sample = 0; sample < 10; ++sample) {
        lsampl_t ai_raw = 0;
        settle();
        if (comedi_data_read(device, (unsigned int)ai, ai_channel,
                             (unsigned int)ai_range_index, ai_reference, &ai_raw) < 0) {
            print_comedi_error("comedi_data_read(AI)");
            comedi_close(device);
            return EXIT_FAILURE;
        }
        double comedilib_value = comedi_to_phys(ai_raw, &ai_range, ai_maxdata);
        double measured = linear_to_phys(ai_raw, &ai_range, ai_maxdata);
        printf("AI sample %d: raw=%u / %u, voltage=%+.9g V, comedi_to_phys=%+.9g, "
               "error=%+.9g V%s\n",
               sample + 1, (unsigned int)ai_raw, (unsigned int)ai_maxdata, measured,
               comedilib_value, measured - quantized_voltage,
               (ai_raw == 0 || ai_raw == ai_maxdata) ? " [AT RAIL/OVERRANGE]" : "");
    }

    printf("%s-referenced AI scan on range 0:\n", reference_name);
    int ai_channels = comedi_get_n_channels(device, (unsigned int)ai);
    if (ai_reference == AREF_DIFF) {
        ai_channels /= 2;
    }
    for (int channel = 0; channel < ai_channels; ++channel) {
        lsampl_t raw = 0;
        comedi_range *channel_range_ptr =
            comedi_get_range(device, (unsigned int)ai, (unsigned int)channel, 0);
        if (!channel_range_ptr) {
            printf("  AI%d: range unavailable\n", channel);
            continue;
        }
        comedi_range channel_range = *channel_range_ptr;
        lsampl_t channel_maxdata =
            comedi_get_maxdata(device, (unsigned int)ai, (unsigned int)channel);
        if (comedi_data_read(device, (unsigned int)ai, (unsigned int)channel, 0,
                             ai_reference, &raw) < 0) {
            printf("  AI%d: read failed: %s\n", channel,
                   comedi_strerror(comedi_errno()));
            continue;
        }
        printf("  AI%d: raw=%u voltage=%+.9g V%s\n", channel, (unsigned int)raw,
               linear_to_phys(raw, &channel_range, channel_maxdata),
               (raw == 0 || raw == channel_maxdata) ? " [AT RAIL/OVERRANGE]" : "");
    }

    int ao_flags = comedi_get_subdevice_flags(device, (unsigned int)ao);
    if (ao_flags & SDF_READABLE) {
        lsampl_t readback_raw = 0;
        if (comedi_data_read(device, (unsigned int)ao, ao_channel,
                             (unsigned int)ao_range_index,
                             AREF_GROUND, &readback_raw) == 0) {
            printf("AO readback: raw=%u voltage=%+.9g V\n", (unsigned int)readback_raw,
                   comedi_to_phys(readback_raw, &ao_range, ao_maxdata));
        } else {
            print_comedi_error("AO readback");
        }
    } else {
        printf("AO subdevice has no readable flag; persistence is verified through AI%u or a meter.\n",
               ai_channel);
    }

    printf("Leaving AO%u at %.9g V. Write 0 V explicitly when finished.\n",
           ao_channel, quantized_voltage);
    comedi_close(device);
    return EXIT_SUCCESS;
}
