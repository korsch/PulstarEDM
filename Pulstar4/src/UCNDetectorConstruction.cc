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
//   UCN detector construction, 9.9.04, peter fierlinger 
// 

#include "UCNDetectorConstruction.hh"
#include "UCNDetectorMessenger.hh"

#include "UCNFieldSetup.hh"

//#include "G4VClusterModel.hh"
//#include "G4PAIclusterModel.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4CutTubs.hh"
#include "G4Cons.hh"
#include "G4EllipticalTube.hh"
#include "G4Orb.hh"
#include "G4CSGSolid.hh"
#include "G4Torus.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4UserLimits.hh"
#include "G4ios.hh"
#include "G4VisAttributes.hh"

#include "G4Torus.hh"
#include "G4Box.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"


G4double t01innerradius(0.0);
double xOffset(20.0*cm);
//G4double WorldSizeZ = 70.0*cm;

/////////////////////////////////////////////////////////////////////////////
//
//

UCNDetectorConstruction::UCNDetectorConstruction()
 : solidWorld(0), logicWorld(0), physiWorld(0),
   solidAbsorber(0),logicAbsorber(0), physiAbsorber(0),
   fEmFieldSetup(0),
   AbsorberMaterial(0), worldchanged(false), WorldMaterial(0)
{
  // default parameter values of the calorimeter

  //  extern G4double WorldSizeZ;
  //  WorldSizeX = 30.*cm;
  //  WorldSizeY = 20.*cm;
  //  WorldSizeZ = 50.*cm;
  WorldSizeX = 30.*cm;
  WorldSizeY = 20.*cm;
  WorldSizeZ = 30.*cm;

  AbsorberThickness = 0.0*mm;

  AbsorberRadius   = 2.*cm;
  zAbsorber = -20.*cm ;

  /*
  Shutter1x      = 0.*cm;
  Shutter1y      = 3.*cm;
  Shutter1z      = -40.*cm;
  Shutter1Radius = 4.*cm;
  Shutter1Thickness = 0.*cm;
  */

  // create commands for interactive definition of the calorimeter  

  detectorMessenger = new UCNDetectorMessenger(this);
  
  DefineMaterials();

  fEmFieldSetup = new UCNFieldSetup() ;
}

//////////////////////////////////////////////////////////////////////////
//
//

UCNDetectorConstruction::~UCNDetectorConstruction()
{ 
  delete detectorMessenger;
  delete fEmFieldSetup ;
}

//////////////////////////////////////////////////////////////////////////
//
//

G4VPhysicalVolume* UCNDetectorConstruction::Construct()
{
  return ConstructCalorimeter();
}

//////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::DefineMaterials()
{ 

#include "UCNDetectorMaterials.icc"

}

/////////////////////////////////////////////////////////////////////////
//
//
  
