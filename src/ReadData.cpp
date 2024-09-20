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

TH1F *histpeak = new TH1F("histpeak","signal peak",1000,0,200 );
TH1F *histintegral = new TH1F("histintegral","signal integral",1000,0,400 );

int minrange = 10; 
int maxrange = 1024;

double threshold = 0;
double minvolt = threshold;
double mintime;
double baseline=0;
double rms = 0;
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
    rms = 0;

    tree->GetEntry(i);
    for(int j=minrange; j<maxrange;j++)
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
        for(int j=minrange; j<minindex-300;j++)
        {
            baseline += e.channel[ch].Volt[j]/(minindex-300.);
        }
        
        for(int j=minrange; j<minindex-300;j++)
        {
            rms += (e.channel[ch].Volt[j]-baseline)*(e.channel[ch].Volt[j]-baseline)/(minindex-300.);
        }
        rms = sqrt(rms);

        peakpeak = baseline - minvolt;
        histpeak->Fill(peakpeak*1000);
        //if((peakpeak<1)&(peakpeak>200e-3)){ cout<<i<<endl;}

        //INTEGRAL
        infint = 0;
        for(int j=minindex; j>minrange; j--)
        {
            if(e.channel[ch].Volt[j]>-2*rms)
            {
                infint = j;
                break;
            }
        }

        supint = 1024;
        for(int j=minindex; j<maxrange; j++)
        {
            if(e.channel[ch].Volt[j]>-2*rms)
            {
                supint = j;
                break;
            }

        }
        
        for(int j= infint; j<supint; j++)
        {
            integral += (e.channel[ch].Volt[j] + e.channel[ch].Volt[j+1] - 2.*baseline)*(e.channel[ch].Time[j+1] - e.channel[ch].Time[j])/2.;
        }
        //cout<<integral<<endl;
        histintegral->Fill(-integral*1e12);

    }   
    if(i == 99439)
    {
        cout<<peakpeak<<endl;
        TGraph *graph = new TGraph();
        for(int j=minrange; j<maxrange;j++)
        {
            graph->SetPoint(graph->GetN(), e.channel[ch].Time[j]*1e9, e.channel[ch].Volt[j]*1e3);
            graph->GetXaxis()->SetTitle("time [ns]");
            graph->GetYaxis()->SetTitle("Voltage [mV]");
        }
        TCanvas *Cgraph = new TCanvas();
        graph->Draw();
        //graph-> SetMarkerStyle(20);
        
    }
}

TCanvas *Cpeak = new TCanvas();
histpeak->Draw();
histpeak->GetXaxis()->SetTitle("peakpeak [mV]");
histpeak->GetYaxis()->SetTitle("counts");

TCanvas *Cintegral = new TCanvas();
histintegral->Draw();
histintegral->GetXaxis()->SetTitle("(-)Integral [mV*ns]");
histintegral->GetYaxis()->SetTitle("counts");




}