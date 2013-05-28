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
// $Id: guidesPrimaryGeneratorAction.cc,v 1.4 2005/06/27 15:28:09 gunter Exp $
// GEANT4 tag $Name: geant4-08-00 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "NedmPrimaryGeneratorAction.hh"
#include "NedmAnalysisManager.hh"
#include "NedmPrimaryGeneratorMessenger.hh"
#include "NedmDetectorParameters.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomTools.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NedmPrimaryGeneratorAction::NedmPrimaryGeneratorAction() : max_vector_(),
	min_vector_(), random_(true), random_direction_(true) {
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  
  //default kinematic
  //
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("opticalphoton");

  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleTime(0.0*ns);
  particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
  particleGun->SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));
  particleGun->SetParticleEnergy(15*eV);

  messenger = new NedmPrimaryGeneratorMessenger(this);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NedmPrimaryGeneratorAction::~NedmPrimaryGeneratorAction()
{
  delete particleGun;
  delete messenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*!
  One optical photon is generated per event. The angle at which the particle travels is randomly determined. Note that data for the analysis histogram is modified here via the call to NedmAnalysisManager::SetPrimaryData.

*/
void NedmPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  if(random_direction_)
    particleGun->SetParticleMomentumDirection(G4RandomDirection());

  G4ThreeVector momentumDirection = particleGun->GetParticleMomentumDirection();

  if(random_) {
    NedmDetectorParameters* parameters = NedmDetectorParameters::instance();
    G4ThreeVector cellSize = parameters->cell_lr_size();
    G4double cell_x  = 3*2.54*cm;
    G4double cell_y  = 4*2.54*cm;
    G4double cell_z  = 40.0*cm;
    G4double x,y,z;
    x = cell_x * (G4UniformRand() - 0.5);
    y = cell_y * (G4UniformRand() - 0.5);
    z = cell_z * (G4UniformRand() - 0.5);
    particleGun->SetParticlePosition(G4ThreeVector(x, y, z));
  
  }

  G4ThreeVector polar = G4PlaneVectorRand(momentumDirection);
  particleGun->SetParticlePolarization(polar);

  particleGun->GeneratePrimaryVertex(anEvent);
  G4ThreeVector position = particleGun->GetParticlePosition();

  NedmAnalysisManager::GetInstance()->set_vertex_data(position.x()/cm,
      position.y()/cm,
      position.z()/cm,
      momentumDirection.cosTheta(),
      momentumDirection.phi());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//....oooOO0Ooooo........oooOO0Ooooo........oooOO0Ooooo........oooOO0Ooooo......
