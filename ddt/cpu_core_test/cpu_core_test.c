#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <errno.h>
#include <time.h>
#include <fnmatch.h>

#include "test.h"
#include "safe_macros.h"

#define LINUX_SYS_CPU_DIRECTORY "/sys/devices/system/cpu"

char *TCID = "cpu_core_test";
int TST_TOTAL = 1;

static const option_t options[] = {
	{NULL, NULL, NULL},
	{NULL, NULL, NULL}
};

static void help(void)
{


}

int main(int argc, char *argv[])
{
	int cpu_count = 0;

	tst_parse_opts(argc, argv, options, help);

	tst_require_root();

	tst_resm(TINFO, "CPU Core Test!");

	DIR *sys_cpu_dir = opendir(LINUX_SYS_CPU_DIRECTORY);
   
	if (sys_cpu_dir == NULL) 
	{
		tst_brkm(TCONF,NULL,"Cannot open %s directory",LINUX_SYS_CPU_DIRECTORY);
   	}
   
	const struct dirent *cpu_dir;
   	
	while((cpu_dir = readdir(sys_cpu_dir)) != NULL) 
	{
       		if (fnmatch("cpu[0-9]*", cpu_dir->d_name, 0) != 0)
       		{
          		/* Skip the file which does not represent a CPU */
          		continue;
       		}
       		
		cpu_count++;
   	}
   
	tst_resm(TINFO,"CPU count: %d\n",cpu_count);

	if(cpu_count == 4)
	{	
		tst_resm(TINFO, "CPU Core Test Passed!");
	}else
	{
		tst_resm(TINFO, "CPU Core Test Failed!");
	}


	tst_resm(TINFO, "CPU Core Test Done!");
	tst_exit();

}
