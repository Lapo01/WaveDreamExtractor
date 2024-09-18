#include "Evento.h"
#include <iostream>
#include <vector>
using namespace std;

void ReadData(const string file, int ch){

TString FileName= file;
TFile *input = new TFile(FileName,"read");
TTree *tree = (TTree*)input->Get("tree");
Evento e;
Evento *p = &e;

cout<<&e<<'\n';
tree->SetBranchAddress("e", &p); 
int entries = tree->GetEntries();
cout<<"entries "<<entries<<endl;

TH1F *histpeak = new TH1F("histpeak","signal peak",1000,0,300 );
TH1F *histintegral = new TH1F("histintegral","signal integral",1000,-10,1 );

double threshold = 0;
double minvolt = threshold;
double mintime;
double baseline=0;
double peakpeak = 0;
double integral = 0;
int minindex = 0;

bool trigger = 0;
int infint = 50;
int supint = 200;

for(int i=0; i<entries;i++)
{
    minvolt = threshold;
    baseline = 0;
    integral = 0;

    tree->GetEntry(i);
    for(int j=0; j<1024;j++)
    {
        if(minvolt>e.channel[ch].Volt[j])
        {
            minvolt = e.channel[ch].Volt[j];
            mintime = e.channel[ch].Time[j];
            minindex = j;
            trigger = 1;
        }
    }
    
    if(trigger)
    {
        //PEAKPEAK
        for(int j=0; j<minindex-300;j++)
        {
            baseline += e.channel[ch].Volt[j]/(minindex-300.);
        }
        peakpeak = baseline - minvolt;

        histpeak->Fill(peakpeak*1000);

        //TRIGGER
        for(int j= minindex-infint; j<minindex+supint; j++)
        {
            integral += (e.channel[ch].Volt[j] + e.channel[ch].Volt[j+1] - 2.*baseline)*(e.channel[ch].Time[j+1] - e.channel[ch].Time[j])/2.;
        }
        //cout<<integral<<endl;
        histintegral->Fill(integral*1e10);

    }   
}

TCanvas *Cpeak = new TCanvas();
histpeak->Draw();
histpeak->GetXaxis()->SetTitle("peakpeak [mV]");
histpeak->GetYaxis()->SetTitle("counts");

TCanvas *Cintegral = new TCanvas();
histintegral->Draw();
histintegral->GetXaxis()->SetTitle("Integral [V*t*10^(-10)]");
histintegral->GetYaxis()->SetTitle("counts");


}