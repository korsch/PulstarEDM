#include "NedmAcrylicWindow.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VisAttributes.hh"

#include "NedmDetectorConstruction.hh"
#include "NedmDetectorParameters.hh"

G4LogicalVolume* NedmAcrylicWindow::the_acrylicwindow_log_;

NedmAcrylicWindow::NedmAcrylicWindow(G4RotationMatrix* pRot,
    const G4ThreeVector& tlate,
    G4LogicalVolume* pMother,
    G4int copy_no) : G4PVPlacement(pRot, tlate, NedmAcrylicWindow::the_acrylicwindow_log_,"AcrylicWindow", pMother, false, copy_no) {

  NedmAcrylicWindow::ConstructSurfaceVolumes();

}



void NedmAcrylicWindow::CreateLogicalVolumes() {
  G4VSolid* AcrylicWindow_box;
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  if(parameters->use_full_acrylicwindow()) {
    AcrylicWindow_box = new G4Tubs("AcrylicWindow", 0., parameters->acrylicwindow_radius(),
        0.5*parameters->acrylicwindow_z(), 0., 360.);
  } else {
    //The only part of the AcrylicWindow which is uncovered is that which is
    //in contact with the Lightguide
    AcrylicWindow_box = new G4Box("AcrylicWindow", parameters->cell_tb_size().x(),
        parameters->cell_tb_size().y(), parameters->acrylicwindow_z());
  }

  NedmAcrylicWindow::the_acrylicwindow_log_ = new G4LogicalVolume(AcrylicWindow_box, the_detector_->acrylic(), "AcrylicWindow");

  G4VisAttributes* logicAcrylicWindowVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0,0.0));
  logicAcrylicWindowVisAtt->SetForceSolid(true);
  NedmAcrylicWindow::the_acrylicwindow_log_->SetVisAttributes(logicAcrylicWindowVisAtt);


  //  new G4LogicalSkinSurface("photocath_surf", NedmAcrylicWindow::the_acrylicwindow_log_, the_detector_->acrylicwindow_surface());
}

