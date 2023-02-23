#include <Bela.h>
#include <libraries/Oscillator/Oscillator.h>
#include <VL53L0X.hpp>

VL53L0X sensor;
Oscillator osc;

uint16_t gDistance;
void readSensor(void*)
{
	sensor.setTimeout(200);
	while(!Bela_stopRequested())
	{
		gDistance = sensor.readRangeSingleMillimeters();
		if (sensor.timeoutOccurred()) {
			fprintf(stderr, "VL530X TIMEOUT OCCURRED\n");
		}
		usleep(50000);
	}
}

bool setup(BelaContext* context, void*)
{
	osc.setup(context->audioSampleRate);
	sensor.initialize();
	Bela_runAuxiliaryTask(readSensor);
	return true;
}

void render(BelaContext* context, void*)
{
	float relDistance = gDistance / 65536.0;
	float freq = map(relDistance, 0, 1, 100, 1000);
	for(unsigned int n = 0; n < context->audioFrames; ++n)
	{
		float out = osc.process(freq);
		for(unsigned int c = 0; c < context->audioOutChannels; ++c)
			audioWrite(context, n, c, out);
	}
}

void cleanup(BelaContext* context, void*)
{
}
