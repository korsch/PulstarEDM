#include "NedmLightGuide.hh"

#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4VisAttributes.hh"

#include "NedmDetectorConstruction.hh"
#include "NedmDetectorParameters.hh"
#include "NedmCompoundSolid.hh"

using namespace std;

std::string itos(int i)      // convert int to string
{
  stringstream s;
  s << i;
  return s.str();
}

G4LogicalVolume* NedmLightGuide::lightguide_log[2000];
G4LogicalVolume* NedmLightGuide::lightGuide_log[3];


NedmLightGuide::NedmLightGuide(G4RotationMatrix* pRot,
    const G4ThreeVector& tlate,
    G4LogicalVolume* pLog,
    const G4String& pName,
    G4LogicalVolume* pMother,
    G4int copy_no) : G4PVPlacement(pRot, tlate,
			pLog, pName,
                       pMother, false, copy_no) {

    }


const G4ThreeVector& NedmLightGuide::lightguide_end_offset() {
  static const G4ThreeVector failure(0, 0, -1);

  NedmCompoundSolid::BentSolid* compound =
    dynamic_cast<NedmCompoundSolid::BentSolid*>(lightGuide_log[1]->GetSolid());

  if(!compound)
    return failure;

  return compound->final_slice().center(); 

}

void NedmLightGuide::CreateLogicalVolumes() {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  if(!parameters->use_lightguide_files()) {

    //    std::vector<G4Box*> lightguide_box;
    std::vector<G4Trd*> lightguide_trd;

    size_t nsegments = parameters->lightguide_segments()-1;
    //    lightguide_box.resize(nsegments);
    lightguide_trd.resize(nsegments);

    //    for(size_t k=0; k<(nsegments); k++){
    for(size_t k=0; k<(nsegments); k++){
      
      //      lightguide_box[k] = new G4Box("LG_box", 
      //				    parameters->lightguide_size(k).getX(),
      //				    parameters->lightguide_size(k).getY(),
      //				    parameters->lightguide_size(k).getZ());
      //      
      //    NedmLightGuide::lightguide_log[k] = new G4LogicalVolume(lightguide_box[k],
      //        the_detector_->acrylic(),
      //	"LG_log");
      //    }


      lightguide_trd[k] = new G4Trd("LG_trd", 
				    parameters->lightguide_size(k).getX(),
				    parameters->lightguide_size(k+1).getX(),
				    parameters->lightguide_size(k).getY(),
				    parameters->lightguide_size(k+1).getY(),
				    parameters->lightguide_size(k).getZ());
      
      
    NedmLightGuide::lightguide_log[k] = new G4LogicalVolume(lightguide_trd[k],
        the_detector_->acrylic(),
	"LG_log");
    }
  }


}

void NedmLightGuide::ConstructLightGuides(G4RotationMatrix* rotation, const G4ThreeVector& center, G4LogicalVolume* pMother, G4int copy_no) {

  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  NedmLightGuide* guides[2000];
  const size_t nsegments = parameters->lightguide_segments()-1;

  G4ThreeVector position[2000];


    for(size_t k=0; k< nsegments; k++){
      position[k] = parameters->lightguide_position(k);
    }

  for(int i = 0; i < 2*nsegments; i++) {
    if(i < nsegments){
      guides[i] = new NedmLightGuide(rotation, position[i], NedmLightGuide::lightguide_log[i], "LG_F", pMother, i);
    }
    else{
      position[i-nsegments] *= -1.0;
      guides[i] = new NedmLightGuide(rotation, position[i-nsegments], NedmLightGuide::lightguide_log[i-nsegments], "LG_B", pMother, nsegments + i);
    }
  }


  //  if(parameters->lightguide_loss()) {
  //    if (!parameters->use_lightguide_files())
  //      ConstructStraightSurfaceVolumes(guides);
  //    //Disabled for now because of bugs.
  //  // else
  //    //  ConstructCustomSurfaceVolumes(guides);
  //  }
  
  
}

