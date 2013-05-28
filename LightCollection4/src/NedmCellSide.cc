#include "NedmCellSide.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "NedmDetectorConstruction.hh"
#include "NedmDetectorParameters.hh"

G4LogicalVolume* NedmCellSide::the_tb_side_withhole_;
G4LogicalVolume* NedmCellSide::the_tb_side_;
G4LogicalVolume* NedmCellSide::the_lr_side_;
G4LogicalVolume* NedmCellSide::the_fb_side_;
G4LogicalVolume* NedmCellSide::the_plug_;
G4LogicalVolume* NedmCellSide::the_mirror_;


NedmCellSide::NedmCellSide(G4RotationMatrix* pRot,
			   const G4ThreeVector& tlate,
			   const G4String whichside,			   
			   G4LogicalVolume* pMotherLogical,
			   G4int copyNo,
			   G4bool lrtbfb, G4bool lrfb, G4bool hole) : G4PVPlacement(pRot, tlate, lrtbfb ? 
										    (lrfb ? (NedmCellSide::the_fb_side_) : (NedmCellSide::the_lr_side_)) : 
										    (hole ? NedmCellSide::the_tb_side_withhole_ : NedmCellSide::the_tb_side_),
										    G4String("CellSide"), pMotherLogical, false, copyNo) { 

  ConstructTPBInterface(hole, whichside, tlate);
  ConstructSurfaceVolumes(hole, whichside, tlate);

  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();
  
  if(parameters->use_plug() && hole) {
    G4ThreeVector displacement = parameters->hole_position();

    G4RotationMatrix* Rot = new G4RotationMatrix;
    Rot->rotateX(90.*deg);

    if(pRot) {
      displacement *= (*pRot);
      *Rot = Rot->transform(*pRot);
    }

    G4VisAttributes* plug_attr = new G4VisAttributes(G4Colour::Cyan());
    plug_attr->SetForceSolid(true);
    the_plug_->SetVisAttributes(plug_attr);

    displacement += tlate;


    new G4PVPlacement(Rot, displacement,
        the_plug_, "Plug",
        the_detector_->get_logical_world(), false, 0);
  }
 
  if(parameters->use_mirror())
    ConstructMirror();
  //  else
  //    ConstructRearSurface();

 }

G4VSolid* NedmCellSide::MakeHole(G4VSolid* box, G4String solid_name, G4String hole_name, G4VSolid** hole_ptr) {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  G4double holethickness = parameters->cell_tb_size().y() + parameters->cell_tb_surface_size().y() + parameters->cell_tb_outside_surface_size().y();
  G4Tubs* hole = new G4Tubs(hole_name, 0., parameters->hole_radius(), holethickness, 0., twopi);

  if(hole_ptr)
    (*hole_ptr) = hole;

  G4RotationMatrix* Rot = new G4RotationMatrix;
  G4ThreeVector displacement = parameters->hole_position();
  Rot->rotateX(90.*deg);

  return new G4SubtractionSolid(solid_name, box,
      hole, Rot, displacement);

}

