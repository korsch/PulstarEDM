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
#include "UCN.hh"
#include "UCN1.hh"


class UCNHists
{
 public:
  
  UCNHists();
  ~UCNHists();

  TTree* nt;

  void Construct(const char*, UCN*);
  void Construct(const char*, UCN1*);



  void Fill();
  void Fill1();
  //  void Fill(double, double, double, double);

};


#endif // _UCNHISTS_HPP_

