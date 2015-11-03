/*
 * GammaJetAnalyzer.h
 *
 * class to analyze photon - jet observables stored a HiForest file.
 */

#ifndef GAMMAJETANALYZER_H_
#define GAMMAJETANALYZER_H_


#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TEventList.h>
#include <TH1D.h>
#include <TMath.h>

#include <iostream>

#include "treeUtil.h"

#define PI 3.141592653589

enum jetType {
    ak3PFJets,
    akPu3PFJets
};

enum collisionType {
    PP,
    PA,
    HI
};

////////// default cuts for event //////////
const float vz = 15;
const int hiBin_gt = -1;
const int hiBin_lt = 9999;
const float hf4sum_gt = -1;
const float hf4sum_lt = 9999;
const int pHBHENoiseFilter = 0;
const int pPAcollisionEventSelectionPA = 0;
const int pcollisionEventSelection = 0;
const int skip_event = -1;

////////// default cuts for photons //////////
const float pt = 40;
const float eta = 1.44;
// spike rejection
const float sigmaIphiIphi = 0.002;
const float sigmaIetaIeta_gt = 0.002;
const float swissCross = 0.9;
const float seedTime = 3;
// Isolation
const float ecalIso = 4.2;     // for pp or pA events
const float hcalIso = 2.2;     // for pp or pA events
const float trackIso = 2.0;    // for pp or pA events
const float sumIso   = 1.0;    // for HI events, sumIso = cr4 + cc4 + ct4PtCut20
const float hadronicOverEm = 0.1;
// purity enhancement
const float sigmaIetaIeta_lt = 0.01;
// is Electron
const int isEle = 0;
////////// default cuts for photons // END ///

////////// default cuts for jets //////////
const float jet_pt = 30;
const float jet_eta = 3;
const float jet_photon_deltaR = 0.4;
const float jet_photon_deltaPhi = PI * 7./8.;
////////// default cuts for jets // END ///

////////// aliases //////////
// photon aliases - alias photons that satisfy a condition
const char* alias_photon_pt  = "PHOTONPT";
const char* alias_photon_eta = "PHOTONETA";
const char* alias_photon_phi = "PHOTONPHI";
// photon aliases - alias for the photon with maximum pT among the photons that satisfy a condition
const char* alias_maxphoton_pt  = "MAXPHOTONPT";
const char* alias_maxphoton_eta = "MAXPHOTONETA";
const char* alias_maxphoton_phi = "MAXPHOTONPHI";
// jet aliases - alias for jets that satisfy a condition
const char* alias_jet_pt   = "JETPT";
const char* alias_jet_eta  = "JETETA";
const char* alias_jet_phi  = "JETPHI";
// jet aliases - alias for the jet with maximum pT among the jets that satisfy a condition
const char* alias_maxjet_pt   = "MAXJETPT";
const char* alias_maxjet_eta  = "MAXJETETA";
const char* alias_maxjet_phi  = "MAXJETPHI";

class GammaJetAnalyzer {
private:
    TTree* ak3PFJetTree;        // for pp events
    TTree* akPu3PFJetTree;      // for pA or HI events

    // special selections
    TString dphi_photon_jet;
    TString deta;
    TString dR;

    void Constructor();         // assume "constructor delegation" is not implemented.
                                // a constructor does not call another constructor,
                                // but uses "Constructor()" to do the reduntant part of the object construction.
    void updateEventSelections();
    void updatePhotonSelections();
    void updateJetSelections();
    void updateEventListSelections();

public:
    GammaJetAnalyzer();
    GammaJetAnalyzer(TFile* hiForestFile, collisionType coll = HI);
    GammaJetAnalyzer(TString hiForestFileName, collisionType coll = HI);
    void setJetTree(jetType jet);
    virtual ~GammaJetAnalyzer();

