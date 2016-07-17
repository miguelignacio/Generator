//____________________________________________________________________________
/*
 Copyright (c) 2003-2016, GENIE Neutrino MC Generator Collaboration
 For the full text of the license visit http://copyright.genie-mc.org
 or see $GENIE/LICENSE

 Author: Daniel Scully ( d.i.scully \at warwick.ac.uk)
   University of Warwick

*/
//____________________________________________________________________________

#include <iostream>
#include <sstream>
#include <string>
#include <complex>

#include "AlvarezRuso/ARWFSolution.h"

namespace genie
{
namespace alvarezruso
{

ARWFSolution::ARWFSolution(bool debug): debug_(debug)
{
  if(debug_) std::cerr << "WFS@ constructor" << std::endl;
}

ARWFSolution::~ARWFSolution()
{
}

} //namespace alvarezruso
} //namespace genie
