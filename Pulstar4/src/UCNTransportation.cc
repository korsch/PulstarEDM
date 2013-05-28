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
// $Id: UCNTransportation.cc,v 1.1.1.1 2004/10/25 12:36:47 kuzniak Exp $
// GEANT4 tag $Name:  $
// 
// ------------------------------------------------------------
//  GEANT 4  include file implementation
//
// ------------------------------------------------------------
//
// This class is a process responsible for the transportation of 
// a particle, ie the geometrical propagation that encounters the 
// geometrical sub-volumes of the detectors.
//
// It is also tasked with part of updating the "safety".
//
// =======================================================================
// Modified:   
//            21 June 2003, J.Apostolakis: Calling field manager with 
//                            track, to enable it to configure its accuracy
//            13 May  2003, J.Apostolakis: Zero field areas now taken into
//                            account correclty in all cases (thanks to W Pokorski).
//            29 June 2001, J.Apostolakis, D.Cote-Ahern, P.Gumplinger: 
//                          correction for spin tracking   
//            20 Febr 2001, J.Apostolakis:  update for new FieldTrack
//            22 Sept 2000, V.Grichine:     update of Kinetic Energy
//             9 June 1999, J.Apostolakis & S.Giani: protect full relocation
//                          used in DEBUG for track that started on surface
//                          and went step < tolerance. Also forced fast
//                          relocation in all DEBUG cases
// Created:  19 March 1997, J. Apostolakis
// =======================================================================

//   4.9.04 
//   adapted for the transportation of UCN in fields by peter fierlinger
// 

#include "UCNTransportation.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ParticleTable.hh"
#include "G4ChordFinder.hh"

//////////////////////////////////////////////////////////////////////////
//
// Constructor

UCNTransportation::UCNTransportation( G4int verboseLevel )
  : G4VProcess( G4String("Transportation") ),
    fParticleIsLooping( false ),
    fPreviousSftOrigin (0.,0.,0.),
    fPreviousSafety    ( 0.0 ),
    fVerboseLevel( verboseLevel )
{
  
   G4TransportationManager* transportMgr ; 

  transportMgr = G4TransportationManager::GetTransportationManager() ; 

  fLinearNavigator = transportMgr->GetNavigatorForTracking() ; 

  // fGlobalFieldMgr = transportMgr->GetFieldManager() ;

  fFieldPropagator = transportMgr->GetPropagatorInField() ;

  // Cannot determine whether a field exists here,
  //  because it would only work if the field manager has informed 
  //  about the detector's field before this transportation process 
  //  is constructed.
  // Instead later the method DoesGlobalFieldExist() is called

  fCurrentTouchableHandle = new G4TouchableHistory();
  
  fEndGlobalTimeComputed  = false;
  fCandidateEndGlobalTime = 0;
}

//////////////////////////////////////////////////////////////////////////

UCNTransportation::~UCNTransportation()
{
}

//////////////////////////////////////////////////////////////////////////
//
// Responsibilities:
//    Find whether the geometry limits the Step, and to what length
//    Calculate the new value of the safety and return it.
//    Store the final time, position and momentum.

