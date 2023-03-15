#include <hackrf.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#ifndef bool
typedef int bool;
        #define true 1
        #define false 0
#endif

#define FREQ_MIN_MHZ (650)    /* 650 MHz */
#define FREQ_MAX_MHZ (7250)  /* 7250 MHz */
#define INVALID_PORT    0xFF

hackrf_device* device = NULL;
int result;

int setup()
{
	int opt;
        const char* serial_number = NULL;
        uint8_t operacake_address = 0;
	uint8_t operacakes[8];
        bool set_mode = false;
        uint8_t mode=OPERACAKE_MODE_MANUAL;
        uint8_t port_a = INVALID_PORT;
        uint8_t port_b = INVALID_PORT;
	uint8_t range_idx = 0;

	hackrf_operacake_freq_range ranges[2];
//        hackrf_operacake_dwell_time dwell_times[HACKRF_OPERACAKE_MAX_DWELL_TIMES];
	result = hackrf_open_by_serial(serial_number, &device);
	hackrf_device* device = NULL;
	int result = hackrf_init();
        if (result) {
                printf("hackrf_init() failed: %s (%d)\n",
                       hackrf_error_name(result),
                       result);
                return -1;
        }
        if (result != HACKRF_SUCCESS) {
                fprintf(stderr,
                        "hackrf_open() failed: %s (%d)\n",
                        hackrf_error_name(result),
                        result);
                return EXIT_FAILURE;
	}      

	result = hackrf_get_operacake_boards(device, operacakes);
                if (result != HACKRF_SUCCESS) {
                        fprintf(stderr,
                                "hackrf_get_operacake_boards() failed: %s (%d)\n",
                                hackrf_error_name(result),
                                result);
                        return EXIT_FAILURE;
                }

	result = hackrf_set_operacake_mode(device, operacake_address, mode);
                if (result != HACKRF_SUCCESS) {
                        fprintf(stderr,
                                "hackrf_set_operacake_mode() failed: %s (%d)\n",
                                hackrf_error_name(result),
                                result);
                        return EXIT_FAILURE;
                }
		hackrf_set_operacake_freq_ranges(device, ranges, range_idx);
	uint8_t port=0;
	for( port=0; port<=3; port++)
	{
		result = hackrf_set_operacake_ports(
                        device,
                        operacake_address,
                        port,
                        port_b);
                if (result) 
		{
                        printf("hackrf_set_operacake_ports() failed: %s (%d)\n",
                               hackrf_error_name(result),
                               result);
                        return EXIT_FAILURE;
                }
                result = hackrf_set_operacake_freq_ranges(device, ranges, range_idx);
	        if (result) 
		{
                	printf("hackrf_set_operacake_freq_ranges() failed: %s (%d)\n",
	                	hackrf_error_name(result), result);
                        return -1;
        	}
	}
	return result;
}

int main()
{
	setup();
	result = hackrf_close(device);
        if (result) {
                printf("hackrf_close() failed: %s (%d)\n",
                       hackrf_error_name(result),
                       result);
                return -1;
        }
        hackrf_exit();

	return 0;
}