void NedmLightGuide::ConstructLightGuides(G4LogicalVolume* pMother) {

  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  NedmLightGuide* guides[2000];
  const size_t nsegments = parameters->lightguide_segments() - 1;

  //  G4RotationMatrix* rotation[2000];// = {0};
  G4ThreeVector position[2000];

  G4RotationMatrix* oneEighty = new G4RotationMatrix;
  oneEighty->rotateX(180*deg);

  G4RotationMatrix* Zero = new G4RotationMatrix;
  Zero->rotateX(0*deg);

    for(size_t k=0; k< nsegments; k++){
      position[k] = parameters->lightguide_position(k);

    }

    
  for(int i = 0; i < 2*nsegments; i++) {
    if(i < nsegments){
      guides[i] = new NedmLightGuide(Zero, position[i], NedmLightGuide::lightguide_log[i], "LG_F", pMother, i);
    }
    else{
    position[i-nsegments] *= -1.0;
    guides[i] = new NedmLightGuide(oneEighty, position[i-nsegments], NedmLightGuide::lightguide_log[i-nsegments], "LG_B", pMother, nsegments + i);
    }
  }


  if(parameters->lightguide_loss()) {
    if (!parameters->use_lightguide_files()){
      ConstructStraightSurfaceVolumes(guides);
      //      ConstructMirrorVolumes(guides);
    }
      //  //    //Disabled for now because of bugs.
      //  //  // else
      //  //    //  ConstructCustomSurfaceVolumes(guides);
  }


}
  