    // set selections/cuts
    void resetCuts();
    void updateSelections();
    void updateEventList();
    void resetEventList();
    void updateEventList(TString condition);
    void setAliases(TString cond_photon = "1", TString cond_jet = "1");
    // draw kinematics
    void drawMax   (TString formula, TString formulaForMax, TString condition = "1", TH1* hist = NULL);
    void drawMax   (TString formula, TString formulaForMax, TString condition = "1", TString cut = "1", TH1* hist = NULL);
    void drawMax2nd(TString formula, TString formulaForMax, TString condition = "1", TH1* hist = NULL);
    void drawMax2nd(TString formula, TString formulaForMax, TString condition = "1", TString cut = "1", TH1* hist = NULL);
    // draw jet kinematics
    void drawMaxJet   (TString jetFormula, TString formulaForJetMax, TString cond = "1", TString cond_photon = "1", TH1* hist = NULL);
    void drawMaxJet   (TString jetFormula, TString formulaForJetMax, TString cond = "1", TString cond_photon = "1", TString cut = "1", TH1* hist = NULL);
    void drawMaxJet2nd(TString jetFormula, TString formulaForJetMax, TString cond = "1", TString cond_photon = "1", TH1* hist = NULL);
    void drawMaxJet2nd(TString jetFormula, TString formulaForJetMax, TString cond = "1", TString cond_photon = "1", TString cut = "1", TH1* hist = NULL);
    // draw photon-jet kinematics/correlations
    void drawPhotonJet(TString formula, TString cond_photon = "1", TString cond_jet = "1", TString selection = "1", TH1* hist = NULL);
    void drawPhotonJet(TString formula, TString cond_photon = "1", TString cond_jet = "1", TString selection = "1", TString cut = "1", TH1* hist = NULL);
    void drawPhotonJet(TString formula, TString cond_photon = "1", TString cond_jet = "1", TString selection = "1", TString histName = "h_drawPhotonJet");
    void drawPhotonJet(TString formula, TString cond_photon = "1", TString cond_jet = "1", TString selection = "1", TString cut = "1", TString histName = "h_drawPhotonJet");
    // construct strings
    TString constructFormula_dR(TString eta1 = alias_maxphoton_eta, TString phi1 = alias_maxphoton_phi, TString eta2 = alias_maxjet_eta, TString phi2 = alias_maxjet_phi);
    TString constructFormula_dphi(TString phi1 = alias_maxphoton_phi, TString phi2 = alias_maxjet_phi);
    TString constructFormula_deta(TString eta1 = alias_maxphoton_eta, TString eta2 = alias_maxjet_eta);
    // merge cuts
    static TString mergeSelections(TString sel1, TString sel2);

    TFile* hiForestFile;
    // Trees
    TTree* tree;    // tree for the whole HiForest file, every tree is friend to this tree.
    TTree* evtTree;
    TTree* skimTree;
    TTree* photonTree;
    TTree* jetTree;
    // EventList
    TEventList* eventlist;
    // Histograms
    TH1D* h;

    // collision info
    collisionType collision;

    ////////// cuts for event //////////
    float cut_vz;                               // evtTree
    int cut_hiBin_gt;                           // evtTree  : centrality for HI events
    int cut_hiBin_lt;                           // evtTree  : centrality for HI events
    float cut_hf4sum_gt;                        // evtTree  : centrality for pA events, hf4sum = hiHFplusEta4 + hiHFminusEta4
    float cut_hf4sum_lt;                        // evtTree  : centrality for pA events, hf4sum = hiHFplusEta4 + hiHFminusEta4
    int cut_pHBHENoiseFilter;                   // skimTree
    int cut_pPAcollisionEventSelectionPA;       // skimTree : for pp or pA events
    int cut_pcollisionEventSelection;           // skimTree : for HI events
    ////////// cuts for event // END ///
    ////////// cuts for photons //////////
    float cut_pt;
    float cut_eta;
    // spike rejection
    float cut_swissCross;
    float cut_seedTime;
    float cut_sigmaIetaIeta_gt;
    float cut_sigmaIphiIphi;
    // Isolation
    float cut_ecalIso;      // for pp or pA events
    float cut_hcalIso;      // for pp or pA events
    float cut_trackIso;     // for pp or pA events
    float cut_sumIso;       // for HI events,  sumIso = cr4 + cc4 + ct4PtCut20
    float cut_hadronicOverEm;
    // purity enhancement
    float cut_sigmaIetaIeta_lt;
    // is Electron
    int   cut_isEle;
    ////////// cuts for photons // END ///
    ////////// cuts for jets //////////
    float cut_jet_pt;
    float cut_jet_eta;
    float cut_jet_photon_deltaR;
    float cut_jet_photon_deltaPhi;
    ////////// cuts for jets // END ///
    ////////// cuts for TEventList (external cuts) //////////
    int cut_skip_event;                         // after the first event, ignore every "skip_event" many events.
                                                // faster results with less data
    ////////// cuts for TEventList (external cuts) // END ///

    // selections are in TTree::Draw() function to perform the selection
    ////////// selections for event //////////
    TString cond_event;     // selection for the event, default is cond_vz
    TString cond_vz;
    TString cond_hiBin;     // for HI events
    TString cond_hf4sum;    // for pA events
    TString cond_noise;
    TString cond_pPAcollisionEventSelectionPA;
    TString cond_pcollisionEventSelection;
    ////////// selections for photons //////////
    TString cond_photon;    // selection for the photons
    TString cond_pt;
    TString cond_eta;
    TString cond_pt_eta;    // cond_pt + cond_eta
    TString cond_spike;
    TString cond_iso;
    TString cond_iso_noHI;  // for pp or pA events
    TString cond_iso_HI;    // for HI events
    TString cond_purity;
    TString cond_isEle;     // not included in cond_photon
    ////////// selections for jets //////////
    TString cond_jet;    // selection for the jets
    TString cond_jet_pt;
    TString cond_jet_eta;
    TString cond_jet_deltaR;
    TString cond_jet_dphi;
    ////////// selections for TEventList (external cuts) //////////
    TString cond_eventlist;
    TString cond_skip_event;

};

#endif /* GAMMAJETANALYZER_H_ */
