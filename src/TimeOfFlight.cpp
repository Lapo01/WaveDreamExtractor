#include "Evento.h"
#include <iostream>
#include <vector>
using namespace std;

void TimeOfFlight(const string file, int ch1, int ch2){

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
TH1F *histtime = new TH1F("histtime", "time difference", 501, -5, 5);

int minrange = 10; 
int maxrange = 1024;

double threshold1 = 0;
double threshold2 = 0;
double minvolt1 = 0;
double minvolt2 = 0;
double mintime1 =0;
double mintime2 =0;
double baseline1=0;
double baseline2=0;
double peakpeak1 = 0;
double peakpeak2 = 0;
double integral = 0;
int minindex1 = 0;
int minindex2 = 0;

bool trigger1 = 0;
bool trigger2 = 0;
int infint = 50;
int supint = 200;

double DeltaT = 0;

int canale1,canale2;

tree->GetEntry(0);
for(int i=0; i<18;i++)
{
    for(int j=0; j<18; j++)
    {
        if(ch1==e.channel[i].NChannel){ canale1 = i;}
        if(ch2==e.channel[i].NChannel){ canale2 = i;}
    }
    
}
ch1 = canale1;
ch2 = canale2;

for(int i=0; i<entries;i++)
{
    minvolt1 = threshold1;
    minvolt2 = threshold2;
    baseline1 = 0;
    baseline2 = 0;




    tree->GetEntry(i);
    for(int j=minrange; j<maxrange;j++)
    {
        if(minvolt1>e.channel[ch1].Volt[j])
        {
            minvolt1 = e.channel[ch1].Volt[j];
            mintime1 = e.channel[ch1].Time[j];
            minindex1 = j;
            trigger1 = 1;
        }
    }

    for(int j=minrange; j<maxrange;j++)
    {
        if(minvolt2>e.channel[ch2].Volt[j])
        {
            minvolt2 = e.channel[ch2].Volt[j];
            mintime2 = e.channel[ch2].Time[j];
            minindex2 = j;
            trigger2 = 1;
        }
    }
    
    if((trigger1)&(trigger2))
    {
        //cout<<i<<endl;
        DeltaT = mintime1-mintime2;
        histtime->Fill((mintime1-mintime2)*1e9);
        //cout<<mintime1<<"  "<<mintime2<<endl;
        //cout<<DeltaT<<endl;

        //PEAKPEAK
        for(int j=minrange; j<minindex1-300;j++)
        {
            baseline1 += e.channel[ch1].Volt[j]/(minindex1-minrange-300.);
        }
        peakpeak1 = baseline1 - minvolt1;
        //histpeak->Fill(peakpeak1*1000);

        for(int j=minrange; j<minindex2-300;j++)
        {
            baseline2 += e.channel[ch2].Volt[j]/(minindex2-minrange-300.);
        }
        peakpeak2 = baseline2 - minvolt2;
        //histpeak->Fill(peakpeak1*1000);
    }   

    if(i == 200)
    {
        TGraph *graph1 = new TGraph();
        for(int j=minrange; j<maxrange;j++)
        {
            graph1->SetPoint(graph1->GetN(), e.channel[ch1].Time[j]*1e9, e.channel[ch1].Volt[j]*1e3);
            graph1->GetXaxis()->SetTitle("time [ns]");
            graph1->GetYaxis()->SetTitle("Voltage [mV]");
        }
        TGraph *graph2 = new TGraph();
        for(int j=minrange; j<maxrange;j++)
        {
            graph2->SetPoint(graph2->GetN(), e.channel[ch2].Time[j]*1e9, e.channel[ch2].Volt[j]*1e3);
            graph2->GetXaxis()->SetTitle("time [ns]");
            graph2->GetYaxis()->SetTitle("Voltage [mV]");
        }
        TCanvas *Cgraph = new TCanvas();
        graph1->SetLineColor(kRed);
        graph1->Draw();
        graph2->Draw("same");
        //graph-> SetMarkerStyle(20); 
    }

}
/*
TCanvas *Cpeak = new TCanvas();
histpeak->Draw();
histpeak->GetXaxis()->SetTitle("peakpeak [mV]");
histpeak->GetYaxis()->SetTitle("counts");

TCanvas *Cintegral = new TCanvas();
histintegral->Draw();
histintegral->GetXaxis()->SetTitle("(-)Integral [mV*ns]");
histintegral->GetYaxis()->SetTitle("counts");
*/

TCanvas *Ctempo = new TCanvas();
histtime->GetXaxis()->SetTitle("time (ch1-ch2) [ns]");
histtime->GetYaxis()->SetTitle("counts");
histtime->Draw();


}