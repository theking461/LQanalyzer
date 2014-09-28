// $Id: AnalyzerCore.cc 1 2013-11-26 10:23:10Z jalmond $
/***************************************************************************
 * @Project: LQAnalyzer Frame - ROOT-based analysis framework for Korea SNU
 * @Package: LQCycles
 *
 * @author John Almond       <jalmond@cern.ch>           - SNU
 *
 ***************************************************************************/

/// Local includes 
#include "AnalyzerCore.h"
#include "EventBase.h"

//Plotting                                                      
#include "MuonPlots.h"
#include "ElectronPlots.h"
#include "JetPlots.h"
#include "SignalPlots.h"


//ROOT includes
#include <TFile.h>


AnalyzerCore::AnalyzerCore() : LQCycleBase(), MCweight(-999.) {

  TH1::SetDefaultSumw2(true);  
  /// clear list of triggers stored in KTrigger
  triggerlist.clear();
  // If running on LQNtuples this is not important.
  // If creating an SKTree ntuple this controls what triggers are accessible
  
  // To have the correct name in the log:                                                                                                                            
  SetLogName("AnalyzerCore");

  Message("In AnalyzerCore constructor", INFO);
  
  TDirectory* origDir = gDirectory;
  /////////////////////////////////////////////////////////////////////// 
  //////// For HN analysis  /////////////////////////////////////////////  
  //////////////////////////////////////////////////////////////////////  
  //// FakeRate Input file           
  //////////////////////////////////////////////////////////////////////                                                                                                   
  string analysisdir = getenv("FILEDIR");

  TFile *infile_sf = TFile::Open((analysisdir+ "HMN_FinalSFNoJets.root").c_str());
  TDirectory* tempDir = getTemporaryDirectory();
  tempDir->cd();
 
  MuonSF =  dynamic_cast<TH2F*> (( infile_sf->Get("etavspt"))->Clone());
  infile_sf->Close();
  delete infile_sf;
  origDir->cd();
  
  string lqdir = getenv("LQANALYZER_DIR");

  m_fakeobj = new HNCommonLeptonFakes(lqdir+"/HNCommonLeptonFakes/share/");
  rmcor = new rochcor2012();

}

float AnalyzerCore::GetEEMass(std::vector<snu::KElectron> electrons){

  if(electrons.size() != 2) return 0.;
  snu::KParticle p = electrons.at(0) + electrons.at(1);
  return p.M();
}

void AnalyzerCore::FakeBkgBreakDown(std::vector<snu::KElectron> electrons, TString cut, float w){

  if(electrons.size() != 2) return;
  if(IsTight(electrons.at(0),  eventbase->GetEvent().JetRho()) &&  IsTight(electrons.at(1),  eventbase->GetEvent().JetRho())){
    //FillHist(("FakeBkg/" + cut  + "_TT").Data(), electrons.at(0).Pt() , 1.,  0. , 100., 20);
    //FillHist(("FakeBkg/" + cut  + "_TT_w").Data(),electrons.at(0).Pt() , w,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_TT").Data(), electrons.at(1).Pt() , 1.,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_TT_w").Data(),electrons.at(1).Pt() , w,  0. , 100., 20);
  }
  if(IsTight(electrons.at(0),  eventbase->GetEvent().JetRho()) &&  !IsTight(electrons.at(1),eventbase->GetEvent().JetRho())){
    //FillHist(("FakeBkg/" + cut  + "_TL").Data(), electrons.at(0).Pt() ,1.,  0. , 100., 20);
    //FillHist(("FakeBkg/" + cut  + "_TL_w").Data(), electrons.at(0).Pt() , w,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_TL").Data(), electrons.at(1).Pt() ,1.,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_TL_w").Data(), electrons.at(1).Pt() , w,  0. , 100., 20);
  }
  if(!IsTight(electrons.at(0),  eventbase->GetEvent().JetRho()) &&  IsTight(electrons.at(1),   eventbase->GetEvent().JetRho())){
    //FillHist(("FakeBkg/" + cut  + "_LT").Data(), electrons.at(0).Pt() ,1.,  0. , 100., 20);
    //FillHist(("FakeBkg/" + cut  + "_LT_w").Data(), electrons.at(0).Pt() , w,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_LT").Data(), electrons.at(1).Pt() ,1.,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_LT_w").Data(), electrons.at(1).Pt() , w,  0. , 100., 20);
  }
  if(!IsTight(electrons.at(0),  eventbase->GetEvent().JetRho()) &&  !IsTight(electrons.at(1),  eventbase->GetEvent().JetRho())){
    //FillHist(("FakeBkg/" + cut  + "_LL").Data(),   electrons.at(0).Pt() ,1.,  0. , 100., 20);
    //FillHist(("FakeBkg/" + cut  + "_LL_w").Data(),electrons.at(0).Pt() , w,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_LL").Data(),   electrons.at(1).Pt() ,1.,  0. , 100., 20);
    FillHist(("FakeBkg/" + cut  + "_LL_w").Data(),electrons.at(1).Pt() , w,  0. , 100., 20);
  }

}


std::vector<snu::KJet> AnalyzerCore::GetJets(TString label){
  
  std::vector<snu::KJet> jetColl;
  if(label.Contains("NoLeptonVeto")){
    eventbase->GetJetSel()->SetID(BaseSelection::PFJET_LOOSE);
    eventbase->GetJetSel()->SetPt(20.);
    eventbase->GetJetSel()->SetEta(2.5);
    eventbase->GetJetSel()->Selection(jetColl);
  }
  else  if(label.Contains("ApplyLeptonVeto")){
    eventbase->GetJetSel()->SetID(BaseSelection::PFJET_LOOSE);
    eventbase->GetJetSel()->SetPt(20.);
    eventbase->GetJetSel()->SetEta(2.5);
    eventbase->GetJetSel()->JetSelectionLeptonVeto(jetColl, GetMuons("veto"), GetElectrons(false,false, "loose"));
  }
  else if(label.Contains("ApplyPileUpID")){
    eventbase->GetJetSel()->JetHNSelection(jetColl,GetMuons("veto"), GetElectrons(false, false, "loose"));
    
  }
  
  return jetColl;
  
}


std::vector<snu::KMuon> AnalyzerCore::GetMuons(TString label){

  std::vector<snu::KMuon> muonColl;
  if(label.Contains("veto")){
    eventbase->GetMuonSel()->HNVetoMuonSelection(muonColl);
  }
  else if(label.Contains("tight")){
    eventbase->GetMuonSel()->HNTightMuonSelection(muonColl);
  }
  else if(label.Contains("NoCut")){
    eventbase->GetMuonSel()->Selection(muonColl);
  }
  
  return muonColl;
  
}

std::vector<snu::KElectron> AnalyzerCore::GetElectrons(bool keepcf, bool keepfake, TString label){
  
  double dummy=1.;
  return GetElectrons(keepcf, keepfake, label, dummy);
}

std::vector<snu::KElectron> AnalyzerCore::GetElectrons(bool keepcf, bool keepfake, TString label, double& w){
  
  bool applyidsf= false;
  std::vector<snu::KElectron> electronColl;

  float dxy= 0.01; float biso= 0.09;    float eiso= 0.05;  bool usetight_id= true;

  if(label.Contains("Analysis_loosereg2")){
    applyidsf= true;
    /// This is the vector of electrons with optimie cuts
    std::vector<snu::KElectron> _electronColl;
    if(k_running_nonprompt) eventbase->GetElectronSel()->HNLooseElectronSelection(_electronColl);
    else eventbase->GetElectronSel()->HNTightElectronSelection(_electronColl, false);
    
    electronColl =ShiftElectronEnergy(_electronColl, k_running_chargeflip);
  }
  
  else if(label.Contains("Analysis_loosereg1")){
    applyidsf= true;
    /// This is the vector of electrons with optimie cuts
    std::vector<snu::KElectron> _electronColl;
    if(k_running_nonprompt) eventbase->GetElectronSel()->HNLooseElectronSelectionWithIPCut(_electronColl);
    else eventbase->GetElectronSel()->HNTightElectronSelection(_electronColl, false);

    electronColl =ShiftElectronEnergy(_electronColl, k_running_chargeflip);
  }

  else if(label.Contains("iso")){
    std::vector<snu::KElectron> electronLooseColl = GetElectrons(keepcf, keepfake, "loose");
    if(label.Contains("iso_b10_e10")){biso = 0.1;    eiso = 0.1;     dxy= 0.01;    usetight_id = true;}
    else if(label.Contains("iso_b09_e09_medium")){biso = 0.09;   eiso = 0.09;    dxy= 0.01;  usetight_id = false;}
    else if(label.Contains("iso_b09_e09")){biso = 0.09;   eiso = 0.09;    dxy= 0.01;  usetight_id = true;}
    else if(label.Contains("iso_b09_e05")){biso = 0.09;   eiso = 0.05;    dxy= 0.01;  usetight_id = true;}
    applyidsf= true;
    
    for(unsigned int ilooseel=0; ilooseel< electronLooseColl.size(); ilooseel++){
      if(!k_running_nonprompt){
	if(eventbase->GetElectronSel()->HNIsTight(electronLooseColl.at(ilooseel), eventbase->GetEvent().JetRho(), dxy, biso, eiso, usetight_id)) electronColl.push_back(electronLooseColl.at(ilooseel));
      }
      else{
	electronColl.push_back(electronLooseColl.at(ilooseel));
      }
    }
  }

  else if(label.Contains("loose"))    eventbase->GetElectronSel()->HNLooseElectronSelectionWithIPCut(electronColl);
  else if(label.Contains("veto"))     eventbase->GetElectronSel()->HNVetoElectronSelection(electronColl);
  
  else if(label.Contains("id")){
    applyidsf= true;
    if(label.Contains("medium")){biso = 0.10;   eiso = 0.10;    dxy= 0.02;      usetight_id = false;}
    if(label.Contains("tight")) {biso = 0.10;   eiso = 0.10;    dxy= 0.02;     usetight_id = true;}
    if(label.Contains("susy")) {biso = 0.09;    eiso = 0.09;    dxy= 0.01;     usetight_id = false;}
    
    std::vector<snu::KElectron> electronLooseColl;    
    eventbase->GetElectronSel()->HNLooseElectronSelection(usetight_id, false, 0.6,electronLooseColl);
    for(unsigned int ilooseel=0; ilooseel< electronLooseColl.size(); ilooseel++){
      if(!k_running_nonprompt){
	if(eventbase->GetElectronSel()->HNIsTight(electronLooseColl.at(ilooseel), eventbase->GetEvent().JetRho(), dxy, biso,eiso,  usetight_id)) electronColl.push_back(electronLooseColl.at(ilooseel));
      }
      else{
	if(eventbase->GetElectronSel()->HNIsTight(electronLooseColl.at(ilooseel), eventbase->GetEvent().JetRho(), dxy, 0.6,0.6, usetight_id)) electronColl.push_back(electronLooseColl.at(ilooseel));
      }
    }
  }
  
  else if(label.Contains("NoCutPtEta")){ 
    eventbase->GetElectronSel()->SetPt(20.);
    eventbase->GetElectronSel()->SetEta(2.5);
    eventbase->GetElectronSel()->Selection(electronColl);
  }
  else if(label.Contains("NoCut")){ eventbase->GetElectronSel()->Selection(electronColl);}
  
  if(!isData){
    if(applyidsf){
      for(std::vector<snu::KElectron>::iterator it = electronColl.begin(); it != electronColl.end(); it++){
	w*=  ElectronScaleFactor(it->Eta(), it->Pt(), usetight_id);
      }
    }
  }

  return  GetTruePrompt(electronColl, keepcf, keepfake); 
  
}

