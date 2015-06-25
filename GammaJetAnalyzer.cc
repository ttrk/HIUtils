/*
 * GammaJetAnalyzer.cc
 *
 * class to analyze photon - jet observables stored a HiForest file.
 */

#include "GammaJetAnalyzer.h"

GammaJetAnalyzer::GammaJetAnalyzer() {

}

GammaJetAnalyzer::GammaJetAnalyzer(TFile* hiForestFile) {

    this->hiForestFile = hiForestFile;

    Constructor();
}


GammaJetAnalyzer::GammaJetAnalyzer(TString hiForestFileName) {

    this->hiForestFile = new TFile(hiForestFileName.Data());

    Constructor();
}

/*
 * assume "constructor delegation" is not implemented.
 * a constructor does not call another constructor,
 * but uses "Constructor()" to do the reduntant part of the object construction.
 */
void GammaJetAnalyzer::Constructor(){

    evtTree        = (TTree*)this->hiForestFile->Get("hiEvtAnalyzer/HiTree");
    skimTree       = (TTree*)this->hiForestFile->Get("skimanalysis/HltTree");
    photonTree     = (TTree*)this->hiForestFile->Get("multiPhotonAnalyzer/photon");
    ak3PFJetTree   = (TTree*)this->hiForestFile->Get("ak3PFJetAnalyzer/t");
    akPu3PFJetTree = (TTree*)this->hiForestFile->Get("akPu3PFJetAnalyzer/t");

    tree = evtTree;
    tree->AddFriend(skimTree,"HltTree");
    tree->AddFriend(photonTree,"photon");
    setJetTree(akPu3PFJets);     // default jets are akPu3PFJets

    resetCuts();
    updateEventSelections();
    updatePhotonSelections();
    updateJetSelections();
}

void GammaJetAnalyzer::setJetTree(jetType jet) {

    // jetTree is not a stand-alone object,
    // it will not be cloned from a tree such as : jetTree = (TTree*)ak3PFJetTree->Clone();
    // jetTree points to the specified tree. Any change in "jetTree" will appear in the tree it points to.
    tree->RemoveFriend(jetTree);

    if(jet == ak3PFJets)  {
        jetTree = ak3PFJetTree;
    }
    else if (jet == akPu3PFJets) {
        jetTree = akPu3PFJetTree;
    }
    else   {
        std::cout << "not a valid jet type entered." << std::endl;
    }

    tree->AddFriend(jetTree, "t");
}

void GammaJetAnalyzer::resetCuts(){

    ////////// cuts for event //////////
    cut_vz = vz;
    cut_hiBin_gt = hiBin_gt;
    cut_hiBin_lt = hiBin_lt;
    cut_hf4sum_gt = hf4sum_gt;
    cut_hf4sum_lt = hf4sum_lt;
    cut_pHBHENoiseFilter = pHBHENoiseFilter;
    cut_pPAcollisionEventSelectionPA = pPAcollisionEventSelectionPA;
    cut_pcollisionEventSelection =pcollisionEventSelection;

    ////////// cuts for photons //////////
    cut_pt = pt;
    cut_eta = eta;
    // spike rejection
    cut_swissCross = swissCross;
    cut_seedTime = seedTime;
    cut_sigmaIetaIeta_gt = sigmaIetaIeta_gt;
    cut_sigmaIphiIphi = sigmaIphiIphi;
    // Isolation
    cut_ecalIso = ecalIso;
    cut_hcalIso = hcalIso;
    cut_trackIso = trackIso;
    cut_hadronicOverEm = hadronicOverEm;
    // purity enhancement
    cut_sigmaIetaIeta_lt = sigmaIetaIeta_lt;
    // is Electron
    cut_isEle = isEle;

    ////////// cuts for jets //////////
    cut_jet_pt = jet_pt;
    cut_jet_eta = jet_eta;
    cut_jet_photon_deltaR = jet_photon_deltaR;
    cut_jet_photon_deltaPhi = jet_photon_deltaPhi;
}

void GammaJetAnalyzer::updateEventSelections() {

    cond_vz = Form("abs(vz) < %f", cut_vz);
    cond_hiBin  = Form("hiBin > %d && hiBin < %d", cut_hiBin_gt, cut_hiBin_lt);
    cond_hf4sum = Form("(hiHFplusEta4+hiHFminusEta4) > %f && (hiHFplusEta4+hiHFminusEta4) < %f", cut_hf4sum_gt, cut_hf4sum_lt);

    cond_noise = Form("pHBHENoiseFilter > %d", cut_pHBHENoiseFilter);
    cond_pPAcollisionEventSelectionPA = Form("pPAcollisionEventSelectionPA > %d", cut_pPAcollisionEventSelectionPA);
    cond_pcollisionEventSelection = Form("pcollisionEventSelection > %d", cut_pcollisionEventSelection);

    cond_event = cond_vz.Data();
}

