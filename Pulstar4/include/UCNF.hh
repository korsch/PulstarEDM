/********************************************************************	\
* File: UCNF.hh                                               *
*                                                                    *
* Author: Wolfgang Korsch <korsch@pa.uky.edu>                        *
* Time-stamp: <2006-04-06 18:51:55>                                  *
\********************************************************************/

#ifndef _UCNF_HPP_
#define _UCNF_HPP_


class UCNF{
public:

  UCNF();
  ~UCNF();


  double time;  
  double energy;
  double x;
  double y;
  double z;
  double px;
  double py;
  double pz;


  void Reset();

};


#endif // _UCNF_HPP_