G4double UCNTransportation::
AlongStepGetPhysicalInteractionLength( const G4Track&  track,
                                             G4double, //  previousStepSize
                                             G4double  currentMinimumStep,
                                             G4double& currentSafety,
                                             G4GPILSelection* selection )
{
//G4cout << "halloooo "<< G4endl;
  

  G4double geometryStepLength, newSafety ; 
  fParticleIsLooping = false ;
  if( track.GetCurrentStepNumber()==1 )
  {
     // reset safety value
     //
     fPreviousSafety    = 0.0 ; 
     fPreviousSftOrigin = G4ThreeVector(0.,0.,0.) ;

     // ChordFinder reset internal state
     //
     if ( DoesGlobalFieldExist() )
     {
        G4ChordFinder* chordF= fFieldPropagator->GetChordFinder();
        if( chordF ) chordF->ResetStepEstimate();
     }

     // We need to update the current transportation's touchable handle
     // to the track's one
     //
     fCurrentTouchableHandle = track.GetTouchableHandle();
  }

  // GPILSelection is set to defaule value of CandidateForSelection
  // It is a return value
  //
  *selection = CandidateForSelection ;

  // Get initial Energy/Momentum of the track
  //
  const G4DynamicParticle*  pParticle  = track.GetDynamicParticle() ;
  G4ThreeVector startMomentumDir       = pParticle->GetMomentumDirection() ;
  G4ThreeVector startPosition          = track.GetPosition() ;

  // G4double   theTime        = track.GetGlobalTime() ;

  // The Step Point safety is now generalised to mean the limit of assumption
  // of all processes, so it is not the previous Step's geometrical safety.
  // We calculate the starting point's safety here.
  //
  G4ThreeVector OriginShift = startPosition - fPreviousSftOrigin ;
  G4double      MagSqShift  = OriginShift.mag2() ;
  
 //G4cout << "hallo1 " << G4endl; 
  if( MagSqShift >= sqr(fPreviousSafety) )
  {
     currentSafety = 0.0 ;
  }
  else
  {
     currentSafety = fPreviousSafety - sqrt(MagSqShift) ;
  }

  // Is the particle charged ?
  //
  G4ParticleDefinition* pParticleDef   = pParticle->GetDefinition() ;
  G4double              particleCharge = pParticleDef->GetPDGCharge() ; 

  fGeometryLimitedStep = false ;
  // fEndGlobalTimeComputed = false ;

  // There is no need to locate the current volume. It is Done elsewhere:
  //   On track construction 
  //   By the tracking, after all AlongStepDoIts, in "Relocation"

  // Check whether the particle have an (EM) field force exerting upon it
  //
  //G4cout << "hallo2 " << G4endl; 
 
  G4FieldManager* fieldMgr=0;
  G4bool          fieldExertsForce = false ;
   if(pParticleDef->GetParticleName() == "neutron" || pParticleDef->GetParticleName() == "ucn" ){
     
    fieldMgr= fFieldPropagator->FindAndSetFieldManager( track.GetVolume() ); 
    fieldExertsForce= this->DoesGlobalFieldExist(); 	// For gravity
  }
	
  if( (particleCharge != 0.0) )
  {

     fieldMgr= fFieldPropagator->FindAndSetFieldManager( track.GetVolume() ); 
     if (fieldMgr != 0) {
        // If the field manager has no field, there is no field !
        fieldExertsForce = (fieldMgr->GetDetectorField() != 0);
     } 
  }

  // Choose the calculation of the transportation: Field or not 
  //
  if( !fieldExertsForce ) 
  {
     G4double linearStepLength ;
     if( currentMinimumStep <= currentSafety )
     {
       // The Step is guaranteed to be taken
       //
       geometryStepLength   = currentMinimumStep ;
       fGeometryLimitedStep = false ;
     }
     else
     {
       //  Find whether the straight path intersects a volume
       //
       linearStepLength = fLinearNavigator->ComputeStep( startPosition, 
                                                         startMomentumDir,
                                                         currentMinimumStep, 
                                                         newSafety) ;
       // Remember last safety origin & value.
       //
       fPreviousSftOrigin = startPosition ;
       fPreviousSafety    = newSafety ; 

       // The safety at the initial point has been re-calculated:
       //
       currentSafety = newSafety ;
          
       if( linearStepLength <= currentMinimumStep)
       {
         // The geometry limits the Step size (an intersection was found.)
         //
         geometryStepLength   = linearStepLength ;
         fGeometryLimitedStep = true ;
       }
       else
       {
         // The full Step is taken.
         //
         geometryStepLength   = currentMinimumStep ;
         fGeometryLimitedStep = false ;
       }
     }
     endpointDistance = geometryStepLength ;

     // Calculate final position
     //
     fTransportEndPosition = startPosition+geometryStepLength*startMomentumDir ;

     // Momentfabsum direction, energy and polarisation are unchanged by transport
     //
     fTransportEndMomentumDir   = startMomentumDir ; 
     fTransportEndKineticEnergy = track.GetKineticEnergy() ;
     fTransportEndSpin          = track.GetPolarization();
     fParticleIsLooping         = false ;
     fMomentumChanged           = false ; 
     fEndGlobalTimeComputed     = false ;
  }
  else   //  A field exerts force
  {
//G4cout << "force1 " << G4endl;
    
     G4double       momentumMagnitude = pParticle->GetTotalMomentum() ;
     G4ThreeVector  EndUnitMomentum ;
     G4double       lengthAlongCurve ;
     G4double       restMass = pParticleDef->GetPDGMass() ;
 
//     fFieldPropagator->SetChargeMomentumMass( particleCharge,    // in e+ units
     fFieldPropagator->SetChargeMomentumMass( 0,    // in e+ units
                                              momentumMagnitude, // in Mev/c 
                                              restMass           ) ;  
//G4cout << "force2 " << G4endl;

      // Message the field Manager, to configure it for this track
     fieldMgr->ConfigureForTrack( &track );
      G4ThreeVector spin        = track.GetPolarization() ;
     //G4cout << "spin " << G4endl;
     G4FieldTrack  aFieldTrack = G4FieldTrack( startPosition, 
                                               track.GetMomentumDirection(),
                                               0.0, 
                                               track.GetKineticEnergy(),
                                               restMass,
                                               track.GetVelocity(),
                                               track.GetGlobalTime(), // Lab.
                                               track.GetProperTime(), // Part.
                                               &spin                  ) ;
//G4cout << "force3 " << G4endl;
      if( currentMinimumStep > 0 ) 
     {
// G4cout << "force4 " << G4endl;
        // Do the Transport in the field (non recti-linear)
        //
        lengthAlongCurve = fFieldPropagator->ComputeStep( aFieldTrack,
                                                          currentMinimumStep, 
                                                          currentSafety,
                                                          track.GetVolume() ) ;
 	//G4cout << "lengthalongcurve " << lengthAlongCurve << G4endl;

	if( lengthAlongCurve < currentMinimumStep)
        {
           geometryStepLength   = lengthAlongCurve ;
           fGeometryLimitedStep = true ;
 	//G4cout << "lengthalongcurve done" << lengthAlongCurve << G4endl;
       }
        else
        {
           geometryStepLength   = currentMinimumStep ;
           fGeometryLimitedStep = false ;
 	//G4cout << "minstep" << lengthAlongCurve << G4endl;
        }
     }
     else
     {
//G4cout << "force5 " << G4endl;
 	geometryStepLength   = lengthAlongCurve= 0.0 ;
        fGeometryLimitedStep = false ;
     }
      // Remember last safety origin & value.
     //
     fPreviousSftOrigin = startPosition ;
     fPreviousSafety    = currentSafety ;         
        
     // Get the End-Position and End-Momentum (Dir-ection)
     //
     fTransportEndPosition = aFieldTrack.GetPosition() ;
//G4cout << "fTransportEndPosition " << fTransportEndPosition << G4endl;

     // Momentum:  Magnitude and direction can be changed too now ...
     //
     fMomentumChanged         = true ; 
     fTransportEndMomentumDir = aFieldTrack.GetMomentumDir() ;
     fTransportEndKineticEnergy  = aFieldTrack.GetKineticEnergy() ; 
//G4cout << "force6 " << G4endl;

     if( fFieldPropagator->GetCurrentFieldManager()->DoesFieldChangeEnergy() )
     {
//G4cout << "force7 " << G4endl;
        // If the field can change energy, then the time must be integrated
        //    - so this should have been updated
        //
        fCandidateEndGlobalTime   = aFieldTrack.GetLabTimeOfFlight();
        fEndGlobalTimeComputed    = true;

        // was ( fCandidateEndGlobalTime != track.GetGlobalTime() );
        // a cleaner way is to have FieldTrack knowing whether time is updated.
//G4cout << "force7a " << G4endl;
     }
     else
     {
//G4cout << "force8 " << G4endl;
        // The energy should be unchanged by field transport,
        //    - so the time changed will be calculated elsewhere
        //
        fEndGlobalTimeComputed = false;

        // Check that the integration preserved the energy 
        //     -  and if not correct this!
        G4double  startEnergy= track.GetKineticEnergy();
        G4double  endEnergy= fTransportEndKineticEnergy; 

        static G4int no_inexact_steps=0, no_large_ediff;
        G4double absEdiff = fabs(startEnergy- endEnergy);
 //G4cout << "force81 " << G4endl;
       if( absEdiff > perMillion * endEnergy )
        {
          no_inexact_steps++;
          // Possible statistics keeping here ...
        }
//G4cout << "force82 " << G4endl;
        if( fVerboseLevel > 1 )
        {
          if( fabs(startEnergy- endEnergy) > perThousand * endEnergy )
          {
            static G4int no_warnings= 0, warnModulo=1,  moduloFactor= 10; 
            no_large_ediff ++;
            if( (no_large_ediff% warnModulo) == 0 )
            {
               no_warnings++;
               G4cout << "0 "
                      << G4endl
	              << "          Energy changed in Step, more than 1/1000: "
                      << G4endl
                      << "          Start= " << startEnergy
                      << G4endl
                      << "          End= "   << endEnergy
                      << G4endl
                      << "          Relative change= "
                      << (startEnergy-endEnergy)/startEnergy << G4endl;
               G4cout << " Energy has been corrected -- however, review"
                      << " field propagation parameters for accuracy."
                      << G4endl;
               G4cerr << "ERROR - G4Transportation::AlongStepGetPIL()"
                      << G4endl
	              << "        Bad 'endpoint'. Energy change detected"
                      << " and corrected,"
                      << G4endl
                      << "        occurred already "
                      << no_large_ediff << " times." << G4endl;
               if( no_large_ediff == warnModulo * moduloFactor )
               {
                  warnModulo *= moduloFactor;
               }
            }
          }
        }  // end of if (fVerboseLevel)

        // Correct the energy for fields that conserve it
        //  This - hides the integration error
        //       - but gives a better physical answer
//G4cout << "force9-1 " << G4endl;
        fTransportEndKineticEnergy= track.GetKineticEnergy(); 
//G4cout << "force9 " << G4endl;
     }

     fTransportEndSpin = aFieldTrack.GetSpin();
     fParticleIsLooping = fFieldPropagator->IsParticleLooping() ;
     endpointDistance   = (fTransportEndPosition - startPosition).mag() ;
  }

  // If we are asked to go a step length of 0, and we are on a boundary
  // then a boundary will also limit the step -> we must flag this.
  //
  //G4cout << "hallo10 " << G4endl;
  if( currentMinimumStep == 0.0 ) 
  {
      if( currentSafety == 0.0 )  fGeometryLimitedStep = true ;
  }

  // Update the safety starting from the end-point,
  // if it will become negative at the end-point.
  //
  //G4cout << "hallo11 " << currentSafety << G4endl;
  if( currentSafety < endpointDistance ) 
  {
//G4cout << "hallo110a " << currentSafety << G4endl;
      G4double endSafety =
               fLinearNavigator->ComputeSafety( fTransportEndPosition) ;
    //G4cout << "hallo11a " << currentSafety << G4endl;
    
      currentSafety      = endSafety ;
      fPreviousSftOrigin = fTransportEndPosition ;
      fPreviousSafety    = currentSafety ; 
  //G4cout << "hallo11b " << currentSafety << G4endl;

      // Because the Stepping Manager assumes it is from the start point, 
      //  add the StepLength
      //
      currentSafety     += endpointDistance ;
 // G4cout << "hallo11c " << currentSafety << G4endl;

#ifdef G4DEBUG_TRANSPORT 
      G4cout.precision(16) ;
      G4cout << "***Transportation::AlongStepGPIL ** " << G4endl  ;
      G4cout << "  Called Navigator->ComputeSafety at "
             << fTransportEndPosition
             << "    and it returned safety= " << endSafety << G4endl ; 
      G4cout << "  Adding endpoint distance " << endpointDistance 
             << "   we obtain pseudo-safety= " << currentSafety << G4endl ; 
#endif
  }            
  //G4cout << "hallo12 " << G4endl;

  fParticleChange.ProposeTrueStepLength(geometryStepLength) ;

  return geometryStepLength ;
}

