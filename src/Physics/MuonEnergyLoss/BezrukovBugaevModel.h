//____________________________________________________________________________
/*!

\class    genie::mueloss::BezrukovBugaevModel

\brief    Bezrukov-Bugaev model for the energy loss of high energy muons due
          to photonuclear interactions.
          Concrete implementation of the MuELossI interface.

\ref      W.Lohmann, R.Kopp and R.Voss,
          Energy Loss of Muons in the Energy Range 1-10000 GeV, CERN 85-03

\author   Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  December 10, 2003

\cpright  Copyright (c) 2003-2019, The GENIE Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _BEZRUKOV_BUGAEV_MODEL_H_
#define _BEZRUKOV_BUGAEV_MODEL_H_

#include <Math/IFunction.h>

#include "Physics/MuonEnergyLoss/MuELossI.h"
///////#include "Numerical/GSFunc.h"

namespace genie {

/////class IntegratorI;

namespace mueloss {

class BezrukovBugaevModel : public MuELossI
{
public:
  BezrukovBugaevModel();
  BezrukovBugaevModel(string config);
  virtual ~BezrukovBugaevModel();

  //! Implement the MuELossI interface
  double       dE_dx    (double E, MuELMaterial_t material) const;
  MuELProcess_t Process (void) const { return eMupNuclearInteraction; }

//  //! Overload the Algorithm::Configure() methods to load private data
//  //! members from configuration options
//  void Configure(const Registry & config);
//  void Configure(string config);

private:
//  void LoadConfig (void);
/////  const IntegratorI * fIntegrator;
};

} // mueloss namespace
} // genie   namespace

//____________________________________________________________________________
/*!
\class    genie::mueloss::BezrukovBugaevIntegrand

\brief    Auxiliary scalar function for the internal integration in Bezrukov
          Bugaev model

\author   Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  December 10, 2003
*/
//____________________________________________________________________________

namespace genie {
 namespace mueloss {
   namespace gsl {

    class BezrukovBugaevIntegrand : public ROOT::Math::IBaseFunctionOneDim
    {
     public:
       BezrukovBugaevIntegrand(double E, double A);
      ~BezrukovBugaevIntegrand();
       // ROOT::Math::IBaseFunctionOneDim interface
       unsigned int                      NDim   (void)       const;
       double                            DoEval (double xin) const;
       ROOT::Math::IBaseFunctionOneDim * Clone  (void)       const;
     private:
       double fE;
       double fA;
     };

  }  // gsl namespace
 }  // mueloss namespace
}  // genie   namespace

#endif // _BEZRUKOV_BUGAEV_MODEL_H_
