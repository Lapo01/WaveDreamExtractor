#include "Evento.h"
#include "TString.h"
#include "TTree.h"
#include "TGraph.h"
#include "TFile.h"




void ExampleRead(TString filename){
	Evento e;
	Evento *p = &e;
	double waveform[0][1024], time[0][1024];

	TFile *input = new TFile(filename,"READ");
	TTree *tree = (TTree)* input->Get("tree");
	tree->SetBranchAddress("e", &e);
	int entries = tree->GetEntries();
	
	tree->GetEntry(1);
	
	TGraph *g = new TGraph(1024, (double *) time[0], (double *) waveform[0]);
	for(int j=0; j++; j<1024){
		g->SetPoint(j, e.channel[0].Time[j], e.channel[0].Volt[j]);
	}
	g->Draw();
	
	
	
	
	
}
