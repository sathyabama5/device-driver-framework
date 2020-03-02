#include <stdio.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
//#include <linux/i2c.h>
#include <i2c/smbus.h>
#include "test.h"
#include "safe_macros.h"

#define MPU_TEMP1 0x41
#define MPU_TEMP2 0x42

#define MPU_POWER1 0x6b
#define MPU_POWER2 0x6c


char *TCID = "gy521_test";
int TST_TOTAL = 3;


int main(int argc, char *argv[])
{
    int gy521_fd ;
		//float data;
		char buffer[10]={0};
    char *fileName = "/dev/i2c-0";
    int  address = 0x68;
	  if (access(fileName,F_OK) == -1)
    {
        tst_brkm(TCONF, NULL, "couldn't find gy-521 device '%s'", fileName);
    }
	  gy521_fd = SAFE_OPEN(NULL, fileName, O_RDWR);
		if (ioctl(gy521_fd, I2C_SLAVE, address) < 0) 
		{
       tst_resm(TFAIL | TERRNO, "I2C_SLAVE ioctl failed");
			 exit(1);        
    }
    /*int8_t power = i2c_smbus_read_byte_data(gy521_fd, MPU_POWER1);
    i2c_smbus_write_byte_data(gy521_fd, MPU_POWER1, ~(1 << 6) & power);*/

    //while (1) 
		for (int i = 0; i<8; i++) 
		{
			if (read(gy521_fd,buffer,1) != 1) 
			{
				tst_resm(TFAIL | TERRNO, "I2C read failed");
			}
			else {
         int16_t data = (float)((buffer[0])<<8)+buffer[1];
        //data = data/4096*5;
        //channel = ((buffer[0] & 0b00110000)>>4);
				//tst_resm(TPASS,"Channel s%02d TEMPERATURE DATA:  %04f\n",channel,data);
				tst_resm(TPASS,"gy-521 test passed");				
				tst_resm(TINFO,"temp: %f\n", (float)temp / 340.0f + 36.53);
				}


		/*{
        int16_t temp = i2c_smbus_read_byte_data(gy521_fd, MPU_TEMP1) << 8 |
                        i2c_smbus_read_byte_data(gy521_fd, MPU_TEMP2);

       
				tst_resm(TPASS,"gy-521 test passed");				
				tst_resm(TINFO,"temp: %f\n", (float)temp / 340.0f + 36.53);*/
       
    }
return 0;
}

































































