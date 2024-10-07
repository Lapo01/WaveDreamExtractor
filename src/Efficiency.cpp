#include "Evento.h"
#include <iostream>
#include <vector>
using namespace std;

void Efficiency(const string file){

TString FileName= file;
TFile *input = new TFile(FileName,"read");
TTree *tree = (TTree*)input->Get("tree");
Evento e;
Evento *p = &e;

cout<<&e<<'\n';
tree->SetBranchAddress("e", &p); 
int entries = tree->GetEntries();
cout<<"entries "<<entries<<endl;

int Nbin = 200;
int Minbin = 0;
int Maxbin = 4000;

TH1F *histpeak = new TH1F("histpeak","signal peak",100,0,200);
TH1F *histintegral = new TH1F("histintegral","Scintillator Signal Integral",Nbin,Minbin,Maxbin);
TH1F *histintegralFibra = new TH1F("histintegralFibra", "Fiber Signal Integral", Nbin, Minbin, Maxbin);
TH1F *histTriggerDistance = new TH1F("histTriggerDistance", "Time defference scintillator and fiber signal", 301,-150,150);
TH1F *histintegraland = new TH1F("histintegraland", "", Nbin,Minbin,Maxbin);
TH1F *histEfficiency = new TH1F("histEfficiency", "Fiber Efficiency as a function of Energy", Nbin,Minbin,Maxbin);


int minrange = 10; 
int maxrange = 1023;

double thresholdScint = -14e-3;
double thresholdFibra = -19e-3;
double minvoltScint = thresholdScint;
double minvoltFibra = thresholdFibra;
double mintimeScint;
double mintimeFibra;
int minindexScint = 0;
int minindexFibra = 0;

double baseline=0;
double rms = 0;
double peakpeak = 0;
double integral = 0;



int triggerScint = 0;
int triggerFibra = 0;

int infint = 0;
int supint = 1024;

double Nscint = 0;
double Nfibra = 0;
double Efficiency;

double a,b;

for(int i=0; i<entries;i++)
{
    minvoltScint = thresholdScint;
    minvoltFibra = thresholdFibra;
    baseline = 0;
    integral = 0;
    rms = 0;
    
    triggerScint = 0;
    triggerFibra = 0;

    tree->GetEntry(i);

    for(int j=minrange; j<maxrange;j++)
    {
        //cout<<e.channel[ch].Volt[j]<<"  "<<Volt[j]<<"  "<<Time[j]<<endl;
        if(minvoltScint>ADCtoV(e.channel[0].Volt[j]))
        {
            minvoltScint = ADCtoV(e.channel[0].Volt[j]);
            mintimeScint = Second(e.channel[0].Time[j]);
            minindexScint = j;
            triggerScint = 1;
        }
        /*
        if(minvoltFibra>ADCtoV(e.channel[1].Volt[j]))
        {
            minvoltFibra = ADCtoV(e.channel[1].Volt[j]);
            mintimeFibra = Second(e.channel[1].Time[j]);
            minindexFibra = j;
            triggerFibra = 1;
        }
        */
    }
    

    if((triggerScint))
    {

        for(int j = minindexScint-20; j<minindexScint+60;j++)
        {
            if(minvoltFibra>ADCtoV(e.channel[1].Volt[j]))
            {
                minvoltFibra = ADCtoV(e.channel[1].Volt[j]);
                mintimeFibra = Second(e.channel[1].Time[j]);
                minindexFibra = j;
                triggerFibra = 1;
            }
        }

        if(triggerScint){ Nscint++; }
        if(triggerFibra){ Nfibra++;}
        //PEAKPEAK
        for(int j=minrange; j<minindexScint-300;j++)
        {
            baseline += ADCtoV(e.channel[0].Volt[j])/(minindexScint-minrange-300.);
        }
        
        for(int j=minrange; j<minindexScint-300;j++)
        {
            rms += (ADCtoV(e.channel[0].Volt[j])-baseline)*(ADCtoV(e.channel[0].Volt[j])-baseline)/(minindexScint-minrange-300.);
        }
        rms = sqrt(rms);

        peakpeak = baseline - minvoltScint;
        
        //if((peakpeak<1)&(peakpeak>200e-3)){ cout<<i<<endl;}

        //INTEGRAL
        infint = 0;
        for(int j=minindexScint; j>minrange; j--)
        {
            if(ADCtoV(e.channel[0].Volt[j])>baseline-3*rms)
            {
                infint = j;
                break;
            }
        }

        supint = 1023;
        for(int j=minindexScint; j<maxrange; j++)
        {
            if(ADCtoV(e.channel[0].Volt[j])>baseline-3*rms)
            {
                supint = j;
                break;
            }

        }
        
        for(int j= infint; j<supint; j++)
        {
            integral += (ADCtoV(e.channel[0].Volt[j]) + ADCtoV(e.channel[0].Volt[j+1]) - 2.*baseline)*(Second(e.channel[0].Time[j+1]) - Second(e.channel[0].Time[j]))/2.;
        }
        //cout<<integral<<endl;

        //Filling histograms
        histpeak->Fill(peakpeak*1e3);
        histintegral->Fill(-integral*1e12);
        //if(-integral*1e12<450){cout<<i<<endl;}

        if(triggerFibra)
        {
            histintegraland->Fill(-integral*1e12);
            histTriggerDistance->Fill((mintimeFibra - mintimeScint)*1e9);
            //cout<<(mintimeFibra - mintimeScint)*1e9<<endl;
        }

    }   
    if(i == 7093)
    {
        TGraph *graph1 = new TGraph();
        TGraph *graph2 = new TGraph();

        for(int j=minrange; j<maxrange;j++)
        {
            graph1->SetPoint(graph1->GetN(), Nanosecond(e.channel[0].Time[j]), ADCtoV(e.channel[0].Volt[j])*1e3);
            graph2->SetPoint(graph2->GetN(), Nanosecond(e.channel[1].Time[j]), ADCtoV(e.channel[1].Volt[j])*1e3);
        }
        TCanvas *Cgraph = new TCanvas();
        graph1->Draw();
        graph2->Draw("same");

        graph1->GetXaxis()->SetTitle("time [ns]");
        graph1->GetYaxis()->SetTitle("Voltage [mV]");
        graph1->SetMinimum(-500);
        graph1->SetMaximum(500);
        graph1->GetXaxis()->SetRangeUser(0.,Nanosecond(e.channel[0].Time[1023]));

        graph1->SetLineColor(kBlue);
        graph2->SetLineColor(kRed);

        TLegend *legend = new TLegend(0.55,0.8,0.9,0.9);
        legend->AddEntry(graph1, "Scintillator","l");
        legend->AddEntry(graph2, "Fiber","l");
        legend->Draw("same");
    }
}

for(int i=1;i<Maxbin+1;i++)
{
    a = histintegral->GetBinContent(i);
    b = histintegraland->GetBinContent(i);
    if(a!=0)
    {
        histEfficiency->SetBinContent(i, b/a);
        histEfficiency->SetBinError(i, sqrt(b/(a*a) - (b*b)/(a*a*a)));
    }
    if(a==0)
    {
        histEfficiency->SetBinContent(i, 0);
    }
}

cout<<"Nscint = "<<Nscint<<endl;
cout<<"Nfibra = "<<Nfibra<<endl;
cout<<"Average efficiency = "<<Nfibra/Nscint<<endl; 

TCanvas *Cpeak = new TCanvas();
histpeak->Draw();
histpeak->GetXaxis()->SetTitle("peakpeak [mV]");
histpeak->GetYaxis()->SetTitle("counts");

TCanvas *Cintegral = new TCanvas();
histintegral->Draw();
histintegral->GetXaxis()->SetTitle("(-)Integral [mV*ns]");
histintegral->GetYaxis()->SetTitle("counts");

TCanvas *Ctime = new TCanvas();
histTriggerDistance->Draw();
histTriggerDistance->GetXaxis()->SetTitle("time [ns]");
histTriggerDistance->GetYaxis()->SetTitle("counts");

TCanvas *CEfficiency = new TCanvas();
histEfficiency->Draw();
histEfficiency->GetXaxis()->SetTitle("(-)Integral [mV*ns]");
histEfficiency->GetYaxis()->SetTitle("efficiency");
histEfficiency->SetStats(0);

}