#include "Evento.h"
#include <iostream>
#include <vector>
using namespace std;

void ReadData(const string file){

TString FileName= file;
TFile *input = new TFile(FileName,"read");
TTree *tree = (TTree*)input->Get("tree");
Evento e;
Evento *p = &e;

cout<<&e<<'\n';
tree->SetBranchAddress("e", &p); 
int entries = tree->GetEntries();
cout<<"entries "<<entries<<endl;

TH1F *histpeak = new TH1F("histpeak","signal peak",1000,0,1 );

double threshold = -0.029;
double minvolt = threshold;
double mintime;
double baseline=0;
double peakpeak;
int minindex = 0;
int ch = 1;
bool flag = 1;
for(int i=0; i<entries;i++)
{
    minvolt = threshold;
    baseline = 0;

    tree->GetEntry(i);
    for(int j=0; j<1024;j++)
    {
        if(minvolt>e.channel[ch].Volt[j])
        {
            minvolt = e.channel[ch].Volt[j];
            mintime = e.channel[ch].Time[j];
            minindex = j;
            bool flag = 1;
        }
        else
        {
			flag = 0
		}
    }
    if (flag == 1){
		for(int j=0; j<minindex-300;j++)
		{
			baseline += e.channel[ch].Volt[j]/(minindex-300.);
		}
	
		peakpeak = baseline - minvolt;

		histpeak->Fill(peakpeak);
	}
    //cout<<peakpeak<<endl;
    
}

TCanvas *c = new TCanvas();
histpeak->Draw();


}