G4LogicalVolume* NedmCellSide::ConstructLogicalVolume(G4bool side1, G4String whichside) {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();
  
  G4VSolid* cellSide_solid;
  G4Box* cellSide_box;
  //  G4VSolid* cellSide_lr_solid;

  if(whichside == "tb"){
    G4ThreeVector cell_tb_size = parameters->cell_tb_size();
    cellSide_box = new G4Box("Cell_tb", cell_tb_size.x(), cell_tb_size.y(), cell_tb_size.z());
  }
  else if (whichside == "lr"){
      G4ThreeVector cell_lr_size = parameters->cell_lr_size();
      cellSide_box = new G4Box("Cell_lr", cell_lr_size.x(), cell_lr_size.y(), cell_lr_size.z());
  }
  else if (whichside == "fb"){
    G4ThreeVector cell_fb_size = parameters->cell_fb_size();
    cellSide_box = new G4Box("Cell_fb", cell_fb_size.x(), cell_fb_size.y(), cell_fb_size.z());
  }
  else{
    std::cerr << "Wrong Cell Wall in NedmCellSide::ConstructLogicalVolume. Please fix it. Exiting ... " << std::endl;
    exit(-1);
  }
    


  if(parameters->use_hole() && side1) {
    G4VSolid* hole_ptr;
    cellSide_solid = MakeHole(cellSide_box, "CellWithHole", "CellHole", &hole_ptr);
    

    if(parameters->use_plug()) {
      
      the_plug_ = new G4LogicalVolume(hole_ptr, the_detector_->torlon(), "Plug");

      G4LogicalSkinSurface* torlonSurface = new G4LogicalSkinSurface("TorlonSurface", the_plug_, the_detector_->torlon_surface());

      NedmDetectorConstruction::RegisterSurface(torlonSurface, "plug");
    }
  } else {
    cellSide_solid = cellSide_box;
  }

  G4LogicalVolume* cellSide_log = new G4LogicalVolume(cellSide_solid, the_detector_->acrylic(), "Cell");

  return cellSide_log;
}

G4LogicalVolume* NedmCellSide::ConstructMirrorLogicalVolume() {

  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  G4ThreeVector mirror_size = parameters->mirror_size();

  G4Box* mirror_box = new G4Box("mirror", mirror_size.x(),mirror_size.y(),mirror_size.z());
  
  G4LogicalVolume* mirror_log = new G4LogicalVolume(mirror_box,
      the_detector_->air(),
      "mirror");

  G4LogicalSkinSurface* mirror_surface = 
    new G4LogicalSkinSurface("mirror_surface", mirror_log, the_detector_->mirror_surface());

  NedmDetectorConstruction::RegisterSurface(mirror_surface, "mirror");

  return mirror_log;


}

void NedmCellSide::CreateLogicalVolumes() {
  NedmCellSide::the_tb_side_withhole_ = ConstructLogicalVolume(true,"tb");
  NedmCellSide::the_tb_side_ = ConstructLogicalVolume(false,"tb");
  NedmCellSide::the_lr_side_ = ConstructLogicalVolume(false,"lr");
  NedmCellSide::the_fb_side_ = ConstructLogicalVolume(false,"fb");
  NedmCellSide::the_mirror_ = ConstructMirrorLogicalVolume();
}