//////////////////////////////////////////////////////////////////////////
//
//   Initialize ParticleChange  (by setting all its members equal
//                               to corresponding members in G4Track)

G4VParticleChange* UCNTransportation::AlongStepDoIt( const G4Track& track,
                                                    const G4Step&  stepData )
{
  fParticleChange.Initialize(track) ;

  //  Code for specific process 
  //
  fParticleChange.ProposePosition(fTransportEndPosition) ;
  fParticleChange.ProposeMomentumDirection(fTransportEndMomentumDir) ;
  fParticleChange.ProposeEnergy(fTransportEndKineticEnergy) ;
  //  fParticleChange.ProposeMomentum(fMomentumChanged) ;

  fParticleChange.ProposePolarization(fTransportEndSpin);
  
  G4double deltaTime = 0.0 ;

  // Calculate  Lab Time of Flight (ONLY if field Equations used it!)
     // G4double endTime   = fCandidateEndGlobalTime;
     // G4double delta_time = endTime - startTime;

  G4double startTime = track.GetGlobalTime() ;
  
  if (!fEndGlobalTimeComputed)
  {
     // The time was not integrated .. make the best estimate possible
     //
     G4double finalVelocity   = track.GetVelocity() ;
     G4double initialVelocity = stepData.GetPreStepPoint()->GetVelocity() ;
     G4double stepLength      = track.GetStepLength() ;

     static const G4ParticleDefinition* fOpticalPhoton =
           G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton");
     const G4DynamicParticle* fpDynamicParticle = track.GetDynamicParticle();
     if (fpDynamicParticle->GetDefinition()== fOpticalPhoton)
     {
        //  A photon is in the medium of the final point
        //  during the step, so it has the final velocity.
        deltaTime = stepLength/finalVelocity ;
     }
     else if (finalVelocity > 0.0)
     {
        G4double meanInverseVelocity ;
        // deltaTime = stepLength/finalVelocity ;
        meanInverseVelocity = 0.5
                            * ( 1.0 / initialVelocity + 1.0 / finalVelocity ) ;
        deltaTime = stepLength * meanInverseVelocity ;
     }
     else
     {
        deltaTime = stepLength/initialVelocity ;
     }
     fCandidateEndGlobalTime   = startTime + deltaTime ;
  }
  else
  {
     deltaTime = fCandidateEndGlobalTime - startTime ;
  }

  fParticleChange.ProposeGlobalTime( fCandidateEndGlobalTime ) ;

  // Now Correct by Lorentz factor to get "proper" deltaTime
  
  G4double  restMass       = track.GetDynamicParticle()->GetMass() ;
  G4double deltaProperTime = deltaTime*( restMass/track.GetTotalEnergy() ) ;

  fParticleChange.ProposeProperTime(track.GetProperTime() + deltaProperTime) ;
  //fParticleChange. SetTrueStepLength( track.GetStepLength() ) ;

  // If the particle is caught looping or is stuck (in very difficult
  // boundaries) in a magnetic field (doing many steps) 
  //   THEN this kills it ...
  //
  if ( fParticleIsLooping )
  {
      // Kill the looping particle 
      //
      fParticleChange.ProposeTrackStatus( fStopAndKill )  ;
#ifdef G4VERBOSE
      G4cout << " G4Transportation is killing track that is looping or stuck "
             << G4endl
             << "   This track has " << track.GetKineticEnergy()
             << " MeV energy." << G4endl;
#endif
      // ClearNumberOfInteractionLengthLeft() ;
  }

  // Another (sometimes better way) is to use a user-limit maximum Step size
  // to alleviate this problem .. 

  // Introduce smooth curved trajectories to particle-change
  //
  fParticleChange.SetPointerToVectorOfAuxiliaryPoints
    (fFieldPropagator->GimmeTrajectoryVectorAndForgetIt() );

  return &fParticleChange ;
}

