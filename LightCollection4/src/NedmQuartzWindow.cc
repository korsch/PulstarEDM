#include "NedmQuartzWindow.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalSkinSurface.hh"

#include "NedmDetectorConstruction.hh"
#include "NedmDetectorParameters.hh"

G4LogicalVolume* NedmQuartzWindow::the_window_log_;

NedmQuartzWindow::NedmQuartzWindow(G4RotationMatrix *pRot,
    const G4ThreeVector& tlate,
    G4LogicalVolume* pMother,
    G4int copy_no) : G4PVPlacement(pRot,
      tlate,
      NedmQuartzWindow::the_window_log_,
      "QuartzWindow",
      pMother,
      false,
      copy_no) { }


void NedmQuartzWindow::CreateLogicalVolumes() {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();
   
  G4Tubs* quartz_box = new G4Tubs("quartz",
      0.,
      parameters->window_radius(),
      parameters->window_z(),
      0.,
      360.);

  the_window_log_ = new G4LogicalVolume(quartz_box,
      the_detector_->quartz(),
      "quartz",
      0,0,0);
}