void NedmLightGuide::ConstructStraightSurfaceVolumes(NedmLightGuide** lightGuide_phys) {

  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();


  std::vector<G4ThreeVector> lightguide_tb_size;
  std::vector<G4ThreeVector> lightguide_lr_size;

  const size_t nsegments = parameters->lightguide_segments()-1;
  
  lightguide_tb_size.resize(nsegments+1);
  lightguide_lr_size.resize(nsegments+1);


  for(size_t k=0; k< (nsegments+1); k++){
    lightguide_tb_size[k] = parameters->lightguide_topbottom_surface_size(k);
    lightguide_lr_size[k] = parameters->lightguide_leftright_surface_size(k);
  }


  G4Trd* lightguideSurfVol_tb_trd[2000];
  G4Trd* lightguideSurfVol_lr_trd[2000];
  G4LogicalVolume* lightguideSurfVol_tb_log[2000];
  G4LogicalVolume* lightguideSurfVol_lr_log[2000];


  for(size_t k=0; k< (nsegments); k++){
    lightguideSurfVol_tb_trd[k] = new G4Trd("lgSurfVol_tb", lightguide_tb_size[k].x(),lightguide_tb_size[k+1].x(), lightguide_tb_size[k].y(), lightguide_tb_size[k].y(), lightguide_tb_size[k].z());
    lightguideSurfVol_lr_trd[k] = new G4Trd("lgSurfVol_lr", lightguide_lr_size[k].x(), lightguide_lr_size[k].x(), lightguide_lr_size[k].y(), lightguide_lr_size[k+1].y(), lightguide_lr_size[k].z());
    lightguideSurfVol_tb_log[k] = new G4LogicalVolume(lightguideSurfVol_tb_trd[k], the_detector_->air(), "lightguideSurfVol_tb");
 
    lightguideSurfVol_lr_log[k] = new G4LogicalVolume(lightguideSurfVol_lr_trd[k], the_detector_->air(), "lightguideSurfVol_lr");

  }


  std::vector<G4ThreeVector> lightguide_tb_position;
  std::vector<G4ThreeVector> lightguide_lr_position;
  std::vector<G4double> lightguide_tb_angle;
  std::vector<G4double> lightguide_lr_angle;


  lightguide_tb_position.resize(2*nsegments);
  lightguide_lr_position.resize(2*nsegments);
  lightguide_tb_angle.resize(2*nsegments);
  lightguide_lr_angle.resize(2*nsegments);

  for(size_t k=0; k< 2*(nsegments); k++){
    if(k<(nsegments)){
      lightguide_tb_position[k] = parameters->lightguide_topbottom_surface_position(k);
      lightguide_lr_position[k] = parameters->lightguide_leftright_surface_position(k);
      lightguide_tb_angle[k] = parameters->lightguide_topbottom_surface_rotation_angle(k);
      //      std::cout << "2: " << k << ' ' << lightguide_tb_rotation[k] << std::endl;
      lightguide_lr_angle[k] = parameters->lightguide_leftright_surface_rotation_angle(k);

    }
    else{
      lightguide_tb_position[k] = parameters->lightguide_topbottom_surface_position(k-nsegments);
      lightguide_tb_position[k].setY(-lightguide_tb_position[k].getY());
      lightguide_lr_position[k] = parameters->lightguide_leftright_surface_position(k-nsegments);
      lightguide_lr_position[k].setX(-lightguide_lr_position[k].getX());
      lightguide_tb_angle[k] = parameters->lightguide_topbottom_surface_rotation_angle(k-nsegments);
      lightguide_lr_angle[k] = parameters->lightguide_leftright_surface_rotation_angle(k-nsegments);
    }
  }


  G4ThreeVector center = G4ThreeVector(0.0,0.0,0.0);
  G4ThreeVector displacement = G4ThreeVector(0.0,0.0,0.0);

  G4PVPlacement* lightguideSurfVol_tb; 
  G4PVPlacement* lightguideSurfVol_lr;

  G4LogicalBorderSurface* surfVolBorder_tb;
  G4LogicalBorderSurface* surfVolBorder_lr;
  std::string surfVolName;

  G4RotationMatrix* rot(0);

  //  G4VisAttributes* logicLightGuideSurfaceVisAtt = new G4VisAttributes(G4Colour(153/255. ,0/255.   ,255/255. ));
  G4VisAttributes* logicLightGuideSurfaceVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0,0.0));
  logicLightGuideSurfaceVisAtt->SetForceSolid(true);
  //  logicLightGuideSurfaceVisAtt->SetForceWireframe(true);

  size_t j(0);

  G4RotationMatrix rotTB[2000];
  G4RotationMatrix rotLR[2000];

  for(size_t k = 0; k < 2*(nsegments); k++) {
    if(k < (nsegments)){

      displacement = lightguide_tb_position[k];
      rotTB[k].rotateX(lightguide_tb_angle[k]);
      

      lightguideSurfVol_tb = new G4PVPlacement(G4Transform3D(rotTB[k],center + displacement), lightguideSurfVol_tb_log[k], "lightguideSurfVol_TB", the_detector_->get_logical_world(), false, j++);

      displacement.setY(-displacement.getY());
      rotTB[k].rotateX(-lightguide_tb_angle[k]);  // rotate back to 0
      rotTB[k].rotateX(-lightguide_tb_angle[k]); // now rotate

      lightguideSurfVol_tb = new G4PVPlacement(G4Transform3D(rotTB[k],center + displacement), lightguideSurfVol_tb_log[k], "lightguideSurfVol_TB", the_detector_->get_logical_world(), false, j++);

      lightguideSurfVol_tb_log[k]->SetVisAttributes(logicLightGuideSurfaceVisAtt);


      surfVolName = "lightguideSurface_tb_"+ itos(k+1);
      
      //      surfVolBorder_tb = new G4LogicalBorderSurface(surfVolName.c_str(), lightGuide_phys[k], lightguideSurfVol_tb, the_detector_->cast_surface());
      //      NedmDetectorConstruction::RegisterSurface(surfVolBorder_tb, "cast");
      
      
      G4LogicalSkinSurface* mirror_surface_tb =  new G4LogicalSkinSurface(surfVolName.c_str(), lightguideSurfVol_tb_log[k], the_detector_->mirror_surface());
      NedmDetectorConstruction::RegisterSurface(mirror_surface_tb, "mirror");
      
    
      displacement = lightguide_lr_position[k];
      rotLR[k].rotateY(lightguide_lr_angle[k]);
      
      lightguideSurfVol_lr = new G4PVPlacement(G4Transform3D(rotLR[k],center + displacement), lightguideSurfVol_lr_log[k], "lightguideSurfVol_LR", the_detector_->get_logical_world(), false, j++);
      
      displacement.setX(-displacement.getX());
      rotLR[k].rotateY(-lightguide_lr_angle[k]);
      rotLR[k].rotateY(-lightguide_lr_angle[k]); //now rotate
      
      lightguideSurfVol_lr = new G4PVPlacement(G4Transform3D(rotLR[k],center + displacement), lightguideSurfVol_lr_log[k], "lightguideSurfVol_LR", the_detector_->get_logical_world(), false, j++);
      
      
      lightguideSurfVol_lr_log[k]->SetVisAttributes(logicLightGuideSurfaceVisAtt);
      
      surfVolName = "lightguideSurface_lr_"+ itos(k+1);
      
      //      surfVolBorder_lr = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[k], lightguideSurfVol_lr, the_detector_->cast_surface());
      //      NedmDetectorConstruction::RegisterSurface(surfVolBorder_lr, "cast");      
      
      G4LogicalSkinSurface* mirror_surface_lr =   new G4LogicalSkinSurface("mirror_surface_lr", lightguideSurfVol_lr_log[k], the_detector_->mirror_surface());      
      NedmDetectorConstruction::RegisterSurface(mirror_surface_lr, "mirror");

      
      
    }
    else{
      
      displacement = lightguide_tb_position[k-nsegments];
      displacement.setZ(-lightguide_tb_position[k-nsegments].getZ());
      rotTB[k-nsegments].rotateX(lightguide_tb_angle[k-nsegments]);
      rotTB[k-nsegments].rotateY(180*deg);
      rotTB[k-nsegments].rotateX(-lightguide_tb_angle[k-nsegments]);
      
      lightguideSurfVol_tb = new G4PVPlacement(G4Transform3D(rotTB[k-nsegments],center + displacement), lightguideSurfVol_tb_log[k-nsegments], "lightguideSurfVol_TB", the_detector_->get_logical_world(), false, j++);
      
      displacement.setY(-displacement.getY());
      rotTB[k-nsegments].rotateX(lightguide_tb_angle[k-nsegments]);
      rotTB[k-nsegments].rotateX(lightguide_tb_angle[k-nsegments]);
      
      lightguideSurfVol_tb = new G4PVPlacement(G4Transform3D(rotTB[k-nsegments],center + displacement), lightguideSurfVol_tb_log[k-nsegments], "lightguideSurfVol_TB", the_detector_->get_logical_world(), false, j++);
      
      lightguideSurfVol_tb_log[k-nsegments]->SetVisAttributes(logicLightGuideSurfaceVisAtt);
      
      surfVolName = "lightguideSurface_tb_"+ itos(k+1);
      
      //      surfVolBorder_tb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[k], lightguideSurfVol_tb, the_detector_->cast_surface());
      //      NedmDetectorConstruction::RegisterSurface(surfVolBorder_tb, "cast");
      //  NedmDetectorConstruction::RegisterSurface(surfVolBorder_tb, "machined");

      G4LogicalSkinSurface* mirror_surface_tb =  new G4LogicalSkinSurface("mirror_surface_tb", lightguideSurfVol_tb_log[k-nsegments], the_detector_->mirror_surface());
      NedmDetectorConstruction::RegisterSurface(mirror_surface_tb, "mirror");


      displacement = lightguide_lr_position[k-nsegments];
      displacement.setZ(-lightguide_lr_position[k-nsegments].getZ());
      rotLR[k-nsegments].rotateY(lightguide_lr_angle[k-nsegments]);
      rotLR[k-nsegments].rotateX(180*deg);
      rotLR[k-nsegments].rotateY(-lightguide_lr_angle[k-nsegments]);
      
      lightguideSurfVol_lr = new G4PVPlacement(G4Transform3D(rotLR[k-nsegments],center + displacement), lightguideSurfVol_lr_log[k-nsegments], "lightguideSurfVol_LR", the_detector_->get_logical_world(), false, j++);
      
      displacement.setX(-displacement.getX());
      rotLR[k-nsegments].rotateY(lightguide_lr_angle[k-nsegments]);
      rotLR[k-nsegments].rotateY(lightguide_lr_angle[k-nsegments]);
      
      lightguideSurfVol_lr = new G4PVPlacement(G4Transform3D(rotLR[k-nsegments],center + displacement), lightguideSurfVol_lr_log[k-nsegments], "lightguideSurfVol_LR", the_detector_->get_logical_world(), false, j++);
      
      lightguideSurfVol_lr_log[k-nsegments]->SetVisAttributes(logicLightGuideSurfaceVisAtt);
      
      surfVolName = "lightguideSurface_lr"+ itos(k+1);
      
      //      surfVolBorder_lr = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[k], lightguideSurfVol_lr, the_detector_->cast_surface());
      //      NedmDetectorConstruction::RegisterSurface(surfVolBorder_lr, "cast");
      //      NedmDetectorConstruction::RegisterSurface(surfVolBorder_lr, "machined");
      
      G4LogicalSkinSurface* mirror_surface_lr =  new G4LogicalSkinSurface("mirror_surface_lr", lightguideSurfVol_lr_log[k-nsegments], the_detector_->mirror_surface());
      NedmDetectorConstruction::RegisterSurface(mirror_surface_lr, "mirror");


    }
  } 
  
  std::vector<G4ThreeVector> lightguide_fb_size;
  std::vector<G4ThreeVector> lightguide_fb_position;

  lightguide_fb_size.resize(2);
  lightguide_fb_position.resize(2);

  for(size_t k=0; k< 2; k++){
  lightguide_fb_size[k] = parameters->lightguide_frontback_surface_size(k);
  lightguide_fb_position[k] = parameters->lightguide_frontback_surface_position(k);
  }


  G4Box* lightguideSurfVol_fb_box[2];
  G4LogicalVolume* lightguideSurfVol_fb_log[2];

  for(size_t i=0; i<2; i++){
    lightguideSurfVol_fb_box[i] = new G4Box("lgSurfVol_fb", lightguide_fb_size[i].x(), lightguide_fb_size[i].y(), lightguide_fb_size[i].z());
    lightguideSurfVol_fb_log[i] = new G4LogicalVolume(lightguideSurfVol_fb_box[i], the_detector_->air(), "lightguideSurfVol_fb");
    lightguideSurfVol_fb_log[i]->SetVisAttributes(logicLightGuideSurfaceVisAtt);

  }


  G4PVPlacement* lightguideSurfVol_fb; 

  displacement = lightguide_fb_position[0];

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[0], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);

  G4LogicalBorderSurface* surfVolBorder_fb;

  surfVolName = "lightguideSurface_fb_1";
  
  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[0], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");

  displacement.setZ(-displacement.getZ());

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[0], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);


  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[0], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");

  displacement = lightguide_fb_position[1];

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[1], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);


  surfVolName = "lightguideSurface_fb_2";
  
  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[nsegments-1], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");

  displacement.setZ(-displacement.getZ());

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[1], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);


  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[nsegments-1], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");



  /*
    //The following logic makes sure the the surface volumes are attached
    //to the correct light guides with border surfaces.
    for(int j = 0; j < 3; j++) {
      if (connections[i][j]) {
        sprintf(surfVolName, "lg%d_XSurface%d", lightGuide_phys[j]->GetCopyNo(), i);
        surfVolBorder = new G4LogicalBorderSurface(surfVolName,
            lightGuide_phys[j],
            lgSurfVol,
            the_detector_->machined_surface());

        NedmDetectorConstruction::RegisterSurface(surfVolBorder, "machined");
      }

    }

  }

  */
  }

