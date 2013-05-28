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
//   UCN guide messenger 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "UCNDetectorMessenger.hh"

#include "UCNDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

UCNDetectorMessenger::UCNDetectorMessenger(UCNDetectorConstruction * UCNDet)
:UCNDetector(UCNDet)
{ 
  UCNdetDir = new G4UIdirectory("/geometry/");
  UCNdetDir->SetGuidance("UCN guide control.");
  
  /*    
  AbsMaterCmd = new G4UIcmdWithAString("/geometry/setAbsMat",this);
  AbsMaterCmd->SetGuidance("Select Material of the Absorber.");
  AbsMaterCmd->SetParameterName("choice",true);
  AbsMaterCmd->SetDefaultValue("Lead");
  AbsMaterCmd->AvailableForStates(G4State_Idle);


  AbsThickCmd = new G4UIcmdWithADoubleAndUnit("/geometry/setAbsThick",this);
  AbsThickCmd->SetGuidance("Set Thickness of the Absorber");
  AbsThickCmd->SetParameterName("SizeZ",false,false);
  AbsThickCmd->SetDefaultUnit("mm");
  AbsThickCmd->SetRange("SizeZ>0.");
  AbsThickCmd->AvailableForStates(G4State_Idle);
  
  AbsRadCmd = new G4UIcmdWithADoubleAndUnit("/geometry/setAbsRad",this);
  AbsRadCmd->SetGuidance("Set radius of the Absorber");
  AbsRadCmd->SetParameterName("SizeR",false,false);
  AbsRadCmd->SetDefaultUnit("mm");
  AbsRadCmd->SetRange("SizeR>0.");
  AbsRadCmd->AvailableForStates(G4State_Idle);
  
  AbsZposCmd = new G4UIcmdWithADoubleAndUnit("/geometry/setAbsZpos",this);
  AbsZposCmd->SetGuidance("Set Z pos. of the Absorber");
  AbsZposCmd->SetParameterName("Zpos",false,false);
  AbsZposCmd->SetDefaultUnit("mm");
  AbsZposCmd->AvailableForStates(G4State_Idle);
  */

  WorldMaterCmd = new G4UIcmdWithAString("/geometry/setWorldMat",this);
  WorldMaterCmd->SetGuidance("Select Material of the World.");
  WorldMaterCmd->SetParameterName("wchoice",true);
  WorldMaterCmd->SetDefaultValue("Air");
  WorldMaterCmd->AvailableForStates(G4State_Idle);
    
  WorldXCmd = new G4UIcmdWithADoubleAndUnit("/geometry/setWorldX",this);
  WorldXCmd->SetGuidance("Set X size of the World");
  WorldXCmd->SetParameterName("WSizeX",false,false);
  WorldXCmd->SetDefaultUnit("mm");
  WorldXCmd->SetRange("WSizeX>0.");
  WorldXCmd->AvailableForStates(G4State_Idle);

  WorldYCmd = new G4UIcmdWithADoubleAndUnit("/geometry/setWorldY",this);
  WorldYCmd->SetGuidance("Set Y size of the World");
  WorldYCmd->SetParameterName("WSizeY",false,false);
  WorldYCmd->SetDefaultUnit("mm");
  WorldYCmd->SetRange("WSizeY>0.");
  WorldYCmd->AvailableForStates(G4State_Idle);

  WorldZCmd = new G4UIcmdWithADoubleAndUnit("/geometry/setWorldZ",this);
  WorldZCmd->SetGuidance("Set Z size of the World");
  WorldZCmd->SetParameterName("WSizeZ",false,false);
  WorldZCmd->SetDefaultUnit("mm");
  WorldZCmd->SetRange("WSizeZ>0.");
  WorldZCmd->AvailableForStates(G4State_Idle);
  
  
  UpdateCmd = new G4UIcmdWithoutParameter("/geometry/update",this);
  UpdateCmd->SetGuidance("Update geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

UCNDetectorMessenger::~UCNDetectorMessenger()
{
  //  delete AbsMaterCmd; 
  //  delete AbsThickCmd; 
  //  delete AbsRadCmd;  
  //  delete AbsZposCmd; 
  delete WorldMaterCmd;
  delete WorldXCmd;
  delete WorldYCmd;
  delete WorldZCmd;
  delete UpdateCmd;
  delete UCNdetDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void UCNDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  /*
  if( command == AbsMaterCmd )
   { UCNDetector->SetAbsorberMaterial(newValue);}
   
  if( command == AbsThickCmd )
   { UCNDetector->SetAbsorberThickness(AbsThickCmd->GetNewDoubleValue(newValue));}
   
  if( command == AbsRadCmd )
   { UCNDetector->SetAbsorberRadius(AbsRadCmd->GetNewDoubleValue(newValue));}
   
  if( command == AbsZposCmd )
   { UCNDetector->SetAbsorberZpos(AbsZposCmd->GetNewDoubleValue(newValue));}
  */

  if( command == WorldMaterCmd )
   { UCNDetector->SetWorldMaterial(newValue);}
      
  if( command == WorldXCmd )
   { UCNDetector->SetWorldSizeZ(WorldXCmd->GetNewDoubleValue(newValue));}

  if( command == WorldYCmd )
   { UCNDetector->SetWorldSizeZ(WorldYCmd->GetNewDoubleValue(newValue));}

  if( command == WorldZCmd )
   { UCNDetector->SetWorldSizeZ(WorldZCmd->GetNewDoubleValue(newValue));}
   
   
  if( command == UpdateCmd )
   { UCNDetector->UpdateGeometry(); }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....