void AnalyzerCore::MakeTriLeptonPlots(std::vector<snu::KElectron> electrons, std::vector<snu::KMuon> muons, std::vector<snu::KJet> jets, TString jetid, float w) {

  if(electrons.size() ==  3 && (muons.size()== 0)) {
    
    float charge1 = electrons.at(0).Charge();
    float charge2 = electrons.at(1).Charge();
    float charge3 = electrons.at(2).Charge();

    /// Find os pair
    snu::KParticle osee;
    float osmassdiff (0.);
    if(charge1 != charge2) {
      osee= electrons.at(0) + electrons.at(1);
      osmassdiff = fabs(osee.M() - 90.);
    }
    if(charge1 != charge3) {
      osee= electrons.at(0) + electrons.at(2);
      if(fabs(osee.M() - 90.) < osmassdiff) osmassdiff = fabs(osee.M() - 90.);
    }
    if(charge2 != charge3) {
      osee= electrons.at(1) + electrons.at(2);
      if(fabs(osee.M() - 90.) <osmassdiff) osmassdiff = fabs(osee.M() - 90.);
    }

    if(jets.size() > 1){
      if(eventbase->GetEvent().PFMET() > 30){
	if(osmassdiff < 10.){
	  FillCLHist(sighist, "TriElCR_dijet_highmet_osZW" + jetid, eventbase->GetEvent(), muons,electrons,jets, w);
	}
      }
    }

    if(osmassdiff <10.){
      if(jets.size() > 0){
	FillCLHist(sighist, "TriElCR_jet_osZW" + jetid, eventbase->GetEvent(), muons,electrons,jets, w);
      }
    }

    FillCLHist(sighist, "TriElCR" + jetid, eventbase->GetEvent(), muons,electrons,jets, w);
    if(jets.size() > 0)
      FillCLHist(sighist, "TriElCR1Jet" + jetid, eventbase->GetEvent(), muons,electrons,jets, w);
    
    if(jets.size() > 1){
      if(eventbase->GetEvent().PFMET() > 30){
	FillCLHist(sighist, "TriElCR_dijet_highmet" + jetid, eventbase->GetEvent(), muons,electrons,jets, w);
      }
    }
  }

  if(electrons.size() ==  4 && (muons.size()== 0) && jets.size() > 1 )
    FillCLHist(sighist, "ZZ" + jetid, eventbase->GetEvent(), muons, electrons,jets, w);
  
  
}


