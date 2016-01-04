#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string> 
#include <unistd.h>

#define MPU6050 0x68
#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75
#define TEMP_H 0x41
#define TEMP_L 0x42

using namespace std;

int main(int argc, char **argv)	//argc is the number of arguments pointed to by the argument vector argv
{

	int err = 0;	//Variable for storing error codes

	const char * device = "/dev/i2c-0";

	char buf[10];	//Buffer for storing data

	//Open the i2c connection
	int fd = open(device, O_RDWR);

	//Check if device open
	if(fd == -1)
	{
		cout << "i2c bus could not open" << endl;
		exit(1);
	}
	else
	{
		cout << "i2c bus is open" << endl;
	}

	//Specify slave address
	err = ioctl(fd, I2C_SLAVE, MPU6050);

	//test if bus is ok
	if(err < 0)
	{
		cout << "Could not aquire bus access" << endl;
		exit(1);
	}

	//Write to slave
	buf[0] = WHO_AM_I;
	write(fd, buf, 1);
	buf[0] = 0x01;

	//Read from slave
	read(fd,buf,1);

	if(buf[0] == 0x68)
	{
		cout << "Connected to device" << endl;
	}
	else
	{
		cout << "No connection to device" << endl;
	}

	//Wake up device by writing 0x00 to power mgmt reg 1
	buf[0] = PWR_MGMT_1;
	buf[1] = 0x00;
	write(fd, buf, 2);

	//Check if device is running
	buf[0] = PWR_MGMT_1;
	write(fd, buf, 1);
	read(fd, buf, 1);

	if( buf[0] == 0x00 )
	{
		cout << "Device is awake!" << endl;
	}
	else
	{
		cout << "Could not wake the device, exiting program" << endl;
		exit(1);
	}

	int temp_l, temp_h;
	float real_temp, temp_var;

	while(1)
	{
	//Read temperature sensor
	buf[0] = TEMP_H;
	write(fd, buf, 1);
	read(fd, buf, 1);

	temp_h = buf[0];

	buf[0] = TEMP_L;
	write(fd, buf, 1);
	read(fd, buf, 1);

	temp_l = buf[0];

	temp_var = (((temp_h * 256) + temp_l) - 65526);

	real_temp = temp_var / 340;

	real_temp = real_temp + 36.53;

	printf("Temperature: %f\n", real_temp);
	usleep(500000);
	}
	//printf("%d\n",temp_l);
	//printf("%d\n",temp_h);


}
