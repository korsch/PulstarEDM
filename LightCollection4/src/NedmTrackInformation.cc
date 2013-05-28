#include "NedmTrackInformation.hh"
#include "NedmDetectorConstruction.hh"

#include "G4String.hh"

/*!
  This methods attempts to look this surface up in NedmDetectorConstruction::borderSurfaceMap. If the surface is found there it increments the corresponding bounce type. Otherwise it does nothing.
 */
void NedmTrackInformation::RegisterBounce(G4LogicalSurface* surface) {
  const SurfaceTypeMap& surfaces = NedmDetectorConstruction::the_border_surface_map();

  SurfaceTypeMap::const_iterator surface_it = surfaces.find(surface);

  if(surface_it != surfaces.end()) {
    G4String type = surface_it->second;
    if(type == "cast" )
      cast_bounces_++;
    if(type == "machined")
      machined_bounces_++;
    if(type == "tpb")
      tpb_bounces_++;
    if(type == "mirror")
      mirror_bounces_++;
  }
}

void NedmTrackInformation::GetBounces(G4int& cast, G4int& mach, G4int& tpb, G4int& mirror) {
  cast = cast_bounces_;
  mach = machined_bounces_;
  tpb = tpb_bounces_;
  mirror = mirror_bounces_;
}

/*! The string to use for the absorber is looked up in NedmDetectorConstruction::borderSurfaceMap */
void NedmTrackInformation::WasAbsorbed(G4LogicalSurface* surface) {
  const SurfaceTypeMap& surfaces = NedmDetectorConstruction::the_border_surface_map();

 SurfaceTypeMap::const_iterator surface_it = surfaces.find(surface);

  if(surface_it != surfaces.end()) {
    absorber_ = surface_it->second;
  }
}

G4String NedmTrackInformation::absorber() {
  if (detected_)
    return G4String("detected");
  else if (tpb_)
    return G4String("tpb");
  else
    return absorber_;
}
