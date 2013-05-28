#ifndef _UCNHISTS_HPP_
#define _UCNHISTS_HPP_
/********************************************************************	\
* File: Ucn4edmHists.hh                                              *
*                                                                    *
* Author: Wolfgang Korsch <korsch@pa.uky.edu>                        *
* Time-stamp: <2006-04-06 17:28:35>                                  *
\********************************************************************/

#include <string>
#include "TTree.h"
#include "TBranch.h"
#include "UCNI.hh"
#include "UCNF.hh"


class UCNHists
{
 public:
  
  UCNHists();
  ~UCNHists();

  TTree* nti;
  TTree* ntf;

  void Construct(const char*, UCNI*);
  void Construct(const char*, UCNF*);

  void FillI();
  void FillF();
  //  void Fill(double, double, double, double);

};


#endif // _UCNHISTS_HPP_

