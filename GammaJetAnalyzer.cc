/*
 * GammaJetAnalyzer.cc
 *
 * class to analyze photon - jet observables stored in a HiForest file.
 */

#include "GammaJetAnalyzer.h"

GammaJetAnalyzer::GammaJetAnalyzer() {

}

GammaJetAnalyzer::GammaJetAnalyzer(TFile* hiForestFile, collisionType coll) {

    this->hiForestFile = hiForestFile;
    collision = coll;

    Constructor();
}


GammaJetAnalyzer::GammaJetAnalyzer(TString hiForestFileName, collisionType coll) {

    this->hiForestFile = new TFile(hiForestFileName.Data());
    collision = coll;

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
    if (collision == PP) {
        setJetTree(ak3PFJets);
    }
    else {
        setJetTree(akPu3PFJets);
    }

    resetCuts();
    updateSelections();
    updateEventList();
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
    cut_pcollisionEventSelection = pcollisionEventSelection;

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
    cut_sumIso  = sumIso;
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

    ////////// cuts for TEventList (external cuts) //////////
    cut_skip_event = skip_event;
}

void GammaJetAnalyzer::updateSelections() {

    updateEventSelections();
    updatePhotonSelections();
    updateJetSelections();
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

    cond_pt  = Form("pt > %f", cut_pt);
    cond_eta = Form("abs(eta) < %f", cut_eta);

    cond_spike = Form("swissCrx < %f", cut_swissCross);
    cond_spike +=Form(" && abs(seedTime) < %f", cut_seedTime);
    cond_spike +=Form(" && sigmaIetaIeta > %f", cut_sigmaIetaIeta_gt);
    cond_spike +=Form(" && sigmaIphiIphi > %f", cut_sigmaIphiIphi);

    cond_iso_noHI = Form("ecalRecHitSumEtConeDR04 < %f", cut_ecalIso);
    cond_iso_noHI +=Form(" && hcalTowerSumEtConeDR04 < %f", cut_hcalIso);
    cond_iso_noHI +=Form(" && trkSumPtHollowConeDR04 < %f", cut_trackIso);

    cond_iso_HI = Form("(cr4 + cc4 + ct4PtCut20)/0.9 < %f", cut_sumIso);


    if(collision == PP || collision == PA) {
        cond_iso = cond_iso_noHI.Data();
    }
    else {
        cond_iso = cond_iso_HI.Data();
    }
    cond_iso +=Form(" && hadronicOverEm < %f", cut_hadronicOverEm);

    cond_purity = Form("sigmaIetaIeta < %f", cut_sigmaIetaIeta_lt);

    cond_isEle = Form("isEle <= %d", cut_isEle);

    cond_pt_eta = cond_pt.Data();
    cond_pt_eta += Form(" && %s", cond_eta.Data());
    // default photon selection applies all the photon cuts
    cond_photon = cond_pt_eta.Data();
    cond_photon += Form(" && %s", cond_spike.Data());
    cond_photon += Form(" && %s", cond_iso.Data());
    cond_photon += Form(" && %s", cond_purity.Data());
}

void GammaJetAnalyzer::updateJetSelections() {

    cond_jet_pt  = Form("jtpt > %f", cut_jet_pt);
    cond_jet_eta = Form("abs(jteta) < %f", cut_jet_eta);

    // simplest expression so far to get the correct abs(dphi_photon_jet)
    // "dphi_photon_jet" does not necessarily give the correct delta phi between photon and jet
    // But "abs(dphi_photon_jet)" gives the correct abs(delta phi) between photon and jet
    dphi_photon_jet = "(abs(MAXPHOTONPHI-jtphi) + (abs(MAXPHOTONPHI-jtphi) > 3.141592653589)*(-6.283185307178))";
    deta = "(MAXPHOTONETA-jteta)";
    dR = Form("sqrt((%s)^2 + (%s)^2)", dphi_photon_jet.Data(), deta.Data());

    cond_jet_dphi   = Form("abs(%s) >= %f", dphi_photon_jet.Data(), cut_jet_photon_deltaPhi);
    cond_jet_deltaR = Form(" %s > %f", dR.Data(), cut_jet_photon_deltaR);

    // default jet selection applies all the jet cuts
    cond_jet = cond_jet_pt.Data();
    cond_jet += Form(" && %s", cond_jet_eta.Data());
    cond_jet += Form(" && %s", cond_jet_dphi.Data());
    cond_jet += Form(" && %s", cond_jet_deltaR.Data());
}

