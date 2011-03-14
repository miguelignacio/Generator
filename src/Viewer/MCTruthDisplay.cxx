//____________________________________________________________________________
/*
 Copyright (c) 2003-2011, GENIE Neutrino MC Generator Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>, Rutherford Lab.
         November 30, 2007

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :
 @ Nov 30, 2007 - CA
   Was added in 2.0.1, intended to replace an older set of GHEP rendering
   classes. No GHEP drawing at that time.

*/
//____________________________________________________________________________

#include <sstream>
#include <string>
#include <vector>

#include <TRootEmbeddedCanvas.h>
#include <TGTextEdit.h>
#include <TLorentzVector.h>
#include <TLine.h>
#include <TEllipse.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TIterator.h>

#include "EVGCore/EventRecord.h"
#include "GHEP/GHepParticle.h"
#include "Messenger/Messenger.h"
#include "Utils/NuclearUtils.h"
#include "Viewer/MCTruthDisplay.h"

using std::ostringstream;

using namespace genie;
using namespace genie::utils;
using namespace genie::gview;

//______________________________________________________________________________
MCTruthDisplay::MCTruthDisplay(TRootEmbeddedCanvas * ec, TGTextEdit * gtx) :
fEmbeddedCanvas(ec),
fGTxt(gtx)
{

}
//______________________________________________________________________________
MCTruthDisplay::~MCTruthDisplay()
{

}
//______________________________________________________________________________
void MCTruthDisplay::DrawDiagram(EventRecord * event) 
{
   if(!fEmbeddedCanvas) return;

   LOG("gviewer", pINFO) << "Drawing input event diagram";

   fEmbeddedCanvas->GetCanvas()->cd();
   fEmbeddedCanvas->GetCanvas()->Clear();

   fEmbeddedCanvas->GetCanvas()->Range(0,0,30,30);
   fEmbeddedCanvas->GetCanvas()->SetFillColor(0);
   fEmbeddedCanvas->GetCanvas()->SetBorderMode(0);

   GHepParticle * target = event->TargetNucleus();
   if(target) {  
      int A = target->A();
      double R = nuclear::Radius(A);
      TEllipse grpx_target(0,0,R,R);
      grpx_target.Draw();
      fEmbeddedCanvas->GetCanvas()->Update();
   }

   GHepParticle * p = 0;
   TIter event_iter(event);
   while ( (p = dynamic_cast<GHepParticle *>(event_iter.Next())) ) {


   }

   //
   // ...
   //

   fEmbeddedCanvas->GetCanvas()->Update();
}
//______________________________________________________________________________
void MCTruthDisplay::PrintEventRecord(EventRecord * event)
{
  if(!fGTxt) return;

  ostringstream ghep;
  ghep << *event;
  string ghepstr = ghep.str(); // GHEP record as a single string
  
  // split GHEP string to get 1 line per particle - use '\n' as delimiter
  vector<string> lines;
  string delim = string("\n");
  while(ghepstr.find_first_of(delim) < ghepstr.length()) {
    lines.push_back(
          ghepstr.substr(0, ghepstr.find_first_of(delim)) );
     ghepstr = ghepstr.substr(
          ghepstr.find_first_of(delim)+1, ghepstr.length());
  }
  lines.push_back(ghepstr);  

  // print GHEP entries to TGTextView
  vector<string>::iterator line_iter;
  for(line_iter = lines.begin(); line_iter != lines.end(); ++line_iter) {
    fGTxt->AddLine( line_iter->c_str() );
  }                                                                             
}
//______________________________________________________________________________

