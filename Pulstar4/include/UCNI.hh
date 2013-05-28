/********************************************************************	\
* File: UCNI.hh                                               *
*                                                                    *
* Author: Wolfgang Korsch <korsch@pa.uky.edu>                        *
* Time-stamp: <2006-04-06 18:51:55>                                  *
\********************************************************************/

#ifndef _UCNI_HPP_
#define _UCNI_HPP_


class UCNI{
public:

  UCNI();
  ~UCNI();


  double energy;
  double x;
  double y;
  double z;
  double px;
  double py;
  double pz;


  void Reset();

};


#endif // _UCNI_HPP_

