#include <hackrf.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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
/* 
int parse_range(char* s, hackrf_operacake_freq_range* range)
{
        char port[16];
        float min;
        float max;
        int result;

        // Read frequency as a float here to support scientific notation (e.g: 1e6)
        if (sscanf(s, "%15[^:]:%f:%f", port, &min, &max) == 3) {
                result = parse_port(port, &(range->port));
                if (result != HACKRF_SUCCESS) {
                        return result;
                }

                range->freq_min = min;
                range->freq_max = max;
                return HACKRF_SUCCESS;
        }
        return HACKRF_ERROR_INVALID_PARAM;
}
*/
int setupopera(int port)
{
	int opt;
        const char* serial_number = NULL;
//	const char* serial_number="0000000000000000176c63c82a3c1a23";
        uint8_t operacake_address = 0;
	uint8_t operacakes[8];
        bool set_mode = false;
        uint8_t mode=OPERACAKE_MODE_MANUAL;
        uint8_t port_a = 4;
        uint8_t port_b = 4;
	uint8_t range_idx = 2;

	hackrf_operacake_freq_range ranges[2];
	/*result = parse_range(optarg, &ranges[2]);
	if (result != HACKRF_SUCCESS) 
	{
        	fprintf(stderr, "failed to parse range\n");
                return EXIT_FAILURE;
        }*/

//        hackrf_operacake_dwell_time dwell_times[HACKRF_OPERACAKE_MAX_DWELL_TIMES];
//	printf("init\n");
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
//	printf("open by serial\n");
	result = hackrf_open_by_serial(serial_number, &device);


//	printf("get_operacake_boards\n");
	result = hackrf_get_operacake_boards(device, operacakes);
                if (result != HACKRF_SUCCESS) {
                        fprintf(stderr,
                                "hackrf_get_operacake_boards() failed: %s (%d)\n",
                                hackrf_error_name(result),
                                result);
                        return EXIT_FAILURE;
                }

//	printf("set_operacake_mode\n");
	result = hackrf_set_operacake_mode(device, operacake_address, mode);
                if (result != HACKRF_SUCCESS) {
                        fprintf(stderr,
                                "hackrf_set_operacake_mode() failed: %s (%d)\n",
                                hackrf_error_name(result),
                                result);
                        return EXIT_FAILURE;
                }

//	printf("set_operacake_freq ranges\n");
		hackrf_set_operacake_freq_ranges(device, ranges, range_idx);
//	uint8_t port=0;
//	for( port=0; port<=3; port++)
	{
		printf("******* Port %d\n",port);
		if (port >3)
		{
			port_b=port%4;
			//port=4;
		}
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

/*                result = hackrf_set_operacake_freq_ranges(device, ranges, range_idx);
	        if (result) 
		{
                	printf("hackrf_set_operacake_freq_ranges() failed: %s (%d)\n",
	                	hackrf_error_name(result), result);
                        return -1;
        	}
		*/
		//sweep

	}
	result = hackrf_close(device);
        if (result) {
                printf("hackrf_close() failed: %s (%d)\n",
                       hackrf_error_name(result),
                       result);
                return -1;
        }
        hackrf_exit();
	return result;
}

void runsweep(char *antstr)
{
	pid_t parent = getpid();
	pid_t pid = fork();

	if (pid == -1)
	{
	    // error, failed to fork()
	}
	else if (pid > 0)
	{
	    int status;
	    waitpid(pid, &status, 0);
	}
	else
	{
		char *args[]={"/usr/local/bin/hackrf_sweep","-N 2","-f 500:4990", antstr,  NULL};
//		char *args[]={"/usr/local/bin/hackrf_sweep","-N 2","-f 500:990", antstr,  NULL};
//		char *args[]={"/usr/local/bin/hackrf_sweep","-f 400:6000", antstr,  NULL};
	        execv(args[0],args);
		printf("Done with Sweep for %s\n", antstr);
	}	
}
int main()
{
	int port=0;
//	fprintf(stdout, "Run Sweep PORT %d, port");

	char* antstr=NULL;
	while (1)
	{
		for(port=0; port<=6; port++)
		{
			if (port==0)
				antstr="-i Float";
			else if (port==1)
				antstr="-i CS Front";
			else if (port==2)
				antstr="-i CSPR";
			else if (port==3)
				antstr="-i Garage";
			else if (port==4)
				antstr="-i Driveway";
			else if (port==5)
				antstr="-i Laundry";
			else if (port==6)
				antstr="-i New";

			setupopera(port);
			runsweep(antstr);
		}
		if (port==3) port=0;
	}

	return 0;
}
