/*
 *  $Id: DmpSimPrimaryGeneratorAction.cc, 2014-10-05 00:06:15 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/03/2014
*/

#include "G4UImanager.hh"
#include "G4GeneralParticleSource.hh"

#include "DmpSimPrimaryGeneratorAction.h"
#include "DmpEvtMCPrimaryParticle.h"
#include "DmpDataBuffer.h"
//#include "DmpMetadata.h"

DmpSimPrimaryGeneratorAction::DmpSimPrimaryGeneratorAction()
 :fPrimaryParticle(0),
  fGPS(0)
  //fMetadata(0)
{
  //fMetadata = dynamic_cast<DmpMetadata*>(gDataBuffer->ReadObject("Metadata/MCTruth/JobOpt"));
  double tmp[3]={0,0,0};
  std::istringstream iss_dir(gRootIOSvc->JobOption()->GetValue("gps/direction"));
  iss_dir>>tmp[0]>>tmp[1]>>tmp[2];
  fDirection.setX(tmp[0]);
  fDirection.setY(tmp[1]);
  fDirection.setZ(tmp[2]);
  std::istringstream iss_cen(gRootIOSvc->JobOption()->GetValue("gps/centre"));
  std::string unit="cm";
  iss_cen>>tmp[0]>>tmp[1]>>tmp[2]>>unit;
  fCentre.setX(tmp[0]);
  fCentre.setY(tmp[1]);
  fCentre.setZ(tmp[2]);
  if("cm" ==unit){
    fCentre *=10;
  }else if("m" == unit){
    fCentre *=100;
  }
  fPrimaryParticle = new DmpEvtMCPrimaryParticle();
  gDataBuffer->RegisterObject("Event/MCTruth/PrimaryParticle",fPrimaryParticle);
  fGPS = new G4GeneralParticleSource();
}

//-------------------------------------------------------------------
DmpSimPrimaryGeneratorAction::~DmpSimPrimaryGeneratorAction(){
  delete fGPS;
}

//-------------------------------------------------------------------
void DmpSimPrimaryGeneratorAction::ApplyGPSCommand(){
  G4UImanager *uiMgr = G4UImanager::GetUIpointer();
  short nCmd = gRootIOSvc->JobOption()->OptionSize();
  for(short i =0; i<nCmd;++i){
    std::string command = gRootIOSvc->JobOption()->GetCommand(i);
DmpLogDebug<<"\t"<<command<<std::endl;
    if(command == "BT/DAMPE/Rotation"){
DmpLogDebug<<"\t"<<command<<std::endl;
      double rad = 0;
      std::istringstream iss(gRootIOSvc->JobOption()->GetValue(command));
      iss>>rad;
      rad = rad / 180 * 3.141592653;
      AdjustmentRotation(rad);
    }else if(command == "BT/DAMPE/Translation"){
DmpLogDebug<<"\t"<<command<<std::endl;
      double tmp[3]={0,0,0};
      std::istringstream iss(gRootIOSvc->JobOption()->GetValue(command));
      iss>>tmp[0]>>tmp[1]>>tmp[2];
      AdjustmentTranslation(G4ThreeVector(tmp[0],tmp[1],tmp[2]));
    }
  }
  for(short i =0; i<nCmd;++i){// must after adjustment
    if(gRootIOSvc->JobOption()->GetCommand(i).find("gps/") != std::string::npos){
      std::string cmd = "/" + gRootIOSvc->JobOption()->GetCommand(i) + " " + gRootIOSvc->JobOption()->GetValue(i);
      uiMgr->ApplyCommand(cmd);
    }
  }
}

//-------------------------------------------------------------------
void DmpSimPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent){
  fGPS->GeneratePrimaryVertex(anEvent);
  // Set information of current event
  fPrimaryParticle->SetEventID(anEvent->GetEventID());
  fPrimaryParticle->SetTime(fGPS->GetParticleTime());
  fPrimaryParticle->SetPosition(fGPS->GetParticlePosition().x(),fGPS->GetParticlePosition().y(),fGPS->GetParticlePosition().z());
DmpLogDebug<<fGPS->GetParticlePosition().x()<<std::endl;
DmpLogDebug<<fGPS->GetParticlePosition().y()<<std::endl;
DmpLogDebug<<fGPS->GetParticlePosition().z()<<std::endl;
  G4ThreeVector direction(fGPS->GetParticleMomentumDirection());
DmpLogDebug<<"xxxxx "<<direction.x()<<"\t"<<direction.y()<<"\t"<<direction.z()<<std::endl;
  fPrimaryParticle->SetDirection(direction.x(),direction.y(),direction.z());
  fPrimaryParticle->SetKineticEnergy(fGPS->GetParticleEnergy());
  G4ParticleDefinition *primaryParticle = fGPS->GetParticleDefinition();
  fPrimaryParticle->SetPDGCode(primaryParticle->GetPDGEncoding());
  fPrimaryParticle->SetMass(primaryParticle->GetPDGMass());
  fPrimaryParticle->SetCharge(primaryParticle->GetPDGCharge());
  fPrimaryParticle->SetComponent(primaryParticle->GetLeptonNumber(),primaryParticle->GetBaryonNumber());
  DmpLogDebug<<" Mass = "<<primaryParticle->GetPDGMass() << ", Kinetic=" << fGPS->GetParticleEnergy() << ", Charge=" << primaryParticle->GetPDGCharge() << std::endl;
}

//-------------------------------------------------------------------
void DmpSimPrimaryGeneratorAction::AdjustmentRotation(const double &rad){
  fDirection.rotateY(rad);
  std::ostringstream oss;
  oss<<fDirection.x()<<" "<<fDirection.y()<<" "<<fDirection.z();
DmpLogDebug<<oss.str()<<std::endl;
  gRootIOSvc->JobOption()->SetOption("gps/direction",oss.str());

  fCentre.rotateY(rad);
  std::ostringstream oss_1;
  oss_1<<fCentre.x()<<" "<<fCentre.y()<<" "<<fCentre.z()<<" mm";
DmpLogDebug<<oss_1.str()<<std::endl;
  gRootIOSvc->JobOption()->SetOption("gps/centre",oss_1.str());
}

//-------------------------------------------------------------------
void DmpSimPrimaryGeneratorAction::AdjustmentTranslation(const G4ThreeVector &v){
  fCentre -= v;
  std::ostringstream oss;
  oss<<fCentre.x()<<" "<<fCentre.y()<<" "<<fCentre.z()<<" mm";
DmpLogDebug<<oss.str()<<std::endl;
  gRootIOSvc->JobOption()->SetOption("gps/centre",oss.str());
}