G4VPhysicalVolume* NedmCellSide::ConstructTPBInterface(G4bool side1, G4String whichside, const G4ThreeVector tlate )
 {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  G4VSolid* TPBInterface_solid;
  G4ThreeVector tpb_size;
  G4ThreeVector tpb_position;
  
  if(whichside == "tb"){
    tpb_size = parameters->cell_tb_surface_size();
    tpb_position = parameters->cell_tb_surface_position();
  }
  else if (whichside == "lr"){
    tpb_size = parameters->cell_lr_surface_size();
    tpb_position = parameters->cell_lr_surface_position();
  }
  else if (whichside == "fb"){
    tpb_size = parameters->cell_fb_surface_size();
    tpb_position = parameters->cell_fb_surface_position();
  }
  else{
    std::cerr << "Wrong size in NedmCellSide::ConstructTPBInterface. Exiting... " << std::endl;
    exit(-1);
  }


  G4Box* TPBInterface_box = new G4Box("TPBInterface", tpb_size.x(), tpb_size.y(), tpb_size.z());

  if(parameters->use_hole() && side1) {
    TPBInterface_solid = MakeHole(TPBInterface_box, "TPBInterfaceWithHole", "TPBInterface_hole", NULL);
  } else {
    TPBInterface_solid = TPBInterface_box;
  }
  
  G4LogicalVolume* TPBInterface_log = new G4LogicalVolume(TPBInterface_solid, the_detector_->air(), "TPBInterface");

  G4int copy_no = GetCopyNo();
  G4RotationMatrix* rot = GetRotation();
  G4ThreeVector cell_position = GetTranslation();
  G4VPhysicalVolume* TPBInterface_phys;  
  
  G4ThreeVector displacement;
  G4ThreeVector offset = G4ThreeVector(0,0,0);

  if(whichside == "lr"){
    offset.setX(parameters->cell_lr_size().getX() + parameters->cell_lr_surface_size().getX());
    displacement = tlate - sign(tlate.x())*(offset);
    TPBInterface_phys  = new G4PVPlacement(rot, displacement, TPBInterface_log, "TPBInterface", the_detector_->get_logical_world(), false, copy_no);
  }
  else if(whichside == "fb"){
    offset.setZ(parameters->cell_fb_size().getZ() + parameters->cell_fb_surface_size().getZ());
    displacement = tlate - sign(tlate.z())*(offset);
    TPBInterface_phys  = new G4PVPlacement(rot, displacement, TPBInterface_log, "TPBInterface", the_detector_->get_logical_world(), false, copy_no);
  }
  else if(whichside == "tb"){
    offset.setY(parameters->cell_tb_size().getY() + parameters->cell_tb_surface_size().getY());
    displacement = tlate - sign(tlate.y())*(offset);
    TPBInterface_phys  = new G4PVPlacement(rot, displacement, TPBInterface_log, "TPBInterface", the_detector_->get_logical_world(), false, copy_no);
  }

  G4LogicalBorderSurface* TPBMachinedSurf =
    new G4LogicalBorderSurface("MachinedSurfaceTPB",
        TPBInterface_phys,
        this,
        the_detector_->conversion_surface());

  G4LogicalBorderSurface* TPBMachinedSurf2 =
    new G4LogicalBorderSurface("MachinedSurfaceTPB2",
        this,
        TPBInterface_phys,
        the_detector_->tpb_surface());

  NedmDetectorConstruction::RegisterSurface(TPBMachinedSurf, "tpb");
  NedmDetectorConstruction::RegisterSurface(TPBMachinedSurf2, "tpb");


  return TPBInterface_phys;
}


