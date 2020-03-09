#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "test.h"
#include "safe_macros.h"

char *TCID = "gy521_id";
int TST_TOTAL = 3;

#define GY521_SLAVE_ADDRESS     0x68
#define GY521_DEVID_REG_ADDR          0x75
#define DEVID_REG_WHO_AM_I_BIT        6
#define	DEVID_REG_WHO_AM_I_LENGTH     6
 #define GY521_PWR_MGMT_1_REG_ADDR       0x6B
#define PWR1_REG_SLEEP_BIT      6

#define false 0
#define true 1
unsigned char buffer[14];
void GetDeviceID(unsigned char *data);
unsigned char readByte(unsigned char devAddr, unsigned char regAddr, unsigned char length, unsigned char *data);
void EnableSleepMode(unsigned char data);
unsigned char writeByte(unsigned char devAddr, unsigned char regAddr, unsigned char length, unsigned char* data);

main()
{
	//Disable sleep
	EnableSleepMode(false);
	//Get Device ID
	GetDeviceID(buffer);
	tst_resm(TPASS,"Device ID recvd is %x\n",buffer[0]);
	
}
void EnableSleepMode(unsigned char data)
{
	unsigned char b;
    	readByte(GY521_SLAVE_ADDRESS,GY521_PWR_MGMT_1_REG_ADDR, 1,&b);
    	b = (data != 0) ? (b | (1 << PWR1_REG_SLEEP_BIT)) : (b & ~(1 << PWR1_REG_SLEEP_BIT ));
    
	writeByte(GY521_SLAVE_ADDRESS, GY521_PWR_MGMT_1_REG_ADDR, 1,&b);

	
	
}

void GetDeviceID(unsigned char *data)
{
	unsigned char count, b;
        if ((count = readByte(GY521_SLAVE_ADDRESS, GY521_DEVID_REG_ADDR, 1,&b)) != 0) 
        {
        	unsigned char mask = ((1 << DEVID_REG_WHO_AM_I_LENGTH) - 1) << (DEVID_REG_WHO_AM_I_BIT - DEVID_REG_WHO_AM_I_LENGTH + 1);
                b &= mask;
                b >>= (DEVID_REG_WHO_AM_I_BIT - DEVID_REG_WHO_AM_I_LENGTH + 1);
                *data = b;
	}	
}	

unsigned char readByte(unsigned char devAddr, unsigned char regAddr, unsigned char length, unsigned char *data)
{
    unsigned char count = 0;
    int fd = open("/dev/i2c-0", O_RDWR);

    if (fd < 0) {
				tst_brkm(TCONF, NULL, "failed to open device %s\n", strerror(errno));
  
        return(-1);
    }
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
       tst_brkm(TCONF, NULL, "failed to select device %s\n", strerror(errno));
        close(fd);
        return(-1);
    }
    if (write(fd, &regAddr, 1) != 1) {
        tst_brkm(TCONF, NULL, "failed to write to device %s\n", strerror(errno));
        close(fd);
        return(-1);
    }
    
    count = read(fd, data, length);
    if (count < 0) {
        tst_brkm(TCONF, NULL, "failed to read device(%d): %s\n", count,strerror(errno));
        
        close(fd);
        return(-1);
    } else if (count != length) {
        tst_brkm(TCONF, NULL, "Short read  from device, expected %d, got %d\n",  length, count);
         
               close(fd);
        return(-1);
    }
    close(fd);

    return count;
}

unsigned char writeByte(unsigned char devAddr, unsigned char regAddr, unsigned char length, unsigned char* data) 
{
    unsigned char count = 0;
    unsigned char buf[128];
    int fd;

    if (length > 127) {
        tst_brkm(TCONF, NULL, "Byte write count (%d) > 127\n", length);
         
        
    }

    fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0) {
				tst_brkm(TCONF, NULL, "Failed to open device: %s\n", strerror(errno));
        
        return(-1);
    }
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
				tst_brkm(TCONF, NULL, "Failed to select device: %s\n", strerror(errno));
       
        close(fd);
        return(-1);
    }
    buf[0] = regAddr;
    memcpy(buf+1,data,length);
    count = write(fd, buf, length+1);
    if (count < 0) {
				tst_brkm(TCONF, NULL, "Failed to write device(%d): %s\n", count,strerror(errno));
       
       
        close(fd);
        return(-1);
    } else if (count != length+1) {
					tst_brkm(TCONF, NULL, "Short write to device, expected %d, got %d\n", length+1, count);
        close(fd);
        return(-1);
    }
    close(fd);

		return count;
}

