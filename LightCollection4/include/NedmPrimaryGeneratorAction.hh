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
// $Id: guidesPrimaryGeneratorAction.hh,v 1.4 2003/01/23 15:34:23 maire Exp $
// GEANT4 tag $Name: geant4-08-00 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NedmPrimaryGeneratorAction_h
#define NedmPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class NedmPrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//! Responsible for creating the primary particles in each event.
/*! Generates on optical photon per event at specified position with the requested properties.
 */
class NedmPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  NedmPrimaryGeneratorAction();
  ~NedmPrimaryGeneratorAction();

public:
  //! Mandatory method called at begining of event
  void GeneratePrimaries(G4Event*);

  //! Turn on or off particle generation position
  inline
  void set_random_position(G4bool on_off) { random_ = on_off; }

  inline
  void set_random_direction(G4bool on_off) { random_direction_ = on_off; }

  //! Set the minimum corner of the random generation box
  inline
  void set_min_random_corner(G4ThreeVector min) { min_vector_ = min; }

  //! Set the maximum corner of the random generation box
  inline
  void set_max_random_corner(G4ThreeVector max) { max_vector_ = max; }
  
private:
  G4ParticleGun* particleGun;

  G4ThreeVector max_vector_;
  G4ThreeVector min_vector_;
  G4bool random_;
  G4bool random_direction_;

  NedmPrimaryGeneratorMessenger* messenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*NedmPrimaryGeneratorAction_h*/
