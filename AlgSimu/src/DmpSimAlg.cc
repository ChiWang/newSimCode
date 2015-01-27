/*
 *  $Id: DmpSimAlg.cc, 2015-01-27 10:52:03 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 10/06/2014
*/

#include <boost/lexical_cast.hpp>

#include "DmpSimAlg.h"
#include "DmpSimRunManager.h"
#include "G4PhysListFactory.hh"
#include "DmpSimDetector.h"
#include "DmpSimMagneticField.h"
#include "DmpSimPrimaryGeneratorAction.h"
#include "DmpSimTrackingAction.h"
#include "DmpCore.h"
#include "DmpDataBuffer.h"
#include "G4UImanager.hh"
#ifdef G4UI_USE_QT
#include "G4UIExecutive.hh"
#endif
#ifdef G4VIS_USE_OPENGLQT
#include "G4VisExecutive.hh"
#endif

//-------------------------------------------------------------------
DmpSimAlg::DmpSimAlg()
 :DmpVAlg("Sim/BootAlg"),
  fSimRunMgr(0),
  fPhyFactory(0),
  fSource(0),
  fDetector(0),
  fTracking(0)
{
  gRootIOSvc->JobOption()->SetOption("Mode","batch");
  gRootIOSvc->JobOption()->SetOption("Physics","QGSP_BIC");
  gRootIOSvc->JobOption()->SetOption("Gdml","FM");        // Flight Mode
  gRootIOSvc->JobOption()->SetOption("Seed",boost::lexical_cast<std::string>(gRootIOSvc->JobOption()->JobTime()));
  gRootIOSvc->JobOption()->SetOption("Nud/DeltaTime","100");  // 100 ns
  gRootIOSvc->JobOption()->SetOption("gps/particle","mu-");
  gRootIOSvc->JobOption()->SetOption("gps/direction","0 0 1");
  gRootIOSvc->JobOption()->SetOption("gps/centre","0 0 -2700 cm");//1700 is not enough for SPS
  gRootIOSvc->SetOutputFile("DmpSim_"+gRootIOSvc->JobOption()->GetValue("Seed"));
  gRootIOSvc->SetOutputKey("sim");
}

//-------------------------------------------------------------------
DmpSimAlg::~DmpSimAlg(){
  //delete fSimRunMgr;
}

//-------------------------------------------------------------------
void DmpSimAlg::Set(const std::string &type,const std::string &argv){
  if("gps/centre"==type || "gps/direction" == type){
    DmpLogWarning<<"Reseting "<<type<<": "<<gRootIOSvc->JobOption()->GetValue(type)<<"\t new value = "<<argv<<DmpLogEndl;
  }
  if("Mode" == type && "batch" != argv){
    gRootIOSvc->SetOutputFile("DmpSimVis_"+gRootIOSvc->JobOption()->GetValue("Seed"));
  }
  gRootIOSvc->JobOption()->SetOption(type,argv);
}

//-------------------------------------------------------------------
#include <stdlib.h>     // getenv()
bool DmpSimAlg::Initialize(){
// set seed
  DmpLogCout<<"\tRandom seed: "<<gRootIOSvc->JobOption()->GetValue("Seed")<<DmpLogEndl;      // keep this information in any case
  CLHEP::HepRandom::setTheSeed(boost::lexical_cast<long>(gRootIOSvc->JobOption()->GetValue("Seed")));
// set G4 kernel
  fSimRunMgr = new DmpSimRunManager();
  fPhyFactory = new G4PhysListFactory();            fSimRunMgr->SetUserInitialization(fPhyFactory->GetReferencePhysList(gRootIOSvc->JobOption()->GetValue("Physics")));
  fSource = new DmpSimPrimaryGeneratorAction();     fSimRunMgr->SetUserAction(fSource);      // only Primary Generator is mandatory
  fDetector = new DmpSimDetector();                 fSimRunMgr->SetUserInitialization(fDetector);
  fTracking = new DmpSimTrackingAction();           fSimRunMgr->SetUserAction(fTracking);
  fSimRunMgr->Initialize();
  fSource->ApplyGPSCommand(); // must after fSimRunMgr->Initialize()
// boot simulation
  if(gRootIOSvc->JobOption()->GetValue("Mode") == "batch"){    // batch mode
    if(fSimRunMgr->ConfirmBeamOnCondition()){   // if not vis mode, do some prepare for this run. refer to G4RunManagr::BeamOn()
      fSimRunMgr->SetNumberOfEventsToBeProcessed(gCore->GetMaxEventNumber());
      fSimRunMgr->ConstructScoringWorlds();
      fSimRunMgr->RunInitialization();
      fSimRunMgr->InitializeEventLoop(gCore->GetMaxEventNumber());
    }else{
      DmpLogError<<"G4RunManager::Initialize() failed"<<DmpLogEndl;
      return false;
    }
  }else{    // visualization mode
    G4UImanager *uiMgr = G4UImanager::GetUIpointer();
#ifdef G4UI_USE_QT
    char *dummyargv[20]={"visual"};
    G4UIExecutive *ui = new G4UIExecutive(1,dummyargv);
#ifdef G4VIS_USE_OPENGLQT
    G4VisExecutive *vis = new G4VisExecutive();
    vis->Initialize();
// *
// *  TODO: publish... check prefix
// *
    G4String prefix = (G4String)getenv("DMPSWWORK")+"/share/Simulation/";
    uiMgr->ApplyCommand("/control/execute "+prefix+"DmpSimVis.mac");
#endif
    if (ui->IsGUI()){
      uiMgr->ApplyCommand("/control/execute "+prefix+"DmpSimGUI.mac");
    }
    ui->SessionStart();
    delete ui;
#ifdef G4VIS_USE_OPENGLQT
    delete vis;
#endif
    gRootIOSvc->FillData("Event");
    gCore->TerminateRun();  // just for check GDML, or run one event while debuging
#endif
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpSimAlg::ProcessThisEvent(){
  fTracking->ResetTrackingData();
  if(fSimRunMgr->SimOneEvent(gCore->GetCurrentEventID())){
    return true;
  }
  return false;
}

//-------------------------------------------------------------------
bool DmpSimAlg::Finalize(){
  if(gRootIOSvc->JobOption()->GetValue("Mode") == "batch"){
    fSimRunMgr->TerminateEventLoop();
    fSimRunMgr->RunTermination();
  }
  if(fTracking){
    delete fTracking;
  }
  if(fDetector){
    delete fDetector;
  }
  if(fSource){
    delete fSource;
  }
  if(fPhyFactory){
    delete fPhyFactory;
  }
  return true;
}

