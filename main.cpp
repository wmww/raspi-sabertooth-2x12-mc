#include <iostream>
#include <unistd.h>
#include "SerialMC.h"

int main(int argc, char** argv)
{
	SerialMC mc;
	mc.set(1.0, -0.5);
	usleep(1.0 * 1000000);
	mc.turnOff();
	return 0;
}
