//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef NedmDetectorConstruction_h
#define NedmDetectorConstruction_h 1

#include <map>

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4OpticalSurface.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Material;
class G4LogicalVolume;
class G4VSolid;

class G4VPhysicalVolume;
class G4OpticalSurface;
class G4MaterialPropertiesTable;
class G4LogicalSurface;

class NedmDetectorMessenger;

typedef std::map<G4LogicalSurface*, G4String> SurfaceTypeMap;

/*! 
  \brief Reponsible for creating the geometry of the simulation

 */
class NedmDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  NedmDetectorConstruction();
  ~NedmDetectorConstruction();
  
public:
  //! Mandatory method called when simulation is initialized.
  G4VPhysicalVolume* Construct();

  //! Output the value of user controlled parameters
  void PrintParameters();

  //! @name General methods
  //@{
  //! Updates the geometry after it has been modified
  void UpdateGeometry();
  //@}}

  //! Returns the a map relating surfaces to their type @see NedmDetectorConstruction::the_border_surface_map_
  inline
  static const SurfaceTypeMap& the_border_surface_map() { return the_border_surface_map_; }

  inline
  static void RegisterSurface(G4LogicalSurface* surface, G4String description) {
    the_border_surface_map_[surface] = description;
  }

  inline G4LogicalVolume* get_logical_world() const { return expHall_log; }

  //!@name Material Accessors
  //@{
  inline G4Material* acrylic() { return acrylic_; }
  inline G4Material* air() { return air_; }
  inline G4Material* quartz() { return quartz_; }
  inline G4Material* torlon() { return torlon_; }

  inline G4OpticalSurface* machined_surface() { return machined_surface_; }
  inline G4OpticalSurface* cast_surface() { return cast_surface_; }
  inline G4OpticalSurface* tpb_surface() { return tpb_surface_; }
  inline G4OpticalSurface* conversion_surface() { return conversion_surface_; }
  inline G4OpticalSurface* pmt_surface() { return pmt_surface_; }
  inline G4OpticalSurface* torlon_surface() { return torlon_surface_; }
  inline G4OpticalSurface* mirror_surface() { return mirror_surface_; }
  //@}

  //!@name Optical Property Mutators
  //@{
  //! Sets the Machined loss per bounce
  void set_machined_loss(G4double);
  //! Sets the Cast loss per bounce
  void set_cast_loss(G4double);
  //! Sets the loss per bounce on the Torlon Plug
  void set_torlon_loss(G4double);
  //! Sets the loss per bounce on the TPB
  void set_tpb_loss(G4double);
  //! Sets the Bulk Attenuation. Given in internal units.
  void set_bulk_attenuation(G4double);
  //! Sets the index of refraction of the acrylic
  void set_acrylic_index(G4double);
  //! Sets the reflectivity of the rear mirror
  void set_mirror_reflectivity(G4double);
  //! Sets the type of the rear reflector
  void set_mirror_type(G4String);
  //! Sets the type of reflector for the V1 plug
  void set_torlon_type(G4String);
  //! Sets the detection efficiency of the PMTs
  void set_pmt_efficiency(G4double);
  //@}

private:
  NedmDetectorMessenger* detMessenger;

  /*! @name Parameter Definition and Calculation

    Calculates necessary positions and such for detector construction
  */
  //@{
  void SetDefaults();
  //@}

  //! Calls logical volume creation code on each of the component classes.
  void CreateComponentLogicalVolumes();

  //! @name Construction Methods
  //@{
  //! Instantiates the materials for use in the detector.
  void ConstructMaterials();

  //! Does the actual creation of the detector
  G4VPhysicalVolume* ConstructGeometry();
  
  //! Create the detector proper.
  void ConstructBox();

  //! Create the Optical Surface and Material Properties
  void ConstructOpticalSurfaces();
  
  //! Creates the box endcaps
  void ConstructEndCaps();
  //@}

  //! The logical volume representing the simulation world.
  G4LogicalVolume* expHall_log;

  //! Physical World.
  G4VPhysicalVolume* expHall_phys;
  
  //! @name Materials
  //@{
  G4Material* quartz_;
  G4Material* air_;
  G4Material* acrylic_;
  G4Material* torlon_;
  //@}

  //! @name Optical Properties Vars
  //@{
  //! The number of different photon energies to use as reference points for our
  // material property vectors
  static const G4int kEnergies;
  //! The energies to use as reference points for optical properties.
  static G4double the_photon_energies_[];

  G4OpticalSurfaceFinish mirror_type_;
  G4OpticalSurfaceFinish torlon_type_;

  G4OpticalSurface* cast_surface_;
  G4OpticalSurface* machined_surface_;
  G4OpticalSurface* tpb_surface_;
  G4OpticalSurface* conversion_surface_;
  //! Surface for V1 plug
  G4OpticalSurface* torlon_surface_;
  G4OpticalSurface* mirror_surface_;
  G4OpticalSurface* pmt_surface_;

  G4MaterialPropertiesTable* machined_mt_;
  G4MaterialPropertiesTable* cast_mt_;
  G4MaterialPropertiesTable* torlon_mt_;
  G4MaterialPropertiesTable* acrylic_mt_;
  G4MaterialPropertiesTable* mirror_mt_;
  G4MaterialPropertiesTable* tpb_mt_;
  G4MaterialPropertiesTable* tpbc_mt_;
  G4MaterialPropertiesTable* photocath_mt_;

  G4double cast_lpb_;
  G4double machined_lpb_;
  G4double torlon_lpb_;
  G4double tpb_lpb_;
  G4double acrylic_absorption_length_;
  G4double acrylic_index_;
  G4double mirror_reflectivity_;
  G4double pmt_efficiency_;
  //@}


  /*! @brief Keeps track of the types of each Logical Surface.

    At the moment each surface added to this map is considered either a cast or machined surface. This is used for bounce tracking code.
  */
  static SurfaceTypeMap the_border_surface_map_;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*NedmDetectorConstruction_h*/
