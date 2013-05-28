//////////////////////////////////////////////////////////////////////
//  time dependent field for UCN
////////////////////////////////////////////////////////////////////////

#include "G4ios.hh"
#include "UCNTimedependentField.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4StepPoint.hh"
#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include <iostream>
#include <fstream>
#include "G4ThreeVector.hh"
#include <cmath>
#include "iomanip"
#include "globals.hh"
#include "G4StepPoint.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"




UCNTimedependentField* UCNTimedependentField::theInstance = 0;
G4double UCNTimedependentField::gtime = 0;
G4ThreeVector UCNTimedependentField::spin; // = 0;

UCNTimedependentField::UCNTimedependentField()
{
  G4cout << "construct timedependent field " << this << G4endl;
  // create the messenger class
  mfMessenger = new UCNFieldMessenger(this);
  theInstance = this;
  
  fieldon = 0;
  G4String fieldfile = "";
  drawfield = 0;
  starttime = 0;
  fieldtime = 0;
  fieldtimefile = "";
  
 }
UCNTimedependentField::~UCNTimedependentField()
{
  delete mfMessenger;
}

/// these routines manage pointers for the communication 
/// between the field routines
UCNTimedependentField* UCNTimedependentField::GetTimedependentField(){
//G4cout << "timedependentfield: returning my address " << theInstance << ", " << G4endl;
return theInstance;
}
// set the spin, used by the UCN spin routine
 void UCNTimedependentField::SetSpin(G4ThreeVector stri)
{
G4cout << "timedependentfield: setspin " << stri << G4endl;
spin = stri;
}

 
 /// these routines are used for the messenger class communication
 /////////////////////////////////////////////////////////////////////////

 
 void UCNTimedependentField::SetField(G4String stri)
{
fieldon = 0;
if (stri != "0") fieldon = 1;
G4cout << "timedependentfield: inhomogeneous magnetic field " << fieldon << G4endl;

}
void UCNTimedependentField::SetFile(G4String stri)
{
fieldfile = stri;
G4cout << "timedependentfield: set values for the fieldcube file = " << stri << G4endl;

}


 void UCNTimedependentField::DrawField(G4String stri){
G4cout << "timedependentfield: set visualization of magfield " << stri << G4endl;
drawfield = 0;
if (stri == "1") drawfield = 1;
else {
 drawfield = 0;
}
}

void UCNTimedependentField::SetFieldOnTime(G4String stri){
G4cout << "timedependentfield: setFieldOnTime " << stri << G4endl;
float txt = 0;
sscanf(stri, "%f", &txt);
starttime = txt;

}

void UCNTimedependentField::SetFieldTime(G4String tru){
G4cout << "timedependentfield: setFieldTimedependence " << tru << G4endl;
int txt = 0;
sscanf(tru, "%d", &txt);
fieldtime = txt;
}

void UCNTimedependentField::SetFieldTimeFile(G4String tru){
G4cout << "timedependentfield: setFieldTimeFile " << tru << G4endl;
fieldtimefile = tru;
  if(fieldtime == 1){
   G4cout << "read the timedependence " << G4endl;
   readtimedependence();
  }
}

// draw the field points
void UCNTimedependentField::FieldVisualization(){


}

void UCNTimedependentField::readtimedependence(){
G4String file;
char stri[1000];
float txt = 0;
float p1 = 0;
int k = 0;
if (fieldtime == 1){
  //G4cout << "read time file " << G4endl;
  file = fieldtimefile;
  std::ifstream examplefile2 (file, std::ios::in);
  if (! examplefile2.is_open())
   { G4cout << "Error opening file";}
  while (! examplefile2.eof())
  {
   examplefile2.getline (stri,100);
   sscanf(stri, "%f %f", &txt, &p1);
   TimeRamp[k][0] = txt;
   TimeRamp[k][1] = p1;
  G4cout << "time behaviour " << TimeRamp[k][0] << ", " 
  << TimeRamp[k][1] << G4endl;
   k++;
  }
  //examplefile2.close;
 }

}

