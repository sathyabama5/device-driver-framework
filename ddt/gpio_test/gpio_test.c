#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <errno.h>
#include <time.h>

#include "test.h"
#include "safe_macros.h"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

char *TCID = "gpio_test";
int TST_TOTAL = 1;

static const option_t options[] = {
	{NULL, NULL, NULL},
	{NULL, NULL, NULL}
};

static void help(void)
{
}


static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		tst_brkm(TCONF, NULL, "Failed to open export for writing!!!\n");
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		tst_brkm(TCONF, NULL, "Failed to open unexport for writing!!!\n");
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		tst_brkm(TCONF, NULL, "Failed to open gpio direction for writing!!\n");

	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		tst_brkm(TCONF, NULL, "Failed to set direction!\n");
	}

	close(fd);
	return(0);
}

static int GPIORead(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		tst_brkm(TCONF, NULL, "Failed to open gpio value for reading!\n");
	}

	if (-1 == read(fd, value_str, 3)) {
		tst_brkm(TCONF, NULL, "Failed to read value!\n");
	}

	close(fd);

	return(atoi(value_str));
}

static int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		tst_brkm(TCONF, NULL, "Failed to open gpio value for writing!\n");
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		tst_brkm(TCONF, NULL, "Failed to write value!\n");
	}

	close(fd);
	return(0);
}

int main(int argc, char *argv[])
{
	tst_parse_opts(argc, argv, options, help);

	tst_require_root();

	int PIN = atoi(argv[1]);
	int direction = atoi(argv[2]);
	int loop = atoi(argv[3]);

	int gpio_read_value = 0;
	int i = 0;

	/*
	 * Enable GPIO pins
	 */
	GPIOExport(PIN);

	/*
         * Set GPIO directions
         */

	if(direction == 0)
	{
		GPIODirection(PIN, IN);
		for(i = 0;i<loop;i++)
		{	
			printf("Read %d in GPIO %d\n", GPIORead(PIN), PIN);	
		}
	}else
	{
		GPIODirection(PIN, OUT);
		do {
                /*
                 * Write GPIO value
                 */
                	GPIOWrite(PIN, loop%2);

                	usleep(500 * 1000);

                	/*
                 	* Read GPIO value
                 	*/
                	gpio_read_value = GPIORead(PIN);

                	if(gpio_read_value != loop%2)
                	{
                        	tst_brkm(TCONF, NULL, "GPIO Test Failed!\n");
                	}
        	}while (loop--);
		
	}

	/*
	 * Disable GPIO pins
	 */
	GPIOUnexport(PIN);

	tst_resm(TINFO, "GPIO Tests Passed!");
	tst_resm(TINFO, "GPIO Tests Done!");
	tst_exit();
}