void NedmAcrylicWindow::ConstructSurfaceVolumes()
{
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  G4ThreeVector acrylicwindow_fb_surface_size = parameters->acrylicwindow_frontback_surface_size();
  G4ThreeVector acrylicwindow_center_surface_size = parameters->acrylicwindow_center_surface_size();


  std::vector<G4ThreeVector> acrylicwindow_fb_surface_position;
  G4ThreeVector acrylicwindow_center_surface_position = parameters->acrylicwindow_center_surface_position();

  acrylicwindow_fb_surface_position.resize(2);

  for(size_t k=0; k< 2; k++){
    acrylicwindow_fb_surface_position[k] = parameters->acrylicwindow_frontback_surface_position(k);
  }


  
  G4Tubs* AcrylicWindow_FB_Surface_Tub;
  G4LogicalVolume* AcrylicWindow_FB_Surface_log;
  G4LogicalSkinSurface* mirror_aw_surface_fb;
  G4PVPlacement* AcrylicWindowSurf_fb; 

  G4Tubs* AcrylicWindow_Center_Surface_Tub;
  G4LogicalVolume* AcrylicWindow_Center_Surface_log;
  G4LogicalSkinSurface* mirror_aw_surface_center;
  G4PVPlacement* AcrylicWindowSurf_center; 


  G4VisAttributes* logicAcrylicWindowSurfaceVisAtt = new G4VisAttributes(G4Colour(255.0/255.0, 102.0/255.0,104.0/255.0));

  G4RotationMatrix* rot(0);
  G4ThreeVector center = G4ThreeVector(0.0,0.0,0.0);
  G4ThreeVector displacement = G4ThreeVector(0.0,0.0,0.0);

  if(parameters->use_full_acrylicwindow()) {
    AcrylicWindow_FB_Surface_Tub = new G4Tubs("AcrylicWindowFBSurface", acrylicwindow_fb_surface_size[0],acrylicwindow_fb_surface_size[1],
        0.5*acrylicwindow_fb_surface_size[2], 0., 360.);
    AcrylicWindow_FB_Surface_log = new G4LogicalVolume(AcrylicWindow_FB_Surface_Tub, the_detector_->acrylic(), "AcrylicWindow_FB_Surface");

    //  logicAcrylicWindowSurfaceVisAtt->SetForceSolid(true);
  logicAcrylicWindowSurfaceVisAtt->SetForceWireframe(true);
  AcrylicWindow_FB_Surface_log->SetVisAttributes(logicAcrylicWindowSurfaceVisAtt);


  displacement = acrylicwindow_fb_surface_position[0];

  AcrylicWindowSurf_fb  = new G4PVPlacement(rot,center + displacement, AcrylicWindow_FB_Surface_log, "AcrylicWindow_FB", the_detector_->get_logical_world(), false, 0);


  mirror_aw_surface_fb = new G4LogicalSkinSurface("acrylicwindowSurface_FB", AcrylicWindow_FB_Surface_log, the_detector_->mirror_surface());

  NedmDetectorConstruction::RegisterSurface(mirror_aw_surface_fb, "mirror");

  displacement.setZ(-displacement.getZ());

  AcrylicWindowSurf_fb  = new G4PVPlacement(rot,center + displacement, AcrylicWindow_FB_Surface_log, "AcrylicWindow_FB", the_detector_->get_logical_world(), false, 1);


  mirror_aw_surface_fb = new G4LogicalSkinSurface("acrylicwindowSurface_FB", AcrylicWindow_FB_Surface_log, the_detector_->mirror_surface());
  
  NedmDetectorConstruction::RegisterSurface(mirror_aw_surface_fb, "mirror");

  displacement = acrylicwindow_fb_surface_position[1];


  AcrylicWindowSurf_fb  = new G4PVPlacement(rot,center + displacement, AcrylicWindow_FB_Surface_log, "AcrylicWindow_FB", the_detector_->get_logical_world(), false, 2);


  mirror_aw_surface_fb = new G4LogicalSkinSurface("acrylicwindowSurface_FB", AcrylicWindow_FB_Surface_log, the_detector_->mirror_surface());

  NedmDetectorConstruction::RegisterSurface(mirror_aw_surface_fb, "mirror");

  displacement.setZ(-displacement.getZ());

  AcrylicWindowSurf_fb  = new G4PVPlacement(rot,center + displacement, AcrylicWindow_FB_Surface_log, "AcrylicWindow_FB", the_detector_->get_logical_world(), false, 3);


  mirror_aw_surface_fb = new G4LogicalSkinSurface("acrylicwindowSurface_FB", AcrylicWindow_FB_Surface_log, the_detector_->mirror_surface());
  
  NedmDetectorConstruction::RegisterSurface(mirror_aw_surface_fb, "mirror");


  AcrylicWindow_Center_Surface_Tub = new G4Tubs("AcrylicWindowCenterSurface", acrylicwindow_center_surface_size[0],acrylicwindow_center_surface_size[1], 0.5*acrylicwindow_center_surface_size[2], 0., 360.);
  AcrylicWindow_Center_Surface_log = new G4LogicalVolume(AcrylicWindow_Center_Surface_Tub, the_detector_->acrylic(), "AcrylicWindow_Center_Surface");
  
  logicAcrylicWindowSurfaceVisAtt->SetForceWireframe(true);
  //  logicAcrylicWindowSurfaceVisAtt->SetForceSolid(true);
  AcrylicWindow_Center_Surface_log->SetVisAttributes(logicAcrylicWindowSurfaceVisAtt);

  displacement = acrylicwindow_center_surface_position;
  
  AcrylicWindowSurf_center  = new G4PVPlacement(rot,center + displacement, AcrylicWindow_Center_Surface_log, "AcrylicWindow_Center", the_detector_->get_logical_world(), false, 0);
  
  mirror_aw_surface_center = new G4LogicalSkinSurface("acrylicwindowSurface_Center", AcrylicWindow_Center_Surface_log, the_detector_->mirror_surface());

  NedmDetectorConstruction::RegisterSurface(mirror_aw_surface_center, "mirror");

  displacement.setZ(-displacement.getZ());
  
  AcrylicWindowSurf_center  = new G4PVPlacement(rot,center + displacement, AcrylicWindow_Center_Surface_log, "AcrylicWindow_Center", the_detector_->get_logical_world(), false, 1);
  
  mirror_aw_surface_center = new G4LogicalSkinSurface("acrylicwindowSurface_Center", AcrylicWindow_Center_Surface_log, the_detector_->mirror_surface());

  NedmDetectorConstruction::RegisterSurface(mirror_aw_surface_center, "mirror");


  }
  else{
    std::cerr << "Wrong implementation in Acrylic Window Logic Volume Creation" << std::endl;
    exit(1);
  }

}