void GammaJetAnalyzer::updatePhotonSelections() {

    cond_pt  = Form("pt > %f", cut_jet_pt);
    cond_eta = Form("abs(eta) < %f", cut_eta);

    cond_spike = Form("swissCrx < %f", cut_swissCross);
    cond_spike +=Form(" && abs(seedTime) < %f", cut_seedTime);
    cond_spike +=Form(" && sigmaIetaIeta > %f", cut_sigmaIetaIeta_gt);
    cond_spike +=Form(" && sigmaIphiIphi > %f", cut_sigmaIphiIphi);

    cond_iso = Form("ecalRecHitSumEtConeDR04 < %f", cut_ecalIso);
    cond_iso +=Form(" && hcalTowerSumEtConeDR04 < %f", cut_hcalIso);
    cond_iso +=Form(" && trkSumPtHollowConeDR04 < %f", cut_trackIso);
    cond_iso +=Form(" && hadronicOverEm < %f", cut_hadronicOverEm);

    cond_purity = Form("sigmaIetaIeta < %f", cut_sigmaIetaIeta_lt);

    cond_isEle = Form("isEle <= %d", cut_isEle);

    // default photon selection applies all the photon cuts
    cond_photon = cond_eta.Data();
    cond_photon += Form(" && %s", cond_spike.Data());
    cond_photon += Form(" && %s", cond_iso.Data());
    cond_photon += Form(" && %s", cond_purity.Data());
}

void GammaJetAnalyzer::updateJetSelections() {

    cond_jet = Form("jtpt > %f", cut_jet_pt);
    cond_jet += Form(" && abs(jteta) < %f", cut_jet_eta);

    cond_jet_deltaR = Form(" && IMPLEMENT_THIS > %f", cut_jet_photon_deltaR);
    cond_jet_dphi = Form("IMPLEMENT_THIS > %f", cut_jet_photon_deltaPhi);

//    cond_jet += Form(" && %s", cond_jet_dphi.Data());
}

void GammaJetAnalyzer::drawMax(TString formula, TString formulaForMax, TString condition, TH1* hist){
    drawMaximumGeneral(tree, formula, formulaForMax, condition, hist);
}

void GammaJetAnalyzer::drawMax(TString formula, TString formulaForMax, TString condition, TString cut, TH1* hist){
    drawMaximumGeneral(tree, formula, formulaForMax, condition, cut, hist);
}

void GammaJetAnalyzer::drawMax2nd(TString formula, TString formulaForMax, TString condition, TH1* hist){
    drawMaximum2ndGeneral(tree, formula, formulaForMax, condition, hist);
}

void GammaJetAnalyzer::drawMax2nd(TString formula, TString formulaForMax, TString condition, TString cut, TH1* hist){
    drawMaximum2ndGeneral(tree, formula, formulaForMax, condition, cut, hist);
}

void GammaJetAnalyzer::drawMaxJet(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TH1* hist)
{
    drawMaximumGeneral(tree, jetFormula, formulaForJetMax, cond, Form("Max$(%s)>0", cond_photon.Data()), hist);
}

void GammaJetAnalyzer::drawMaxJet(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TString cut, TH1* hist)
{
    drawMaximumGeneral(tree, jetFormula, formulaForJetMax, cond, mergeSelections(Form("Max$(%s)>0", cond_photon.Data()), cut), hist);
}

void GammaJetAnalyzer::drawMaxJet2nd(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TH1* hist)
{
    drawMaximum2ndGeneral(tree, jetFormula, formulaForJetMax, cond, Form("Max$(%s)>0", cond_photon.Data()), hist);
}

void GammaJetAnalyzer::drawMaxJet2nd(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TString cut, TH1* hist)
{
    drawMaximum2ndGeneral(tree, jetFormula, formulaForJetMax, cond, mergeSelections(Form("Max$(%s)>0", cond_photon.Data()), cut), hist);
}

// no need to use "static" keyword in function definition after it has been used in function declaration
TString GammaJetAnalyzer::mergeSelections(TString sel1, TString sel2)
{
    // this general purpose function was already implemented in "treeUtil.h"
    return mergeCuts(sel1, sel2);
}

GammaJetAnalyzer::~GammaJetAnalyzer() {

    if(hiForestFile->IsOpen())   {
        hiForestFile->Close();
    }
}

