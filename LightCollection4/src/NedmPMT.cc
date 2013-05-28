#include "NedmPMT.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4VisAttributes.hh"

#include "NedmDetectorConstruction.hh"
#include "NedmDetectorParameters.hh"

G4LogicalVolume* NedmPMT::the_pmt_log_;

NedmPMT::NedmPMT(G4RotationMatrix* pRot,
    const G4ThreeVector& tlate,
    G4LogicalVolume* pMother,
    G4int copy_no) : G4PVPlacement(pRot, tlate, NedmPMT::the_pmt_log_,"PMT", pMother, false, copy_no) {

}

void NedmPMT::CreateLogicalVolumes() {
  G4VSolid* PMT_box;
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  if(parameters->use_full_pmt()) {
    PMT_box = new G4Tubs("PMT", 0., parameters->pmt_radius(),
        0.5*parameters->pmt_z(), 0., 360.);
  } else {
    //The only part of the PMT which is uncovered is that which is
    //in contact with the Lightguide
    PMT_box = new G4Box("PMT", parameters->cell_tb_size().x(),
        parameters->cell_tb_size().y(), parameters->pmt_z());
  }

  NedmPMT::the_pmt_log_ = new G4LogicalVolume(PMT_box, the_detector_->acrylic(), "PMT");

  G4VisAttributes* logicPMTVisAtt = new G4VisAttributes(G4Colour(204.0/255.0, 51.0/255.0,255.0/255.0));
  logicPMTVisAtt->SetForceSolid(true);
  NedmPMT::the_pmt_log_->SetVisAttributes(logicPMTVisAtt);



  new G4LogicalSkinSurface("photocath_surf", NedmPMT::the_pmt_log_,
      the_detector_->pmt_surface());
}