void GammaJetAnalyzer::updateEventListSelections(){

    cond_skip_event = "";
    if(cut_skip_event > 0)
    {
        cond_skip_event += "Entry$ %";
        cond_skip_event += Form("%d == 0", cut_skip_event);
    }

    cond_eventlist = cond_skip_event.Data();

    if(cond_eventlist.Length() <= 0)
        cond_eventlist = "1";       // default event list
}

void GammaJetAnalyzer::updateEventList(){

    updateEventListSelections();
    updateEventList(cond_eventlist);
}


void GammaJetAnalyzer::resetEventList() {

    cond_eventlist = "1";       // default event list
    updateEventList(cond_eventlist);
}

void GammaJetAnalyzer::updateEventList(TString condition){

    if(condition.Length() <= 0)
        condition = "1";

    const char* elist_tmp_name="elist_tmp";
    tree->Draw(Form(">> %s", elist_tmp_name) , condition);
    eventlist = ((TEventList*)gDirectory->Get(elist_tmp_name)->Clone("eventlist_GammaJetAnalyzer"));
    gDirectory->Delete(elist_tmp_name);

    jetTree->SetEventList(eventlist);
    tree->SetEventList(eventlist);
}

void GammaJetAnalyzer::setAliases(TString cond_photon, TString cond_jet)
{
    tree->SetAlias(alias_photon_pt,  Form("pt*(%s)", cond_photon.Data()));
    tree->SetAlias(alias_photon_eta, Form("eta*(%s)", cond_photon.Data()));
    tree->SetAlias(alias_photon_phi, Form("phi*(%s)", cond_photon.Data()));

    tree->SetAlias(alias_maxphoton_pt,  Form("Max$(pt*(%s))", cond_photon.Data()));
    tree->SetAlias(alias_maxphoton_eta, Form("Sum$(eta*(pt == Max$(pt*(%s))))", cond_photon.Data()));
    tree->SetAlias(alias_maxphoton_phi, Form("Sum$(phi*(pt == Max$(pt*(%s))))", cond_photon.Data()));

    tree->SetAlias(alias_jet_pt,  Form("jtpt*(%s)",  cond_jet.Data()));
    tree->SetAlias(alias_jet_eta, Form("jteta*(%s)", cond_jet.Data()));
    tree->SetAlias(alias_jet_phi, Form("jtphi*(%s)", cond_jet.Data()));

    tree->SetAlias(alias_maxjet_pt,  Form("Max$(jtpt*(%s))", cond_jet.Data()));
    tree->SetAlias(alias_maxjet_eta, Form("Sum$(jteta*(jtpt == Max$(jtpt*(%s))))", cond_jet.Data()));
    tree->SetAlias(alias_maxjet_phi, Form("Sum$(jtphi*(jtpt == Max$(jtpt*(%s))))", cond_jet.Data()));
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
    drawMaxJet(jetFormula, formulaForJetMax, cond, cond_photon, "1", hist);
}

void GammaJetAnalyzer::drawMaxJet(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TString cut, TH1* hist)
{
    TEventList* elist = getMaximumEventList(tree, "pt", cond_photon.Data(), cut);   // list of events that passes the photon selection
    elist->SetName("eventlistMax_Photons");     // prevent duplicate names in the current directory
    tree->SetEventList(elist);

    setAliases(cond_photon, "1");

    drawMaximumGeneral(tree, jetFormula, formulaForJetMax, cond, hist);

    tree->SetEventList(eventlist);      // restore the original event list after making the histogram
    elist->Delete();
}

void GammaJetAnalyzer::drawMaxJet2nd(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TH1* hist)
{
    drawMaxJet2nd(jetFormula, formulaForJetMax, cond, cond_photon, "1", hist);
}

