/*   rtc-test.c
 *    *
 *    */

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

int rtc_fd = -1;
char *TCID = "rtc_test";
int TST_TOTAL = 3;

struct rtc_time rtc_tm;
struct tm* local;


static char *rtc_dev = "/dev/rtc0";
static int dflag;
static const option_t options[] = {
	{"d:", &dflag,&rtc_dev},
	{NULL, NULL, NULL}
};


static void help(void)
{
	printf("  -d x    rtc device node, default is %s\n",rtc_dev);
}

/*Read RTC Time */
void read_rtc_time(void)
{
	int ret;
	tst_resm(TINFO, "RTC READ TEST:");
	ret = ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm);
	if (ret == -1) 
	{
	tst_resm(TFAIL | TERRNO, "RTC_RD_TIME ioctl failed");
	return;
	}
	tst_resm(TPASS, "RTC READ TEST Passed");
	tst_resm(TINFO, "Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
}

	
/*Read System Time*/
void read_system_time(void)
{	
	time_t t=time(NULL);
	local=localtime(&t);
	tst_resm(TINFO, "SYSTEM TIME : %s",asctime(local));
	tst_resm(TPASS,"SYSTEM TIME READ TEST PASSED");
}

void compare_rtc_and_system_time(void)
{
		
	if((local->tm_hour)==(rtc_tm.tm_hour) ){
	tst_resm(TINFO,"value of local->tm_hour and rtc_tm.tm_hour is %d and %d",local->tm_hour,rtc_tm.tm_hour);
	tst_resm(TPASS,"RTC & SYSTEM TIME ARE EQUAL");
	}
	else{
	tst_resm(TFAIL,"RTC & SYSTEM TIME ARE NOT EQUAL");
	}
	if((local->tm_mday)==(rtc_tm.tm_mday)){
	tst_resm(TPASS,"RTC & SYSTEM DATES ARE EQUAL");
	}
	else{
	tst_resm(TFAIL,"RTC & SYSTEM DATES ARE NOT EQUAL");
	}
}



int main(int argc, char *argv[])
{
	tst_parse_opts(argc, argv, options, help);
	tst_require_root();
	if (access(rtc_dev, F_OK) == -1)
	tst_brkm(TCONF, NULL, "couldn't find rtc device '%s'", rtc_dev);
	rtc_fd = SAFE_OPEN(NULL, rtc_dev, O_RDONLY);
						
	read_rtc_time();
	read_system_time();
	compare_rtc_and_system_time();
					
	close(rtc_fd);

	tst_resm(TINFO, "RTC Tests Done!");
	tst_exit();
}