G4VPhysicalVolume* UCNDetectorConstruction::ConstructCalorimeter()
{
  // complete the Calor parameters definition and Print 

  ComputeCalorParameters();
  //PrintCalorParameters();
      
  // Cleanup old geometry

  if (physiWorld)
  {
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
  }

  // World

  solidWorld
    = new G4Box("World",WorldSizeX,WorldSizeY,WorldSizeZ);

  logicWorld = new G4LogicalVolume(solidWorld,		//its solid
                                   WorldMaterial,	        //its material
                                   "World");		//its name
                                   
  physiWorld = new G4PVPlacement(0,			//no rotation
  				 G4ThreeVector(),	//at (0,0,0)
                                 "World",		//its name
                                 logicWorld,		//its logical volume
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number                             
  

  //  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);

  /*
  G4VisAttributes* logicWorldVisAtt
    = new G4VisAttributes(G4Colour(0,0,0));
  logicWorldVisAtt->SetForceWireframe(true);
  logicWorld->SetVisAttributes(logicWorldVisAtt);
  */


  G4UserLimits * theWorldUserLimits=new G4UserLimits(2.0*cm);
  logicWorld->SetUserLimits(theWorldUserLimits);
 

 G4VisAttributes* logicTubeVisAtt = new G4VisAttributes(G4Colour(0/255.,204/255. ,204/255. ));
 logicTubeVisAtt->SetForceWireframe(true);
 // logicTubeVisAtt->SetForceSolid(true);

  G4double delta = 0.1*mm;
  G4double epsilon = 1.0e-3*mm;

  extern G4double t01innerradius;

  //  G4double totallength = 508.0*mm + 3.0*cm;
  G4double totallength = 58.0*mm + 3.0*cm;
  G4double c1length = 30.0*mm; //mm  

  // tube from Pulstar
  t01innerradius = 0.5*64.77*mm; // mm 
  //  G4double t01outerradius = 0.5*69.85*mm; // mm 
  G4double t01outerradius = t01innerradius + 0.5*mm; // mm 
  G4double t01length = totallength - c1length;
  G4Tubs* tube01 = new G4Tubs("tube01",t01innerradius,t01outerradius,0.5*t01length,0.,twopi); 

  extern double xOffset;

  G4LogicalVolume*   logicTube01 = new G4LogicalVolume(tube01, BeamTubeMaterial,"tube01");   
  G4double phit01, xt01, yt01, zt01;
  phit01 = 0.0*degree;
  //  xt01 = 0.0*mm;
  xt01 = - WorldSizeX + xOffset;
  yt01 = 0.0*mm; 
  zt01 = -WorldSizeZ  + 0.5*t01length;
  G4RotationMatrix rott01;
  rott01.rotateY(phit01);
  G4VPhysicalVolume* physiTube01 = new G4PVPlacement(G4Transform3D(rott01,G4ThreeVector(xt01,yt01,zt01)),"tube01", logicTube01, physiWorld, false, 0);

  /*
  G4VisAttributes * Roux       = new G4VisAttributes( G4Colour(204/255. ,0/255.   ,51/255.  ));
  G4VisAttributes * Brown      = new G4VisAttributes( G4Colour(255/255. ,153/255. ,153/255. ));
  G4VisAttributes * Metal      = new G4VisAttributes( G4Colour(204/255. ,204/255. ,255/255. ));
  G4VisAttributes * Bleu       = new G4VisAttributes( G4Colour(0/255.   ,0/255.   ,255/255. ));
  G4VisAttributes * Violet     = new G4VisAttributes( G4Colour(153/255. ,0/255.   ,255/255. ));
  G4VisAttributes * Gray       = new G4VisAttributes( G4Colour(153/255. ,153/255. ,153/255. ));
  G4VisAttributes* Red        = new G4VisAttributes( G4Colour(255/255. ,0/255.   ,0/255.   ));
  G4VisAttributes * Yellow     = new G4VisAttributes( G4Colour(255/255. ,255/255. ,0/255.   ));
  G4VisAttributes * LightBleu  = new G4VisAttributes( G4Colour(0/255.   ,204/255. ,204/255. ));
  G4VisAttributes * LightGreen = new G4VisAttributes( G4Colour(153/255. ,255/255. ,153/255. ));
  G4VisAttributes* Violet     = new G4VisAttributes( G4Colour(153/255. ,0/255.   ,255/255. ));
  */
  G4VisAttributes* logicTube01VisAtt = new G4VisAttributes(G4Colour(153/255. ,0/255.   ,255/255. ));
  //  logicTube01VisAtt->SetForceWireframe(true);
  logicTube01VisAtt->SetForceSolid(true);
  logicTube01->SetVisAttributes(logicTube01VisAtt);

  // tube from Pulstar (inner)
  G4double t01aouterradius = t01innerradius;
  G4double t01ainnerradius = 0.0; // mm 
  G4Tubs* tube01a = new G4Tubs("tube01a",t01ainnerradius,t01aouterradius,0.5*t01length,0.,twopi); 

  G4LogicalVolume*   logicTube01a = new G4LogicalVolume(tube01a, WorldMaterial,"tube01a");   
  G4double phit01a, xt01a, yt01a, zt01a;
  phit01a = 0.0*degree;
  //  xt01 = 0.0*mm;
  xt01a = xt01;
  yt01a = yt01;
  zt01a = zt01;
  G4RotationMatrix rott01a;
  rott01a.rotateY(phit01a);
  //  //  G4VPhysicalVolume* physiTube01a = new G4PVPlacement(G4Transform3D(rott01a,G4ThreeVector(xt01a,yt01a,zt01a)),"tube01a", logicTube01a, physiWorld, false, 0);

  G4VisAttributes* logicTube01aVisAtt = new G4VisAttributes(G4Colour(0.8,0.8,0));
  //  logicTube01aVisAtt->SetForceWireframe(true);
  logicTube01aVisAtt->SetForceSolid(true);
  logicTube01a->SetVisAttributes(logicTube01aVisAtt);

  // V1 valve, 1 cm hole
  //  G4double Rv1 = 6.0*mm;
  //  G4double gapopening  = 5.0*mm; // < t4innerradius (5 mm), ring
  //  G4double RemoveCompletely =  5.0*mm; //0.5*t3alength-Rv1; //0.0*cm;
  //  G4double t4innerradius = 0.5*10.0 * mm; // mm 
  //  G4double t3innerradius = 0.5*31.75; // mm 


  //V1 valve 2 cm hole
  G4double Rv1 = 12.0*mm;
  G4double gapopening  = 10.0*mm; // < t4innerradius (5 mm), ring
  //  G4double gapopening  = 5.0*mm; // < t4innerradius (5 mm), ring
  G4double RemoveCompletely =  1.31663*mm; //0.5*t3alength-Rv1; //0.0*cm;
  //  G4double RemoveCompletely =  0.0*mm; //0.5*t3alength-Rv1; //0.0*cm;
  G4double t4innerradius = 0.5*20.0 * mm; // mm 
  G4double t3innerradius = 0.5*51.75; // mm 
  //------------------------

  //  G4double t3outerradius = 0.5*45.18; // mm 
  G4double t3outerradius = t3innerradius+0.5*mm; // mm 

  G4double b1outerradius = 1.5*0.5*2.54*cm;
  G4double b1innerradius = 1.5*0.5*24.89*mm;
  //  G4double b1outerradius = 1.0*0.5*2.54*cm;
  //  G4double b1innerradius = 1.0*0.5*24.89*mm;


  // Cone for neutron guide, reduce large diameter to small diameter for EDM
  G4double c1a_outerradius = t01outerradius; // mm 
  G4double c1a_innerradius = t01innerradius; // mm 
  G4double c1b_outerradius = b1outerradius;
  G4double c1b_innerradius = b1innerradius;

  G4Cons* cons1 = new G4Cons("cons1",c1a_innerradius,c1a_outerradius,c1b_innerradius,c1b_outerradius,0.5*c1length,0.,twopi); 


  G4LogicalVolume*   logicCons1 = new G4LogicalVolume(cons1, BeamTubeMaterial,"cons1");   
  G4double phic1, xc1, yc1, zc1;
  phit01 = 0.0*degree;
  //  xt01 = 0.0*mm;
  xc1 = xt01; 
  yc1 = yt01;
  zc1 = zt01 + 0.5*(t01length+c1length);
  G4RotationMatrix rotc1;
  rotc1.rotateY(phic1);
  G4VPhysicalVolume* physiCons1 = new G4PVPlacement(G4Transform3D(rotc1,G4ThreeVector(xc1,yc1,zc1)),"cons1", logicCons1, physiWorld, false, 0);

 G4VisAttributes* logicConsVisAtt = new G4VisAttributes(G4Colour(204/255. ,0/255.   ,51/255.  ));
 // logicConsVisAtt->SetForceWireframe(true);
 logicConsVisAtt->SetForceSolid(true);
 logicCons1->SetVisAttributes(logicConsVisAtt);

  // Cone (inner)
  G4double c2a_outerradius = c1a_innerradius; // mm 
  G4double c2a_innerradius = 0.0;
  G4double c2b_outerradius = c1b_innerradius;
  G4double c2b_innerradius = 0.0;

  G4Cons* cons2 = new G4Cons("cons2",c2a_innerradius,c2a_outerradius,c2b_innerradius,c2b_outerradius,0.5*c1length,0.,twopi); 

  G4LogicalVolume* logicCons2 = new G4LogicalVolume(cons2, WorldMaterial,"cons2");   
  G4double phic2, xc2, yc2, zc2;
  phit01 = 0.0*degree;
  //  xt01 = 0.0*mm;
  xc2 = xc1; 
  yc2 = yc1;
  zc2 = zc1;
  G4RotationMatrix rotc2;
  rotc2.rotateY(phic2);
  //  //  G4VPhysicalVolume* physiCons2 = new G4PVPlacement(G4Transform3D(rotc2,G4ThreeVector(xc2,yc2,zc2)),"cons2", logicCons2, physiWorld, false, 0);

 G4VisAttributes* logicCons2VisAtt = new G4VisAttributes(G4Colour(0.8,0.8,0));
 // logicCons2VisAtt->SetForceWireframe(true);
 logicCons2VisAtt->SetForceSolid(true);
 logicCons2->SetVisAttributes(logicCons2VisAtt);


  // reduced neutron guide, straight section
 //  G4double b1length = 109.22*mm;
  G4double b1length = 189*mm;
  G4double b1SPhi = 0.0*degree;
  G4double b1DPhi = 360.0*degree;
  G4ThreeVector b1LowNorm(0,0,-1);
  G4ThreeVector b1HighNorm(0.383,0,0.924);

  G4CutTubs* bend1 = new G4CutTubs("bend1",b1innerradius, b1outerradius, 0.5*b1length, b1SPhi, b1DPhi, b1LowNorm, b1HighNorm);
  G4LogicalVolume*   logicBend1 = new G4LogicalVolume(bend1, YellowTubeMaterial,"bend1");   

  G4double phib1, xb1, yb1, zb1;
  phib1 = 0*degree;
  xb1 = xc1;
  yb1 = yc1;
  zb1 = zc1+0.5*(c1length+b1length);
  G4RotationMatrix rotb1;
  rotb1.rotateX(phib1);
  G4VPhysicalVolume* physiBend1 = new G4PVPlacement(G4Transform3D(rotb1,G4ThreeVector(xb1,yb1,zb1)),"bend1", logicBend1, physiWorld, false, 0);

  logicBend1->SetVisAttributes(logicTubeVisAtt);

  // bend left (inner)
  G4double b1aouterradius = b1innerradius;
  G4double b1ainnerradius = 0.0;
  G4double b1alength = b1length;
  G4double b1aSPhi = 0.0*degree;
  G4double b1aDPhi = 360.0*degree;
  G4ThreeVector b1aLowNorm(0,0,-1);
  G4ThreeVector b1aHighNorm(0.383,0,0.924);

  G4CutTubs* bend1a = new G4CutTubs("bend1a",b1ainnerradius, b1aouterradius, 0.5*b1alength, b1aSPhi, b1aDPhi, b1aLowNorm, b1aHighNorm);
  G4LogicalVolume* logicBend1a = new G4LogicalVolume(bend1a, WorldMaterial,"bend1a");   

  G4double phib1a, xb1a, yb1a, zb1a;
  phib1a = 0*degree;
  xb1a = xc1;
  yb1a = yc1;
  zb1a = zc1+0.5*(c1length+b1alength);
  G4RotationMatrix rotb1a;
  rotb1a.rotateX(phib1a);
  //  //G4VPhysicalVolume* physiBend1a = new G4PVPlacement(G4Transform3D(rotb1a,G4ThreeVector(xb1a,yb1a,zb1a)),"bend1a", logicBend1a, physiWorld, false, 0);

  logicBend1a->SetVisAttributes(logicTubeVisAtt);
  
  G4double t3length = 100.0*mm; //mm
  //  G4double t3length = 71.0*mm; //mm
  //  G4double t3length = 51.0*mm; //mm

  double TubeWallAdjustment = (t3outerradius-t3innerradius); 

  double theta = asin(b1innerradius/t3innerradius);

  // bend2a (top half) and bend2b (bottom half)  are final sections to vestibule
  G4double b2aouterradius = b1outerradius;
  G4double b2ainnerradius = b1innerradius;
  //  G4double b2alength = 78.74*mm;
  G4double b2alength = 81*mm;
  G4double b2aSPhi = 0.0*degree;
  G4double b2aDPhi = 180.0*degree;
  G4double alpha = atan((t3innerradius*(1.0-cos(theta)))/b2ainnerradius);
  G4ThreeVector b2aLowNorm(0.383,0,-0.924);
  //  G4ThreeVector b2aHighNorm(0,0,1);
  G4ThreeVector b2aHighNorm(0,-sin(alpha),cos(alpha));


  G4CutTubs* bend2a = new G4CutTubs("bend2a",b2ainnerradius, b2aouterradius, 0.5*b2alength, b2aSPhi, b2aDPhi, b2aLowNorm, b2aHighNorm);
  G4LogicalVolume*   logicBend2a = new G4LogicalVolume(bend2a, YellowTubeMaterial,"bend2a");   

  // small n guide 45 degree bent section, top
  G4double phib2a, xb2a, yb2a, zb2a;
  phib2a = 45.0*degree;
  xb2a = xb1 + 0.5*b2alength*cos(phib2a);
  yb2a = yb1;
  zb2a = zb1 + 0.5*b1length  + 0.5*b2alength*sin(phib2a);
  G4RotationMatrix rotb2a;
  rotb2a.rotateY(phib2a);
  G4VPhysicalVolume* physiBend2a = new G4PVPlacement(G4Transform3D(rotb2a,G4ThreeVector(xb2a,yb2a,zb2a)),"bend2a", logicBend2a, physiWorld, false, 0);

  logicBend2a->SetVisAttributes(logicTubeVisAtt);


  G4double b2bouterradius = b2aouterradius;
  G4double b2binnerradius = b2ainnerradius;
  G4double b2blength = b2alength;
  G4double b2bSPhi = 180.0*degree;
  G4double b2bDPhi = 180.0*degree;
  G4ThreeVector b2bLowNorm(0.383,0,-0.924);
  G4ThreeVector b2bHighNorm(0,sin(alpha),cos(alpha));


  G4CutTubs* bend2b = new G4CutTubs("bend2b",b2binnerradius, b2bouterradius, 0.5*b2blength, b2bSPhi, b2bDPhi, b2bLowNorm, b2bHighNorm);
  G4LogicalVolume*   logicBend2b = new G4LogicalVolume(bend2b, YellowTubeMaterial,"bend2b");   

  // small n guide 45 degree bent section, bottom
  G4double phib2b, xb2b, yb2b, zb2b;
  phib2b = 45.0*degree;
  xb2b = xb1 + 0.5*b2blength*cos(phib2b);
  yb2b = yb1;
  zb2b = zb1 + 0.5*b1length  + 0.5*b2blength*sin(phib2b);
  G4RotationMatrix rotb2b;
  rotb2b.rotateY(phib2b);
  G4VPhysicalVolume* physiBend2b = new G4PVPlacement(G4Transform3D(rotb2b,G4ThreeVector(xb2b,yb2b,zb2b)),"bend2b", logicBend2b, physiWorld, false, 0);

  logicBend2b->SetVisAttributes(logicTubeVisAtt);


 //  begin vacuum transition: neutron guide to vestibule
  // fill inner tube with vacuum
  //short transition section (insert) from guide to vestibule, top
  //------------------------------------
  G4double b3aouterradius = b2ainnerradius;
  G4double b3ainnerradius = 0.0*mm;
  //  G4double b3alength = b2alength + delta;
  G4double b3alength = TubeWallAdjustment + t3innerradius*(1.0 - cos(theta));
  G4double b3aSPhi = 0.0*degree;
  G4double b3aDPhi = 180.0*degree;
  G4ThreeVector b3aLowNorm(0,0,-1);
//  G4ThreeVector b3aLowNorm(0.383,0,-0.924);
  G4ThreeVector b3aHighNorm(0,-sin(alpha),cos(alpha));


  G4CutTubs* bend3a = new G4CutTubs("bend3a",b3ainnerradius, b3aouterradius, 0.5*b3alength, b3aSPhi, b3aDPhi, b3aLowNorm, b3aHighNorm);
  G4LogicalVolume*   logicBend3a = new G4LogicalVolume(bend3a, WorldMaterial,"bend3a");   


  G4double phib3a, xb3a, yb3a, zb3a;
  phib3a = 45.0*degree;
  xb3a = xb2a + 0.5*(b2alength-b3alength)*cos(phib3a);
  yb3a = yb2a;
  zb3a = zb2a + 0.5*(b2alength -b3alength)*sin(phib3a);
  G4RotationMatrix rotb3a;
  rotb3a.rotateY(phib3a);
  G4VPhysicalVolume* physiBend3a = new G4PVPlacement(G4Transform3D(rotb3a,G4ThreeVector(xb3a,yb3a,zb3a)),"bend3a", logicBend3a, physiWorld, false, 0);

 G4VisAttributes* logicBend3VisAtt = new G4VisAttributes(G4Colour(1.0,0,1.0));
 logicBend3VisAtt->SetForceWireframe(true);
 // logicBend3VisAtt->SetForceSolid(true);
 logicBend3a->SetVisAttributes(logicBend3VisAtt);


  //short transition section (insert) from guide to vestibule, bottom
  //------------------------------------
  G4double b3bouterradius = b3aouterradius;
  G4double b3binnerradius = 0.0*mm;
  G4double b3blength = b3alength;
  G4double b3bSPhi = 180.0*degree;
  G4double b3bDPhi = 180.0*degree;
  //G4ThreeVector b3bLowNorm(0.383,0,-0.924);
  G4ThreeVector b3bLowNorm(0,0,-1);
  G4ThreeVector b3bHighNorm(0,sin(alpha),cos(alpha));


  G4CutTubs* bend3b = new G4CutTubs("bend3b",b3binnerradius, b3bouterradius, 0.5*b3blength, b3bSPhi, b3bDPhi, b3bLowNorm, b3bHighNorm);
  G4LogicalVolume*   logicBend3b = new G4LogicalVolume(bend3b, WorldMaterial,"bend3b");   


  G4double phib3b, xb3b, yb3b, zb3b;
  //  phib3b = 90*degree;
  phib3b = 45.0*degree;
  xb3b = xb3a;
  yb3b = yb3a;
  zb3b = zb3a;
  G4RotationMatrix rotb3b;
  rotb3b.rotateY(phib3b);
  G4VPhysicalVolume* physiBend3b = new G4PVPlacement(G4Transform3D(rotb3b,G4ThreeVector(xb3b,yb3b,zb3b)),"bend3b", logicBend3b, physiWorld, false, 0);

 logicBend3b->SetVisAttributes(logicBend3VisAtt);
 // end vacuum transition: neutron guide to vestibule

  G4double t2clength = 1.0*mm; //mm
  G4Tubs* tube2c = new G4Tubs("tube2c",0.0*mm,b1innerradius,0.5*t2clength,0.,twopi); 

  G4LogicalVolume*   logicTube2c = new G4LogicalVolume(tube2c, BeamTubeWindowMaterial,"tube2c");   
  G4double phit2c, xt2c, yt2c, zt2c;
  phit2c = 0*degree;
  xt2c = 0.0;
  yt2c = 0.0;
  zt2c = 0.5*b2blength;
  G4RotationMatrix rott2c;
  rott2c.rotateY(phit2c);
  //  //  //  G4VPhysicalVolume* physiTube2c = new G4PVPlacement(G4Transform3D(rott2c,G4ThreeVector(xt2c,yt2c,zt2c)),"tube2c", logicTube2c, physiBend2b, false, 0);

  G4VisAttributes* logicTube2cVisAtt = new G4VisAttributes(G4Colour(0,0,1));
  //  logicTube2cVisAtt->SetForceWireframe(true);
  logicTube2cVisAtt->SetForceSolid(true);
  logicTube2c->SetVisAttributes(logicTube2cVisAtt);


  // neutron entrance window to vestibule ----------------------------------
  G4double w1outerradius = b1innerradius;
  G4double w1innerradius = 0.0;
  //  G4double w1length = 1*cm;
  //  G4double w1length = 0.1*mm;
  G4double w1length = 1.0*mm;
  G4double w1SPhi = 0.0*degree;
  G4double w1DPhi = 360.0*degree;

  G4Tubs* window1 = new G4Tubs("window1",w1innerradius, w1outerradius, 0.5*w1length, w1SPhi, w1DPhi);
  G4LogicalVolume* logicWindow1 = new G4LogicalVolume(window1, BeamTubeWindowMaterial,"window1");   

  G4double phiw1, xw1, yw1, zw1;
  phiw1 = 45.0*degree;
  xw1 = xb3a + 0.5*(b3alength-w1length)*sin(phiw1); 
  yw1 = yb3a; 
  zw1 = zb3a + 0.5*(b3alength-w1length)*cos(phiw1); 
  G4RotationMatrix rotw1;
  rotw1.rotateY(phiw1);
    G4VPhysicalVolume* physiWindow1 = new G4PVPlacement(G4Transform3D(rotw1,G4ThreeVector(xw1,yw1,zw1)),"window1", logicWindow1, physiWorld, false, 0);

  G4VisAttributes* logicW1VisAtt = new G4VisAttributes(G4Colour(255./255.,0.0/255. ,0.0/255. ));
  //  logicW1VisAtt->SetForceWireframe(true);
  logicW1VisAtt->SetForceSolid(true);

  logicWindow1->SetVisAttributes(logicW1VisAtt);
  // end entrance window to vestibule ------------------------------


  //injection chamber outer - vestibule
  G4Tubs* tube3 = new G4Tubs("tube3",t3innerradius,t3outerradius,0.5*t3length,0.,twopi); 

  G4LogicalVolume*   logicTube3 = new G4LogicalVolume(tube3, CellCoating,"tube3");   
  G4double phit3, xt3, yt3, zt3;
  phit3 = -45.0*degree;
  double offcenter = 0.0*cm; // 
  xt3 = xb2a - (0.5*b2alength+t3outerradius-TubeWallAdjustment)*sin(phit3) + offcenter*cos(phit3);
  yt3 = yb2a; 
  zt3 = zb2a + (0.5*b2alength+t3outerradius-TubeWallAdjustment)*cos(phit3) + offcenter*sin(phit3);;
  G4RotationMatrix rott3;
  rott3.rotateY(phit3);
  G4VPhysicalVolume* physiTube3 = new G4PVPlacement(G4Transform3D(rott3,G4ThreeVector(xt3,yt3,zt3)),"tube3", logicTube3, physiWorld, false, 0);

  G4VisAttributes* logicTube3VisAtt = new G4VisAttributes(G4Colour(1,0.5,1));
  logicTube3VisAtt->SetForceWireframe(true);
  //  logicTube3VisAtt->SetForceSolid(true);
  logicTube3->SetVisAttributes(logicTube3VisAtt);

  //injection chamber inner - inner vestibule
  G4double t3aouterradius = t3innerradius; //+5*mm; //+epsilon; 
  G4double t3ainnerradius = 0.0*mm; // mm 
  G4double t3alength = t3length;// + 3*mm; //mm
  G4Tubs* tube3a = new G4Tubs("tube3a",t3ainnerradius,t3aouterradius,0.5*t3alength,0.,twopi); 

  G4LogicalVolume*   logicTube3a = new G4LogicalVolume(tube3a, WorldMaterial,"tube3a");   
  G4double phit3a, xt3a, yt3a, zt3a;
  phit3a = phit3;
  //  phit3a = 0;
  xt3a = xt3;
  yt3a = yt3; 
  zt3a = zt3;
  //  xt3a = 0;
  //  yt3a = 0; 
  //  zt3a = 0;

  G4SubtractionSolid* solidA = new G4SubtractionSolid("diffA", tube3a, bend2a); 
  G4SubtractionSolid* solidB = new G4SubtractionSolid("diffB", tube3a, bend2b); 

  G4RotationMatrix rott3a;
  rott3a.rotateY(phit3a);
  //  //  G4VPhysicalVolume* physiTube3a = new G4PVPlacement(G4Transform3D(rott3a,G4ThreeVector(xt3a,yt3a,zt3a)),"tube3a", logicTube3a, physiTube3, false, 0);
  //  //  G4VPhysicalVolume* physiTube3a = new G4PVPlacement(G4Transform3D(rott3a,G4ThreeVector(xt3a,yt3a,zt3a)),"tube3a", logicTube3a, physiWorld, false, 0);

  // G4SubtractionSolid(  const G4String& pName,
  //                               G4VSolid* pSolidA ,
  //                               G4VSolid* pSolidB   ) ;


  G4VisAttributes* logicTube3aVisAtt = new G4VisAttributes(G4Colour(1,1,1));
  //  logicTube3aVisAtt->SetForceWireframe(true);
  logicTube3aVisAtt->SetForceSolid(true);
  logicTube3a->SetVisAttributes(logicTube3aVisAtt);

  //reflector
  G4double phir, xr, yr, zr;
  phir = -90.0*degree;
  //   phir = -105.0*degree;
  //  phir = 0.0*degree;

  G4double rx= 2*(t3innerradius)/cos(fabs(phir) - fabs(phit3));
  G4double rz= 0.10*mm;
  G4double ry= 2*t3innerradius;
  G4EllipticalTube* reflector = new G4EllipticalTube("reflector", 0.5*rx, 0.5*ry, 0.5*rz);
  G4LogicalVolume* logicReflector = new G4LogicalVolume(reflector, CellCoating,"reflector");   
  xr = xt3;
  yr = yt3;
  zr = zt3;
  G4RotationMatrix rotr;
  rotr.rotateY(phir);
  //  //G4VPhysicalVolume* physiReflector = new G4PVPlacement(G4Transform3D(rotr,G4ThreeVector(xr,yr,zr)),"reflector", logicReflector, physiWorld, false, 0);

  G4VisAttributes* logicReflectorVisAtt = new G4VisAttributes(G4Colour(1,1));
  //  logicReflectorVisAtt->SetForceWireframe(true);
  logicReflectorVisAtt->SetForceSolid(true);
  logicReflector->SetVisAttributes(logicReflectorVisAtt);


  // lid 1
  G4double t4outerradius = t3outerradius; // mm 
  G4double t4length = 0.1*mm; // (1.88) * mm; //mm
  G4Tubs* tube4 = new G4Tubs("tube4",t4innerradius,t4outerradius,0.5*t4length,0.,twopi); 

  G4LogicalVolume* logicTube4 = new G4LogicalVolume(tube4, CellCoating,"tube4");   
  G4double phit4, xt4, yt4, zt4;
  //  phit4 = 0.0*degree;
  phit4 = -45.0*degree;
  xt4 = xt3 + 0.5*(t3length + t4length)*sin(phit4); 
  yt4 = yt3; 
  zt4 = zt3 + 0.5*(t3length + t4length)*cos(phit4); 
  G4RotationMatrix rott4;
  rott4.rotateY(phit4);
  G4VPhysicalVolume* physiTube4 = new G4PVPlacement(G4Transform3D(rott4,G4ThreeVector(xt4,yt4,zt4)),"tube4", logicTube4, physiWorld, false, 0);

  G4VisAttributes* logicTube4VisAtt = new G4VisAttributes(G4Colour(1,0.5,1));
  logicTube4VisAtt->SetForceWireframe(true);
  //  logicTube4VisAtt->SetForceSolid(true);
  logicTube4->SetVisAttributes(logicTube4VisAtt);

  // hole through lid 1 (+extension) 
  G4double t4ainnerradius = t4innerradius - 0.1*mm; 
  G4double t4aouterradius = t4innerradius; 
  //  G4double t4alength = 1.88*mm+delta; //mm
  G4double t4alength = 10.0*mm; //mm
  G4Tubs* tube4a = new G4Tubs("tube4a",t4ainnerradius,t4aouterradius,0.5*t4alength,0.,twopi); 

  G4LogicalVolume* logicTube4a = new G4LogicalVolume(tube4a, CellCoating,"tube4a");   
  G4double phit4a, xt4a, yt4a, zt4a;
  phit4a = phit4;
  xt4a = xt4 - 0.5*(t4length+t4alength)*cos(phit4a);
  yt4a = yt4; 
  zt4a = zt4 - 0.5*(t4length +t4alength)*sin(phit4a);
  G4RotationMatrix rott4a;
  rott4a.rotateY(phit4a);
  G4VPhysicalVolume* physiTube4a = new G4PVPlacement(G4Transform3D(rott4a,G4ThreeVector(xt4a,yt4a,zt4a)),"tube4a", logicTube4a, physiWorld, false, 0);

  G4VisAttributes* logicTube4aVisAtt = new G4VisAttributes(G4Colour(1,1,1));
  logicTube4aVisAtt->SetForceWireframe(true);
  //  logicTube4aVisAtt->SetForceSolid(true);
  logicTube4a->SetVisAttributes(logicTube4aVisAtt);


  // fill hole through lid 1 
  G4double t4binnerradius = 0.0;
  G4double t4bouterradius = t4ainnerradius; // mm 
  G4double t4blength = t4alength; //mm
  G4Tubs* tube4b = new G4Tubs("tube4b",t4binnerradius,t4bouterradius,0.5*t4blength,0.,twopi); 

  G4LogicalVolume* logicTube4b = new G4LogicalVolume(tube4b, FillMaterial,"tube4b");   
  G4double phit4b, xt4b, yt4b, zt4b;
  phit4b = phit4;
  xt4b = xt4a;
  yt4b = yt4a; 
  zt4b = zt4a;
  G4RotationMatrix rott4b;
  rott4b.rotateY(phit4b);
  //  //  G4VPhysicalVolume* physiTube4b = new G4PVPlacement(G4Transform3D(rott4b,G4ThreeVector(xt4b,yt4b,zt4b)),"tube4b", logicTube4b, physiWorld, false, 0);

  G4VisAttributes* logicTube4bVisAtt = new G4VisAttributes(G4Colour(1,1,1));
  logicTube4bVisAtt->SetForceWireframe(true);
  //  logicTube4bVisAtt->SetForceSolid(true);
  logicTube4b->SetVisAttributes(logicTube4bVisAtt);


  // lid 2
  G4double t5outerradius = t3outerradius; // mm 
  G4double t5innerradius = 0; // mm 
  G4double t5length = t4length; //mm
  G4Tubs* tube5 = new G4Tubs("tube5",t5innerradius,t5outerradius,0.5*t5length,0.,twopi); 

  G4LogicalVolume* logicTube5 = new G4LogicalVolume(tube5, CellCoating,"tube5");   
  G4double phit5, xt5, yt5, zt5;
  phit5 = -45.0*degree;
  xt5 = xt3 - 0.5*(t3length + t5length)*sin(phit5); 
  yt5 = yt3; 
  zt5 = zt3  - 0.5*(t3length + t5length)*cos(phit5); 
  G4RotationMatrix rott5;
  rott5.rotateY(phit5);
  G4VPhysicalVolume* physiTube5 = new G4PVPlacement(G4Transform3D(rott5,G4ThreeVector(xt5,yt5,zt5)),"tube5", logicTube5, physiWorld, false, 0);

  G4VisAttributes* logicTube5VisAtt = new G4VisAttributes(G4Colour(1,0.5,1));
  logicTube5VisAtt->SetForceWireframe(true);
  //  logicTube5VisAtt->SetForceSolid(true);
  logicTube5->SetVisAttributes(logicTube5VisAtt);

  // Start V1 Valve ------
  G4double V1Stopper1length = 0.965*cm;
  G4double V1Stopper2length = 0.965*cm;
  G4double v1rodlength = 2.48*2.54*cm; // t3length - V1Stopper1length - V1Stopper2length - valveposition;

  G4double V1Stopper1innerradius = 0.0; 
  G4double V1Stopper1outerradius = 0.5*2.34*cm; 
  G4Tubs* V1Stopper1 = new G4Tubs("V1Stopper1",V1Stopper1innerradius,V1Stopper1outerradius,0.5*V1Stopper1length,0.,twopi); 

  G4LogicalVolume* logicV1Stopper1 = new G4LogicalVolume(V1Stopper1, CellCoating,"V1Stopper1");   

  G4double phiv1, xv1, yv1, zv1;
  phiv1 = -45.0*degree;
  G4double v1offset = 1.0*cm;
  G4double valveposition = v1offset;

  G4cout << "+++++++++++++++++++++++++ Valve position: " << valveposition << " mm " << G4endl;
  G4cout << "+++++++++++++++++++++++++ gapopening: " << gapopening << " mm " << G4endl;
  G4cout << "+++++++++++++++++++++++++ Opening area: " << (t4innerradius*t4innerradius - (t4innerradius-gapopening)*(t4innerradius-gapopening))*M_PI  << " mm^2 " << G4endl;


  xv1 = xt3 + 0.5*(-t3length + 2.0*v1rodlength + V1Stopper1length + 2.0*V1Stopper2length)*sin(phit5); 
  yv1 = yt3; 
  zv1 = zt3   + 0.5*(-t3length + 2.0*v1rodlength + V1Stopper1length + 2.0*V1Stopper2length)*cos(phit5); 
  G4RotationMatrix rotv1;
  rotv1.rotateY(phiv1);
  G4VPhysicalVolume* physiV1Stopper1 = new G4PVPlacement(G4Transform3D(rotv1,G4ThreeVector(xv1,yv1,zv1)),"V1Stopper1", logicV1Stopper1, physiWorld, false, 0);

  G4VisAttributes* logicV1valveVisAtt = new G4VisAttributes(G4Colour(184/255. ,115/255. ,51/255. ));
  //  logicV1valveVisAtt->SetForceWireframe(true);
  logicV1valveVisAtt->SetForceSolid(true);
  logicV1Stopper1->SetVisAttributes(logicV1valveVisAtt);

  
  // central part (stem)
  //  G4double v1rodouterradius = 0.5*2.5*mm; 
  G4double v1rodouterradius = 0.5*1.27*cm;
  G4double v1rodinnerradius = 0; 
  //  G4double v1rodlength = t3length; //mm
  G4Tubs* v1rod = new G4Tubs("v1rod",v1rodinnerradius,v1rodouterradius,0.5*v1rodlength,0.,twopi); 

  G4LogicalVolume* logicV1rod = new G4LogicalVolume(v1rod, CellCoating,"v1rod");   
  G4double phiv1rod, xv1rod, yv1rod, zv1rod;
  phiv1rod = -45.0*degree;
  xv1rod = xt3 + 0.5*(-t3length + v1rodlength + 2.0*V1Stopper2length)*sin(phit5); 
  yv1rod = yt3; 
  zv1rod = zt3  + 0.5*(-t3length + v1rodlength + 2.0*V1Stopper2length)*cos(phit5); 
  G4RotationMatrix rotv1rod;
  rotv1rod.rotateY(phiv1rod);
  G4VPhysicalVolume* physiV1rod = new G4PVPlacement(G4Transform3D(rotv1rod,G4ThreeVector(xv1rod,yv1rod,zv1rod)),"v1rod", logicV1rod, physiWorld, false, 0);

  logicV1rod->SetVisAttributes(logicV1valveVisAtt);

  // stopper to helium4 inlet
  G4double V1Stopper2innerradius = 0.0; 
  G4double V1Stopper2outerradius = 0.5*2.34*cm; 
  G4Tubs* V1Stopper2 = new G4Tubs("V1Stopper2",V1Stopper2innerradius,V1Stopper2outerradius,0.5*V1Stopper2length,0.,twopi); 

  G4LogicalVolume* logicV1Stopper2 = new G4LogicalVolume(V1Stopper2, CellCoating,"V1Stopper2");   


  G4double xv2, yv2, zv2;

  xv2 = xt3 + 0.5*(-t3length + V1Stopper2length)*sin(phit5); 
  yv2 = yt3; 
  zv2 = zt3   + 0.5*(-t3length + V1Stopper2length)*cos(phit5); 
  G4VPhysicalVolume* physiV1Stopper2 = new G4PVPlacement(G4Transform3D(rotv1,G4ThreeVector(xv2,yv2,zv2)),"V1Stopper2", logicV1Stopper2, physiWorld, false, 0);
  logicV1Stopper2->SetVisAttributes(logicV1valveVisAtt);
  // End V1 Valve ------



  // target cell ....
  G4double InnerCellWidth = 3.0*2.54*cm;
  G4double InnerCellHeight = 4.0*2.54*cm;
  G4double InnerCellLength = 40.0*cm;
  G4double CellWallThickness = 1.0*cm;

  //  target cell
  // wall front/back
  G4double tc_fbx= InnerCellLength;
  G4double tc_fby= InnerCellHeight;
  G4double tc_fbz= CellWallThickness;
  G4Box* TargetCell_fb = new G4Box("TargetCell_fb", 0.5*tc_fbx, 0.5*tc_fby, 0.5*tc_fbz);
  G4LogicalVolume* logicTargetCell_fb = new G4LogicalVolume(TargetCell_fb, CellCoating,"TargetCell_fb");   

  G4double phitc_fb, xtc_fb, ytc_fb, ztc_fb;
  phitc_fb = phit3;
  xtc_fb = xt3 + 0.5*(t3length + 2.0*t4length + tc_fbz)*sin(phitc_fb);
  ytc_fb = yt3;
  ztc_fb = zt3 + 0.5*(t3length + 2.0*t4length + tc_fbz)*cos(phitc_fb);
  G4RotationMatrix rottc_fb;
  rottc_fb.rotateY(phitc_fb);
  //   G4VPhysicalVolume* physiTargetCell_fb = new G4PVPlacement(G4Transform3D(rottc_fb,G4ThreeVector(xtc_fb,ytc_fb,ztc_fb)),"TargetCell_fb", logicTargetCell_fb, physiWorld, false, 0);


  // targetcell inner chamber
  G4double tcix= InnerCellLength;
  G4double tciy= InnerCellHeight;
  G4double tciz= InnerCellWidth;
  G4Box* TargetCellInner = new G4Box("TargetCellInner", 0.5*tcix, 0.5*tciy, 0.5*tciz);
  G4LogicalVolume* logicTargetCellInner = new G4LogicalVolume(TargetCellInner, FillMaterial,"TargetCellInner");   

  G4double phitci, xtci, ytci, ztci;
  phitci = phit3;
  xtci = xtc_fb + 0.5*(tciz+tc_fbz)*sin(phitci);
  ytci = ytc_fb;
  ztci = ztc_fb + 0.5*(tciz+tc_fbz)*cos(phitci);
  G4RotationMatrix rottci;
  rottci.rotateY(phitci);
  //  G4VPhysicalVolume* physiTargetCelInner = new G4PVPlacement(G4Transform3D(rottci,G4ThreeVector(xtci,ytci,ztci)),"TargetCellInner", logicTargetCellInner, physiWorld, false, 0);

  G4VisAttributes* logicTargetCell1VisAtt = new G4VisAttributes(G4Colour(1,1,1));
  //  logicTargetCellVisAtt->SetForceWireframe(true);
  logicTargetCell1VisAtt->SetForceSolid(true);
  logicTargetCellInner->SetVisAttributes(logicTargetCell1VisAtt);


  G4double WallSeparationZ = InnerCellWidth + CellWallThickness; 
  xtc_fb = xtc_fb + WallSeparationZ*sin(phitc_fb);
  ytc_fb = ytc_fb;
  ztc_fb = ztc_fb + WallSeparationZ*cos(phitc_fb);

  //  physiTargetCell_fb = new G4PVPlacement(G4Transform3D(rottc_fb,G4ThreeVector(xtc_fb,ytc_fb,ztc_fb)),"TargetCell_fb", logicTargetCell_fb, physiWorld, false, 1);

  G4VisAttributes* logicTargetCellVisAtt = new G4VisAttributes(G4Colour(1,0,0));
  logicTargetCellVisAtt->SetForceWireframe(true);
  //  logicTargetCellVisAtt->SetForceSolid(true);
  logicTargetCell_fb->SetVisAttributes(logicTargetCellVisAtt);

  // wall top/bottom
  G4double tc_tbx= InnerCellLength;
  G4double tc_tby= CellWallThickness;
  G4double tc_tbz= InnerCellWidth + 2*tc_fbz;
  G4Box* TargetCell_tb = new G4Box("TargetCell_tb", 0.5*tc_tbx, 0.5*tc_tby, 0.5*tc_tbz);
  G4LogicalVolume* logicTargetCell_tb = new G4LogicalVolume(TargetCell_tb, CellCoating,"TargetCell_tb");   

  G4double phitc_tb, xtc_tb, ytc_tb, ztc_tb;
  phitc_tb = phit3;
  xtc_tb = xtci;
  ytc_tb = ytci + 0.5*(tc_fby + tc_tby);
  ztc_tb = ztci;
  G4RotationMatrix rottc_tb;
  rottc_tb.rotateY(phitc_tb);
  //  G4VPhysicalVolume* physiTargetCell_tb = new G4PVPlacement(G4Transform3D(rottc_tb,G4ThreeVector(xtc_tb,ytc_tb,ztc_tb)),"TargetCell_tb", logicTargetCell_tb, physiWorld, false, 0);

  ytc_tb = ytci - 0.5*(tc_fby + tc_tby);

  //  physiTargetCell_tb = new G4PVPlacement(G4Transform3D(rottc_tb,G4ThreeVector(xtc_tb,ytc_tb,ztc_tb)),"TargetCell_tb", logicTargetCell_tb, physiWorld, false, 1);

  logicTargetCell_tb->SetVisAttributes(logicTargetCellVisAtt);

  // wall left/right
  G4double tc_lrx= CellWallThickness;
  G4double tc_lry= InnerCellHeight + 2*tc_tby;
  G4double tc_lrz= InnerCellWidth + 2*tc_fbz;
  G4Box* TargetCell_lr = new G4Box("TargetCell_lr", 0.5*tc_lrx, 0.5*tc_lry, 0.5*tc_lrz);
  G4LogicalVolume* logicTargetCell_lr = new G4LogicalVolume(TargetCell_lr, CellCoating,"TargetCell_lr");   

  G4double WallSeparationX = 0.5*tc_tbx + 0.5*tc_lrx; 
  G4double phitc_lr, xtc_lr, ytc_lr, ztc_lr;
  phitc_lr = phit3;
  xtc_lr = xtci + WallSeparationX*cos(phitc_lr);
  ytc_lr = ytci;
  ztc_lr = ztci - WallSeparationX*sin(phitc_lr);
  G4RotationMatrix rottc_lr;
  rottc_lr.rotateY(phitc_lr);
  //  G4VPhysicalVolume* physiTargetCell_lr = new G4PVPlacement(G4Transform3D(rottc_lr,G4ThreeVector(xtc_lr,ytc_lr,ztc_lr)),"TargetCell_lr", logicTargetCell_lr, physiWorld, false, 0);


  xtc_lr = xtci - WallSeparationX*cos(phitc_lr);
  ztc_lr = ztci + WallSeparationX*sin(phitc_lr);

  //  physiTargetCell_lr = new G4PVPlacement(G4Transform3D(rottc_lr,G4ThreeVector(xtc_lr,ytc_lr,ztc_lr)),"TargetCell_lr", logicTargetCell_lr, physiWorld, false, 1);


  logicTargetCell_lr->SetVisAttributes(logicTargetCellVisAtt);

  // injection hole
  G4double ihinnerradius = 0.0;
  G4double ihouterradius = t4innerradius;
  G4double ihlength= tc_fbz + delta;
  G4Tubs* injectionhole = new G4Tubs("injectionhole", ihinnerradius, ihouterradius, 0.5*ihlength,0,twopi);
  G4LogicalVolume* logicInjectionHole = new G4LogicalVolume(injectionhole, FillMaterial,"injectionhole");   

  G4double phiih, xih, yih, zih;
  phiih = phit3;
  //  xih = 0.0;
  //  yih = 0.0;
  //  zih = 0.5*t3length + t4length + 1.0*cm;
  xih = xt3 + 0.5*(t3length + 2.0*t4length + ihlength-delta)*sin(phiih);
  yih = yt3;
  zih = zt3 + 0.5*(t3length + 2.0*t4length + ihlength-delta)*cos(phiih);
  G4RotationMatrix rotih;
  rotih.rotateY(phiih);
  //  G4VPhysicalVolume* physiInjectionHole = new G4PVPlacement(G4Transform3D(rotih,G4ThreeVector(xih,yih,zih)),"InjectionHole", logicInjectionHole, physiWorld, false, 0);

  G4VisAttributes* logicInjectionHoleVisAtt = new G4VisAttributes(G4Colour(1,1,1));
  //  logicInjectionHoleVisAtt->SetForceWireframe(true);
  logicInjectionHoleVisAtt->SetForceSolid(true);
  logicInjectionHole->SetVisAttributes(logicInjectionHoleVisAtt);



  // neutron stopper
  G4double nsinnerradius = 0.0;
  G4double nsouterradius = t4innerradius;
  G4double nslength= 0.1*mm;
  G4Tubs* neutronStopper = new G4Tubs("NeutronStopper", nsinnerradius, nsouterradius, 0.5*nslength,0,twopi);
  G4LogicalVolume* logicNeutronStopper = new G4LogicalVolume(neutronStopper, BeamTubeMaterial,"NeutronStopper");   

  G4double phins, xns, yns, zns;
  phins = phit3;
  xns = xt4a + 0.5*(t4alength + nslength-delta)*sin(phins);
  yns = yt4a;
  zns = zt4a + 0.5*(t4alength + nslength-delta)*cos(phins);
  G4RotationMatrix rotns;
  rotns.rotateY(phins);
  G4VPhysicalVolume* physiNeutronStopper = new G4PVPlacement(G4Transform3D(rotns,G4ThreeVector(xns,yns,zns)),"NeutronStopper", logicNeutronStopper, physiWorld, false, 0);

  G4VisAttributes* logicNeutronStopperVisAtt = new G4VisAttributes(G4Colour(0,1,1));
  //  logicNeutronStopperVisAtt->SetForceWireframe(true);
  logicNeutronStopperVisAtt->SetForceSolid(true);
  logicNeutronStopper->SetVisAttributes(logicNeutronStopperVisAtt);


  return physiWorld;
}

////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n The  WORLD   is made of " 
	 << WorldSizeZ/mm << "mm of " << WorldMaterial->GetName() ;
  G4cout << ", the transverse size (X) of the world is " << WorldSizeX/mm << " mm. " << G4endl;
  G4cout << " The ABSORBER is made of " 
	 << AbsorberThickness/mm << "mm of " << AbsorberMaterial->GetName() ;
  G4cout << ", the transverse size (R) is " << AbsorberRadius/mm << " mm. " << G4endl;
  G4cout << " Z position of the (middle of the) absorber " << zAbsorber/mm << "  mm." << G4endl;
  G4cout << G4endl;
}

