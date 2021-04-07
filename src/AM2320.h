#ifndef AM2320_H
#define AM2320_H

#include <Arduino.h>
#include "GlobalDefinitions.h"


class AM2320
{
	public:
		AM2320();
		float temp;
		float hum;
		int Read(void);
};

#endif