void NedmLightGuide::ConstructCustomSurfaceVolumes(NedmLightGuide** lightGuide_phys) {
  NedmCompoundSolid::BentSolid* solid;
  for(int i = 0; i < 3; i++) {
    solid =
      dynamic_cast<NedmCompoundSolid::BentSolid*>(
          lightGuide_phys[i]->GetLogicalVolume()->GetSolid());
    if(solid)
      solid->ConstructSurfaceVolumes(lightGuide_phys[i]);
  }
}

/*
void NedmLightGuide::ConstructMirrorVolumes(NedmLightGuide** lightGuide_phys) {

  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();


  std::vector<G4ThreeVector> lightguide_tb_size;
  std::vector<G4ThreeVector> lightguide_lr_size;

  const size_t nsegments = parameters->lightguide_segments();

  lightguide_tb_size.resize(nsegments);
  lightguide_lr_size.resize(nsegments);


  for(size_t k=0; k< nsegments; k++){
    lightguide_tb_size[k] = parameters->lightguide_topbottom_surface_size(k);
    lightguide_lr_size[k] = parameters->lightguide_leftright_surface_size(k);
  }


  G4Box* lightguideSurfVol_tb_box[2000];
  G4Box* lightguideSurfVol_lr_box[2000];
  G4LogicalVolume* lightguideSurfVol_tb_log[2000];
  G4LogicalVolume* lightguideSurfVol_lr_log[2000];

  for(size_t k=0; k< nsegments; k++){
    lightguideSurfVol_tb_box[k] = new G4Box("lgSurfVol_tb", lightguide_tb_size[k].x(), lightguide_tb_size[k].y(), lightguide_tb_size[k].z());
    lightguideSurfVol_lr_box[k] = new G4Box("lgSurfVol_lr", lightguide_lr_size[k].x(), lightguide_lr_size[k].y(), lightguide_lr_size[k].z());

    lightguideSurfVol_tb_log[k] = new G4LogicalVolume(lightguideSurfVol_tb_box[k], the_detector_->air(), "lightguideSurfVol_tb");
 
    lightguideSurfVol_lr_log[k] = new G4LogicalVolume(lightguideSurfVol_lr_box[k], the_detector_->air(), "lightguideSurfVol_lr");

  }

  std::vector<G4ThreeVector> lightguide_tb_position;
  std::vector<G4ThreeVector> lightguide_lr_position;


  lightguide_tb_position.resize(2*nsegments);
  lightguide_lr_position.resize(nsegments);

  for(size_t k=0; k< 2*nsegments; k++){
    if(k<nsegments){
      lightguide_tb_position[k] = parameters->lightguide_topbottom_surface_position(k);
      lightguide_lr_position[k] = parameters->lightguide_leftright_surface_position(k);
    }
    else{
      lightguide_tb_position[k] = parameters->lightguide_topbottom_surface_position(k-nsegments);
      lightguide_tb_position[k].setY(-lightguide_tb_position[k].getY());
      lightguide_lr_position[k] = parameters->lightguide_leftright_surface_position(k-nsegments);
      lightguide_lr_position[k].setX(-lightguide_lr_position[k].getX());
    }
  }


  G4ThreeVector center = G4ThreeVector(0.0,0.0,0.0);
  G4ThreeVector displacement = G4ThreeVector(0.0,0.0,0.0);

  G4PVPlacement* lightguideSurfVol_tb; 
  G4PVPlacement* lightguideSurfVol_lr;

  G4LogicalBorderSurface* surfVolBorder_tb;
  G4LogicalBorderSurface* surfVolBorder_lr;
  std::string surfVolName;
  G4RotationMatrix* rot(0);

  G4VisAttributes* logicLightGuideSurfaceVisAtt = new G4VisAttributes(G4Colour(153/255. ,0/255.   ,255/255. ));
  //  logicLightGuideSurfaceVisAtt->SetForceSolid(true);
  logicLightGuideSurfaceVisAtt->SetForceWireframe(true);

  size_t j(0);

  for(size_t k = 0; k < 2*nsegments; k++) {
    if(k < nsegments){
      displacement = lightguide_tb_position[k];
      
      lightguideSurfVol_tb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_tb_log[k], "lightguideSurfVol_TB", the_detector_->get_logical_world(), false, j++);

      displacement.setY(-displacement.getY());

      lightguideSurfVol_tb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_tb_log[k], "lightguideSurfVol_TB", the_detector_->get_logical_world(), false, j++);


  lightguideSurfVol_tb_log[k]->SetVisAttributes(logicLightGuideSurfaceVisAtt);


  surfVolName = "lightguideSurface_tb_"+ itos(k+1);
    
  surfVolBorder_tb = new G4LogicalBorderSurface(surfVolName.c_str(), lightGuide_phys[k], lightguideSurfVol_tb,
						   the_detector_->cast_surface());
  
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_tb, "cast");
  
  displacement = lightguide_lr_position[k];


  lightguideSurfVol_lr = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_lr_log[k], "lightguideSurfVol_LR", the_detector_->get_logical_world(), false, j++);
  
  displacement.setX(-displacement.getX());

  lightguideSurfVol_lr = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_lr_log[k], "lightguideSurfVol_LR", the_detector_->get_logical_world(), false, j++);


  lightguideSurfVol_lr_log[k]->SetVisAttributes(logicLightGuideSurfaceVisAtt);

  surfVolName = "lightguideSurface_lr_"+ itos(k+1);
  
  surfVolBorder_lr = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[k], lightguideSurfVol_lr,
						       the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_lr, "cast");


    }
    else{

      displacement = lightguide_tb_position[k-nsegments];
      displacement.setZ(-lightguide_tb_position[k-nsegments].getZ());

      lightguideSurfVol_tb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_tb_log[k-nsegments], "lightguideSurfVol_TB", the_detector_->get_logical_world(),false, j++);
      displacement.setY(-displacement.getY());

      lightguideSurfVol_tb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_tb_log[k-nsegments], "lightguideSurfVol_TB", the_detector_->get_logical_world(),false, j++);


      lightguideSurfVol_tb_log[k-nsegments]->SetVisAttributes(logicLightGuideSurfaceVisAtt);

      surfVolName = "lightguideSurface_tb_"+ itos(k+1);

      surfVolBorder_tb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[k], lightguideSurfVol_tb,
						    the_detector_->cast_surface());

      
      NedmDetectorConstruction::RegisterSurface(surfVolBorder_tb, "cast");

      displacement = lightguide_lr_position[k-nsegments];
      displacement.setZ(-lightguide_lr_position[k-nsegments].getZ());

      lightguideSurfVol_lr = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_lr_log[k-nsegments], "lightguideSurfVol_LR", the_detector_->get_logical_world(),false,j++);
            
      displacement.setX(-displacement.getX());

      lightguideSurfVol_lr = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_lr_log[k-nsegments], "lightguideSurfVol_LR", the_detector_->get_logical_world(),false,j++);

      lightguideSurfVol_lr_log[k-nsegments]->SetVisAttributes(logicLightGuideSurfaceVisAtt);

      surfVolName = "lightguideSurface_lr"+ itos(k+1);
      
      surfVolBorder_lr = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[k], lightguideSurfVol_lr,
						       the_detector_->cast_surface());
      
      NedmDetectorConstruction::RegisterSurface(surfVolBorder_lr, "cast");

    }
  } 


  std::vector<G4ThreeVector> lightguide_fb_size;
  std::vector<G4ThreeVector> lightguide_fb_position;

  lightguide_fb_size.resize(2);
  lightguide_fb_position.resize(2);

  for(size_t k=0; k< 2; k++){
  lightguide_fb_size[k] = parameters->lightguide_frontback_surface_size(k);
  lightguide_fb_position[k] = parameters->lightguide_frontback_surface_position(k);
  }


  G4Box* lightguideSurfVol_fb_box[2];
  G4LogicalVolume* lightguideSurfVol_fb_log[2];

  for(size_t i=0; i<2; i++){
    lightguideSurfVol_fb_box[i] = new G4Box("lgSurfVol_fb", lightguide_fb_size[i].x(), lightguide_fb_size[i].y(), lightguide_fb_size[i].z());
    lightguideSurfVol_fb_log[i] = new G4LogicalVolume(lightguideSurfVol_fb_box[i], the_detector_->air(), "lightguideSurfVol_fb");
    lightguideSurfVol_fb_log[i]->SetVisAttributes(logicLightGuideSurfaceVisAtt);

  }


  G4PVPlacement* lightguideSurfVol_fb; 

  displacement = lightguide_fb_position[0];

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[0], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);

  G4LogicalBorderSurface* surfVolBorder_fb;

  surfVolName = "lightguideSurface_fb_1";
  
  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[0], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");

  displacement.setZ(-displacement.getZ());

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[0], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);


  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[0], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");

  displacement = lightguide_fb_position[1];

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[1], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);


  surfVolName = "lightguideSurface_fb_2";
  
  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[nsegments-1], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");

  displacement.setZ(-displacement.getZ());

  lightguideSurfVol_fb = new G4PVPlacement(rot,center + displacement, lightguideSurfVol_fb_log[1], "lightguideSurfVol_FB", the_detector_->get_logical_world(), false, j++);


  surfVolBorder_fb = new G4LogicalBorderSurface(surfVolName, lightGuide_phys[nsegments-1], lightguideSurfVol_fb, the_detector_->cast_surface());
      
  NedmDetectorConstruction::RegisterSurface(surfVolBorder_fb, "cast");


  
  //   //The following logic makes sure the the surface volumes are attached
  //   //to the correct light guides with border surfaces.
  //   for(int j = 0; j < 3; j++) {
  //     if (connections[i][j]) {
  //       sprintf(surfVolName, "lg%d_XSurface%d", lightGuide_phys[j]->GetCopyNo(), i);
  //       surfVolBorder = new G4LogicalBorderSurface(surfVolName,
  //           lightGuide_phys[j],
  //           lgSurfVol,
  //           the_detector_->machined_surface());

  //       NedmDetectorConstruction::RegisterSurface(surfVolBorder, "machined");
  //     }

  //   }

  // }

  
  }
*/