///////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::SetAbsorberMaterial(G4String materialChoice)
{
  // get the pointer to the material table
  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
  
  // search the material by its name   
  G4Material* pttoMaterial;
  for (size_t J=0 ; J<theMaterialTable->size() ; J++)
    { pttoMaterial = (*theMaterialTable)[J];     
      if(pttoMaterial->GetName() == materialChoice)
        {
	  AbsorberMaterial = pttoMaterial;
          logicAbsorber->SetMaterial(pttoMaterial); 
        }             
    }
}

////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::SetWorldMaterial(G4String materialChoice)
{
  // get the pointer to the material table
  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
  
  // search the material by its name   
  G4Material* pttoMaterial;
  for (size_t J=0 ; J<theMaterialTable->size() ; J++)
   { pttoMaterial = (*theMaterialTable)[J];     
     if(pttoMaterial->GetName() == materialChoice)
       {
	 WorldMaterial = pttoMaterial;
	 logicWorld->SetMaterial(pttoMaterial); 
       }             
   }
}

///////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::SetAbsorberThickness(G4double val)
{
  // change Absorber thickness and recompute the calorimeter parameters
  AbsorberThickness = val;
  ComputeCalorParameters();
}  

/////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::SetAbsorberRadius(G4double val)
{
  // change the transverse size and recompute the calorimeter parameters
  AbsorberRadius = val;
  ComputeCalorParameters();
}  

////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::SetWorldSizeX(G4double val)
{
  worldchanged=true;
  WorldSizeX = val;
  ComputeCalorParameters();
}  

//////////////////////////////////////////////////////////////////////////////
//
//
void UCNDetectorConstruction::SetWorldSizeY(G4double val)
{
  worldchanged=true;
  WorldSizeY = val;
  ComputeCalorParameters();
}  

//////////////////////////////////////////////////////////////////////////////
//
//
void UCNDetectorConstruction::SetWorldSizeZ(G4double val)
{
  worldchanged=true;
  WorldSizeZ = val;
  ComputeCalorParameters();
}  

//////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::SetAbsorberZpos(G4double val)
{
  zAbsorber  = val;
  ComputeCalorParameters();
}  


////////////////////////////////////////////////////////////////////////////
//
//

void UCNDetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructCalorimeter());
}

//
//
////////////////////////////////////////////////////////////////////////////
