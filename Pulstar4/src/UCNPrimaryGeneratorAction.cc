//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: UCNPrimaryGeneratorAction.cc,v 1.1.1.1 2004/10/25 12:36:47 kuzniak Exp $
// GEANT4 tag $Name:  $
//
//   changed for UCN, 9.9.04 peter fierlinger

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "UCNPrimaryGeneratorAction.hh"

#include "UCNDetectorConstruction.hh"
#include "UCNPrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "UCNSpin.hh"
#include "TRandom3.h"


#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "UCNHists.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
using namespace std;

TRandom3 r;

G4String UCNPrimaryGeneratorAction::thePrimaryParticleName = "ucn" ; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

UCNPrimaryGeneratorAction::UCNPrimaryGeneratorAction(UCNDetectorConstruction* UCNDC)
  : UCNDetector(UCNDC), rndmFlag("off"),
    xvertex(0.),yvertex(0.),zvertex(0.),
    vertexdefined(false)
{
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);
  
  // create a messenger for this class
  //
  gunMessenger = new UCNPrimaryGeneratorMessenger(this);

  // default particle kinematic

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName="ucn");
  particleGun->SetParticleDefinition(particle);
  
  thePrimaryParticleName = particle->GetParticleName() ;
  
  extern double xOffset;

  xStart = -UCNDC->GetWorldSizeX() + xOffset;
  zStart = -UCNDC->GetWorldSizeZ(); // slightly inside deuterium
  zvertex = 0. ; //  -0.5*(F02Detector->GetAbsorberThickness());
  vis = 1;
  filltime = 0;
  G4cout << xvertex << ' ' << yvertex << G4endl;
  particleGun->SetParticlePosition(G4ThreeVector(xvertex,yvertex,zvertex));
  
  
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

UCNPrimaryGeneratorAction::~UCNPrimaryGeneratorAction()
{
  delete particleGun;
  delete gunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void UCNPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // this function is called at the begining of event
  // 


  //  G4cout << "++++++++++++++++++++++++++++ Primary Generator " << G4endl;

  thePrimaryParticleName = particleGun->GetParticleDefinition()->GetParticleName() ;

  //  G4cout << particleGun->GetParticleCharge() << G4endl;

  G4double x0,y0,z0 ;
  G4double px=0,py=0,pz=0 ;
  G4double n_energy = 0;
  G4double massnew = 1.674927e-27;     //neutronmasse in kg
  G4double p_length = 0;
   // ask for the initial spin of the particle
  G4ThreeVector spin = (UCNSpin::GetInstance())->GetSpin();
  double time = 0.3; // s
  n_energy =  GetUCNEnergy(); // 50 -> 170 neV (Pulstar) 

  //  px = 0.3;
  //  py = 0.3;
  //  pz = 0.7;

  G4ThreeVector momentum = cos_distribution();
  //  px = 0.5 - r.Uniform(1.0);
  //  py = 0.5 - r.Uniform(1.0);
  //  pz = r.Uniform(1.0);

  //  G4ThreeVector momentum(px,py,pz);


  //  extern G4double UCNDetectorConstruction::WorldSizeZ;
  GetXY(x0,y0);
  x0 += xStart;
  //  y0 = 0;
  //  z0 = 200.; // slightly inside deuterium
  //  z0 = -WorldSizeZ; // slightly inside deuterium
  z0 = zStart;
  //  G4cout << "UCNPrimaryGenerator: " << n_energy << G4endl;


  extern UCNI* initial;
  extern UCNHists* ucn;
  //  extern TFile* rootfile;

  initial->x = x0;
  initial->y = y0;
  initial->z = z0;
  initial->px =  momentum.getX();
  initial->py =  momentum.getY();
  initial->pz =  momentum.getZ();
  initial->energy = n_energy;

  ucn->FillI();

  
  particleGun->SetParticleTime(time);
  particleGun->SetParticlePolarization(spin);
  particleGun->SetParticleMomentumDirection(momentum);
  particleGun->SetParticleEnergy(n_energy*1e-9 *eV);
  particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  particleGun->GeneratePrimaryVertex(anEvent);

  
  if (vis ==1){
    visualize(G4ThreeVector(x0,y0,z0), n_energy);
  }
}

///////////////////////////////////////////////////////////////////////
//
//

G4String UCNPrimaryGeneratorAction::GetPrimaryName()
{
   return thePrimaryParticleName ;
}

void UCNPrimaryGeneratorAction::Setzvertex(G4double z)
{
  vertexdefined = true ;
  zvertex = z ;
  G4cout << " Z coordinate of the primary vertex = " << zvertex/mm << " mm." << G4endl;
}
void UCNPrimaryGeneratorAction::Setxvertex(G4double x)
{
  vertexdefined = true ;
  xvertex = x ;
  G4cout << " X coordinate of the primary vertex = " << xvertex/mm << " mm." << G4endl;
}

