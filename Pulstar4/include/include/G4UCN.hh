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
// $Id: G4UCN.hh,v 1.1.1.1 2004/10/25 12:36:46 kuzniak Exp $
// GEANT4 tag $Name:  $
//
// 4.9.04 changed for UCN by peter fierlinger
//
// 

// Each class inheriting from G4VBaryon
// corresponds to a particle type; one and only one
// instance for each class is guaranteed.

#ifndef G4UCN_h
#define G4UCN_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4Material.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         UCN  0                                 ###
// ######################################################################

class G4UCN : public G4ParticleDefinition
{
 private:
   static G4UCN* theUCN;

 private:
  G4UCN(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable
   );

 public:
   virtual ~G4UCN(){}

   static G4UCN* UCNDefinition();
   static G4UCN* UCN();

 public:  //With Description
   G4int    GetAtomicNumber() const;
   G4int    GetAtomicMass() const;

   G4double GetExcitationEnergy() const {return 0.;} 
  void     SetExcitationEnergy(G4double ){};
   // These two methods are dummy because all particles derived from 
   // G4UCN is "groud state" nuclei  

   virtual void CalcEnergyCuts( const G4Material* ); 
};

inline
 G4int G4UCN::GetAtomicNumber() const 
{
  return 0; 
}

inline
 G4int G4UCN::GetAtomicMass() const 
{
  return 1;
}

#endif
