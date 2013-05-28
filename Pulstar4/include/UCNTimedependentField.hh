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
////////////////////////////////////////////////////////////////////////
// 4.9.04 UCN time dependent field, peter fierlinger
// 
////////////////////////////////////////////////////////////////////////


#ifndef UCNTimedependentField_h
#define UCNTimedependentField_h 1

/////////////
// Includes
/////////////

#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4Step.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4UCN.hh"
#include "UCNUniformGravField.hh"
#include "UCNFieldMessenger.hh"

/////////////////////
// Class Definition
/////////////////////

class UCNTimedependentField
{

public: 
         UCNTimedependentField();
	~UCNTimedependentField();

public:

 	// routines that read the field conditions		    
	int readfield();
	
	// messenger communication
	void SetField(G4String stri);
	void SetFile(G4String stri);
	void DrawField(G4String stri);
	void SetFieldOnTime(G4String stri);
	void SetFieldTime(G4String stri);
	void SetFieldTimeFile(G4String tru);

	// set the neutron spin 	
	void SetSpin(G4ThreeVector stri);

	// visualization of field points
	void FieldVisualization();
	void readtimedependence();
        G4double GetFieldPercentage(double time);
        G4ThreeVector GetGradient(double posx, double posy, double posz);
 	G4double GetValue();
	    
	// communicate with the field    
	static UCNTimedependentField* GetTimedependentField();
	//void SetFieldEqPointer(UCNUniformGravField* grav);
	//void SetTimedependentField(UCNTimedependentField* inst);


private:

	UCNFieldMessenger* mfMessenger;  // pointer to the Messenger

	G4int just_reflected;
        static UCNTimedependentField*  theInstance;
        static G4double gtime;
        static G4ThreeVector spin;
        //static UCNUniformGravField * gravfield;
	// field array if we use coordinates
      	double FieldValues[10000][4];
       	double TimeRamp[50][2];
       	long fieldlen;      	
	double fieldstrength;
	// messenger variables
      	
	int fieldon;
     	G4String fieldfile;
      	int drawfield;
      	double starttime;
      	double fieldtime;
       	G4String fieldtimefile;
     	
};

#endif /* UCNTimedependentField_h */
