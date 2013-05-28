/********************************************************************	\
* File: UCNI.cc                                                       *
*                                                                    *
* Author: Wolfgang Korsch <korsch@pa.uky.edu>                        *
* Time-stamp: <2006-04-06 18:48:49>                                  *
\********************************************************************/

#include "UCNI.hh"

UCNI::UCNI() { 
}

UCNI::~UCNI() {}

void UCNI::Reset()
{
  energy = -1000;
  x =-1000;
  y =-1000;
  z =-1000;
  px =-1000;
  py =-1000;
  pz =-1000;

  return;
}

