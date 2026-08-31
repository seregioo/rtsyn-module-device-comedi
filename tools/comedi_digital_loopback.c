#include <comedilib.h>

#include <stdio.h>
#include <stdlib.h>

static void report_error(const char *operation) {
    fprintf(stderr, "%s failed: %s\n", operation, comedi_strerror(comedi_errno()));
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "/dev/comedi0";
    unsigned int output_channel = argc > 2 ? (unsigned int)strtoul(argv[2], NULL, 10) : 0;
    unsigned int input_channel = argc > 3 ? (unsigned int)strtoul(argv[3], NULL, 10) : 1;

    if (output_channel == input_channel) {
        fprintf(stderr, "Output and input must use different physical DIO channels.\n");
        return EXIT_FAILURE;
    }

    comedi_t *device = comedi_open(path);
    if (!device) {
        report_error("comedi_open");
        return EXIT_FAILURE;
    }

    int subdevice = comedi_find_subdevice_by_type(device, COMEDI_SUBD_DIO, 0);
    if (subdevice < 0) {
        fprintf(stderr, "No DIO subdevice found.\n");
        comedi_close(device);
        return EXIT_FAILURE;
    }
    int channel_count = comedi_get_n_channels(device, (unsigned int)subdevice);
    printf("RTSyn Comedi digital loopback diagnostic revision 1\n");
    printf("device=%s DIO subdevice=%d channels=%d output=DIO%u input=DIO%u\n",
           path, subdevice, channel_count, output_channel, input_channel);
    if (output_channel >= (unsigned int)channel_count ||
        input_channel >= (unsigned int)channel_count) {
        fprintf(stderr, "Requested DIO channel is out of range.\n");
        comedi_close(device);
        return EXIT_FAILURE;
    }

    if (comedi_dio_config(device, (unsigned int)subdevice, output_channel,
                          COMEDI_OUTPUT) < 0) {
        report_error("configure output");
        comedi_close(device);
        return EXIT_FAILURE;
    }
    if (comedi_dio_config(device, (unsigned int)subdevice, input_channel,
                          COMEDI_INPUT) < 0) {
        report_error("configure input");
        comedi_close(device);
        return EXIT_FAILURE;
    }

    int failed = 0;
    for (unsigned int expected = 0; expected <= 1; ++expected) {
        unsigned int observed = 0;
        if (comedi_dio_write(device, (unsigned int)subdevice, output_channel,
                             expected) < 0) {
            report_error("digital write");
            failed = 1;
            break;
        }
        if (comedi_dio_read(device, (unsigned int)subdevice, input_channel,
                            &observed) < 0) {
            report_error("digital read");
            failed = 1;
            break;
        }
        printf("wrote DIO%u=%u, read DIO%u=%u: %s\n", output_channel, expected,
               input_channel, observed, observed == expected ? "PASS" : "FAIL");
        if (observed != expected) {
            failed = 1;
        }
    }

    (void)comedi_dio_write(device, (unsigned int)subdevice, output_channel, 0);
    printf("DIO%u reset to 0.\n", output_channel);
    comedi_close(device);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
