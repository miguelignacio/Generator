//____________________________________________________________________________
/*!

\class    genie::TuneId

\brief    GENIE tune ID

\author   Marco Roda <Marco.Roda \at liverpool.ac.uk>
          University of Liverpool

          Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
          University of Liverpool & STFC Rutherford Appleton Lab

\created  April 19, 2018

\cpright  Copyright (c) 2003-2018, The GENIE Collaboration
          For the full text of the license visit http://copyright.genie-mc.org
          or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#ifndef _TUNE_ID_H_
#define _TUNE_ID_H_

#include <string>
#include <iostream>

using std::string;
using std::ostream;

namespace genie {

class TuneId;
ostream & operator << (ostream & stream, const TuneId & id);
bool      operator == (const TuneId & id1, const TuneId & id2);
bool      operator != (const TuneId & id1, const TuneId & id2);

class TuneId  {

public:

  TuneId(const string & id_str) : fName(id_str) { Build(fName); }
  TuneId(const TuneId & id);
  ~TuneId() {;}

  // The typical tune name in neutrino mode is : Gdd_MMv_PP_xxx
  string Name            (void) const { return fName;                         } // Gdd_MMv_PP_xxx
  string Prefix          (void) const { return fPrefix;                       } // G
  string Year            (void) const { return fYear;                         } // dd
  string ModelId         (void) const { return fMajorModelId + fMinorModelId; } // MMv
  string MajorModelId    (void) const { return fMajorModelId;                 } // MM
  string MinorModelId    (void) const { return fMinorModelId;                 } // v
  string TunedParamSetId (void) const { return fTunedParamSetId;              } // PP
  string FitDataSetId    (void) const { return fFitDataSetId;                 } // xxx

  bool   IsConfigured    (void) const { return fPrefix.size() > 0 ;           }
  // this is true if the name of the tune has been decoded into its parts

  bool   IsValidated     (void) const { return fBaseDirectory.size() > 0 ;    }
  // this is true if the existence of the tune directory in the system has been checked

  bool   IsCustom        (void) const { return fCustomSource.size() > 0 ;     }
  // this check if the configuration had a GXMLPATH configuration that took priority
  // over the standard $GXMLPATH
  // This boolean is reliable only if IsValidated() is true

  // A tune can be a simple configuration of models or the ouput of a complete tuning procedure
  // This changes the position the tune files are stored so we need a quick way to know this
  bool   OnlyConfiguration()   const  { return (TunedParamSetId() == "00") ; }

  // Methods related to config directory
  string CGC             (void) const ;   // Comprehensive Global Confguration
  string Tail            (void) const ;
  string CGCDirectory    (void) const ;
  string TuneDirectory   (void) const ;

  void   Build   (const string & name = "" ) ;
  void   Decode  (string id_str);
  void   Copy    (const TuneId & id);
  bool   Compare (const TuneId & id) const;
  void   Print   (ostream & stream) const;

  friend ostream & operator << (ostream & stream, const TuneId & id);

private:

  TuneId() {;}

  bool CheckDirectory() ;

  string fName;

  string fPrefix;
  string fYear;
  string fModelId;
  string fMajorModelId;
  string fMinorModelId;
  string fTunedParamSetId;
  string fFitDataSetId;

  string fBaseDirectory ;
  string fCustomSource  ;

};

}       // genie namespace

#endif  // _TUNE_ID_H_
