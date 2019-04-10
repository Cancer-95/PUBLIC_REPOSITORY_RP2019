#include "system.h"

int main(void)
{
	system_init();
	
	while(1)
	{
		loop();
	}
}
