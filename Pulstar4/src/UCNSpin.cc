///////////////////////////////////////////////////////////////////////
//
//   UCN spin calculation, 7.9.04, peter fierlinger
//
//   calculate multiple "elastic scattering" inside materials
///////////////////////////////////////////////////////////////////////

#include "G4ios.hh"
#include <cmath>
#include "UCNSpin.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4StepPoint.hh"
#include "Randomize.hh"
#include "G4ElementVector.hh"

#include "UCNTimedependentField.hh"
#include "UCNSaveToFile.hh"
#include "UCNSpinMessenger.hh"

G4ThreeVector UCNSpin::spin(0);
UCNSpin* UCNSpin::theInstance = 0;

UCNSpin::UCNSpin(const G4String& processName)
              : G4VContinuousDiscreteProcess(processName)
{
  // create the messenger class instance
      theMessenger = new UCNSpinMessenger(this);
      theInstance = this;
      spintrue = 0;	
      totalAngle = 0;
	
	if (verboseLevel>0) {
           G4cout << GetProcessName() << " is created " << G4endl;
        }
	
}

UCNSpin::~UCNSpin(){}

G4double UCNSpin::GetContinuousStepLimit(const G4Track& ,
                             G4double,
                             G4double ,
			     G4double& ){
      return 1e300;
}

G4VParticleChange* UCNSpin::AlongStepDoIt(
			     const G4Track&,
			     const G4Step& 
			    ){
  return &aParticleChange;
}



G4VParticleChange*
UCNSpin::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
  aParticleChange.Initialize(aTrack);
  G4ThreeVector current_spin = aTrack.GetPolarization();
  if (spin == current_spin){
     // return G4VContinuousDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  else {spin = current_spin;
    UCNTimedependentField* thefield = UCNTimedependentField::GetTimedependentField();
    thefield->SetSpin(spin);
  }
  
  // start spin tracking
  if (spintrue == 1){
    UCNTimedependentField* thefield = UCNTimedependentField::GetTimedependentField();
    // for the transportation of the spin vector we calculate here only the 
    // adiabatic case, where the magnetic moment is always parallel to the 
    // magnetic field lines, so we are only interested how much the spin rotates
    // around the fieldlines during each step. the magnetic field is constant 
    // during each step
    // the (angular)frequency is 183 MHz per tesla
      G4double currentField = thefield->GetValue();
      G4double currentStepLength = aTrack.GetStepLength();
      G4double currentVelocity = aTrack.GetVelocity()*1e6; 
      G4double t = currentStepLength * 1000. / currentVelocity;
      double anglePerStep = 183e6*currentField*t;
        // if spin plus, turn the spin clockwise, otherwise ounterclockwise

      // FIX Following two lines!!!! WK 3/2012
       //      if (current_spin > 0){totalAngle = totalAngle + anglePerStep;}
      //      else if (current_spin < 0){totalAngle = totalAngle - anglePerStep;}
      double newangle = fmod(totalAngle,(2*pi));
      //G4cout << "angleperstep " << anglePerStep << ", totalAngle " << totalAngle << 
      //		", effective angle " << newangle << G4endl;
  }
  // start spin tracking
  if (lowfield == 1){
  
    //G4cout << "claculate low field transitions" << G4endl;      	
    double dBdt = 0;
    double mu = 0;
    double hbar = 0;
    UCNTimedependentField* thefield = UCNTimedependentField::GetTimedependentField();
    G4double currentField = thefield->GetValue();
        
    double transitionProbability = exp( - mu * currentField / (hbar * fabs(dBdt)));
    
    // low field transitions
    // v.v. vladimirski, JETP 12, 740 (1961)
  
  }
  return G4VContinuousDiscreteProcess::PostStepDoIt(aTrack, aStep);
	
}

G4double UCNSpin::GetMeanFreePath(const G4Track&,
 				         G4double ,
				         G4ForceCondition* condition)
{

  *condition = Forced;
        return 1e300;
             
}

void UCNSpin::SetInitSpin(G4String valu){
 if (valu == "random") {
  G4cout << "spin: set a random initial spin direction"  << G4endl; 
  //G4ThreeVector final(0.,0.,1.);
  //double theta = G4UniformRand() * pi;
  //double phi = G4UniformRand() * 2 * pi;
  //final.rotateY(theta);
  //final.rotateZ(phi);
  //final = final.unit();
  if (G4UniformRand() > 0.5){spin.setZ(1);}
  else {spin.setZ(-1);}
  spin.setX(0);
  spin.setY(0);
  
 }
 else if (valu == "+"){
  G4cout << "spin: set initial spin +"  << G4endl; 
  spin.setX(0); spin.setY(0); spin.setZ(1);
 }
 else if (valu == "-"){
  G4cout << "spin: set initial spin -"  << G4endl; 
  spin.setX(0); spin.setY(0); spin.setZ(-1);
 }
 G4cout << "spin: Z = " << spin.getZ() << G4endl; 
 UCNTimedependentField* thefield = UCNTimedependentField::GetTimedependentField();
 thefield->SetSpin(spin);

 
}        

UCNSpin * UCNSpin::GetInstance(){

return theInstance;
}

G4ThreeVector UCNSpin::GetSpin(){

return spin;
}
void UCNSpin::SetSpinTrans(G4String stri)
{
spintrue = 0;
if (stri == "1") spintrue = 1;
G4cout << "spin: set spintransport " << stri << G4endl;

}
void UCNSpin::SetLowField(G4String stri)
{
lowfield = 0;
if (stri == "1") lowfield = 1;
G4cout << "spin: set lowfield calculation " << stri << G4endl;

}