//////////////////////////////////////////////////////////////////////////
//
//  This ensures that the PostStep action is always called,
//  so that it can do the relocation if it is needed.
// 

G4double UCNTransportation::
PostStepGetPhysicalInteractionLength( const G4Track&,
                                            G4double, // previousStepSize
                                            G4ForceCondition* pForceCond )
{ 
  *pForceCond = Forced ; 
  return DBL_MAX ;  // was kInfinity ; but convention now is DBL_MAX
}

/////////////////////////////////////////////////////////////////////////////
//

G4VParticleChange* UCNTransportation::PostStepDoIt( const G4Track& track,
                                                   const G4Step& )
{
  
//G4cout << "trans poststep1 " << G4endl;  
  G4TouchableHandle retCurrentTouchable ;   // The one to return

  // Initialize ParticleChange  (by setting all its members equal
  //                             to corresponding members in G4Track)
  // fParticleChange.Initialize(track) ;  // To initialise TouchableChange

  fParticleChange.ProposeTrackStatus(track.GetTrackStatus()) ;

  // If the Step was determined by the volume boundary,
  // logically relocate the particle
  
//G4cout << "trans poststep2 " << G4endl;  
  if(fGeometryLimitedStep)
  {  
    // fCurrentTouchable will now become the previous touchable, 
    // and what was the previous will be freed.
    // (Needed because the preStepPoint can point to the previous touchable)

    fLinearNavigator->SetGeometricallyLimitedStep() ;
    fLinearNavigator->
    LocateGlobalPointAndUpdateTouchableHandle( track.GetPosition(),
                                               track.GetMomentumDirection(),
                                               fCurrentTouchableHandle,
                                               true                      ) ;
    // Check whether the particle is out of the world volume 
    // If so it has exited and must be killed.
    //
//G4cout << "trans poststep3 " << G4endl;  
    if( fCurrentTouchableHandle->GetVolume() == 0 )
    {
       fParticleChange.ProposeTrackStatus( fStopAndKill ) ;
    }
    retCurrentTouchable = fCurrentTouchableHandle ;
    fParticleChange.SetTouchableHandle( fCurrentTouchableHandle ) ;
  }
  else                 // fGeometryLimitedStep  is false
  {                    
#ifdef G4DEBUG_POSTSTEP_TRANSPORT

    // Although the location is changed, we know that the physical 
    // volume remains constant. 
    // In order to help in checking the user geometry
    // we perform a full-relocation and check its result 
    // *except* if we have made a very small step from a boundary
    // (i.e. remaining inside the tolerance)

    G4bool  startAtSurface_And_MoveEpsilon ;
    startAtSurface_And_MoveEpsilon =
             (stepData.GetPreStepPoint()->GetSafety() == 0.0) && 
             (stepData.GetStepLength() < kCarTolerance ) ;

//G4cout << "trans poststep4 " << G4endl;  
    if( startAtSurface_And_MoveEpsilon ) 
    {
       fLinearNavigator->
       LocateGlobalPointAndUpdateTouchableHandle( track.GetPosition(),
                                                  track.GetMomentumDirection(),
                                                  fCurrentTouchableHandle,
                                                  true                     );
       if( fCurrentTouchableHandle->GetVolume() != track.GetVolume() )
       {
         G4cerr << " ERROR: A relocation within safety has"
                << " caused a volume change! " << G4endl  ; 
         G4cerr << "   The old volume is called " 
                << track.GetVolume()->GetName() << G4endl ; 
         G4cerr << "   The new volume is called " ;

         if ( fCurrentTouchableHandle->GetVolume() != 0 )
         {
            G4cerr << fCurrentTouchableHandle->GetVolume()->GetName()
                   << G4endl ; 
         }
         else
         {
            G4cerr << "Out of World" << G4endl ; 
         }
         G4cerr.precision(7) ;
         G4cerr << "   The position is " << track.GetPosition() <<  G4endl ;

         // Let us relocate again, for debuging
         //
         fLinearNavigator->
         LocateGlobalPointAndUpdateTouchableHandle(track.GetPosition(),
                                                   track.GetMomentumDirection(),
                                                   fCurrentTouchableHandle,
                                                   true                     ) ;
         G4cerr << "   The newer volume is called "  ;

         if ( fCurrentTouchableHandle->GetVolume() != 0 )
         {
            G4cerr << fCurrentTouchableHandle->GetVolume()->GetName()
                   << G4endl ;
         } 
         else
         {
            G4cerr << "Out of World" << G4endl ; 
         }
       }
//G4cout << "trans poststep5 " << G4endl;  

       assert( fCurrentTouchableHandle->GetVolume()->GetName() ==
               track.GetVolume()->GetName() ) ;

       retCurrentTouchable = fCurrentTouchableHandle ; 
       fParticleChange.SetTouchableHandle( fCurrentTouchableHandle ) ;
       
    }
    else
    {
       retCurrentTouchable = track.GetTouchableHandle() ;
       fParticleChange.SetTouchableHandle( track.GetTouchableHandle() ) ;
    }
//G4cout << "trans poststep6 " << G4endl;  

    //  This must be done in the above if ( AtSur ) fails
    //  We also do it for if (true) in order to get debug/opt to  
    //  behave as exactly the same way as possible.
    //
    fLinearNavigator->LocateGlobalPointWithinVolume( track.GetPosition() ) ;

#else    // ie #ifndef G4DEBUG_POSTSTEP_TRANSPORT does a quick relocation

    // This serves only to move the Navigator's location
    //
    fLinearNavigator->LocateGlobalPointWithinVolume( track.GetPosition() ) ;

    // The value of the track's current Touchable is retained. 
    // (and it must be correct because we must use it below to
    // overwrite the (unset) one in particle change)
    // Although in general this is fCurrentTouchable, at the start of
    // a step it could be different ... ??
    //
    fParticleChange.SetTouchableHandle( track.GetTouchableHandle() ) ;
    retCurrentTouchable = track.GetTouchableHandle() ;

#endif
  }         // endif ( fGeometryLimitedStep ) 

  const G4VPhysicalVolume* pNewVol = retCurrentTouchable->GetVolume() ;
  const G4Material* pNewMaterial   = 0 ;
 
  if( pNewVol != 0 ) pNewMaterial= pNewVol->GetLogicalVolume()->GetMaterial() ; 

  // ( <const_cast> pNewMaterial ) ;

  fParticleChange.SetMaterialInTouchable( (G4Material *) pNewMaterial ) ;
//G4cout << "trans poststep7 " << G4endl;  

  const G4MaterialCutsCouple* pNewMaterialCutsCouple = 0;
  if( pNewVol != 0 )
  {
    pNewMaterialCutsCouple=pNewVol->GetLogicalVolume()->GetMaterialCutsCouple();
  }

  if( pNewVol!=0 && pNewMaterialCutsCouple->GetMaterial()!=pNewMaterial )
  {
    // for parametrized volume
    //
    pNewMaterialCutsCouple =
      G4ProductionCutsTable::GetProductionCutsTable()
                             ->GetMaterialCutsCouple(pNewMaterial,
                               pNewMaterialCutsCouple->GetProductionCuts());
  }
  fParticleChange.SetMaterialCutsCoupleInTouchable( pNewMaterialCutsCouple );

  // temporarily until Get/Set Material of ParticleChange, 
  // and StepPoint can be made const. 
  // Set the touchable in ParticleChange
  // this must always be done because the particle change always
  // uses this value to overwrite the current touchable pointer.
  //
  fParticleChange.SetTouchableHandle(retCurrentTouchable) ;
//G4cout << "trans poststep8 " << G4endl;  

  return &fParticleChange ;
}