void NedmCellSide::ConstructSurfaceVolumes(G4bool side1, G4String whichside, const G4ThreeVector tlate ) {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();


  G4int copy_no = GetCopyNo();
  G4RotationMatrix* rot = GetRotation();
  G4ThreeVector cell_position = GetTranslation();


  G4ThreeVector outside_surface_size;
  G4ThreeVector outside_surface_position;

  if(whichside == "lr"){
    outside_surface_size = parameters->cell_lr_outside_surface_size();
    outside_surface_position = parameters->cell_lr_outside_surface_position();
  }
  else if(whichside == "tb"){
    outside_surface_size = parameters->cell_tb_outside_surface_size();
    outside_surface_position = parameters->cell_tb_outside_surface_position();
  }
  else if(whichside == "fb"){
    outside_surface_size = parameters->cell_fb_outside_surface_size();
    outside_surface_position = parameters->cell_fb_outside_surface_position();
  }
  else{
    std::cerr << "Wrong size in NedmCellSide::ConstructSurfaceVolumes. Exiting... " << std::endl;
    exit(-1);
  }




  
  G4Box* outsurfVol_box;


  //Should make these static variables to avoid recreating this all the time.
  outsurfVol_box = new G4Box("cellSurfaceLR", outside_surface_size.x(), outside_surface_size.y(), outside_surface_size.z());

  G4VSolid* outsurfVol_solid;

  if(parameters->use_hole() && side1) {
    outsurfVol_solid = MakeHole(outsurfVol_box, "OutsideSurfaceWithHole", "OutsideSurface_hole", NULL);
  } else {
    outsurfVol_solid = outsurfVol_box;
  }



  G4LogicalVolume* outsurfVol_log = new G4LogicalVolume(outsurfVol_solid, the_detector_->air(), "OutsideCellSurface");

  G4VPhysicalVolume* outsurfVol_phys;  
  G4ThreeVector displacement;
  G4ThreeVector offset = G4ThreeVector(0,0,0);


  if(whichside == "lr"){
    offset.setX(parameters->cell_lr_size().getX() + parameters->cell_lr_outside_surface_size().getX());
    displacement = tlate  + sign(tlate.x())*(offset);
    outsurfVol_phys  = new G4PVPlacement(rot, displacement, outsurfVol_log, "OutsideCellSurface", the_detector_->get_logical_world(), false, copy_no);
  }
  else if(whichside == "tb"){
    offset.setY(parameters->cell_tb_size().getY() + parameters->cell_tb_outside_surface_size().getY());
    displacement = tlate + sign(tlate.y())*(offset);
    outsurfVol_phys  = new G4PVPlacement(rot, displacement, outsurfVol_log, "OutsideCellSurface", the_detector_->get_logical_world(), false, copy_no);
  }
  else if(whichside == "fb"){
    offset.setZ(parameters->cell_fb_size().getZ() + parameters->cell_fb_outside_surface_size().getZ());
    displacement = tlate + sign(tlate.z())*(offset);
    outsurfVol_phys  = new G4PVPlacement(rot, displacement, outsurfVol_log, "OutsideCellSurface", the_detector_->get_logical_world(), false, copy_no);
  }
 
  G4LogicalBorderSurface* cellMachinedOutsideSurface1 = new G4LogicalBorderSurface("cellMachinedOutsideSurfacePx", this, outsurfVol_phys, the_detector_->machined_surface());
  NedmDetectorConstruction::RegisterSurface(cellMachinedOutsideSurface1, "machined");

  //  G4LogicalSkinSurface* cellReflectingOuterSurface =  new G4LogicalSkinSurface("cellReflectingOuterSurface", outsurfVol_log, the_detector_->mirror_surface());
  //  NedmDetectorConstruction::RegisterSurface(cellReflectingOuterSurface, "mirror");

  return;
}


/*! @warning Do not call if you are calling NedmCellSide::ConstructRearSurface
 */
G4VPhysicalVolume* NedmCellSide::ConstructMirror() {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  G4RotationMatrix* rot = GetRotation();
  G4ThreeVector translation = GetTranslation();
  G4int copy_no = GetCopyNo();

  G4ThreeVector position = translation + G4ThreeVector(0.,0.,
      parameters->mirror_z_position());

  G4VPhysicalVolume* mirror_phys
    = new G4PVPlacement(rot, position, the_mirror_, "mirror",
			the_detector_->get_logical_world(), false, copy_no);

  return mirror_phys;
}

/*! @warning Do not call this method if you are calling NedmCellSide::ConstructMirror
 */
G4VPhysicalVolume* NedmCellSide::ConstructRearSurface() {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();
  G4RotationMatrix* rot = GetRotation();
  G4ThreeVector translation = GetTranslation();
  G4int copy_no = GetCopyNo();
  G4ThreeVector mirror_size = parameters->mirror_size();
  G4Box* rearSurf_box = new G4Box("rearSurf", 
                                  mirror_size.x(),
                                  mirror_size.y(),
                                  mirror_size.z());

  G4LogicalVolume* rearSurf_log = new G4LogicalVolume(rearSurf_box,
      the_detector_->acrylic(),
      "rear_surface");
  
  G4ThreeVector position = translation + G4ThreeVector(0.,0.,
      parameters->mirror_z_position());

  G4VPhysicalVolume* rearSurf_phys
    = new G4PVPlacement(rot, position, rearSurf_log, "rearSurf",
			the_detector_->get_logical_world(), false, copy_no);

    new G4LogicalBorderSurface("RearMachinedSurface", this,
			       rearSurf_phys, the_detector_->machined_surface());

  return rearSurf_phys;
}

