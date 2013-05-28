#include "UCNHists.hh"
#include <string>

using namespace std;


string itos(int);      // convert int to string


UCNHists::UCNHists()
{


  nti = new TTree("UCNI","UCNI");
  ntf = new TTree("UCNF","UCNF");

}


void UCNHists::Construct(const char* name, UCNI* bl)
{


  string name1 = "energy";
  string totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->energy,totalname.c_str());

  name1 = "x";
  totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->x,totalname.c_str());

  name1 = "y";
  totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->y,totalname.c_str());


  name1 = "z";
  totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->z,totalname.c_str());

  name1 = "px";
  totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->px,totalname.c_str());

  name1 = "py";
  totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->py,totalname.c_str());


  name1 = "pz";
  totalname = name1 + "/D";
  nti->Branch(name1.c_str(),&bl->pz,totalname.c_str());



  return;
}

void UCNHists::Construct(const char* name, UCNF* bl)
{


  string name1 = "time";
  string totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->time,totalname.c_str());

  name1 = "energy";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->energy,totalname.c_str());

  name1 = "x";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->x,totalname.c_str());

  name1 = "y";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->y,totalname.c_str());

  name1 = "z";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->z,totalname.c_str());

  name1 = "px";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->px,totalname.c_str());

  name1 = "py";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->py,totalname.c_str());

  name1 = "pz";
  totalname = name1 + "/D";
  ntf->Branch(name1.c_str(),&bl->pz,totalname.c_str());


  return;
}


void UCNHists::FillI()
{

  nti->Fill();
  return;
}


void UCNHists::FillF()
{

  ntf->Fill();
  return;
}