void UCNPrimaryGeneratorAction::Setyvertex(G4double y)
{
  vertexdefined = true ;
  yvertex = y ;
  G4cout << " Y coordinate of the primary vertex = " << yvertex/mm << " mm." << G4endl;
}


//////////////   make the usual UCN spectrum for the typical eperiment at ILL

G4ThreeVector UCNPrimaryGeneratorAction::Spectrum(double vmax){

 double px1=6.8,px2=6.8,px3=15.;
 double n_energy = 1e10;
 double massnew = 1.674927e-27;     //neutronmasse in kg
 double p_length = 1e10;
 double max_E = vmax;
 while (p_length > max_E || p_length < 0.1){
   // uniform from -px1 to +px1
   px1 = G4UniformRand() * px1;
   double pf1 = G4UniformRand();
   if (pf1 >= 0.5){px1 = -px1;}
   px2 = G4UniformRand() * px2;
   double pf2 = G4UniformRand();
   if (pf2 >= 0.5){px2 = -px2;}
   ////// linear distribution from 0 to 15 m/s linear distributed
   px3 = -sqrt(2 * G4UniformRand() * 112.5);
    // calculate momentum length
   p_length = sqrt(px1 * px1 + px2 * px2 + px3 * px3);
   n_energy = massnew * p_length * p_length / 2 * 6.242e18 * 1e9;
 }
 //G4cout << "p_length = " << p_length << ", n_energy " << n_energy << G4endl;
 G4ThreeVector momentum(px1,px2,px3);
 return momentum;
}

void UCNPrimaryGeneratorAction::SetSpec(G4String sp){
  usespectrum = 0; 
  G4cout << "primarygenerator: use a spectrum for UCN production " << sp << G4endl;
if ( sp == "1"){
  usespectrum = 1; 
}
}
void UCNPrimaryGeneratorAction::SetSeed(G4String sp){
  // set a randomseed
  float seed = 0;
  sscanf(sp,"%f", &seed);
  G4cout << "primarygenerator: set a new randomseed : " << seed << G4endl;
  if (seed) CLHEP::HepRandom::setTheSeed((long int)seed);


}
void UCNPrimaryGeneratorAction::SetVis(G4String sp){
  vis = 0;
  if (sp == "1"){ 
  vis = 1;
  G4cout << "primarygenerator: visualize vertices : " << sp << G4endl;
  }

}
void UCNPrimaryGeneratorAction::SetFillTime(G4String sp){
  // set a randomseed
  float seed = 0;
  sscanf(sp,"%f", &seed);
  G4cout << "primarygenerator: set a filltime : " << seed << G4endl;
  filltime = seed;

}
void UCNPrimaryGeneratorAction::visualize(G4ThreeVector position, double energy){
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();

  double diam = energy / 400. * 7.;  // max energy is about 400, diameter prop to e
  if (pVVisManager) {
       G4Circle circle(position);
       G4Colour colour(1.,0.,0.);
       circle.SetScreenDiameter (diam);
       G4VisAttributes attribs(colour);        // Define a red visualization attribute
       circle.SetVisAttributes(attribs);       // Assign the red attribute to the circle
       pVVisManager -> Draw(circle);

   }
}


double UCNPrimaryGeneratorAction::GetUCNEnergy()
{
  double rhomin(10.0);
  double rhomax(100.0);
  double Emin(50.0);
  //  double Emax(150.0);
  double Emax(170.0);
  double slope = (rhomax-rhomin)/(Emax-Emin);
  double deltaRho(rhomax-rhomin);
  double deltaE(Emax-Emin);
  double minFlux =(slope*Emin + (rhomin - 0.5*deltaRho))*sqrt(Emin);
  double maxFlux =(slope*Emax + (rhomin - 0.5*deltaRho))*sqrt(Emax);
  double deltaFlux(maxFlux- minFlux);

  double test(1.0);
  double flux(0.0);
  double E(0.0);

  extern TRandom3 r;

  static int k(0);

  while (test >= flux) 
  {
    test = r.Uniform(deltaFlux) + minFlux;
    E = r.Uniform(deltaE) + Emin;
    
    flux =(slope*E + (rhomin - 0.5*deltaRho))*sqrt(E);
 
    }
  
   
  return E;
 }
		   
 

void UCNPrimaryGeneratorAction::GetXY(double& x, double& y)
{

  extern G4double t01innerradius;

  double R = t01innerradius;
  double rtest(1000000.0);


  while(rtest > R)
    {
      x = 2*(0.5*R - r.Uniform(R));
      y = 2*(0.5*R - r.Uniform(R));
      rtest = sqrt(x*x + y*y);
    }



  return;
}

G4ThreeVector  UCNPrimaryGeneratorAction::cos_distribution()
{

  G4ThreeVector momentum;

  //cos-theta distribution, angles in RAD
  //set r,theta, phi, 
  momentum.setRThetaPhi(1., acos(sqrt(G4UniformRand())), 2.*M_PI*G4UniformRand());
  
  return momentum;

}
