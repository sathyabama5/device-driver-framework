#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <errno.h>
#include <time.h>
#include <linux/watchdog.h>

#include "test.h"
#include "safe_macros.h"

int wdt_fd = -1;
char *TCID = "wdt_test";
int TST_TOTAL = 1;

static char *wdt_dev = "/dev/watchdog";
static int dflag;
static const option_t options[] = {
	{"d:", &dflag, &wdt_dev},
	{NULL, NULL, NULL}
};

static void help(void)
{
	printf("  -d x    wdt device node, default is %s\n",
		wdt_dev);
}

void setgetwdttimeout(void)
{
	int ret;
	// get timeout info of watchdog (try to set it to 15s before)
	int timeout = 15;

	tst_resm(TINFO, "WDT SET TIMEOUT TEST :");	

	ret = ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);
	if(ret == -1)
	{
		tst_resm(TFAIL | TERRNO, "WDIOC_SETTIMEOUT ioctl failed");
		return;
	}
	
	ret = ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
	if(ret == -1)
        {
                tst_resm(TFAIL | TERRNO, "WDIOC_GETTIMEOUT ioctl failed");
                return;
        }

	
	tst_resm(TINFO,"WDT TIMEOUT is %d seconds",timeout);

	tst_resm(TPASS, "WDT SET TIMEOUT TEST Passed");

}

void refresh_wdt(void)
{
	// feed watchdog 3 times with heartbeats
	int i;
	
	int ret;

	tst_resm(TINFO, "WDT REFRESH TEST :");

	for (i = 0; i < 3; i++) 
	{
		ret = ioctl(wdt_fd, WDIOC_KEEPALIVE, 0);
		if(ret == -1)
        	{
                	tst_resm(TFAIL | TERRNO, "WDIOC_KEEPALIVE ioctl failed");
                	return;
        	}

		sleep(10);
	}

	tst_resm(TPASS, "WDT REFRESH TEST Passed");

}

//The watchdog is automatically started once the device is opened. 
//Once the watchdog is started, it needs to get feeded with heartbeats 
//- this is done by writing anything but the character “V” to the device (see watchdog api). 
//“V” is a magic character which will disable the watchdog - so it’s important 
//to know that just closing the device without writing the magic char to it 
//won’t stops the watchdog, quite the contrary will happen and the timer limit 
//will be reached and the reset gets triggered.
int main(int argc, char *argv[])
{
	int ret ; 

	tst_parse_opts(argc, argv, options, help);

	tst_require_root();

	if (access(wdt_dev, F_OK) == -1)
		tst_brkm(TCONF, NULL, "couldn't find wdt device '%s'", wdt_dev);

	//Device opening will enable the watchdog
	wdt_fd = SAFE_OPEN(NULL, wdt_dev, O_RDWR | O_NOCTTY);

	//Set an Get WDT timeout info
	setgetwdttimeout();

	//feed WDT
	refresh_wdt();

	//Disable the watchdog
	ret = write(wdt_fd, "V", 1);

	if(ret !=1)
	{
		tst_resm(TFAIL | TERRNO, "WDT Disable failed");
	}

	//Close the device
	close(wdt_fd);

	tst_resm(TINFO, "WDT Tests Done!");
	tst_exit();
}
