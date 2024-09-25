#include <iostream>
#include <vector>
using namespace std;

class Channel
{
public: 
	int NChannel;
	double Time[1024];
	int Volt[1024];
	
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
