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
// $Id: UCNUCN0.cc,v 1.2 2005/07/05 22:55:52 kuzniak Exp $
// GEANT4 tag $Name:  $
//
// 4.9.04 adapted for UCN by peter fierlinger

#include "fstream"
#include "iomanip"

#include "G4UCN.hh"
#include "G4ParticleTable.hh"
#include "G4NeutronBetaDecayChannel.hh"
#include "G4DecayTable.hh"


// ######################################################################
// ###                           NEUTRON                              ###
// ######################################################################


G4UCN* G4UCN::theUCN = 0;


G4UCN::G4UCN(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,
       G4int               iSpin,        G4int               iParity,
       G4int               iConjugation, G4int               iIsospin,
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable )
 : G4ParticleDefinition( aName,mass,width,charge,iSpin,iParity,
              iConjugation,iIsospin,iIsospin3,gParity,pType,
              lepton,baryon,encoding,stable,lifetime,decaytable )
{
   SetParticleSubType("nucleon");  //////////photon before....

   //create Decay Table
   //   G4DecayTable*   table = GetDecayTable();
   //   if (table!=NULL) delete table;
   //   table = new G4DecayTable();

   // create a decay channel
   //   G4VDecayChannel* mode = new G4NeutronBetaDecayChannel("neutron",1.00);
   //   table->Insert(mode);

   //   SetDecayTable(table);
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// ......................................................................
// ...                 static member definitions                      ...
// ......................................................................
//
//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table


G4UCN* G4UCN::UCNDefinition()
{
  if(!theUCN)
    {
      theUCN = new G4UCN(
			 "ucn",  0.93956563*GeV,       0.0*MeV,         0.0,
			 1,              +1,             0,
			 1,              -1,             0,
			 "BARYON",        0,            +1,        2112,  //baryon
			 false,    886.7*second,          NULL);
      
      // lifetime of ucn 0 = 886.7 s
      
      G4cout << "UCN is created" << G4endl;
      }
      return theUCN;
}


G4UCN* G4UCN::UCN()
{    
  if(!theUCN) { theUCN = UCNDefinition(); }
  return theUCN;
} 


// **********************************************************************
// **************************** SetCuts *********************************
// **********************************************************************
//  In this version Input Cut Value is meaning less
//  theKineticEnergyCuts for all materials are set to LowestEnergy
void G4UCN::CalcEnergyCuts( const G4Material* )
{


  // Set Energy Cut values to lowest  for all materials
  //SetEnergyCutValues(LowestEnergy);

}







