/*
 
 Name:           read_binary.C
 Created by:     Stefan Ritt <stefan.ritt@psi.ch>
 Date:           December 21st, 2016
 
 Purpose:        Example program under ROOT to read a binary data file written
 by the DRSOsc program. Decode time and voltages from waveforms
 and display them as a graph. Put values into a ROOT Tree for
 further analysis.
 
 To run it, do:
 
 - Crate a file test.dat via the "Save" button in the browser connected
 to the wds server
 - start ROOT
 root [0] .L read_binary.C+
 root [1] decode("test.dat");
 
 */


#include <string.h>
#include <stdio.h>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "Getline.h"

/*
 * Added custom class to produce output
 */
#include "Evento.h"


#define NBOARDS 16

typedef struct {
   char tag[3];
   char version;
} FHEADER;

typedef struct {
   char time_header[4];
} THEADER;

typedef struct {
   char bn[2];
   unsigned short board_serial_number;
} BHEADER;

typedef struct {
   char event_header[4];
   unsigned int event_serial_number;
   unsigned short year;
   unsigned short month;
   unsigned short day;
   unsigned short hour;
   unsigned short minute;
   unsigned short second;
   unsigned short millisecond;
   unsigned short range;
} EHEADER;

typedef struct {
   char tc[2];
   unsigned short trigger_cell;
} TCHEADER;


typedef struct {
   char c[1];
   char cn[3];
} CHEADER;

/*-----------------------------------------------------------------------------*/

