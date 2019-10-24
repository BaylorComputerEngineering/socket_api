#include <stdio.h>
#include <string.h>
#include "socket_api.h"
#include "BBBiolib.h"

/* ----------------------------------------------------------- */
#define BUFFER_SIZE 100
#define SAMPLE_SIZE 10
/* ----------------------------------------------------------- */

void initialize_adc_using_bbbio(void);
int read_adc_value_using_bbbio(void);
void initialize_pwm(void);
void set_duty_cycle(int);
void run_server(int);
void run_client(int, char *, char *);
void extract_value(char buffer[256], char *attribute, char *attribute_value);

const int PWM_PERIOD = 500000;
const int MAX_PORT_NUMBER = 65535;

typedef enum
{
    SERVER,
    CLIENT
}ROLE_TYPE;


int main(int argc, char *argv[])
{
    ROLE_TYPE role = SERVER;
    char client_ip[100];
    char server_ip[100];

    int port = -1;
    
     ROLE_TYPE role = SERVER;
    char client_ip[100];
    char server_ip[100];

    int port = -1;
    if(argc < 3)
    {
        printf("ERROR: Less than 2 arguments: exiting\n");
        return -1;
    }
    else
    {
        if(!strcmp(argv[1], "SERVER"))
            role = SERVER;
        else if(!strcmp(argv[1], "CLIENT"))
            role = CLIENT;
        else
        {
            printf("ERROR: No valid role defined: exiting\n");
            return -1;
        }
        port = atoi(argv[2]);
        if(port < 0 || port > MAX_PORT_NUMBER)
        {
            printf("ERROR: Invalid port number defined: exiting\n");
            return -1;
        }
        if(role == CLIENT)
        {
            if(argc < 5)
            {
                printf("ERROR: IP Addresses not defined: exiting\n");
                return -1;
            }
            else
            {
                strncpy(client_ip, argv[3], sizeof(client_ip));
                strncpy(server_ip, argv[4], sizeof(server_ip));
                printf("Client IP = %s Server IP = %s\n", client_ip, server_ip);
            }
        }
        printf("Port = %d\n", port);
    }
    
    if(role == SERVER)
        run_server(port);
    else if(role == CLIENT)
        run_client(port, client_ip, server_ip);
    else
        printf("ERROR: Neither server nor client\n");
    return 0;
}

void run_server(int port)
{
 	printf("Starting Server\n");
 	char buffer[256];
	
	// initialize anything that needs to be used by the server
	
	while(1)
	{
		// reset the buffer to all null characters '\0' every time through the loop using the memset function
	    
		// read ADC values and send them to the client using the socket_api

		// delay 100 ms
		usleep(100000);	
	}
   
}

void run_client(int port, char *client_ip, char *server_ip)
{
	char buffer[256];
	// initialize anything that needs to be used by the client
    
    while(1)
    {
        // reset the buffer to all null characters '\0' every time through the loop using the memset function
        
        // receive data from the socket and the PWM duty cycle based on the received value

		// delay 1 ms    	
    	usleep(1000);
    }

}

void initialize_adc_using_bbbio()
{
	unsigned int sample;
	int i ,j;
	unsigned int buffer_AIN_0[BUFFER_SIZE] ={0};

	/* BBBIOlib init*/
	iolib_init();

	/* using ADC_CALC toolkit to decide the ADC module argument . Example Sample rate : 10000 sample/s
	 *
	 *	#./ADC_CALC -f 10000 -t 5
	 *
	 *	Suggest Solution :
	 *		Clock Divider : 160 ,   Open Dly : 0 ,  Sample Average : 1 ,    Sample Dly : 1
	 *
	 */
//	const int clk_div = 34 ;
	const int clk_div = 160;
	const int open_dly = 0;
	const int sample_dly = 1;

	/*ADC work mode : Timer interrupt mode
	 *	Note : This mode handle SIGALRM using signale() function in BBBIO_ADCTSC_work();
	 */
	BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_TIMER_INT, clk_div);

	/*ADC work mode : Busy polling mode  */
	/* BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_BUSY_POLLING, clk_div);*/


	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN0, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, open_dly, sample_dly, \
				BBBIO_ADC_STEP_AVG_1, buffer_AIN_0, BUFFER_SIZE);

				
	BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN0);
}

int read_adc_value_using_bbbio(void)
{
	int ADC_value = 0;
	unsigned int sample;
	int i ,j;
	unsigned int buffer_AIN_0[BUFFER_SIZE] ={0};

	BBBIO_ADCTSC_work(SAMPLE_SIZE);

	for(j = 0 ; j < SAMPLE_SIZE ; j++) 
	{
		sample = buffer_AIN_0[j];
//		printf("\t[sample : %d , %f v]\n", sample, ((float)sample / 4095.0f) * 1.8f);
	}
		
	ADC_value = sample;

	return(ADC_value);  
}

void initialize_pwm()
{
	FILE *fp;
	char *mode = "r+b";

	//Configure P9_14 as a PWM pin 
	system("config-pin P9.14 pwm");
	//Wait for 10 ms to allow changes to take effect
	usleep(10000);

	//Check to see if the PWM0 directory has been created by attempting 
	//to open the pwm0/enable file. If the file exists, then you should
	//not echo 0 to the export file. If it does not exist, then you need
	//to echo 0 to the export file to create the pwm0 directory
	fp=fopen("/sys/class/pwm/pwmchip2/pwm0/enable",mode);

	if (fp==NULL)
	{
	printf("Creating PWM0 for the first time\n");
	system("echo 0 > /sys/class/pwm/pwmchip2/export");
	//Wait for 10 ms to allow changes to take effect
	usleep(10000);
	}
	else
	{
	fclose(fp);
	}

	//Turn the pwm on
	system("echo 1 > /sys/class/pwm/pwmchip2/pwm0/enable");
	//Wait for 10 ms to allow changes to take effect
	usleep(10000);

	//Set the PWM PWM_PERIOD to 500000
	system("echo 500000 > /sys/class/pwm/pwmchip2/pwm0/period");
	//Wait for 10 ms to allow changes to take effect 
	usleep(10000);
	
	system("echo 0 > /sys/class/pwm/pwmchip2/pwm0/duty_cycle");
	//Wait for 10 ms to allow changes to take effect 
	usleep(10000);
}

void set_duty_cycle(int duty_cycle)
{
	if ((duty_cycle < 0) | (duty_cycle > PWM_PERIOD))
	{
	printf("Error. Duty cycle out of range.\n");
	}

	else
	{
	char buffer [100];
	sprintf(buffer, "echo %d >/sys/class/pwm/pwmchip2/pwm0/duty_cycle", duty_cycle);
	system(buffer);
	}
}