#include <iostream>
#include <vector>
using namespace std;

class Channel
{
public: 
	uint8_t NChannel;
	uint16_t Time[1024];
	uint16_t Volt[1024];
	
};


class Evento
{
public:
	int NEvento;
	vector<Channel> channel;
};


double ADCconversion(int &adc)
{
	
	return  adc/65536. - 0.5;
}

/*
double Timeconversion(int index)
{
	int Time = 0;
	for(int j = 0; j < index; j++)
	{
		Time +=
	}
}
*/