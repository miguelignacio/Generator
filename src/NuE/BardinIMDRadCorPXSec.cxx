//____________________________________________________________________________
/*
 Copyright (c) 2003-2011, GENIE Neutrino MC Generator Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory - February 14, 2005

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include <TMath.h>

#include "Base/XSecIntegratorI.h"
#include "Conventions/GBuild.h"
#include "Conventions/Constants.h"
#include "Conventions/RefFrame.h"
#include "NuE/BardinIMDRadCorPXSec.h"
#include "Messenger/Messenger.h"
#include "Numerical/IntegratorI.h"
#include "Utils/KineUtils.h"

using namespace genie;
using namespace genie::constants;

//____________________________________________________________________________
BardinIMDRadCorPXSec::BardinIMDRadCorPXSec() :
XSecAlgorithmI("genie::BardinIMDRadCorPXSec")
{

}
//____________________________________________________________________________
BardinIMDRadCorPXSec::BardinIMDRadCorPXSec(string config) :
XSecAlgorithmI("genie::BardinIMDRadCorPXSec", config)
{

}
//____________________________________________________________________________
BardinIMDRadCorPXSec::~BardinIMDRadCorPXSec()
{

}
//____________________________________________________________________________
double BardinIMDRadCorPXSec::XSec(
                 const Interaction * interaction, KinePhaseSpace_t kps) const
{
  if(! this -> ValidProcess    (interaction) ) return 0.;
  if(! this -> ValidKinematics (interaction) ) return 0.;

  const InitialState & init_state = interaction -> InitState();

  double E    = init_state.ProbeE(kRfLab);
  double sig0 = kGF2 * kElectronMass * E / kPi;
  double re   = 0.5 * kElectronMass / E;
  double r    = (kMuonMass2 / kElectronMass2) * re;
  double y    = interaction->Kine().y();

  y = 1-y;  //Note: y = (Ev-El)/Ev but in Bardin's paper y=El/Ev.

  double ymin = r + re;
  double ymax = 1 + re + r*re / (1+re);

  double e = 1E-5;
  ymax = TMath::Min(ymax,1-e); // avoid ymax=1, due to a log(1-y)

#ifdef __GENIE_LOW_LEVEL_MESG_ENABLED__
  LOG("BardinIMD", pDEBUG)
           << "sig0 = " << sig0 << ", r = " << r << ", re = " << re;
  LOG("BardinIMD", pDEBUG)
                   << "allowed y: [" << ymin << ", " << ymax << "]";
#endif

  if(y<ymin || y>ymax) return 0;

  double xsec = 2 * sig0 * ( 1 - r + (kAem/kPi) * Fa(re,r,y) );

#ifdef __GENIE_LOW_LEVEL_MESG_ENABLED__
  LOG("BardinIMD", pINFO)
     << "dxsec[1-loop]/dy (Ev = " << E << ", y = " << y << ") = " << xsec;
#endif

  //----- The algorithm computes dxsec/dy
  //      Check whether variable tranformation is needed
  if(kps!=kPSyfE) {
    double J = utils::kinematics::Jacobian(interaction,kPSyfE,kps);
    xsec *= J;
  }

  //----- If requested return the free electron xsec even for nuclear target
  if( interaction->TestBit(kIAssumeFreeElectron) ) return xsec;

  //----- Scale for the number of scattering centers at the target
  int Ne = init_state.Tgt().Z(); // num of scattering centers
  xsec *= Ne; 

  return xsec;
}
//____________________________________________________________________________
double BardinIMDRadCorPXSec::Integral(const Interaction * interaction) const
{
  double xsec = fXSecIntegrator->Integrate(this,interaction);
  return xsec;
}
//____________________________________________________________________________
bool BardinIMDRadCorPXSec::ValidProcess(const Interaction * interaction) const
{
  if(interaction->TestBit(kISkipProcessChk)) return true;

  return true;
}
//____________________________________________________________________________
double BardinIMDRadCorPXSec::Fa(double re, double r, double y) const
{
  double y2  = y * y;
  double rre = r * re;
  double r_y = r/y;
  double y_r = y/r;

  double fa = 0;

  fa = (1-r) *       ( TMath::Log(y2/rre) * TMath::Log(1-r_y) +
                       TMath::Log(y_r) * TMath::Log(1-y) -
                       this->Li2( r ) +
                       this->Li2( y ) +
                       this->Li2( (r-y) / (1-y) ) +
                       1.5 * (1-r) * TMath::Log(1-r)
                     )
                     +

       0.5*(1+3*r) * ( this->Li2( (1-r_y) / (1-r) ) -
                       this->Li2( (y-r)   / (1-r) ) -
                       TMath::Log(y_r) * TMath::Log( (y-r) / (1-r) )
                     )
                     +

       this->P(1,r,y)                   -
       this->P(2,r,y) * TMath::Log(r)   -
       this->P(3,r,y) * TMath::Log(re)  +
       this->P(4,r,y) * TMath::Log(y)   +
       this->P(5,r,y) * TMath::Log(1-y) +
       this->P(6,r,y) * (1 - r_y) * TMath::Log(1-r_y);

  return fa;
}
//____________________________________________________________________________
double BardinIMDRadCorPXSec::P(int i, double r, double y) const
{
  int kmin = -3;
  int kmax =  2;
  double p = 0;
  for(int k = kmin; k <= kmax; k++) {
     double c  = this->C(i,k,r);
     double yk = TMath::Power(y,k);
     p += (c*yk);
  }
  return p;
}
//____________________________________________________________________________
double BardinIMDRadCorPXSec::Li2(double z) const
{
  double epsilon = 1e-2;
  Range1D_t t(epsilon, 1.0 - epsilon);

#ifdef __GENIE_LOW_LEVEL_MESG_ENABLED__
  LOG("BardinIMD", pDEBUG)
    << "Summing BardinIMDRadCorIntegrand in [" << t.min<< ", " << t.max<< "]";
#endif

  BardinIMDRadCorIntegrand * func = new BardinIMDRadCorIntegrand(z);
  func->SetParam(0,"t",t);

  double li2 = fIntegrator->Integrate(*func);

#ifdef __GENIE_LOW_LEVEL_MESG_ENABLED__
  LOG("BardinIMD", pDEBUG) << "Li2(z = " << z << ")" << li2;
#endif

  delete func;
  return li2;
}
//____________________________________________________________________________
double BardinIMDRadCorPXSec::C(int i, int k, double r) const
{
  if        ( i == 1 ) {

      if      (k == -3) return -0.19444444*TMath::Power(r,3.);
      else if (k == -2) return (0.083333333+0.29166667*r)*TMath::Power(r,2.);
      else if (k == -1) return -0.58333333*r - 0.5*TMath::Power(r,2.) - TMath::Power(r,3.)/6.;
      else if (k ==  0) return -1.30555560 + 3.125*r + 0.375*TMath::Power(r,2.);
      else if (k ==  1) return -0.91666667 - 0.25*r;
      else if (k ==  2) return 0.041666667;
      else              return 0.;

  } else if ( i == 2 ) {

      if      (k == -3) return 0.;
      else if (k == -2) return 0.5*TMath::Power(r,2.);
      else if (k == -1) return 0.5*r - 2*TMath::Power(r,2.);
      else if (k ==  0) return 0.25 - 0.75*r + 1.5*TMath::Power(r,2);
      else if (k ==  1) return 0.5;
      else if (k ==  2) return 0.;
      else              return 0.;

  } else if ( i == 3 ) {

      if      (k == -3) return 0.16666667*TMath::Power(r,3.);
      else if (k == -2) return 0.25*TMath::Power(r,2.)*(1-r);
      else if (k == -1) return r-0.5*TMath::Power(r,2.);
      else if (k ==  0) return 0.66666667;
      else if (k ==  1) return 0.;
      else if (k ==  2) return 0.;
      else              return 0.;

  } else if ( i == 4 ) {

      if      (k == -3) return 0.;
      else if (k == -2) return TMath::Power(r,2.);
      else if (k == -1) return r*(1-4.*r);
      else if (k ==  0) return 1.5*TMath::Power(r,2.);
      else if (k ==  1) return 1.;
      else if (k ==  2) return 0.;
      else              return 0.;

  } else if ( i == 5 ) {

      if      (k == -3) return 0.16666667*TMath::Power(r,3.);
      else if (k == -2) return -0.25*TMath::Power(r,2.)*(1+r);
      else if (k == -1) return 0.5*r*(1+3*r);
      else if (k ==  0) return -1.9166667+2.25*r-1.5*TMath::Power(r,2);
      else if (k ==  1) return -0.5;
      else if (k ==  2) return 0.;
      else              return 0.;

  } else if ( i == 6 ) {

      if      (k == -3) return 0.;
      else if (k == -2) return 0.16666667*TMath::Power(r,2.);
      else if (k == -1) return -0.25*r*(r+0.33333333);
      else if (k ==  0) return 1.25*(r+0.33333333);
      else if (k ==  1) return 0.5;
      else if (k ==  2) return 0.;
      else              return 0.;

  } else return 0.;
}
//____________________________________________________________________________
void BardinIMDRadCorPXSec::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void BardinIMDRadCorPXSec::Configure(string param_set)
{
  Algorithm::Configure(param_set);
  this->LoadConfig();
}
//____________________________________________________________________________
void BardinIMDRadCorPXSec::LoadConfig(void)
{
  fIntegrator = 
      dynamic_cast<const IntegratorI *> (this->SubAlg("Integrator"));
  assert(fIntegrator);

  fXSecIntegrator =
      dynamic_cast<const XSecIntegratorI *> (this->SubAlg("XSec-Integrator"));
  assert(fXSecIntegrator);
}
//____________________________________________________________________________
// Auxiliary scalar function for internal integration
//____________________________________________________________________________
BardinIMDRadCorIntegrand::BardinIMDRadCorIntegrand(double z) :
GSFunc(1)
{
  fZ = z;
}
//____________________________________________________________________________
BardinIMDRadCorIntegrand::~BardinIMDRadCorIntegrand()
{

}
//____________________________________________________________________________
double BardinIMDRadCorIntegrand::operator() (const vector<double> & param)
{
  double t = param[0];
  if(t==0) return 0.;

  if(t*fZ>=1.) return 0.;
  double f = TMath::Log(1.-fZ*t)/t;
  return f;
}
//____________________________________________________________________________