void AnalyzerCore::RunMCCLosureTest( std::vector<snu::KJet> jets, float w){
  
  if(jets.size() <  2 ) return;
  if(!isData){
    if(k_running_nonprompt){
      
      TString looseregion = "loosereg1";
      std::vector<snu::KElectron> electronAnalysisColl_mcclosure =  GetElectrons(false, true, "Analysis_" + looseregion); // removes CF and fake in mc
      std::vector<snu::KElectron> electronAnalysisColl_loosereg2_mcclosure =  GetElectrons(false, true, "Analysis_loosereg2"); // removes CF and fake in mc

      if(SameCharge(electronAnalysisColl_loosereg2_mcclosure)){
	FakeBkgBreakDown(electronAnalysisColl_loosereg2_mcclosure, "mcclosure,loosereg2",1.);
	//float fake_weight40 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_loosereg2_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05, "mc_40_loosereg2", 0);
	float fake_weight40 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_loosereg2_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05, "mc_40_loosereg2", 0);
	float fake_weight60 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_loosereg2_mcclosure,  eventbase->GetEvent().JetRho(), true,0.01,0.09,0.05,"mc_60_loosereg2", 0);
	
	if(IsTight(electronAnalysisColl_loosereg2_mcclosure.at(0),   eventbase->GetEvent().JetRho()) && IsTight(electronAnalysisColl_loosereg2_mcclosure.at(1),   eventbase->GetEvent().JetRho())  ){
	  FillHist(("MCSSclosure_loosereg2_fake_pt_measured"), electronAnalysisColl_loosereg2_mcclosure.at(1).Pt() ,w, 0. , 200., 10);
	  
	}
	FillHist(("MCSSclosure_loosereg2_fake_pt_predicted40"),  electronAnalysisColl_loosereg2_mcclosure.at(1).Pt(), fake_weight40, 0. , 200., 10);
	FillHist(("MCSSclosure_loosereg2_fake_pt_predicted60"),  electronAnalysisColl_loosereg2_mcclosure.at(1).Pt(), fake_weight60, 0. , 200., 10);
	
	if(IsTight(electronAnalysisColl_loosereg2_mcclosure.at(0),   eventbase->GetEvent().JetRho()) && !IsTight(electronAnalysisColl_loosereg2_mcclosure.at(1),   eventbase->GetEvent().JetRho())   ){
	  FillHist(("mcclosure_Doubletosingle_loosereg2"), 1., w, 0. , 2., 2);
	}
	if(!IsTight(electronAnalysisColl_loosereg2_mcclosure.at(0),   eventbase->GetEvent().JetRho()) && IsTight(electronAnalysisColl_loosereg2_mcclosure.at(1),   eventbase->GetEvent().JetRho())	){
	  FillHist(("mcclosure_Doubletosingle_loosereg2"), 1., w, 0. , 2., 2);
	}
	if(!IsTight(electronAnalysisColl_loosereg2_mcclosure.at(0),   eventbase->GetEvent().JetRho()) &&!IsTight(electronAnalysisColl_loosereg2_mcclosure.at(1),   eventbase->GetEvent().JetRho())   ){
	  //if(true){
	  float f1 = m_fakeobj->getFakeRate_electronEta(0, electronAnalysisColl_loosereg2_mcclosure.at(0).Pt(), fabs(electronAnalysisColl_loosereg2_mcclosure.at(0).Eta()), "mc_40_loosereg2");
	  float f2 = m_fakeobj->getFakeRate_electronEta(0, electronAnalysisColl_loosereg2_mcclosure.at(1).Pt(), fabs(electronAnalysisColl_loosereg2_mcclosure.at(1).Eta()), "mc_40_loosereg2");
	  float sd = f1/(1.-f1) + f2/(1.-f2);
	  
	  FillHist(("mcclosure_single_loosereg2"), 1., sd*w, 0. , 2., 2);
	}
      }
      if(electronAnalysisColl_mcclosure.size() == 2){
	float mcclosure_weight = w;
	if(SameCharge(electronAnalysisColl_mcclosure)){
	  //  if(true){
	  if(electronAnalysisColl_mcclosure.at(1).Pt() > 15. && jets.size() > 0){
	    
	    /// weights using pt eta binning only
	    float fake_weight20 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09  ,0.05,  "mc_20_"+looseregion, 0);
	    float fake_weight30 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05, "mc_30_"+looseregion, 0);
	    float fake_weight40 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05, "mc_40_"+looseregion, 0);
	    float fake_weight60 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true,0.01,0.09,0.05,"mc_60_"+looseregion, 0);
	    float fake_weight80 = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true,0.01,0.09,0.05,"mc_80_"+looseregion, 0);
	    
	    
	    bool closejet_el1=false;
	    bool closejet_el2=false;
	    bool awayjet_el1=false;
	    bool awayjet_el2=false;

	    std::vector<snu::KJet> alljets = GetJets("NoLeptonVeto");
	    for(int ij =0; ij < alljets.size(); ij++){
	      if(electronAnalysisColl_mcclosure.at(0).DeltaR(alljets.at(ij)) < 0.4){
		if(alljets.at(ij).CombinedSecVertexBtag() > 0.244) closejet_el1 = true;
	      }
	      if(electronAnalysisColl_mcclosure.at(1).DeltaR(alljets.at(ij)) < 0.4){
		if(alljets.at(ij).CombinedSecVertexBtag() > 0.244) closejet_el2 = true;
              }
	      float dphi_el1 =fabs(TVector2::Phi_mpi_pi(electronAnalysisColl_mcclosure.at(0).Phi() - alljets.at(ij).Phi()));
	      float dphi_el2 =fabs(TVector2::Phi_mpi_pi(electronAnalysisColl_mcclosure.at(1).Phi() - alljets.at(ij).Phi()));
	      if(dphi_el1 > 2.5){
		if(alljets.at(ij).CombinedSecVertexBtag() > 0.244) awayjet_el1 = true;
              }
	      if(dphi_el2 > 2.5){
		if(alljets.at(ij).CombinedSecVertexBtag() > 0.244) awayjet_el2 = true;
              }
	    }
	    


	    //// weights for eta/pt but for 4 different ht bins
            TString htlabel = "";

            if(SumPt(jets) < 100.)        htlabel = "_ht1_loosereg1";
            else if(SumPt(jets) < 150.)   htlabel = "_ht2_loosereg1";
            else if(SumPt(jets) < 200.)   htlabel = "_ht3_loosereg1";
            else if(SumPt(jets) < 10000.) htlabel = "_ht4_loosereg1";


	    TString label = "";
            if(closejet_el1) label+= "cl1_";
            else if(closejet_el2) label+= "cl2_";
            else if(awayjet_el1)  label+= "aw1_";
            else if(awayjet_el2)  label+= "aw2_";
	    
	    label += htlabel;
	    // wights using pt eta. But having separate maps for el with closeby bjet
	    float fake_weight20_awaybjet = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05,  "mc_20_"+label, 0);
            float fake_weight30_awaybjet = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05, "mc_20_"+label, 0);
            float fake_weight40_awaybjet = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true, 0.01,0.09,0.05, "mc_40_"+label, 0);
            float fake_weight60_awaybjet = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true,0.01,0.09, 0.05,"mc_60_"+label, 0);
            float fake_weight80_awaybjet = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(), true,0.01,0.09, 0.05,"mc_60_"+label, 0);

	    //// weights for eta/pt but for 4 different ht bins

	    
            float fake_weight20_ht = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(),true,0.01,0.09, 0.05,"mc_20"+htlabel, 0);
            float fake_weight40_ht = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(),true,0.01,0.09, 0.05,"mc_40"+htlabel, 0);
            float fake_weight60_ht = w* Get_DataDrivenWeight_EE(electronAnalysisColl_mcclosure,  eventbase->GetEvent().JetRho(),true,0.01,0.09, 0.05,"mc_60"+htlabel, 0);

	    
	    cout << "fake_weight40_ht = " << fake_weight40_ht  << "  : normal weight = " << fake_weight40 << endl;
	    
	    bool el1_fake=false;
	    bool el2_fake=false;
	    if(electronAnalysisColl_mcclosure.at(0).GetType() == 1 || electronAnalysisColl_mcclosure.at(0).GetType() == 2 || electronAnalysisColl_mcclosure.at(0).GetType() == 3 || electronAnalysisColl_mcclosure.at(0).GetType() == 6 || electronAnalysisColl_mcclosure.at(0).GetType() ==8 ){
	     
	      if(IsTight(electronAnalysisColl_mcclosure.at(0), eventbase->GetEvent().JetRho())) FillHist("mcclosure_el1_fake_checktight", 1, 1., 0.,2.,2);
              else FillHist("mcclosure_el1_fake_checktight", 0, 1.,0.,2.,2);
	      
	      el1_fake= true;
	      if(closejet_el1)
		FillHist("mcclosure_cbj_el1", 1 , 1.,  0. , 2, 2);
	      else
		FillHist("mcclosure_cbj_el1", 0 , 1.,  0. , 2, 2);
	      if(awayjet_el1)
                FillHist("mcclosure_abj_el1", 1 , 1.,  0. , 2, 2);
	      else
		FillHist("mcclosure_abj_el1", 0 , 1.,  0. , 2, 2);

	    }	    
	    if(electronAnalysisColl_mcclosure.at(1).GetType() == 1 || electronAnalysisColl_mcclosure.at(1).GetType() == 2 || electronAnalysisColl_mcclosure.at(1).GetType() == 3 || electronAnalysisColl_mcclosure.at(1).GetType() == 6 || electronAnalysisColl_mcclosure.at(0).GetType() ==8){
	      el2_fake= true;
	      if(IsTight(electronAnalysisColl_mcclosure.at(1), eventbase->GetEvent().JetRho())) FillHist("mcclosure_el2_fake_checktight", 1, 1.,0.,2.,2);
              else FillHist("mcclosure_el2_fake_checktight", 0, 1.,0.,2.,2);

	      if(closejet_el2)
		FillHist("mcclosure_cbj_el2", 1 , 1.,  0. , 2, 2);
	      else
		FillHist("mcclosure_cbj_el2", 0 , 1.,  0. , 2, 2);
	      if(awayjet_el2)
		FillHist("mcclosure_abj_el2", 1 , 1.,  0. , 2, 2);
	      else
		FillHist("mcclosure_abj_el2", 0 , 1.,  0. , 2, 2);
	    }
	    
	    if(el1_fake &&el2_fake)                 FillHist("mcclosure_fakeevent_type", 0 , 1.,  0. , 4, 4);
	    if(!el1_fake &&el2_fake)                FillHist("mcclosure_fakeevent_type", 1 , 1.,  0. , 4, 4);
	    if(el1_fake &&!el2_fake)                FillHist("mcclosure_fakeevent_type", 2 , 1.,  0. , 4, 4);
	    if(!el1_fake &&!el2_fake)                FillHist("mcclosure_fakeevent_type", 3 , 1.,  0. ,4, 4);

	    if(!el1_fake &&!el2_fake)   FillHist("mcclosure_realevent_type", electronAnalysisColl_mcclosure.at(0).GetType() , 1., 0., 8., 8);	    
	    if(!el1_fake &&!el2_fake)   FillHist("mcclosure_realevent_type", electronAnalysisColl_mcclosure.at(1).GetType() , 1., 0., 8., 8);	    
	    
	    FakeBkgBreakDown(electronAnalysisColl_mcclosure, "mcclosure",fake_weight60); 
	    
	    if(IsTight(electronAnalysisColl_mcclosure.at(0),   eventbase->GetEvent().JetRho()) && IsTight(electronAnalysisColl_mcclosure.at(1),   eventbase->GetEvent().JetRho()) ){
	      
	      FillHist(("MCSSclosure_fake_pt_measured"), electronAnalysisColl_mcclosure.at(1).Pt(), mcclosure_weight, 0. , 200., 10);
	      FillHist(("MCSSclosure_fake_njet_measured"), jets.size(), mcclosure_weight, 0. , 10., 10);
	      FillHist(("MCSSclosure_fake_nbjet_measured"), NBJet(jets), mcclosure_weight, 0. , 4., 4);
	      FillHist(("MCSSclosure_fake_ht_measured"), SumPt(jets), mcclosure_weight, 0. , 400., 40);
	      FillHist(("MCSSclosure_fake_eta_measured"), electronAnalysisColl_mcclosure.at(1).Eta(), mcclosure_weight, -2.5 , 2.5, 50);
	      FillHist(("MCSSclosure_fake_eta_measured"), electronAnalysisColl_mcclosure.at(0).Eta(), mcclosure_weight, -2.5 , 2.5, 50);
	      
	      
	      if(el1_fake &&el2_fake)                 FillHist("mcclosure_tt_fakeevent_type", 0 , 1.,  0. , 3, 3);
	      if(!el1_fake &&el2_fake)                FillHist("mcclosure_tt_fakeevent_type", 1 , 1.,  0. , 3, 3);
	      if(el1_fake &&!el2_fake)                FillHist("mcclosure_tt_fakeevent_type", 2 , 1.,  0. , 3, 3);
	      if(!el1_fake &&!el2_fake)                FillHist("mcclosure_tt_fakeevent_type", 3 , 1.,  0. , 4, 4);
	      
	    }
	    
	    if(IsTight(electronAnalysisColl_mcclosure.at(0),   eventbase->GetEvent().JetRho()) && !IsTight(electronAnalysisColl_mcclosure.at(1),   eventbase->GetEvent().JetRho()) ){
              FillHist(("mcclosure_Doubletosingle"), 1., mcclosure_weight, 0. , 2., 2);
	      
	      if(el1_fake &&el2_fake)                 FillHist("mcclosure_tl_fakeevent_type", 0 , 1.,  0. , 3, 3);
	      if(!el1_fake &&el2_fake)                FillHist("mcclosure_tl_fakeevent_type", 1 , 1.,  0. , 3, 3);
	      if(el1_fake &&!el2_fake)                FillHist("mcclosure_tl_fakeevent_type", 2 , 1.,  0. , 3, 3);

	    }
	    if(!IsTight(electronAnalysisColl_mcclosure.at(0),   eventbase->GetEvent().JetRho()) && IsTight(electronAnalysisColl_mcclosure.at(1),   eventbase->GetEvent().JetRho()) ){
              FillHist(("mcclosure_Doubletosingle"), 1., mcclosure_weight, 0. , 2., 2);
	      if(el1_fake &&el2_fake)                 FillHist("mcclosure_tl_fakeevent_type", 0 , 1.,  0. , 3, 3);
              if(!el1_fake &&el2_fake)                FillHist("mcclosure_tl_fakeevent_type", 1 , 1.,  0. , 3, 3);
              if(el1_fake &&!el2_fake)                FillHist("mcclosure_tl_fakeevent_type", 2 , 1.,  0. , 3, 3);

            }
	    if(!IsTight(electronAnalysisColl_mcclosure.at(0),   eventbase->GetEvent().JetRho()) &&!IsTight(electronAnalysisColl_mcclosure.at(1),   eventbase->GetEvent().JetRho()) ){
	      //if(true){
	      float f1 = m_fakeobj->getFakeRate_electronEta(0, electronAnalysisColl_mcclosure.at(0).Pt(), fabs(electronAnalysisColl_mcclosure.at(0).Eta()), "mc_20_loosereg1");
	      float f2 = m_fakeobj->getFakeRate_electronEta(1, electronAnalysisColl_mcclosure.at(1).Pt(), fabs(electronAnalysisColl_mcclosure.at(1).Eta()), "mc_20_loosereg1");
	      float sd = f1/(1.-f1) + f2/(1.-f2);
	      
	      FillHist(("mcclosure_single"), 1., sd*mcclosure_weight, 0. , 2., 2);
	      
	      if(el1_fake &&el2_fake)                 FillHist("mcclosure_ll_fakeevent_type", 0 , 1.,  0. , 3, 3);
              if(!el1_fake &&el2_fake)                FillHist("mcclosure_ll_fakeevent_type", 1 , 1.,  0. , 3, 3);
              if(el1_fake &&!el2_fake)                FillHist("mcclosure_ll_fakeevent_type", 2 , 1.,  0. , 3, 3);

	    }
	    
	    FillHist(("MCSSclosure_fake_pt_predicted20"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight20, 0. , 200., 20);
	    FillHist(("MCSSclosure_fake_pt_predicted30"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight30, 0. , 200., 20);
	    FillHist(("MCSSclosure_fake_pt_predicted40"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight40, 0. , 200., 20);
	    FillHist(("MCSSclosure_fake_pt_predicted60"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight60, 0. , 200., 20);
	    FillHist(("MCSSclosure_fake_pt_predicted80"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight80, 0. , 200., 20);

	    FillHist(("MCSSclosure_fake_eta_predicted20"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight20, -2.5 , 2.5, 50);
	    FillHist(("MCSSclosure_fake_eta_predicted20"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight20, -2.5 , 2.5, 50);
	    FillHist(("MCSSclosure_fake_eta_predicted40"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight40, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_predicted40"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight40, -2.5 , 2.5, 50);
	    FillHist(("MCSSclosure_fake_eta_predicted60"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight60, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_predicted60"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight60, -2.5 , 2.5, 50);

	    
	    /// ht prediction
	    FillHist(("MCSSclosure_fake_pt_ht_predicted20"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight20_ht, 0. , 200., 20);
	    FillHist(("MCSSclosure_fake_pt_ht_predicted40"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight40_ht, 0. , 200., 20);
	    FillHist(("MCSSclosure_fake_pt_ht_predicted60"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight60_ht, 0. , 200., 20);

	    FillHist(("MCSSclosure_fake_ht_ht_predicted20"), SumPt(jets), fake_weight20_ht, 0. , 400., 40);
	    FillHist(("MCSSclosure_fake_ht_ht_predicted40"), SumPt(jets), fake_weight40_ht, 0. , 400., 40);
	    FillHist(("MCSSclosure_fake_ht_ht_predicted60"), SumPt(jets), fake_weight60_ht, 0. , 400., 40);
	    
	    FillHist(("MCSSclosure_fake_eta_ht_predicted20"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight20_ht, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_ht_predicted20"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight20_ht, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_ht_predicted40"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight40_ht, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_ht_predicted40"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight40_ht, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_ht_predicted60"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight60_ht, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_ht_predicted60"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight60_ht, -2.5 , 2.5, 50);



	    FillHist(("MCSSclosure_fake_njet_ht_predicted20"), jets.size(), fake_weight20_ht, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_njet_ht_predicted40"), jets.size(), fake_weight40_ht, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_njet_ht_predicted60"), jets.size(), fake_weight60_ht, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_nbjet_ht_predicted20"),  NBJet(jets),fake_weight20_ht, 0. , 4., 4);
	    FillHist(("MCSSclosure_fake_nbjet_ht_predicted40"),  NBJet(jets),fake_weight40_ht,  0. , 4., 4);
	    FillHist(("MCSSclosure_fake_nbjet_ht_predicted60"),  NBJet(jets),fake_weight60_ht, 0. , 4., 4);

	    /// close bjet prediction
	    FillHist(("MCSSclosure_fake_njet_awayjet_predicted20"), jets.size(), fake_weight20_awaybjet, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_njet_awayjet_predicted30"), jets.size(), fake_weight30_awaybjet, 0. , 10., 10);
            FillHist(("MCSSclosure_fake_njet_awayjet_predicted40"), jets.size(), fake_weight40_awaybjet, 0. , 10., 10);
            FillHist(("MCSSclosure_fake_njet_awayjet_predicted60"), jets.size() , fake_weight60_awaybjet, 0. ,10., 10);
            FillHist(("MCSSclosure_fake_njet_awayjet_predicted80"), jets.size() , fake_weight80_awaybjet, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_nbjet_awayjet_predicted20"),  NBJet(jets), fake_weight20_awaybjet, 0. , 4., 4);
            FillHist(("MCSSclosure_fake_nbjet_awayjet_predicted40"),  NBJet(jets), fake_weight40_awaybjet, 0. , 4., 4);
            FillHist(("MCSSclosure_fake_nbjet_awayjet_predicted60"),  NBJet(jets), fake_weight60_awaybjet, 0. , 4., 4);
            FillHist(("MCSSclosure_fake_nbjet_awayjet_predicted80"),  NBJet(jets), fake_weight80_awaybjet, 0. , 4., 4);

	    FillHist(("MCSSclosure_fake_ht_awayjet_predicted20"), SumPt(jets), fake_weight20_awaybjet, 0. , 400., 40);
	    FillHist(("MCSSclosure_fake_ht_awayjet_predicted40"), SumPt(jets), fake_weight40_awaybjet, 0. , 400., 40);
	    FillHist(("MCSSclosure_fake_ht_awayjet_predicted60"), SumPt(jets), fake_weight60_awaybjet, 0. , 400., 40);
	    FillHist(("MCSSclosure_fake_ht_awayjet_predicted80"), SumPt(jets), fake_weight80_awaybjet, 0. , 400., 40);

	    FillHist(("MCSSclosure_fake_eta_awayjet_predicted20"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight20_awaybjet, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_awayjet_predicted20"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight20_awaybjet, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_awayjet_predicted40"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight40_awaybjet, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_awayjet_predicted40"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight40_awaybjet, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_awayjet_predicted60"),  electronAnalysisColl_mcclosure.at(0).Eta(), fake_weight60_awaybjet, -2.5 , 2.5, 50);
            FillHist(("MCSSclosure_fake_eta_awayjet_predicted60"),  electronAnalysisColl_mcclosure.at(1).Eta(), fake_weight60_awaybjet, -2.5 , 2.5, 50);




	    
	    
	    FillHist(("MCSSclosure_fake_pt_predicted_now"),  electronAnalysisColl_mcclosure.at(1).Pt(), w, 0. , 100., 10);
	    if(NBJet(jets) > 0) {
	      FillHist(("MCSSclosure_fake_pt_bjet_predicted20"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight20, 0. , 100., 10);
	      FillHist(("MCSSclosure_fake_pt_bjet_predicted40"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight40, 0. , 100., 10);
	      FillHist(("MCSSclosure_fake_pt_bjet_predicted60"),  electronAnalysisColl_mcclosure.at(1).Pt(), fake_weight60, 0. , 100., 10);
	    }
	    FillHist(("MCSSclosure_fake_njet_predicted20"), jets.size(), fake_weight20, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_njet_predicted40"), jets.size(), fake_weight40, 0. , 10., 10);
	    FillHist(("MCSSclosure_fake_njet_predicted60"), jets.size() , fake_weight60, 0. , 10., 10); 
	    FillHist(("MCSSclosure_fake_njet_predicted80"), jets.size() , fake_weight80, 0. , 10., 10); 
	    FillHist(("MCSSclosure_fake_nbjet_predicted20"),  NBJet(jets), fake_weight20, 0. , 4., 4);
	    FillHist(("MCSSclosure_fake_nbjet_predicted40"),  NBJet(jets), fake_weight40, 0. , 4., 4);
	    FillHist(("MCSSclosure_fake_nbjet_predicted60"),  NBJet(jets), fake_weight60, 0. , 4., 4);
	    FillHist(("MCSSclosure_fake_nbjet_predicted80"),  NBJet(jets), fake_weight80, 0. , 4., 4);
	    FillHist(("MCSSclosure_fake_ht_predicted20"), SumPt(jets), fake_weight20, 0. , 400., 40);
            FillHist(("MCSSclosure_fake_ht_predicted40"), SumPt(jets), fake_weight40, 0. , 400., 40);
            FillHist(("MCSSclosure_fake_ht_predicted60"), SumPt(jets), fake_weight60, 0. , 400., 40);
            FillHist(("MCSSclosure_fake_ht_predicted80"), SumPt(jets), fake_weight80, 0. , 400., 40);


	  }
        }
      }
    }
  }

  return;
}
  
  
void AnalyzerCore::PlotFakeLeptons( std::vector<snu::KJet> jets, float w){

  std::vector<snu::KMuon>  muons = GetMuons("veto");
  std::vector<snu::KElectron>  electronNoCutColl = GetElectrons( true, true,"NoCut");
  std::vector<snu::KElectron>  fakeElectronSample;
  if(SameCharge(electronNoCutColl)){
    if(!(electronNoCutColl.at(0).GetType()== 0 || electronNoCutColl.at(0).GetType()== 7 )) {
      if(isData) {
        if(electronNoCutColl.at(0).RelIso03(eventbase->GetEvent().JetRho() ,electronNoCutColl.at(0).Pt()) > 0.5) fakeElectronSample.push_back(electronNoCutColl.at(0));
      }
      else fakeElectronSample.push_back(electronNoCutColl.at(0));

      if(electronNoCutColl.at(0).GetType()==6){
        FillHist("Wgamma_el_iso",electronNoCutColl.at(0).RelIso03(eventbase->GetEvent().JetRho() ,electronNoCutColl.at(0).Pt()), w  , 0., 0.6, 60);
        FillHist("Wgamma_el_dxy",fabs(electronNoCutColl.at(0).dxy()) , w  ,0., 0.1, 100);
      }
    }
    else {
      FillHist("W_el_iso",electronNoCutColl.at(0).RelIso03(eventbase->GetEvent().JetRho() ,electronNoCutColl.at(0).Pt())  ,w  , 0., 0.6, 60);
      FillHist("W_el_dxy",fabs(electronNoCutColl.at(0).dxy())  ,w  ,0., 0.1, 100);
    }

    if(!(electronNoCutColl.at(1).GetType()== 0 || electronNoCutColl.at(1).GetType()==7 )) {
      if(isData) {
        if(electronNoCutColl.at(1).RelIso03(eventbase->GetEvent().JetRho() ,electronNoCutColl.at(1).Pt()) > 0.5) fakeElectronSample.push_back(electronNoCutColl.at(1));
      }
      else fakeElectronSample.push_back(electronNoCutColl.at(1));
      if(electronNoCutColl.at(1).GetType()==6){
        FillHist("Wgamma_el_iso",electronNoCutColl.at(1).RelIso03(eventbase->GetEvent().JetRho() ,electronNoCutColl.at(1).Pt())  , w  ,0., 0.6, 60);
        FillHist("Wgamma_el_dxy",fabs(electronNoCutColl.at(1).dxy()) , w  ,0., 0.1, 100);
      }
    }
    else{
      FillHist("W_el_iso",electronNoCutColl.at(1).RelIso03(eventbase->GetEvent().JetRho() ,electronNoCutColl.at(1).Pt())  , w  ,0., 0.6, 60);
      
      FillHist("W_el_dxy",fabs(electronNoCutColl.at(1).dxy()) , w  ,0., 0.1, 100);
    }
  }
  FillCLHist(sighist, "Fake", eventbase->GetEvent(), muons, fakeElectronSample,jets, w);

}


void AnalyzerCore::GetIDEfficiency( std::vector<snu::KElectron> electronNoCutColl, std::vector<snu::KJet> jets){

  //// CHECK EFFICIENCY OF CUTS
  std::vector<snu::KElectron>  electronPtEtaCutColl;

  for(unsigned int iel = 0; iel < electronNoCutColl.size(); iel++){
    if(iel ==0){
      if(electronNoCutColl[iel].Pt() < 20.) continue;
    }
    else{
      if(electronNoCutColl[iel].Pt() < 15.) continue;
    }
    if(fabs(electronNoCutColl[iel].Eta()) > 2.4) continue;

    electronPtEtaCutColl.push_back(electronNoCutColl[iel]);
  }


  
  /// Tight ID eff
  std::vector<snu::KElectron>  electronTight_NoIsoCutColl;
  for(unsigned int iel = 0; iel < electronNoCutColl.size(); iel++){
    double egamma_e  = electronNoCutColl.at(iel).CaloEnergy();
    double egamma_p  = electronNoCutColl.at(iel).CaloEnergy() / electronNoCutColl.at(iel).ESuperClusterOverP();
    double egamma_ep = fabs ( ( 1.0 / egamma_e ) - ( 1.0 / egamma_p ) );
    if ( fabs(electronNoCutColl.at(iel).SCEta()) < 1.479 ){
      if(fabs(electronNoCutColl.at(iel).DeltaEta())   <= 0.004) {
        if(fabs(electronNoCutColl.at(iel).DeltaPhi())   <= 0.03){
          if(electronNoCutColl.at(iel).SigmaIEtaIEta()    <= 0.01){
            if(fabs(electronNoCutColl.at(iel).LeadVtxDistZ ())< 0.1){
              if(electronNoCutColl.at(iel).HoE            ()  <= 0.12){
                if( egamma_ep          <= 0.05){
                  if(electronNoCutColl[iel].ConvFitProb  ()    <=  1e-6){
                    if(electronNoCutColl[iel].MissingHits()    <=  0){
                      if(!electronNoCutColl.at(iel).HasMatchedConvPhot()){
                        if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
                          if(electronNoCutColl.at(iel).GsfCtfScPixChargeConsistency()){
                            electronTight_NoIsoCutColl.push_back(electronNoCutColl.at(iel));
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else {
      if(fabs(electronNoCutColl.at(iel).DeltaEta())   <= 0.005){
	if(fabs(electronNoCutColl.at(iel).DeltaPhi())   <= 0.02) {
	  if(electronNoCutColl.at(iel).SigmaIEtaIEta()    <= 0.03){
	    if(fabs(electronNoCutColl.at(iel).LeadVtxDistZ ())< 0.1){
	      if(electronNoCutColl.at(iel).HoE            ()  <= 0.10){
		if( egamma_ep          <=0.05){
		  if(electronNoCutColl[iel].ConvFitProb  ()    <=  1e-6){
		    if(electronNoCutColl[iel].MissingHits()    <=  0){
		      if(!electronNoCutColl.at(iel).HasMatchedConvPhot()){
			if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
			  if(electronNoCutColl.at(iel).GsfCtfScPixChargeConsistency()){
			    electronTight_NoIsoCutColl.push_back(electronNoCutColl.at(iel));
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  
  // oital efficiency + tight brekdownd
  std::vector<snu::KElectron>  electron_analysisCutColl;
  std::vector<snu::KElectron>  electron_analysisNPFCutColl;
  std::vector<snu::KElectron>   electronTight_DeltaEta;
  std::vector<snu::KElectron>   electronTight_DeltaPhi;
  std::vector<snu::KElectron>   electronTight_SigmaIEta;
  std::vector<snu::KElectron>   electronTight_DZ;
  std::vector<snu::KElectron>   electronTight_HoverE;
  std::vector<snu::KElectron>   electronTight_convfit;
  std::vector<snu::KElectron>   electronTight_ep;
  std::vector<snu::KElectron>   electronTight_missinghit;
  std::vector<snu::KElectron>   electronTight_convphoton;
  std::vector<snu::KElectron>   electronTight_DXY;
  std::vector<snu::KElectron>   electronTight_chargeconst;
  std::vector<snu::KElectron>   electronTight_tightref;
  for(unsigned int iel = 0; iel < electronNoCutColl.size(); iel++){
    double egamma_e  = electronNoCutColl.at(iel).CaloEnergy();
    double egamma_p  = electronNoCutColl.at(iel).CaloEnergy() / electronNoCutColl.at(iel).ESuperClusterOverP();
    double egamma_ep = fabs ( ( 1.0 / egamma_e ) - ( 1.0 / egamma_p ) );
    if(iel ==0){
      if(electronNoCutColl[iel].Pt() < 20.) continue;
    }
    else{
      if(electronNoCutColl[iel].Pt() < 15.) continue;
    }
    if(fabs(electronNoCutColl[iel].Eta()) > 2.4) continue;
    

    if ( fabs(electronNoCutColl.at(iel).SCEta()) < 1.479 ){
      if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
        electronTight_tightref.push_back(electronNoCutColl.at(iel));
        if(fabs(electronNoCutColl.at(iel).DeltaEta())   <= 0.004) {
          electronTight_DeltaEta.push_back(electronNoCutColl.at(iel));
        }
        if(fabs(electronNoCutColl.at(iel).DeltaPhi())   <= 0.03){
          electronTight_DeltaPhi.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl.at(iel).SigmaIEtaIEta()    <= 0.01){
          electronTight_SigmaIEta.push_back(electronNoCutColl.at(iel));
        }
        if(fabs(electronNoCutColl.at(iel).LeadVtxDistZ ())< 0.1){
          electronTight_DZ.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl.at(iel).HoE            ()  <= 0.12){
          electronTight_HoverE.push_back(electronNoCutColl.at(iel));
        }
        if( egamma_ep          <= 0.05){
          electronTight_ep.push_back(electronNoCutColl.at(iel));
	}
        if(electronNoCutColl[iel].ConvFitProb  ()    <=  1e-6){
          electronTight_convfit.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl[iel].MissingHits()    <=  0){
          electronTight_missinghit.push_back(electronNoCutColl.at(iel));
        }
        if(!electronNoCutColl.at(iel).HasMatchedConvPhot()){
          electronTight_convphoton.push_back(electronNoCutColl.at(iel));
        }

        if(electronNoCutColl.at(iel).GsfCtfScPixChargeConsistency()){
          electronTight_chargeconst.push_back(electronNoCutColl.at(iel));
        }
      }
      if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
        electronTight_DXY.push_back(electronNoCutColl.at(iel));
      }

      if(fabs(electronNoCutColl.at(iel).DeltaEta())   <= 0.004) {
        if(fabs(electronNoCutColl.at(iel).DeltaPhi())   <= 0.03){
          if(electronNoCutColl.at(iel).SigmaIEtaIEta()    <= 0.01){
            if(fabs(electronNoCutColl.at(iel).LeadVtxDistZ ())< 0.1){
              if(electronNoCutColl.at(iel).HoE            ()  <= 0.12){
		if( egamma_ep          <= 0.05){
                  if(electronNoCutColl[iel].ConvFitProb  ()    <=  1e-6){
                    if(electronNoCutColl[iel].MissingHits()    <=  0){
                      if(!electronNoCutColl.at(iel).HasMatchedConvPhot()){
                        if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
                          if(electronNoCutColl.at(iel).GsfCtfScPixChargeConsistency()){

                            Double_t PHONH_03[7]          = {0.13, 0.14, 0.07, 0.09, 0.11, 0.11, 0.14};
                            int ifid=0;
                            if (fabs(electronNoCutColl.at(iel).SCEta()) < 1.0) ifid = 0;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 1.479) ifid = 1;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.0) ifid = 2;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.2) ifid = 3;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.3) ifid = 4;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.4) ifid = 5;
                            else ifid = 6;

                            float LeptonRelIsoDR03(0.);
                            float ElectronIsoDR03 =  electronNoCutColl.at(iel).PFChargedHadronIso03() + max( electronNoCutColl.at(iel).PFNeutralHadronIso03() + electronNoCutColl.at(iel).PFPhotonIso03() - eventbase->GetEvent().JetRho() * PHONH_03[ifid],  0.);

                            if(electronNoCutColl.at(iel).Pt() > 0.)  LeptonRelIsoDR03 = ElectronIsoDR03/  electronNoCutColl.at(iel).Pt();
                            else LeptonRelIsoDR03 = -999.;
                            float trkiso =electronNoCutColl.at(iel).TrkIsoDR03()/electronNoCutColl.at(iel).Pt();
                            float ecaliso =electronNoCutColl.at(iel).ECalIsoDR03()/electronNoCutColl.at(iel).Pt();
                            float hcaliso =electronNoCutColl.at(iel).HCalIsoDR03()/electronNoCutColl.at(iel).Pt();
                            bool passNPFiso = true;
                            if(trkiso > 0.1) passNPFiso = false;
                            if(ecaliso > 0.25) passNPFiso = false;
                            if(hcaliso > 0.2)  passNPFiso = false;

                            float isocut= 0.09;
                            if(fabs(electronNoCutColl.at(iel).Eta() > 1.5) )isocut = 0.05;
                            else   if(electronNoCutColl.at(iel).Pt()< 20.) isocut=0.07;
                            if(LeptonRelIsoDR03 < isocut){
                              electron_analysisCutColl.push_back(electronNoCutColl.at(iel));
                              if(passNPFiso)       electron_analysisNPFCutColl.push_back(electronNoCutColl.at(iel));
                            }
                          }
                        }
                      }
                    }
                  }
                }
	      }
            }
          }
        }
      }
    }
    else {
      if(iel ==0){
        if(electronNoCutColl[iel].Pt() < 20.) continue;
      }
      else{
        if(electronNoCutColl[iel].Pt() < 15.) continue;
      }
      if(fabs(electronNoCutColl[iel].Eta()) > 2.4) continue;

      if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
        electronTight_tightref.push_back(electronNoCutColl.at(iel));

        if(fabs(electronNoCutColl.at(iel).DeltaEta())   <= 0.005){
          electronTight_DeltaEta.push_back(electronNoCutColl.at(iel));
        }
        if(fabs(electronNoCutColl.at(iel).DeltaPhi())   <= 0.02) {
          electronTight_DeltaPhi.push_back(electronNoCutColl.at(iel));
	}
        if(electronNoCutColl.at(iel).SigmaIEtaIEta()    <= 0.03){
          electronTight_SigmaIEta.push_back(electronNoCutColl.at(iel));
        }
        if(fabs(electronNoCutColl.at(iel).LeadVtxDistZ ())< 0.1){
          electronTight_DZ.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl.at(iel).HoE            ()  <= 0.10){
          electronTight_HoverE.push_back(electronNoCutColl.at(iel));
        }
        if( egamma_ep          <=0.05){
          electronTight_ep.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl[iel].ConvFitProb  ()    <=  1e-6){
          electronTight_convfit.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl[iel].MissingHits()    <=  0){
          electronTight_missinghit.push_back(electronNoCutColl.at(iel));
        }
        if(!electronNoCutColl.at(iel).HasMatchedConvPhot()){
          electronTight_convphoton.push_back(electronNoCutColl.at(iel));
        }
        if(electronNoCutColl.at(iel).GsfCtfScPixChargeConsistency()){
	  electronTight_chargeconst.push_back(electronNoCutColl.at(iel));
        }
      }
      if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
        electronTight_DXY.push_back(electronNoCutColl.at(iel));
      }

      if(fabs(electronNoCutColl.at(iel).DeltaEta())   <= 0.005){
        if(fabs(electronNoCutColl.at(iel).DeltaPhi())   <= 0.02) {
          if(electronNoCutColl.at(iel).SigmaIEtaIEta()    <= 0.03){
            if(fabs(electronNoCutColl.at(iel).LeadVtxDistZ ())< 0.1){
              if(electronNoCutColl.at(iel).HoE            ()  <= 0.10){
                if( egamma_ep          <=0.05){
                  if(electronNoCutColl[iel].ConvFitProb  ()    <=  1e-6){
                    if(electronNoCutColl[iel].MissingHits()    <=  0){
                      if(!electronNoCutColl.at(iel).HasMatchedConvPhot()){
                        if(fabs(electronNoCutColl.at(iel).dxy())  <= 0.01){
                          if(electronNoCutColl.at(iel).GsfCtfScPixChargeConsistency()){

                            Double_t PHONH_03[7]          = {0.13, 0.14, 0.07, 0.09, 0.11, 0.11, 0.14};
                            int ifid=0;
                            if (fabs(electronNoCutColl.at(iel).SCEta()) < 1.0) ifid = 0;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 1.479) ifid = 1;
			    else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.0) ifid = 2;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.2) ifid = 3;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.3) ifid = 4;
                            else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.4) ifid = 5;
                            else ifid = 6;

                            float LeptonRelIsoDR03(0.);
                            float ElectronIsoDR03 =  electronNoCutColl.at(iel).PFChargedHadronIso03() + max( electronNoCutColl.at(iel).PFNeutralHadronIso03() + electronNoCutColl.at(iel).PFPhotonIso03() - eventbase->GetEvent().JetRho() * PHONH_03[ifid],  0.);

                            if(electronNoCutColl.at(iel).Pt() > 0.)  LeptonRelIsoDR03 = ElectronIsoDR03/  electronNoCutColl.at(iel).Pt();
                            else LeptonRelIsoDR03 = -999.;

                            float trkiso =electronNoCutColl.at(iel).TrkIsoDR03()/electronNoCutColl.at(iel).Pt();
                            float ecaliso =electronNoCutColl.at(iel).ECalIsoDR03()/electronNoCutColl.at(iel).Pt();
                            float hcaliso =electronNoCutColl.at(iel).HCalIsoDR03()/electronNoCutColl.at(iel).Pt();
                            bool passNPFiso = true;
                            if(trkiso > 0.1) passNPFiso = false;
                            if(ecaliso > 0.25) passNPFiso = false;
                            if(hcaliso > 0.2)  passNPFiso = false;

                            float isocut= 0.09;
                            if(fabs(electronNoCutColl.at(iel).Eta() > 1.5) )isocut = 0.05;
			    else   if(electronNoCutColl.at(iel).Pt()< 20.) isocut=0.07;
                            if(LeptonRelIsoDR03 < isocut){
                              electron_analysisCutColl.push_back(electronNoCutColl.at(iel));
                              if(passNPFiso)       electron_analysisNPFCutColl.push_back(electronNoCutColl.at(iel));
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  std::vector<snu::KElectron>  electron_IsoCutColl;
  std::vector<snu::KElectron>  electron_NPFIsoCutColl;
  for(unsigned int iel = 0; iel < electronNoCutColl.size(); iel++){
    Double_t PHONH_03[7]          = {0.13, 0.14, 0.07, 0.09, 0.11, 0.11, 0.14};
    int ifid=0;
    if (fabs(electronNoCutColl.at(iel).SCEta()) < 1.0) ifid = 0;
    else if (fabs(electronNoCutColl.at(iel).SCEta()) < 1.479) ifid = 1;
    else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.0) ifid = 2;
    else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.2) ifid = 3;
    else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.3) ifid = 4;
    else if (fabs(electronNoCutColl.at(iel).SCEta()) < 2.4) ifid = 5;
    else ifid = 6;

    float LeptonRelIsoDR03(0.);
    float ElectronIsoDR03 =  electronNoCutColl.at(iel).PFChargedHadronIso03() + max( electronNoCutColl.at(iel).PFNeutralHadronIso03() + electronNoCutColl.at(iel).PFPhotonIso03()    - eventbase->GetEvent().JetRho() * PHONH_03[ifid],  0.);

    if(electronNoCutColl.at(iel).Pt() > 0.)  LeptonRelIsoDR03 = ElectronIsoDR03/  electronNoCutColl.at(iel).Pt();
    else LeptonRelIsoDR03 = -999.;

    float trkiso =electronNoCutColl.at(iel).TrkIsoDR03()/electronNoCutColl.at(iel).Pt();
    float ecaliso =electronNoCutColl.at(iel).ECalIsoDR03()/electronNoCutColl.at(iel).Pt();
    float hcaliso =electronNoCutColl.at(iel).HCalIsoDR03()/electronNoCutColl.at(iel).Pt();
    bool passNPFiso = true;
    if(trkiso > 0.1) passNPFiso = false;
    if(ecaliso > 0.25) passNPFiso = false;
    if(hcaliso > 0.2)  passNPFiso = false;
    float isocut= 0.09;
    if(fabs(electronNoCutColl.at(iel).Eta() > 1.5) )isocut = 0.05;
    else  if(electronNoCutColl.at(iel).Pt()< 20.) isocut=0.07;
    if(LeptonRelIsoDR03 < isocut){
      electron_IsoCutColl.push_back(electronNoCutColl.at(iel));
      if(passNPFiso)       electron_NPFIsoCutColl.push_back(electronNoCutColl.at(iel));

    }
  }

  FillHist("electronRef", 1.,weight, 0.,2.,2);
  if(SameCharge(electronPtEtaCutColl))   FillHist("electronPtEtaCutColl", 1.,weight, 0.,2.,2);
  if(SameCharge(electronTight_NoIsoCutColl))   FillHist("electron_NoIsoCutColl", 1.,weight, 0.,2.,2);
  if(SameCharge(electron_IsoCutColl))   FillHist("electron_IsoCutColl", 1.,weight, 0.,2.,2);
  if(SameCharge(electron_NPFIsoCutColl))   FillHist("electron_NPFIsoCutColl", 1.,weight, 0.,2.,2);
  if(SameCharge(electron_analysisCutColl))   FillHist("electron_analysisCutColll", 1.,weight, 0.,2.,2);
  if(SameCharge(electron_analysisNPFCutColl))   FillHist("electron_analysisNPFCutColll", 1.,weight, 0.,2.,2);
  if(jets.size()  > 1) FillHist("JetSelection", 1.,weight, 0.,2.,2);
  if(SameCharge(electron_analysisCutColl) && jets.size()  > 1)  FillHist("PreSelection", 1.,weight, 0.,2.,2);
  if(SameCharge(electron_analysisNPFCutColl) && jets.size()  > 1)  FillHist("PreSelectionNPF", 1.,weight, 0.,2.,2);

  if(SameCharge(electronTight_tightref)){
    FillHist("electronTight_tightref", 1., weight, 0.,2.,2);
    if(SameCharge(electronTight_DeltaEta))   FillHist("electron_DeltaEta", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_DeltaPhi))   FillHist("electron_DeltaPhi", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_SigmaIEta))   FillHist("electron_SigmaIEta", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_DZ))   FillHist("electron_DZ", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_HoverE))   FillHist("electron_HoverE", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_convfit))   FillHist("electron_convfit", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_ep))   FillHist("electron_ep", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_missinghit))   FillHist("electron_missinghit", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_convphoton))   FillHist("electron_convphoton", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_DXY))   FillHist("electron_DXY", 1.,weight, 0.,2.,2);
    if(SameCharge(electronTight_chargeconst))   FillHist("electron_chargeconst", 1.,weight, 0.,2.,2);
  }



}


TDirectory* AnalyzerCore::getTemporaryDirectory(void) const
{

  
  gROOT->cd();
  TDirectory* tempDir = 0;
  int counter = 0;
  while (not tempDir) {
    // First, let's find a directory name that doesn't exist yet:               
    std::stringstream dirname;
    dirname << "AnalyzerCore_%i" << counter;
    if (gROOT->GetDirectory((dirname.str()).c_str())) {
      ++counter;
      continue;
    }
    // Let's try to make this directory:                                        
    tempDir = gROOT->mkdir((dirname.str()).c_str());

  }

  return tempDir;
}



double AnalyzerCore::MuonScaleFactor(double eta, double pt, int sys){
  
  double sf = 0.;
  if(fabs(eta) > 2.4) return 1.;
  if(pt < 15.) return 1.;
  int bin = MuonSF->FindBin(fabs(eta),pt);
  
  sf = MuonSF->GetBinContent(bin);

  if(sys==1) sf*= sqrt(1.02); 
  if(sys==-1) sf/= sqrt(1.02); 
  
  return sf;
  
}

double AnalyzerCore::TriggerScaleFactor( vector<snu::KElectron> el){
  if(isData) return 1.;
  if(el.size() != 2) return 1.;
  float pt = el.at(1).Pt();
  if( pt < 30.) return 0.92;
  else return 0.96;
  
}

double AnalyzerCore::ElectronScaleFactor( double eta, double pt, bool tight_electron){
  
  ///https://twiki.cern.ch/twiki/bin/view/Main/EGammaScaleFactors2012
  double sf = 0.;
  
  /// tight working point


  bool medium_electron=!tight_electron;
  
  if(tight_electron){
    if(fabs(eta) < 0.8 ) {
      if( pt < 15.) sf = 0.827;
      else if( pt < 20.) sf = 0.924;
      else if( pt < 30.) sf = 0.960;
      else if( pt < 40.) sf = 0.978;
      else if( pt < 50.) sf = 0.981;
      else sf = 0.982;
    }
    else  if(fabs(eta) <  1.442){
      
      if( pt < 15.) sf = 0.948;
      else if( pt < 20.) sf = 0.932;
      else if( pt < 30.) sf = 0.936;
      else if( pt < 40.) sf = 0.958;
      else if( pt < 50.) sf = 0.969;
      else sf = 0.969;
    }
    else  if(fabs(eta) <1.556){
      if( pt < 15.) sf = 1.073;
      else if( pt < 20.) sf = 0.808;
      else if( pt < 30.) sf = 0.933;
      else if( pt < 40.) sf = 0.907;
      else if( pt < 50.) sf = 0.904;
      else sf = 0.926;
    }
    else if(fabs(eta) <2.0){
      
      if( pt < 15.) sf = 0.854;
      else if( pt < 20.) sf = 0.853;
      else if( pt < 30.) sf = 0.879;
      else if( pt < 40.) sf = 0.909;
      else if( pt < 50.) sf = 0.942;
      else sf = 0.957;
    }
    else{
      if( pt < 15.) sf = 1.007;
      else if( pt < 20.) sf = 0.903;
      else if( pt < 30.) sf = 0.974;
      else if( pt < 40.) sf = 0.987;
      else if( pt < 50.) sf = 0.991;
      else sf = 0.999;
    }
    return sf;
  }
  if(medium_electron){
    /// medium working point
    if(fabs(eta) < 0.8 ) {
      if( pt < 15.) sf = 0.834;
      else if( pt < 20.) sf = 0.918;
      else if( pt < 30.) sf = 0.954;
      else if( pt < 40.) sf = 0.960;
      else if( pt < 50.) sf = 0.972;
      else sf = 0.969;
    }
    else  if(fabs(eta) <  1.442){
      
      if( pt < 15.) sf = 0.973;
      else if( pt < 20.) sf = 0.906;
      else if( pt < 30.) sf = 0.923;
      else if( pt < 40.) sf = 0.935;
      else if( pt < 50.) sf = 0.955;
      else sf = 0.956;
    }
    else  if(fabs(eta) <1.556){
      if( pt < 15.) sf = 0.954;
      else if( pt < 20.) sf = 0.909;
      else if( pt < 30.) sf = 0.921;
      else if( pt < 40.) sf = 0.924;
      else if( pt < 50.) sf = 0.950;
      else sf = 0.995;
    }
    else if(fabs(eta) <2.0){
      
      if( pt < 15.) sf = 0.954;
      else if( pt < 20.) sf = 0.909;
      else if( pt < 30.) sf = 0.921;
      else if( pt < 40.) sf = 0.924;
      else if( pt < 50.) sf = 0.950;
      else sf = 0.995;
    }
    else{
      if( pt < 15.) sf = 1.119;
      else if( pt < 20.) sf = 0.944;
      else if( pt < 30.) sf = 0.993;
      else if( pt < 40.) sf = 0.959;
      else if( pt < 50.) sf = 0.968;
      else sf = 0.969;
      
    }
    return sf;

  }
  return sf;
  /// From higgs note Z->llll
  /// reconstruction scale factors
  if(fabs(eta) < 0.8 ) {
    if( pt < 15.) sf *= 0.967;
    else if( pt < 20.) sf *= 0.997;
    else if( pt < 30.) sf *= 0.982;
    else if( pt < 40.) sf *= 0.988;
    else sf *= 0.990;
  }
  else if (fabs(eta) < 1.4442 ) {
    if( pt < 15.) sf *= 0.967;
    else if( pt < 20.) sf *= 0.997;
    else if( pt < 30.) sf *= 0.993;
    else if( pt < 40.) sf *= 0.993;
    else sf *= 0.992;
  }
  else if (fabs(eta) < 1.566 ) {
    if( pt < 15.) sf *= 1.126;
    else if( pt < 20.) sf *= 0.955;
    else if( pt < 30.) sf *= 1.015;
    else if( pt < 40.) sf *= 0.985;
    else sf *= 0.985;
  }
  else if (fabs(eta) < 2. ) {
    if( pt < 15.) sf *= 1.097;
    else if( pt < 20.) sf *= 1.012;
    else if( pt < 30.) sf *= 0.988;
    else if( pt < 40.) sf *= 0.992;
    else sf *= 0.991;
  }
  else{
    if( pt < 15.) sf *= 1.097;
    else if( pt < 20.) sf *= 1.012;
    else if( pt < 30.) sf *= 1.002;
    else if( pt < 40.) sf *= 1.004;
    else sf *= 1.004;
  }

    
}

void AnalyzerCore::AddTriggerToList(TString triggername){
  
  triggerlist.push_back(triggername);
}

AnalyzerCore::~AnalyzerCore(){
  
  Message("In AnalyzerCore Destructor" , INFO);
  if(FRHist) delete FRHist;

  for(map<TString, TH1*>::iterator it = maphist.begin(); it!= maphist.end(); it++){
    delete it->second;
  }
  maphist.clear();

  for(map<TString, TH2*>::iterator it = maphist2D.begin(); it!= maphist2D.end(); it++){
    delete it->second;
  }
  maphist2D.clear();



  for(map<TString, MuonPlots*>::iterator it = mapCLhistMu.begin(); it != mapCLhistMu.end(); it++){
    delete it->second;
  }
  mapCLhistMu.clear();
  

  for(map<TString, JetPlots*>::iterator it = mapCLhistJet.begin(); it != mapCLhistJet.end(); it++){
    delete it->second;
  }
  mapCLhistJet.clear();

  for(map<TString, ElectronPlots*>::iterator it = mapCLhistEl.begin(); it != mapCLhistEl.end(); it++){
    delete it->second;
  }
  mapCLhistEl.clear();

  for(map<TString, SignalPlots*>::iterator it = mapCLhistSig.begin(); it != mapCLhistSig.end(); it++){
    delete it->second;
  }
  mapCLhistSig.clear();
  
  }

//###
//###   IMPORTANT BASE FUNCTION: SETS UP EVENT FOR ALL CYCLES
//###

void AnalyzerCore::SetUpEvent(Long64_t entry, float ev_weight) throw( LQError ) {
  
  Message("In SetUpEvent(Long64_t entry) " , DEBUG);
  m_logger << DEBUG << "This is entry " << entry << LQLogger::endmsg;
  if (!fChain) throw LQError( "Chain is not initialized",  LQError::SkipCycle );     
  
  if(LQinput){
    m_logger << DEBUG << "k_isdata = " << k_isdata << " and isData = " << isData << LQLogger::endmsg;
    if(k_isdata != isData) throw LQError( "!!! Event is confused. It does not know if it is data or MC", LQError::SkipCycle );
  }
  else isData = k_isdata;
  
  if (!(entry % output_interval)) {
    m_logger << INFO <<  "Processing entry " << entry <<  "/" << nentries << LQLogger::endmsg;

  }

  snu::KEvent eventinfo = GetEventInfo();
  
  if(k_isdata){
    if(ev_weight!=1.) Message("ERROR in setting weights. This is Data...", INFO);
    MCweight=1.;
    weight = 1.;
  }
  else {
    MCweight = eventinfo.MCWeight(); //Get MC weight here FIX ME                                                              
    weight= ev_weight; 
  }
  
  //
  // creates object that stores all SKTree classes	
  //                                                                                                        

  snu::KTrigger triggerinfo = GetTriggerInfo(triggerlist);

  std::vector<snu::KJet> skjets= GetAllJets();
  std::vector<snu::KGenJet> skgenjets=GetAllGenJets();
  
  LQEvent lqevent(GetAllMuons(), GetAllElectrons(), GetAllTaus(), skjets, skgenjets,GetTruthParticles(), triggerinfo,eventinfo);
  
  //  eventbase is master class to use in analysis 
  //
  
  eventbase = new EventBase(lqevent);
  
}

float AnalyzerCore::SumPt( std::vector<snu::KJet> particles){

  float sumpt=0.;
  
  for(std::vector<snu::KJet>::iterator it = particles.begin(); it != particles.end(); it++){
    sumpt += it->Pt();
  }
  return sumpt;
}
  

std::vector<snu::KElectron> AnalyzerCore::ShiftElectronEnergy(std::vector<snu::KElectron> electrons, bool applyshift){
  
  std::vector<snu::KElectron> shiftedel;

  for(unsigned int iel=0; iel < electrons.size(); iel++){
    float scale =0.98;

    if(applyshift)electrons.at(iel).SetPtEtaPhiM(electrons.at(iel).Pt()*scale, electrons.at(iel).Eta(), electrons.at(iel).Phi(), 0.511e-3);
    shiftedel.push_back(electrons.at(iel));
  }    
  return shiftedel;
}

bool AnalyzerCore::isPrompt(long pdgid) {
  /// mother pdgid
  pdgid = abs(pdgid);
  if (pdgid == 24) return true; // Z
  else if (pdgid == 23) return true; // W
  else if (pdgid == 15) return true; // taus
  else if (pdgid == 90) return true; // N
  else return false;
}


float  AnalyzerCore::JetResCorr(snu::KJet jet, std::vector<KGenJet> genjets){
  
  /// This function is not needed when smeaing is already applied to LQNtuples in production stage
  return 1.;
  
  float genpt= -999.;
  for(std::vector<KGenJet>::iterator it = genjets.begin(); it != genjets.end(); it++){
    if(it->DeltaR(jet) < 0.3){
      genpt = it->Pt();
    }
  }
  
  double fabs_eta = fabs ( jet.Eta() );
  float c(0.);
  if      ( fabs_eta > 0.0 && fabs_eta <= 0.5 ) c= 1.052;
  else if ( fabs_eta > 0.5 && fabs_eta <= 1.1 ) c= 1.057;
  else if ( fabs_eta > 1.1 && fabs_eta <= 1.7 ) c= 1.096;
  else if ( fabs_eta > 1.7 && fabs_eta <= 2.3 ) c= 1.134;
  else                                          c= 1.288;
  
  float newpt = std::max(0., genpt + c*(jet.Pt() - genpt));
  if(genpt < 0.) newpt = jet.Pt();
  
  return newpt;
}

void AnalyzerCore::EndEvent()throw( LQError ){

  delete eventbase;                                                                                                            

}
  
void AnalyzerCore::CheckFile(TFile* file)throw( LQError ){

  if(file) m_logger << INFO << "Analyzer: File " << file->GetName() << " was found." << LQLogger::endmsg;
  else m_logger  << INFO <<"Analyzer  " << file->GetName()  << "  : ERROR Rootfile failed to open." << LQLogger::endmsg;

  if(!file)  throw LQError( "!!! File is not found", LQError::SkipCycle);
  return;
}

bool AnalyzerCore::PassTrigger(vector<TString> list, int& prescaler){
  
  return TriggerSelector(list, eventbase->GetTrigger().GetHLTInsideDatasetTriggerNames(), eventbase->GetTrigger().GetHLTInsideDatasetTriggerDecisions(), eventbase->GetTrigger().GetHLTInsideDatasetTriggerPrescales(), prescaler);

}

TDirectory* AnalyzerCore::GetTemporaryDirectory(void) const
{
  gROOT->cd();
  TDirectory* tempDir = 0;
  int counter = 0;
  while (not tempDir) {
    // First, let's find a directory name that doesn't exist yet:                                              
    std::stringstream dirname;
    dirname << "HNCommonLeptonFakes_%i" << counter;
    if (gROOT->GetDirectory((dirname.str()).c_str())) {
      ++counter;
      continue;
    }
    // Let's try to make this directory:                                                                       
    tempDir = gROOT->mkdir((dirname.str()).c_str());

  }

  return tempDir;

}


void AnalyzerCore::Message(TString message, LQMsgType type){
  m_logger <<  type << message << LQLogger::endmsg;
}


void AnalyzerCore::MakeCleverHistograms(histtype type, TString clhistname ){
  
  //// ELECTRON PLOTs                                                                                          
  if(type==elhist) mapCLhistEl[clhistname] = new ElectronPlots(clhistname);
  //// MUON PLOTs                                                                                              
  if(type==muhist) mapCLhistMu[clhistname] = new MuonPlots(clhistname);
  /// JET PLOTs                                                                                                
  if(type==jethist) mapCLhistJet[clhistname] = new JetPlots(clhistname);
  /// Signal plots                                                                                             
  if(type==sighist)  mapCLhistSig[clhistname] = new SignalPlots(clhistname);
      
  return;
}

void AnalyzerCore::MakeHistograms(){
  //// Additional plots to make                                                                                
  maphist.clear();
  maphist2D.clear();

    
}

void AnalyzerCore::MakeHistograms(TString hname, int nbins, float xbins[]){
  maphist[hname] =  new TH1F(hname.Data(),hname.Data(),nbins,xbins);
}

void AnalyzerCore::MakeHistograms(TString hname, int nbins, float xmin, float xmax){

  maphist[hname] =  new TH1F(hname.Data(),hname.Data(),nbins,xmin,xmax);
}

void AnalyzerCore::MakeHistograms2D(TString hname, int nbinsx, float xmin, float xmax, int nbinsy, float ymin, float ymax) {

  maphist2D[hname] =  new TH2F(hname.Data(),hname.Data(),nbinsx,xmin,xmax, nbinsy,ymin,ymax);
}

void AnalyzerCore::MakeHistograms2D(TString hname, int nbinsx,  float xbins[], int nbinsy,  float ybins[]) {

  maphist2D[hname] =  new TH2F(hname.Data(),hname.Data(),nbinsx , xbins, nbinsy,ybins);
}

bool AnalyzerCore::PassBasicEventCuts(){
  
  bool pass (true);
  
  ///https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFilters
  /// The recommendations of the MET group regarding noise cleaning are summarized in the talk https://indico.cern.ch/getFile.py/access?subContId=1&contribId=4&resId=0&materialId=slides&confId=172431
  
  //CSC tight beam halo filter
  if (!eventbase->GetEvent().PassBeamHaloFilterTight()) {
    pass = false;
    m_logger << DEBUG << "Event Fails PassBeamHaloFilterTight " << LQLogger::endmsg;
  }
  //if (!eventbase->GetEvent().PassBeamHaloFilterLoose()) pass = false;
  //HBHE noise filter with isolated noise rejection
  if (!eventbase->GetEvent().PassHBHENoiseFilter()) {
    pass = false; 
    m_logger << DEBUG << "Event Fails PassHBHENoiseFilter " << LQLogger::endmsg;
  }
  //HCAL laser filter (post-ICHEP: updated to reduce over-tagging rate in channels with low Bias Voltage)
  
  //ECAL dead cell trigger primitive (TP) filter
  if(eventbase->GetEvent().PassEcalDeadCellTriggerPrimitiveFilter()) {
    pass = false;
    m_logger << DEBUG << "Event Fails PassEcalDeadCellTriggerPrimitiveFilter" << LQLogger::endmsg;
  }
  if(  eventbase->GetEvent().PassEcalDeadCellBoundaryEnergyFilter()) {
    pass = false;
    m_logger << DEBUG << "Event Fails PassEcalDeadCellBoundaryEnergyFilter" << LQLogger::endmsg;
  }
  //Tracking failure filter
  if (eventbase->GetEvent().IsTrackingFailure()) {
    pass = false;
    m_logger << DEBUG << "Event Fails IsTrackingFailure" << LQLogger::endmsg; 
  }
  //Bad EE Supercrystal filter (post-ICHEP: extend to include an additional problematic SC --only for 2012)
  if (eventbase->GetEvent().PassBadEESupercrystalFilter()) {
    pass = false;
    m_logger << DEBUG << "Event Fails PassBadEESupercrystalFilter" << LQLogger::endmsg;
  }
  //ECAL Laser correction filter (only mandatory for 53X rereco of 2012A+B, i.e., Jul13 rereco; An optional filter for 2012C prompt reco Mandatory for Jan2013 ReReco)
  if(eventbase->GetEvent().PassEcalLaserCorrFilter()) {
    pass = false;
    m_logger << DEBUG << "Event Fails PassEcalLaserCorrFilter" << LQLogger::endmsg;
  }
  //Tracking POG filters (new. Only work on AOD >=53X)
  if(eventbase->GetEvent().PassTrackingFailureFilter()){
    m_logger << DEBUG << "Event Fails PassTrackingFailureFilter" << LQLogger::endmsg;
    pass = false;
  }
  return pass;
}



void AnalyzerCore::FillHist(TString histname, float value, float w, float xbins[], int nbins){
  m_logger << DEBUG << "FillHist : " << histname << LQLogger::endmsg;
  if(GetHist(histname)) GetHist(histname)->Fill(value, w);
  
  else{
    if (nbins < 0) {
      m_logger << ERROR << histname << " was NOT found. Nbins was not set also... please configure histogram maker correctly" << LQLogger::endmsg;
      exit(0);
    }
    m_logger << DEBUG << "Making the histogram" << LQLogger::endmsg;
    MakeHistograms(histname, nbins, xbins);
    if(GetHist(histname)) GetHist(histname)->Fill(value, w);
  }

}

void AnalyzerCore::FillHist(TString histname, float value, float w, float xmin, float xmax, int nbins){
  
  m_logger << DEBUG << "FillHist : " << histname << LQLogger::endmsg;
  if(GetHist(histname)) GetHist(histname)->Fill(value, w);  
  else{
    if (nbins < 0) {
      m_logger << ERROR << histname << " was NOT found. Nbins was not set also... please configure histogram maker correctly" << LQLogger::endmsg;
      exit(0);
    }
    m_logger << DEBUG << "Making the histogram" << LQLogger::endmsg;
    MakeHistograms(histname, nbins, xmin, xmax);
    if(GetHist(histname)) GetHist(histname)->Fill(value, w);
  }
  
}

void AnalyzerCore::FillHist(TString histname, float value1, float value2, float w, float xmin, float xmax, int nbinsx, float ymin, float ymax, int nbinsy){

  m_logger << DEBUG << "FillHist : " << histname << LQLogger::endmsg;
  if(GetHist2D(histname)) GetHist2D(histname)->Fill(value1,value2, w);
  else{
    if (nbinsx < 0) {
      m_logger << ERROR << histname << " was NOT found. Nbins was not set also... please configure histogram maker correctly" << LQLogger::endmsg;
      exit(0);
    }
    m_logger << DEBUG << "Making the histogram" << LQLogger::endmsg;
    MakeHistograms2D(histname, nbinsx, xmin, xmax,nbinsy, ymin, ymax );
    if(GetHist2D(histname)) GetHist2D(histname)->Fill(value1,value2, w);
  }

}

void AnalyzerCore::FillHist(TString histname, float valuex, float valuey, float w, float xbins[], int nxbins, float ybins[], int nybins){
  m_logger << DEBUG << "FillHist : " << histname << LQLogger::endmsg;
  if(GetHist2D(histname)) GetHist2D(histname)->Fill(valuex,valuey, w);

  else{
    if (nxbins < 0) {
      m_logger << ERROR << histname << " was NOT found. Nbins was not set also... please configure histogram maker correctly" << LQLogger::endmsg;
      exit(0);
    }
    m_logger << DEBUG << "Making the histogram" << LQLogger::endmsg;
    MakeHistograms2D(histname, nxbins, xbins, nybins, ybins );
    if(GetHist2D(histname)) GetHist2D(histname)->Fill(valuex, valuey, w);
  }

}



void AnalyzerCore::FillHist(TString histname, float value, float w){

  if(GetHist(histname)) GetHist(histname)->Fill(value, w);  /// Plots Z peak                                   
  else m_logger << INFO << histname << " was NOT found. Will add the histogram to the hist map on first event." << LQLogger::endmsg;
  
  
  return;
}

void AnalyzerCore::FillCLHist(histtype type, TString hist, vector<snu::KMuon> muons, double w){

  if(type==muhist){
    map<TString, MuonPlots*>::iterator mupit = mapCLhistMu.find(hist);
    if(mupit != mapCLhistMu.end()) mupit->second->Fill(w,muons);
    else m_logger << INFO  << hist << " not found in mapCLhistMu" << LQLogger::endmsg;
  }
  else  m_logger << INFO  << "Type not set to muhist, is this a mistake?" << LQLogger::endmsg;

}


void AnalyzerCore::FillCLHist(histtype type, TString hist, vector<snu::KElectron> electrons, double rho, double w){

  if(type==elhist){
    map<TString, ElectronPlots*>::iterator elpit = mapCLhistEl.find(hist);
    if(elpit !=mapCLhistEl.end()) elpit->second->Fill(w,electrons,rho);
    else m_logger << INFO  << hist << " not found in mapCLhistEl" <<LQLogger::endmsg;
  }
  else  m_logger << INFO  << "Type not set to elhist, is this a mistake?" << LQLogger::endmsg;
}

void AnalyzerCore::FillCLHist(histtype type, TString hist, vector<snu::KJet> jets, double w){

  if(type==jethist){
    map<TString, JetPlots*>::iterator jetpit = mapCLhistJet.find(hist);
    if(jetpit !=mapCLhistJet.end()) jetpit->second->Fill(w,jets);
    else m_logger << INFO  << hist << " not found in mapCLhistJet" <<LQLogger::endmsg;
  }
  else  m_logger << INFO  <<"Type not set to jethist, is this a mistake?" << LQLogger::endmsg;

}


void AnalyzerCore::FillCLHist(histtype type, TString hist, snu::KEvent ev,vector<snu::KMuon> muons, vector<snu::KElectron> electrons, vector<snu::KJet> jets,double w){

  if(type==sighist){

    map<TString, SignalPlots*>::iterator sigpit = mapCLhistSig.find(hist);
    if(sigpit !=mapCLhistSig.end()) sigpit->second->Fill(ev, muons, electrons, jets,w);
    else {
      mapCLhistSig[hist] = new SignalPlots(hist);
      sigpit = mapCLhistSig.find(hist);
      sigpit->second->Fill(ev, muons, electrons, jets,w);
    }
  }
  else  m_logger << INFO  <<"Type not set to sighist, is this a mistake?" << LQLogger::endmsg;
}


void AnalyzerCore::FillCLHist(histtype type, TString hist, snu::KEvent ev,vector<snu::KElectron> electrons, vector<snu::KJet> jets,double w){

  if(type==sighist){
    map<TString, SignalPlots*>::iterator sigpit = mapCLhistSig.find(hist);
    if(sigpit !=mapCLhistSig.end()) sigpit->second->Fill(ev, electrons, jets, w);
    else {
      mapCLhistSig[hist] = new SignalPlots(hist);
      sigpit = mapCLhistSig.find(hist);
      sigpit->second->Fill(ev, electrons, jets, w);
    }
  }
  else  m_logger << INFO  <<"Type not set to sighist, is this a mistake?" << LQLogger::endmsg;
}


void AnalyzerCore::WriteHistograms() throw (LQError){
  // This function is called after the cycle is ran. It wrues all histograms to the output file. This function is not used by user. But by the contrioller code.
  WriteHists();
  WriteCLHists();
}

  
void AnalyzerCore::WriteCLHists(){

  for(map<TString, MuonPlots*>::iterator mupit = mapCLhistMu.begin(); mupit != mapCLhistMu.end(); mupit++){

    Dir = m_outputFile->mkdir(mupit->first);
    m_outputFile->cd( Dir->GetName() );
    mupit->second->Write();
    m_outputFile->cd();
  }

  for(map<TString, ElectronPlots*>::iterator elpit = mapCLhistEl.begin(); elpit != mapCLhistEl.end(); elpit++)\
    {

      Dir = m_outputFile->mkdir(elpit->first);
      m_outputFile->cd( Dir->GetName() );
      elpit->second->Write();
      m_outputFile->cd();
    }

  for(map<TString, JetPlots*>::iterator jetpit = mapCLhistJet.begin(); jetpit != mapCLhistJet.end(); jetpit++)\
    {
      
      Dir = m_outputFile->mkdir(jetpit->first);
      m_outputFile->cd( Dir->GetName() );
      jetpit->second->Write();
      m_outputFile->cd();
    }
  for(map<TString, SignalPlots*>::iterator sigpit = mapCLhistSig.begin(); sigpit != mapCLhistSig.end(); sigpit++){
    
    Dir = m_outputFile->mkdir(sigpit->first);
    m_outputFile->cd( Dir->GetName() );
    sigpit->second->Write();
    m_outputFile->cd();
  }

  return;
}

void AnalyzerCore::WriteHists(){

  /// Open Output rootfile
  m_outputFile->cd();

  for(map<TString, TH1*>::iterator mapit = maphist.begin(); mapit != maphist.end(); mapit++){

    if(mapit->first.Contains("closejet")){
      if(!m_outputFile->GetDirectory( "closejet" )){
	Dir = m_outputFile->mkdir("closejet");
	m_outputFile->cd( Dir->GetName() );
      }
      else  m_outputFile->cd("closejet");
      mapit->second->Write();
      m_outputFile->cd();
    }
    else if (mapit->first.Contains("awayjet")){
      if(!m_outputFile->GetDirectory("awayjet")){
	Dir = m_outputFile->mkdir("awayjet");
	m_outputFile->cd(Dir->GetName() );
      }
      else m_outputFile->cd("awayjet");
      mapit->second->Write();
      m_outputFile->cd();
    }
    else if (mapit->first.Contains("FakeBkg")){
      if(!m_outputFile->GetDirectory("FakeBkg")){
	Dir = m_outputFile->mkdir("FakeBkg");
	m_outputFile->cd(Dir->GetName() );
      }
      else m_outputFile->cd("FakeBkg");
      mapit->second->Write();
      m_outputFile->cd();
    }
    else if (mapit->first.Contains("susyid")){
      if(!m_outputFile->GetDirectory("susyid")){
        Dir = m_outputFile->mkdir("susyid");
        m_outputFile->cd(Dir->GetName() );
      }
      else m_outputFile->cd("susyid");
      mapit->second->Write();
      m_outputFile->cd();
    }
    else if (mapit->first.Contains("medium")){
      if(!m_outputFile->GetDirectory("medium")){
        Dir = m_outputFile->mkdir("medium");
        m_outputFile->cd(Dir->GetName() );
      }
      else m_outputFile->cd("medium");
      mapit->second->Write();
      m_outputFile->cd();
    }
    else if (mapit->first.Contains("tight")){
      if(!m_outputFile->GetDirectory("tight")){
        Dir = m_outputFile->mkdir("tight");
        m_outputFile->cd(Dir->GetName() );
      }
      else m_outputFile->cd("tight");
      mapit->second->Write();
      m_outputFile->cd();
    }
    
    else     mapit->second->Write();
  }

  for(map<TString, TH2*>::iterator mapit = maphist2D.begin(); mapit != maphist2D.end(); mapit++){
    mapit->second->Write();
  }


  return;
}

TH1* AnalyzerCore::GetHist(TString hname){

  TH1* h = NULL;
  std::map<TString, TH1*>::iterator mapit = maphist.find(hname);
  if(mapit != maphist.end()) return mapit->second;
  else m_logger << DEBUG  << hname << " was not found in map" << LQLogger::endmsg;

  return h;
}



TH2* AnalyzerCore::GetHist2D(TString hname){

  TH2* h = NULL;
  std::map<TString, TH2*>::iterator mapit = maphist2D.find(hname);
  if(mapit != maphist2D.end()) return mapit->second;
  else m_logger << DEBUG  << hname << " was not found in map" << LQLogger::endmsg;

  return h;
}


bool AnalyzerCore::Zcandidate(std::vector<snu::KElectron> electrons, float interval, bool require_os){
  
  if(electrons.size()!=2) return false;
  if(require_os&&SameCharge(electrons)) return false;
  
  KParticle Z = electrons.at(0) + electrons.at(1);
  if(fabs(Z.M() - 90.) <  interval) return true;
  else return false;
  
}
  
bool AnalyzerCore::SameCharge(std::vector<snu::KElectron> electrons){
  
  if(electrons.size()!=2) return false;
  if(electrons.at(0).Charge() == electrons.at(1).Charge()) return true;
  return false;
}

int AnalyzerCore::NBJet(std::vector<snu::KJet> jets){
  
  int nbjet=0;
  for(unsigned int ij=0; ij <jets.size(); ij++){
    if(jets.at(ij).CombinedSecVertexBtag() > 0.679) nbjet++;
  }
  return nbjet;
}


double AnalyzerCore::MuonDYMassCorrection(std::vector<snu::KMuon> mu, double w){
  
  if(mu.size()< 2) return 0.;
  snu::KParticle Z = mu.at(0) + mu.at(1);
  
  double factor (1.);
  if(Z.M() > 90.){
    factor = 8.37401e-01 + 1.61277e-03*Z.M();
  }
  return w*factor;
}

float AnalyzerCore::CFRate(snu::KElectron el, bool use_oldrates){
  
  Double_t frac = 0. ;
  float pt = el.Pt();
  Double_t p0 = 0. ;
  Double_t p1 = 0. ;

  Double_t scale_factor_EE = 1. ;
  Double_t scale_factor_BB = 1. ;


  if( fabs(el.Eta()) <= 1.4442 ) {
    scale_factor_BB = 1.29;
    if(use_oldrates)  scale_factor_BB = 1.32;

    //--region:  1/pt > 0.02
    p0 = 8.01e-05 ; p1 = -1.80e-03 ;
    if(use_oldrates){
      p0 = 8.16e-05 ; p1 = -1.82e-03 ;
    }
    frac = p0 + p1*(1./pt);
    
    if( (1./pt) <= 0.02 ) {
      p0 = 3.13e-04 ;  p1 = -1.43e-02 ;
      if(use_oldrates){
	p0 = 3.37e-04 ;  p1 = -1.55e-02 ;
      }
      frac = max(p0 + p1*(1./pt), frac);
    }
    frac *= scale_factor_BB ;
    
  } else {  // fabs(eta) > 1.4
    
    
    scale_factor_EE = 1.40 ; //
    if(use_oldrates)scale_factor_EE = 1.32;
    //--region:  1/pt > 0.02
    p0 = 4.91e-04 ; p1 = -0.952e-02 ;
    if(use_oldrates){
      p0 = 5.41e-04 ; p1 = -1.10e-02 ;
    }
    frac = p0 + p1*(1./pt) ;

    if( (1./pt) <= 0.02 ){
      p0 = 2.70e-03 ;  p1 = -1.21e-01 ;
      if(use_oldrates){
	p0 = 2.95e-03 ;  p1 = -1.32e-01 ;
      }
      frac = max(p0 + p1*(1./pt), frac) ;
    }
    frac *= scale_factor_EE ;
  }
  
  return float(frac) ;
}

bool AnalyzerCore::IsTight(snu::KMuon muon){
  /// ADD TIGHT MUON REQUIREMENT
  float reliso=0.;
  if (muon.Pt() > 0.01)  reliso = (muon.SumIsoCHDR03() + std::max(0.0, muon.SumIsoNHDR03() + muon.SumIsoPHDR03() - 0.5* muon.SumPUIsoR03()))/muon.Pt() ;
  else reliso = 9999.;
  if (reliso<0) reliso=0.0001;
  

  if(( reliso >= 0.05)) return false;
  if(( muon.GlobalChi2() >= 10.)) return false;
 
  if(fabs(muon.dXY()) >= 0.005) return false; 
  return true;
}


bool AnalyzerCore::IsTight(snu::KElectron el, double jetrho , double dxy, double biso, double eiso, bool usetight){
  
  return eventbase->GetElectronSel()->HNIsTight(el, jetrho, dxy, biso, eiso, usetight, false);

}
  

bool AnalyzerCore::IsTight(snu::KElectron electron, double rho){

  return eventbase->GetElectronSel()->HNIsTight(electron, rho, false);
}

vector<snu::KElectron> AnalyzerCore::GetTruePrompt(vector<snu::KElectron> electrons, bool keep_chargeflip, bool keepfake){
  
  vector<int> toremove;
  if(!keepfake){
    toremove.push_back(1);
    toremove.push_back(2);
    toremove.push_back(3);
  }
  if(!keep_chargeflip){
    toremove.push_back(6);
    toremove.push_back(4);
    toremove.push_back(5);
  }

  toremove.push_back(8);
  toremove.push_back(9);
  
  vector<snu::KElectron> prompt_electrons;
  for(unsigned int i = 0; i < electrons.size(); i++){
    if(!k_isdata){
      bool remove_el=false;
      for(unsigned int j=0; j < toremove.size(); j++){
	if(electrons.at(i).GetType() == toremove.at(j)) remove_el=true;
      }
      if(!remove_el) prompt_electrons.push_back(electrons.at(i));
    }// Data
    else prompt_electrons.push_back(electrons.at(i));
  }/// loop
  
  return prompt_electrons;
}



void AnalyzerCore::CorrectMuonMomentum(vector<snu::KMuon>& k_muons){
  
  vector<TLorentzVector> tlv_muons = MakeTLorentz(k_muons);
  int imu(0);
  for(std::vector<snu::KMuon>::iterator it = k_muons.begin(); it != k_muons.end(); it++, imu++){
    float qter =1.; /// uncertainty
    if(k_isdata)rmcor->momcor_data(tlv_muons[imu], float(it->Charge()), 0, qter);
    else rmcor->momcor_mc(tlv_muons[imu], float(it->Charge()), 0, qter);
    it->SetPtEtaPhiM(tlv_muons[imu].Pt(),tlv_muons[imu].Eta(), tlv_muons[imu].Phi(), tlv_muons[imu].M());
  }
}


float AnalyzerCore::Get_DataDrivenWeight_EM(vector<snu::KMuon> k_muons, vector<snu::KElectron> k_electrons, double rho){

  float em_weight = 0.;
  if(k_muons.size()==1 && k_electrons.size()==1){

    bool is_mu1_tight    = IsTight(k_muons.at(0));
    bool is_el1_tight    = IsTight(k_electrons.at(0),rho);

    vector<TLorentzVector> muons=MakeTLorentz(k_muons);
    vector<TLorentzVector> electrons=MakeTLorentz(k_electrons);


    em_weight =m_fakeobj->get_dilepton_em_eventweight(muons,electrons, is_mu1_tight,is_el1_tight);
  }

  return em_weight;
}

float AnalyzerCore::Get_DataDrivenWeight_MM(vector<snu::KMuon> k_muons){

  float mm_weight = 0.;
  if(k_muons.size()==2){
    
    bool is_mu1_tight    = IsTight(k_muons.at(0));
    bool is_mu2_tight    = IsTight(k_muons.at(1));

    vector<TLorentzVector> muons=MakeTLorentz(k_muons);

    mm_weight =m_fakeobj->get_dilepton_mm_eventweight(muons,  is_mu1_tight,is_mu2_tight);
  }

  return mm_weight;
}




float AnalyzerCore::Get_DataDrivenWeight_EE(vector<snu::KElectron> k_electrons,  double rho){
  
  return Get_DataDrivenWeight_EE(k_electrons, rho, 0.01, 0.09, 0.05, "40", 0); /// dxy cut ,  biso, eciso, awayjet pt
}



float AnalyzerCore::Get_DataDrivenWeight_EE(vector<snu::KElectron> k_electrons ,  double rho, bool usetight,  double dxy, double biso, double eiso,TString cut){
  return Get_DataDrivenWeight_EE(k_electrons, rho, usetight, dxy, biso, eiso, cut, 0);
}

float AnalyzerCore::Get_DataDrivenWeight_EE(vector<snu::KElectron> k_electrons ,  double rho,  bool usetight,  double dxy, double biso, double eiso, TString cut, int type){

  float ee_weight = 0.;
  if(k_electrons.size()==2){
    
    bool is_el1_tight    = IsTight(k_electrons.at(0),  rho, dxy, biso, eiso, usetight);
    bool is_el2_tight    = IsTight(k_electrons.at(1),  rho, dxy, biso, eiso, usetight);

    vector<TLorentzVector> electrons=MakeTLorentz(k_electrons);
    ee_weight =m_fakeobj->get_dilepton_ee_eventweight(electrons, is_el1_tight,is_el2_tight, cut, type);
        
  }
  return ee_weight;

}

float AnalyzerCore::Get_DataDrivenWeight_EE(vector<snu::KElectron> k_electrons, double rho, double dxy, double biso, double eiso, TString cut){
  return Get_DataDrivenWeight_EE(k_electrons, rho, dxy, biso, eiso, cut, 0);
}

float AnalyzerCore::Get_DataDrivenWeight_EE(vector<snu::KElectron> k_electrons, double rho, double dxy, double biso, double eiso, TString cut, int type){
  
  float ee_weight = 0.;
  if(k_electrons.size()==2){
    
    bool is_el1_tight    = IsTight(k_electrons.at(0),  rho, dxy, biso, eiso, true);
    bool is_el2_tight    = IsTight(k_electrons.at(1),  rho, dxy, biso, eiso, true);
    
    vector<TLorentzVector> electrons=MakeTLorentz(k_electrons);
    ee_weight =m_fakeobj->get_dilepton_ee_eventweight(electrons, is_el1_tight,is_el2_tight, cut, type);
    
  }
  return ee_weight;
}



float  AnalyzerCore::Get_DataDrivenWeight_E(vector<snu::KElectron> k_electrons, int njets, int nbjets, double rho, double dxy, double biso, double eiso, bool    usetight,TString cut, bool applypucorr){
  
  if(k_electrons.size()==1){
    bool is_el1_tight    = IsTight(k_electrons.at(0), rho, dxy, biso, eiso,  usetight);
    vector<TLorentzVector> electrons=MakeTLorentz(k_electrons);
    
    TString rcut = cut;
    
    float r = 1.;


    float f = m_fakeobj->getFakeRate_electronEta(0, k_electrons.at(0).Pt(), fabs(k_electrons.at(0).Eta()), cut);
    
    float w = m_fakeobj->lepton_weight(!is_el1_tight, r,f);
    return w;
  }
  return 0.;
}
    


vector<TLorentzVector> AnalyzerCore::MakeTLorentz(vector<snu::KElectron> el){

  vector<TLorentzVector> tl_el;
  for(vector<KElectron>::iterator itel=el.begin(); itel!=el.end(); ++itel) {
    TLorentzVector tmp_em;
    tmp_em.SetPtEtaPhiM((*itel).Pt(),(*itel).Eta(),(*itel).Phi(),(*itel).M());
    tl_el.push_back(tmp_em);
  }
  return tl_el;
}

vector<TLorentzVector> AnalyzerCore::MakeTLorentz(vector<snu::KMuon> mu){
  
  vector<TLorentzVector> tl_mu;
  for(vector<KMuon>::iterator itmu=mu.begin(); itmu!=mu.end(); ++itmu) {
    TLorentzVector tmp_mu;
    tmp_mu.SetPtEtaPhiM((*itmu).Pt(),(*itmu).Eta(),(*itmu).Phi(),(*itmu).M());
    tl_mu.push_back(tmp_mu);
  }
  return tl_mu;
}


vector<TLorentzVector> AnalyzerCore::MakeTLorentz(vector<snu::KJet> j){

  vector<TLorentzVector> tl_jet;
  for(vector<KJet>::iterator itj=j.begin(); itj!=j.end(); ++itj) {
    TLorentzVector tmp_j;
    tmp_j.SetPtEtaPhiM((*itj).Pt(),(*itj).Eta(),(*itj).Phi(),(*itj).M());
    tl_jet.push_back(tmp_j);
  }
  return tl_jet;
}





