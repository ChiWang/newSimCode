/*
 *  $Id: DmpSimSSD_SD.cc, 2014-10-15 23:25:15 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 14/10/2014
*/

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include "DmpSimSSD_SD.h"
#include "DmpDataBuffer.h"

//-------------------------------------------------------------------
DmpSimSSD_SD::DmpSimSSD_SD(const std::string &ID)
 :G4VSensitiveDetector("SSD_"+ID+"_SD"),
  fEvtMCSSD(0)
{
  fEvtMCSSD = new DmpEvtMCSSD();
  gDataBuffer->RegisterObject("Event/MCTruth/SSD_"+ID,fEvtMCSSD);
}

//-------------------------------------------------------------------
DmpSimSSD_SD::~DmpSimSSD_SD(){
}

//-------------------------------------------------------------------
#include <boost/lexical_cast.hpp>
G4bool DmpSimSSD_SD::ProcessHits(G4Step *aStep,G4TouchableHistory*){
  G4Track *aTrack = aStep->GetTrack();
  fEvtMCSSD->fTrackID.push_back(aTrack->GetTrackID());
  fEvtMCSSD->fParentID.push_back(aTrack->GetParentID());
  fEvtMCSSD->fPDGCode.push_back(aTrack->GetDynamicParticle()->GetPDGcode());

  G4ThreeVector pos = aStep->GetPreStepPoint()->GetPosition();
  fEvtMCSSD->fPosition.push_back(TVector3(pos.x()/mm,pos.y()/mm,pos.z()/mm));
  G4ThreeVector dir = aStep->GetPreStepPoint()->GetMomentumDirection();
  fEvtMCSSD->fDirection.push_back(TVector3(dir.x(),dir.y(),dir.z()));
  fEvtMCSSD->fEnergy.push_back(aStep->GetTotalEnergyDeposit()/MeV);
  return true;
}

//-------------------------------------------------------------------
void DmpSimSSD_SD::Initialize(G4HCofThisEvent*){
  fEvtMCSSD->Reset();
}

//-------------------------------------------------------------------
void DmpSimSSD_SD::EndOfEvent(G4HCofThisEvent* HCE){
}