void GammaJetAnalyzer::drawMaxJet2nd(TString jetFormula, TString formulaForJetMax, TString cond, TString cond_photon, TString cut, TH1* hist)
{
    TEventList* elist = getMaximumEventList(tree, "pt", cond_photon.Data(), cut);   // list of events that passes the photon selection
    elist->SetName("eventlistMax_Photons");     // prevent duplicate names in the current directory
    tree->SetEventList(elist);

    setAliases(cond_photon, "1");

    drawMaximum2ndGeneral(tree, jetFormula, formulaForJetMax, cond, hist);

    tree->SetEventList(eventlist);      // restore the original event list after making the histogram
    elist->Delete();
}

void GammaJetAnalyzer::drawPhotonJet(TString formula, TString cond_photon, TString cond_jet, TString selection, TH1* hist)
{
    drawPhotonJet(formula, cond_photon, cond_jet, selection, "1", hist);
}

void GammaJetAnalyzer::drawPhotonJet(TString formula, TString cond_photon, TString cond_jet, TString selection, TString cut, TH1* hist)
{

    const char* histName="h_drawPhotonJet";
    if(hist!=NULL)
    {
       histName = hist->GetName();
    }

    drawPhotonJet(formula, cond_photon, cond_jet, selection, cut, histName);
}

void GammaJetAnalyzer::drawPhotonJet(TString formula, TString cond_photon, TString cond_jet, TString selection, TString histName)
{
    drawPhotonJet(formula, cond_photon, cond_jet, selection, "1", histName);
}

/*
 * selection = object (photon, jet, ...) selection applied in the drawing step. Do not merge selection for different objects into "selection",
 * because this will take the binary "AND" of these different selections.
 * Example : selection = cond_photon && cond_jet
 * Say normally photons at indices 1, 3 pass "cond_photon" and jets at indices 0, 2 pass "cond_jet".
 * In that case no photon or jet passes "selection".
 * "selection" must contain selections for at most one object
 * and should better be used for selection of a "list", not a single object.
 */
void GammaJetAnalyzer::drawPhotonJet(TString formula, TString cond_photon, TString cond_jet, TString selection, TString cut, TString histName)
{
    setAliases(cond_photon, cond_jet);

    const char* eventlist_name="eventlistPhotonJet";
    tree->Draw(Form(">> %s", eventlist_name), Form("Max$(%s)>0 && Max$(%s)>0 && %s",cond_photon.Data() ,cond_jet.Data() ,cut.Data()));
    // why use "Max$(%s)>0" instead of "%s"
    // conditions for photon and jet should not be binary "AND"ed. Evaluate photon and jet conditions separately.
    TEventList* elist = (TEventList*)gDirectory->Get(eventlist_name);
    tree->SetEventList(elist);

    tree->Draw(Form("%s >> %s", formula.Data(), histName.Data()), selection.Data());

    tree->SetEventList(eventlist);      // restore the original event list after making the histogram
    elist->Delete();
}

// construct formula for a specific deltaR (photon, jet)
TString GammaJetAnalyzer::constructFormula_dR(TString eta1, TString phi1, TString eta2, TString phi2)
{
    TString dphi = Form("(abs(%s-%s) + (abs(%s-%s) > 3.141592653589)*(-6.283185307178))", phi1.Data(), phi2.Data(), phi1.Data(), phi2.Data());
    TString deta = Form("(%s-%s)",eta1.Data(), eta2.Data());
    return Form("sqrt((%s)^2 + (%s)^2)", dphi.Data(), deta.Data());
}

// construct formula for a specific deltaPhi (photon, jet)
TString GammaJetAnalyzer::constructFormula_dphi(TString phi1, TString phi2)
{
    return  Form("abs((abs(%s-%s) + (abs(%s-%s) > 3.141592653589)*(-6.283185307178)))", phi1.Data(), phi2.Data(), phi1.Data(), phi2.Data());
}

// construct formula for a specific deltaEta (photon, jet)
TString GammaJetAnalyzer::constructFormula_deta(TString eta1, TString eta2)
{
    return Form("abs((%s-%s))",eta1.Data(), eta2.Data());
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

