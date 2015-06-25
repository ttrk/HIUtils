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
#include <TMath.h>

#include <iostream>

#include "treeUtil.h"

enum jetType {
    ak3PFJets,
    akPu3PFJets
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

////////// default cuts for photons //////////
const float pt = 40;
const float eta = 1.44;
// spike rejection
const float sigmaIphiIphi = 0.002;
const float sigmaIetaIeta_gt = 0.002;
const float swissCross = 0.9;
const float seedTime = 3;
// Isolation
const float ecalIso = 4.2;
const float hcalIso = 2.2;
const float trackIso = 2.0;
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
const float jet_photon_deltaPhi = (7 * TMath::Pi()) / 8 ;   // 7/8 * TMath::Pi() --> evaluates to 0;
////////// default cuts for jets // END ///


class GammaJetAnalyzer {
private:
    TTree* ak3PFJetTree;        // for pp events
    TTree* akPu3PFJetTree;      // for pA or HI events

    void Constructor();         // assume "constructor delegation" is not implemented.
                                // a constructor does not call another constructor,
                                // but uses "Constructor()" to do the reduntant part of the object construction.
public:
    GammaJetAnalyzer();
    GammaJetAnalyzer(TFile* hiForestFile);
    GammaJetAnalyzer(TString hiForestFileName);
    void setJetTree(jetType jet);
    virtual ~GammaJetAnalyzer();

    // set selections/cuts
    void resetCuts();
    void updateEventSelections();
    void updatePhotonSelections();
    void updateJetSelections();
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

    // merge cuts
    static TString mergeSelections(TString sel1, TString sel2);


    TFile* hiForestFile;
    // Trees
    TTree* tree;    // tree for the whole HiForest file, every tree is friend to this tree.
    TTree* evtTree;
    TTree* skimTree;
    TTree* photonTree;
    TTree* jetTree;
    // Histograms
    TH1D* h;

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
    float cut_ecalIso;
    float cut_hcalIso;
    float cut_trackIso;
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
    TString cond_spike;
    TString cond_iso;
    TString cond_purity;
    TString cond_isEle;     // not included in cond_photon
    ////////// selections for jets //////////
    TString cond_jet;    // selection for the jets
    TString cond_jet_deltaR;
    TString cond_jet_dphi;

};

#endif /* GAMMAJETANALYZER_H_ */