G4double UCNTimedependentField::GetFieldPercentage(double time){
//G4cout << "time " << time << G4endl;
double m = 1e30;
double t[30];
double p[30];
int inside =0;
int k = 0;
//G4cout << "starttieme" << starttime << G4endl;
if (time < starttime) {
}
else if (time >= starttime){
 //G4cout << "time groessser als starttiime " << G4endl;
// while (time <= m){
//  k++;
//  m = TimeRamp[k][0];
//  G4cout << "m " << m << G4endl;
//  }
// if (time > (starttime + m)){
//  }
// else {
//  G4cout << "time is smaller than max " << G4endl;
  for (int j = 0; j <30; j++){
    t[j] = TimeRamp[j][0];
    p[j] = TimeRamp[j][1];
    //G4cout << "t " << t[j] << ", p " << p[j] << G4endl;
  }
  inside = 1;
// }
}

float probability = 1;
double tim = 0;
if (inside > 0){
 k = 0;
 if (inside == 1){tim = time - starttime;}

 while (k<30)
 {
	if (tim < t[k]){
	   probability = p[k-1] +  (p[k]-p[k-1])/(t[k]-t[k-1])*(tim-t[k-1]);
           return probability;
         }
  k++;
 }
}
return probability;
}

G4double UCNTimedependentField::GetValue(){
  
 return fieldstrength;
}


G4ThreeVector UCNTimedependentField::GetGradient(double posx,double posy,double posz)
{


  G4ThreeVector FalseVector(0);
  
  G4double a = 0.;
  //G4cout << "spin " << spin << G4endl;
  //G4cout << "fiedtrue " << fieldon << G4endl;
  if (fieldon == 0){ return FalseVector;}
  
  
  
  
  if (spin.getZ() > 0.){         // force repulsive
    a = 1.;
  }
  else {                      // force attractive
    a = -1.;
  }
  
  int simplified = 1;
    G4ThreeVector simple_grad;

  if (simplified == 1){
    float gx = 0;
    float gy = 0;
    float gz = 0;
    if (posz > 0 && posz <= 100) {
      gz =  -0.015 ;
      fieldstrength = 1.5 -gz * posz;
    }
    if (posz < 0 && posz >= -100) {
      gz = 0.015;
      fieldstrength = gz * posz + 1.5;
    }
    posx = 0;
    posy = 0;
    
    simple_grad.setX((float)gx);
    simple_grad.setY((float)gy);
    simple_grad.setZ((float)gz);
    
    //G4cout << "fieldstrength " << fieldstrength << G4endl;
    //// set the coefficient for the spin equal 0 for test purpose //////////////////
    return simple_grad * a;
  }
    return simple_grad * a;
}


int UCNTimedependentField::readfield(){
  int k = 0;
  char stri[1000];
  float px = 0.0;
  float py = 0.0;
  float pz = 0.0;
  float B = 0.0;
  std::ifstream file2 (fieldfile, std::ios::in);
  if (! file2.is_open())
    { G4cout << "Error opening fieldfile";}

  while (! file2.eof())
  {
   file2.getline (stri,40);
   if (strcmp(stri, "00") == 0) {
     break;}
   sscanf(stri, "%f %f %f %f", &px, &py, &pz, &B);

   px = px * 1000;
   py = py * 1000;
   pz = pz * 1000;

   FieldValues[k][0] = 0;
   FieldValues[k][1] = 0;
   FieldValues[k][2] = 0;
   FieldValues[k][3] = 0;

   if (px) FieldValues[k][0] = (double)px;
   if (py) FieldValues[k][1] = (double)py;
   if (pz) FieldValues[k][2] = (double)pz;
   if (B)  FieldValues[k][3] = (double)B;

   k++;
 } 
  fieldlen = k;
return 0;
}