void decode(const char *filename) {
   FHEADER fh;
   THEADER th;
   BHEADER bh;
   EHEADER eh;
   TCHEADER tch;
   CHEADER ch;

   unsigned int scaler;
   unsigned short voltage[1024];
   double waveform[NBOARDS][18][1024], time[NBOARDS][18][1024];
   float adc_waveform[NBOARDS][16][2048];
   unsigned char tdc_waveform[NBOARDS][16][512];
   unsigned long trg_data[NBOARDS][512];
   float bin_width[NBOARDS][18][1024];
   int i, j, b, chn, n, chn_index, n_boards;
   double t1, t2, dt;
   double amplitude[18];

   // open the binary waveform file
   FILE *f = fopen(Form("%s", filename), "r");
   if (f == NULL) {
      printf("Cannot find file \'%s\'\n", filename);
      return;
   }

	
	
	
	
	
   // mia modifica per leggere il file  efare un output
   TFile *fnew = new TFile("output.root", "recreate");
   Evento e;
   Channel Placeholder;
   TTree *tree = new TTree("e", "e");
   tree->Branch("e",&e);
   
   
   
   
   // define the rec tree
   TTree *rec = new TTree("rec", "rec");
   rec->Branch("amp0", &amplitude[0], "amp0/D");

   
   
   
   
   
   // create canvas
   TCanvas *c1 = new TCanvas();

   // create graph
   TGraph *g = new TGraph(1024, (double *) time[0], (double *) waveform[0]);

   // read file header
   fread(&fh, sizeof(fh), 1, f);
   if (fh.tag[0] != 'D' || fh.tag[1] != 'R' || fh.tag[2] != 'S') {
      printf("Found invalid file header in file \'%s\', aborting.\n", filename);
      return;
   }

   if (fh.version != '8') {
      printf("Found invalid file version \'%c\' in file \'%s\', should be \'8\', aborting.\n", fh.version, filename);
      return;
   }

   // read time header
   fread(&th, sizeof(th), 1, f);
   if (memcmp(th.time_header, "TIME", 4) != 0) {
      printf("Invalid time header in file \'%s\', aborting.\n", filename);
      return;
   }

   for (b = 0;; b++) {
      // read board header
      fread(&bh, sizeof(bh), 1, f);
      if (memcmp(bh.bn, "B#", 2) != 0) {
         // probably event header found
         fseek(f, -4, SEEK_CUR);
         break;
      }

      printf("Found data for board #%d\n", bh.board_serial_number);

      // read time bin widths
      memset(bin_width[b], sizeof(bin_width[0]), 0);
      for (chn = 0; chn < 18; chn++) {
         fread(&ch, sizeof(ch), 1, f);
         if (ch.c[0] != 'C') {
            // event header found
            fseek(f, -4, SEEK_CUR);
            break;
         }
         i = (ch.cn[1] - '0') * 10 + ch.cn[2] - '0';
         printf("Found timing calibration for channel #%d\n", i);
         fread(&bin_width[b][i][0], sizeof(float), 1024, f);
      }
   }
   n_boards = b;

   // loop over all events in data file
   for (n = 0; ; n++) {
      // read event header
      i = fread(&eh, sizeof(eh), 1, f);
      if (i < 1)
         break;
         
         
         
      //fill custom object with ID   
      e.channel.clear();
      e.NEvento = eh.event_serial_number;
	  // end customization
	  
	  
	  
      printf("Found event #%d\r", eh.event_serial_number);
      fflush(stdout);

      // loop over all boards in data file
      for (b = 0; b < n_boards; b++) {

         // read board header
         fread(&bh, sizeof(bh), 1, f);
         if (memcmp(bh.bn, "B#", 2) != 0) {
            printf("Invalid board header in file \'%s\', aborting.\n", filename);
            return;
         }

         if (n_boards > 1)
            printf("Found data for board #%d\n", bh.board_serial_number);

         // reach channel data
         for (chn=0 ; chn<18 ; chn++) {

			//specify in tree the channel number
			Placeholder.NChannel = chn;
			
			


            // read channel header
            fread(&ch, sizeof(ch), 1, f);
            if (ch.c[0] == 'E') {
               // event header found
               fseek(f, -4, SEEK_CUR);
               break;
            }
            chn_index = (ch.cn[1] - '0')*10 + ch.cn[2] - '0';

            if (ch.c[0] == 'C') {

               // Read DRS data
               fread(&scaler, sizeof(int), 1, f);

               // read trigger cell
               fread(&tch, sizeof(tch), 1, f);
               if (memcmp(tch.tc, "T#", 2) != 0) {
                  printf("Invalid trigger cell header in file \'%s\', aborting.\n", filename);
                  return;
               }

               fread(voltage, sizeof(short), 1024, f);
               for (i = 0; i < 1024; i++) {
                  // convert data to volts
                  waveform[b][chn_index][i] = (voltage[i] / 65536. + eh.range / 1000.0 - 0.5);

                  // calculate time for this cell
                  for (j = 0, time[b][chn_index][i] = 0; j < i; j++)
                     time[b][chn_index][i] += bin_width[b][chn_index][(j + tch.trigger_cell) % 1024];
                  
                  
                  
               }
               for(int i = 0; i++; i<1024){ 
					Placeholder.Time[i] = waveform[b][chn_index][i];
					Placeholder.Volt[i] = time[b][chn_index][i];
				}
				e.channel.push_back(Placeholder);

            } 
            
            else if (ch.c[0] == 'A') {

               // Read ADC data
               short adc_voltage[2048];
               fread(adc_voltage, sizeof(short), 2048, f);
               for (i = 0; i < 2048; i++) {
                  // convert data to volts
                  adc_waveform[b][chn_index][i] = (adc_voltage[i] / 65536. + eh.range / 1000.0 - 0.5);
               }

            } else if (ch.c[0] == 'T') {

               if (ch.cn[0] == '0') {
                  // Read TDC
                  fread(tdc_waveform[b][chn_index], sizeof(unsigned char), 512, f);
               } else {
                  // Read TRG
                  fread(trg_data[b], sizeof(unsigned long), 512, f);
               }
            }
         }

         // align cell #0 of all channels
         t1 = time[b][0][(1024 - tch.trigger_cell) % 1024];
         for (chn = 1; chn < 9; chn++) {
            t2 = time[b][chn][(1024 - tch.trigger_cell) % 1024];
            dt = t1 - t2;
            for (i = 0; i < 1024; i++)
               time[b][chn][i] += dt;
         }

         // calculate amplitude
         double minval = waveform[0][0][i];
         double maxval = waveform[0][0][i];
         for (i=0 ; i<1024 ; i++) {
            if (waveform[0][0][i] < minval)
               minval = waveform[0][0][i];
            if (waveform[0][0][i] > maxval)
               maxval = waveform[0][0][i];
         }
         amplitude[0] = maxval - minval;

         // fill root tree
         rec->Fill();

         // uncomment the following to show raw waveforms
         /*
         for (i = 0; i < 1024; i++)
            g->SetPoint(i, time[0][1][i], waveform[0][1][i]);

         // draw graph and wait for user click
         g->Draw("ACP");
         c1->Update();
         gPad->WaitPrimitive();
         */
         tree->Fill();
      }
      
   }

   printf("\n");

   // project tree
   //rec->Draw("amp0");
   fnew->Write();
   fnew->Close();
}

void read_binary()
{
   decode("test.dat");
}
