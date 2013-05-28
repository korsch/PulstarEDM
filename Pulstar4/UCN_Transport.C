#include <iostream>
#include "TGraph.h"

using namespace std;

void FillTime()
{

  const int n(11);
  double x[n]={0.146033, 0.652002, 1.1555, 1.87953, 2.34023, 2.49285,2.59945, 2.68338, 3.68338,4.68338,5.68338};
  double y[n]={4514, 671.6, 357.4, 230.4, 193.6, 185.9, 177.1, 176.2, 142.7, 129.3, 125.0};
  TGraph* gr = new TGraph(n,x,y);
  
  gr->Draw("PA");


  return;
}
