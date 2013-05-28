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
// $Id: UCNUniformGravField.cc,v 1.2 2005/02/08 10:57:28 kuzniak Exp $
// GEANT4 tag $Name:  $
//
// 30.1.97 V.Grichine
//
// 9.4.04 adapted for gravity by peter fierlinger
// 
//     
//
// -------------------------------------------------------------------

#include "UCNUniformGravField.hh"
#include "UCNTimedependentField.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4SteppingControl.hh"
#include "G4VParticleChange.hh"

UCNTimedependentField* UCNUniformGravField::theInstance = 0;
G4int UCNUniformGravField::index = 0;

UCNUniformGravField::UCNUniformGravField(const G4ThreeVector FieldVector )
{

      fFieldComponents[2] = FieldVector.z();
      fFieldComponents[0] = FieldVector.x();
      fFieldComponents[1] = FieldVector.y();

  G4cout << "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << G4endl;

 if (theInstance == 0) {
    UCNTimedependentField* timedep;
    timedep = new UCNTimedependentField();
 // G4cout << "get an instance of time dependence " << G4endl;      
  // UCNTimedependentField * timedep = UCNTimedependentField::GetTimedependentField();
    //  G4cout << "UCNUniformGravFieldfirst: getfieldvalue is first time called, ref to timedepfield " << timedep << G4endl;
  theInstance = timedep;
 }
}
   
UCNUniformGravField::UCNUniformGravField(G4double vField,
                                               G4double vTheta,
                                               G4double vPhi    )
{
  if(vField >= 0 &&
     vTheta >= 0 && vTheta <= pi &&
     vPhi >= 0 && vPhi <= twopi)
    {
      fFieldComponents[0] = vField*cos(vTheta) ;
      fFieldComponents[1] = vField*cos(vTheta) ;
      fFieldComponents[2] = vField*cos(vTheta) ;
    }
  else
    {
      //      G4Exception("Invalid parameters in UCNUniformGravField::UCNUniformGravField") ;
      G4cerr << "Invalid parameters in UCNUniformGravField::UCNUniformGravField" << G4endl;
      exit(1);
    }
  if (theInstance == 0) {
    UCNTimedependentField* timedep = new UCNTimedependentField();
    // G4cout << "get an instance of time dependence " << G4endl;      
    // UCNTimedependentField * timedep = UCNTimedependentField::GetTimedependentField();
    G4cout << "UCNUniformGravFieldfirst: getfieldvalue is first time called, ref to timedepfield " << timedep << G4endl;
    theInstance = timedep;
  }
}

UCNUniformGravField::~UCNUniformGravField()
{
  if(theInstance) delete theInstance;
}

UCNUniformGravField::UCNUniformGravField (const UCNUniformGravField &p)
   : G4ElectricField(p)
{
   for (G4int i=0; i<6; i++)
      fFieldComponents[i] = p.fFieldComponents[i];
}

UCNUniformGravField&
UCNUniformGravField::operator = (const UCNUniformGravField &p)
{
   for (G4int i=0; i<6; i++)
      fFieldComponents[i] = p.fFieldComponents[i];
   return *this;
}

// ------------------------------------------------------------------------

void UCNUniformGravField::GetFieldValue (const G4double point[4],
                                            G4double *G) const 
{


if (index == 0){//

 UCNTimedependentField * timedep = UCNTimedependentField::GetTimedependentField();
 //G4cout << "UCNUniformGravFieldfirst: getfieldvalue is first time called, ref to timedepfield " << timedep << G4endl;
 theInstance = timedep;

 index = 1;
}

  if( ! theInstance == 0){
    G4ThreeVector newgrad = theInstance->GetGradient(point[0],point[1],point[2]);
    double fieldpercentage = theInstance->GetFieldPercentage(point[3]*1e-9);
   //G4cout << "theinstance " << theInstance << ", newgrad " << newgrad 
    //<< "; fieldpercentage " << fieldpercentage << G4endl;
    //G4cout << "point " << point[0] << "; 1 " << point[1] << ", p2 " << point[2] << G4endl;
    //G4cout << "point3 " << point[3] << "; 4 " << point[4] << G4endl;
    
    //G4cout << "fieldcomponents " << fFieldComponents[0] << ", " <<
    //fFieldComponents[1] << ", " << fFieldComponents[2] << G4endl; 
    
    // force scalingfactor
    G4double coef1 = 1.915e-14;
    
    newgrad = newgrad * fieldpercentage * coef1;
    
    // add the magneticfield to the fieldcoordinates 
    G[0]= fFieldComponents[0] - newgrad.getX();
    G[1]= fFieldComponents[1] - newgrad.getY() ;
    G[2]= fFieldComponents[2] - newgrad.getZ() ;
  }
  else{
    G[0]= fFieldComponents[0] ;
    G[1]= fFieldComponents[1] ;
    G[2]= fFieldComponents[2] ;
  }
   //G4cout << "calc fieldpoint " << G[0] << ", " << G[1] << ", " << G[2] << G4endl;
   return ;
   
}

