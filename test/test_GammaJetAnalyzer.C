/*
 * test_GammaJetAnalyzer.C
 *
 * macro to test GammaJetAnalyzer class
 *  1. histograms by GammayJetAnalyzer
 */

#include "../GammaJetAnalyzer.h"
#include "../GammaJetAnalyzer.cc"   // need to use this include if this macro and "GammaJetAnalyzer.h" are not in the same directory.
#include "../histoUtil.h"
#include "../smallPhotonUtil.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>

#include <iostream>
#include <ctime>
#include <iomanip>

enum collisionType {
    pp,
    NOTpp       // pPb or PbPb
};

void test_GammaJetAnalyzer(const char* inputfileName = "/mnt/hadoop/cms/store/user/luck/2014-photon-forests/pPb_DATA_photon30trig_localJEC_v2.root", const char* outputFileName = "test_GammaJetAnalyzer.root", collisionType collision = pp);

const int MAXPHOTONS = 500;
const int MAXJETS = 500;

void test_GammaJetAnalyzer(const char* inputfileName , const char* outputFileName, collisionType collision)
{
    TFile *inputFile = new TFile(inputfileName, "READ");
    std::cout << "input HiForest : " << inputFile->GetName() << std::endl;

    TTree *photonTree = (TTree*)inputFile->Get("multiPhotonAnalyzer/photon");
    TTree *evtTree = (TTree*)inputFile->Get("hiEvtAnalyzer/HiTree");
    TTree *skimTree = (TTree*)inputFile->Get("skimanalysis/HltTree");
    TTree *jetTree_ak3PF =(TTree*)inputFile->Get("ak3PFJetAnalyzer/t");
    TTree *jetTree_akPu3PF =(TTree*)inputFile->Get("akPu3PFJetAnalyzer/t");
    TTree *jetTree;

    std::cout << "collisionType = " << collision << std::endl;
    if(collision == pp)   {
        jetTree = jetTree_ak3PF;
        std::cout << "jetTree = ak3PF jets " << std::endl;
    }
    else {
        jetTree = jetTree_akPu3PF;
        std::cout << "jetTree = akPu3PF jets " << std::endl;
    }

//    Int_t f_evt, f_run, f_lumi;
    Float_t vz;
//    Int_t hiBin;
//    fEvtTree->SetBranchAddress("evt",&f_evt);
//    fEvtTree->SetBranchAddress("run",&f_run);
//    fEvtTree->SetBranchAddress("lumi",&f_lumi);
    evtTree->SetBranchAddress("vz",&vz);
//    evtTree->SetBranchAddress("hiBin",&hiBin);

//    Int_t pcollisionEventSelection, pHBHENoiseFilter;
//    skimTree->SetBranchAddress("pcollisionEventSelection",&pcollisionEventSelection);
//    skimTree->SetBranchAddress("pHBHENoiseFilter",&pHBHENoiseFilter);

    Int_t nPhoton;
    Float_t photon_pt[MAXPHOTONS];
    Float_t photon_eta[MAXPHOTONS];
    Float_t photon_phi[MAXPHOTONS];
    Float_t cc4[MAXPHOTONS];
    Float_t cr4[MAXPHOTONS];
    Float_t ct4PtCut20[MAXPHOTONS];
    Float_t trkSumPtHollowConeDR04[MAXPHOTONS];
    Float_t hcalTowerSumEtConeDR04[MAXPHOTONS];
    Float_t ecalRecHitSumEtConeDR04[MAXPHOTONS];
    Float_t hadronicOverEm[MAXPHOTONS];
    Float_t sigmaIetaIeta[MAXPHOTONS];
    Int_t isEle[MAXPHOTONS];
    Float_t sigmaIphiIphi[MAXPHOTONS];
    Float_t swissCrx[MAXPHOTONS];
    Float_t seedTime[MAXPHOTONS];

    photonTree->SetBranchAddress("nPhotons",&nPhoton);
    photonTree->SetBranchAddress("pt",photon_pt);
    photonTree->SetBranchAddress("eta",photon_eta);
    photonTree->SetBranchAddress("phi",photon_phi);

    photonTree->SetBranchAddress("cc4",cc4);
    photonTree->SetBranchAddress("cr4",cr4);
    photonTree->SetBranchAddress("ct4PtCut20",ct4PtCut20);
    photonTree->SetBranchAddress("trkSumPtHollowConeDR04",trkSumPtHollowConeDR04);
    photonTree->SetBranchAddress("hcalTowerSumEtConeDR04",hcalTowerSumEtConeDR04);
    photonTree->SetBranchAddress("ecalRecHitSumEtConeDR04",ecalRecHitSumEtConeDR04);
    photonTree->SetBranchAddress("hadronicOverEm",hadronicOverEm);
    photonTree->SetBranchAddress("sigmaIetaIeta",sigmaIetaIeta);
    photonTree->SetBranchAddress("isEle",isEle);
    photonTree->SetBranchAddress("sigmaIphiIphi",sigmaIphiIphi);
    photonTree->SetBranchAddress("swissCrx",swissCrx);
    photonTree->SetBranchAddress("seedTime",seedTime);

    Int_t nJet;
    Float_t jet_pt[MAXJETS];
    Float_t jet_eta[MAXJETS];
    Float_t jet_phi[MAXJETS];

    jetTree->SetBranchAddress("nref",&nJet);
    jetTree->SetBranchAddress("jtpt",jet_pt);
    jetTree->SetBranchAddress("jteta",jet_eta);
    jetTree->SetBranchAddress("jtphi",jet_phi);

//    inputFile->Close();
    TFile* outputFile=new TFile(outputFileName, "RECREATE");

    const int numHistos = 6;

    const char* histoSuffix[numHistos] =  {"",            // no selection
                                           "_event",      // event selection
                                           "_eta",        // event selection + eta cut
                                           "_spike",      // event selection + eta cut + spike rejection
                                           "_iso",        // event selection + eta cut + spike rejection + isolation
                                           "_purity"};    // event selection + eta cut + spike rejection + isolation + purity
    // eta cut includes photon pT > 40 GeV cut as well.

    // TH1D
    // leading photon histograms
    const int nBins = 1000;
    const float maxPt = 500;
    TH1D* fPt[numHistos];       // photon pT histograms for leading photon, to be filled by LOOP
    TH1D* fPt_gja[numHistos];   // photon pT histograms for leading photon, to be filled by GammaJetAnalyzer

    const float maxSigmaIetaIeta = 0.1;
    TH1D* fSigmaIetaIeta[numHistos];       // sigmaIetaIeta histograms for leading, to be filled by LOOP
    TH1D* fSigmaIetaIeta_gja[numHistos];   // sigmaIetaIeta histograms for leading, to be filled by GammaJetAnalyzer

    const float maxPhi = 3.5;
    TH1D* fPhi[numHistos];       // phi histograms for leading, to be filled by LOOP
    TH1D* fPhi_gja[numHistos];   // phi histograms for leading, to be filled by GammaJetAnalyzer

    const int maxIndex = 10;
    TH1D* fIndex[numHistos];       // photon index histograms for leading, to be filled by LOOP
    TH1D* fIndex_gja[numHistos];   // photon index histograms for leading, to be filled by GammaJetAnalyzer

    // subleading photon histograms
    TH1D* fPt_2nd[numHistos];       // photon pT histograms for subleading photon, to be filled by LOOP
    TH1D* fPt_2nd_gja[numHistos];   // photon pT histograms for subleading photon, to be filled by GammaJetAnalyzer

    TH1D* fSigmaIetaIeta_2nd[numHistos];       // sigmaIetaIeta histograms for subleading, to be filled by LOOP
    TH1D* fSigmaIetaIeta_2nd_gja[numHistos];   // sigmaIetaIeta histograms for subleading, to be filled by GammaJetAnalyzer

    TH1D* fPhi_2nd[numHistos];       // phi histograms for subleading, to be filled by LOOP
    TH1D* fPhi_2nd_gja[numHistos];   // phi histograms for subleading, to be filled by GammaJetAnalyzer

    // leading jet histograms
    TH1D* fJetPt[numHistos];       // jet pT histograms for leading jet, to be filled by LOOP
    TH1D* fJetPt_gja[numHistos];   // jet pT histograms for leading jet, to be filled by GammaJetAnalyzer

    TH1D* fJetPhi[numHistos];       // phi histograms for leading jet, to be filled by LOOP
    TH1D* fJetPhi_gja[numHistos];   // phi histograms for leading jet, to be filled by GammaJetAnalyzer

    TH1D* fJetIndex[numHistos];       // jet index histograms for leading jet, to be filled by LOOP
    TH1D* fJetIndex_gja[numHistos];   // jet index histograms for leading jet, to be filled by GammaJetAnalyzer

    // subleading jet histograms
    TH1D* fJetPt_2nd[numHistos];       // jet pT histograms for subleading jet, to be filled by LOOP
    TH1D* fJetPt_2nd_gja[numHistos];   // jet pT histograms for subleading jet, to be filled by GammaJetAnalyzer

    TH1D* fJetPhi_2nd[numHistos];       // phi histograms for subleading jet, to be filled by LOOP
    TH1D* fJetPhi_2nd_gja[numHistos];   // phi histograms for subleading jet, to be filled by GammaJetAnalyzer

    // TH2D
    // leading photon histograms
    const int nBins2D = nBins;
    TH2D* fPtvsSigmaIetaIeta[numHistos];       // photon pT vs. sigmaIetaIeta histograms for leading photon, to be filled by LOOP
    TH2D* fPtvsSigmaIetaIeta_gja[numHistos];   // photon pT vs. sigmaIetaIeta histograms for leading photon, to be filled by GammaJetAnalyzer

    const float maxEta = 2;
    TH2D* fPtvsEta[numHistos];       // photon pT vs. eta histograms for leading photon, to be filled by LOOP
    TH2D* fPtvsEta_gja[numHistos];   // photon pT vs. eta histograms for leading photon, to be filled by GammaJetAnalyzer

    // subleading photon histograms
    TH2D* fPtvsSigmaIetaIeta_2nd[numHistos];       // photon pT vs. sigmaIetaIeta histograms for subleading photon, to be filled by LOOP
    TH2D* fPtvsSigmaIetaIeta_2nd_gja[numHistos];   // photon pT vs. sigmaIetaIeta histograms for subleading photon, to be filled by GammaJetAnalyzer

    TH2D* fPtvsEta_2nd[numHistos];       // photon pT vs. eta histograms for subleading photon, to be filled by LOOP
    TH2D* fPtvsEta_2nd_gja[numHistos];    // photon pT vs. eta histograms for subleading photon, to be filled by GammaJetAnalyzer

    // leading jet histograms
    TH2D* fJetPtvsEta[numHistos];       // jet pT vs. eta histograms for leading jet, to be filled by LOOP
    TH2D* fJetPtvsEta_gja[numHistos];   // jet pT vs. eta histograms for leading jet, to be filled by GammaJetAnalyzer

    TH2D* fJetPtvsPt[numHistos];       // jet pT vs. leading photon pT histograms for leading jet, to be filled by LOOP
    TH2D* fJetPtvsPt_gja[numHistos];   // jet pT vs. leading photon pT histograms for leading jet, to be filled by GammaJetAnalyzer

    TH2D* fJetEtavsEta[numHistos];       // jet eta vs. leading photon eta histograms for leading jet, to be filled by LOOP
    TH2D* fJetEtavsEta_gja[numHistos];   // jet eta vs. leading photon eta histograms for leading jet, to be filled by GammaJetAnalyzer

    TH2D* fJetPhivsPhi[numHistos];       // jet phi vs. leading photon phi histograms for leading jet, to be filled by LOOP
    TH2D* fJetPhivsPhi_gja[numHistos];   // jet phi vs. leading photon phi histograms for leading jet, to be filled by GammaJetAnalyzer

    // subleading jet histograms
    TH2D* fJetPtvsEta_2nd[numHistos];       // jet pT vs. eta histograms for subleading jet, to be filled by LOOP
    TH2D* fJetPtvsEta_2nd_gja[numHistos];   // jet pT vs. eta histograms for subleading jet, to be filled by GammaJetAnalyzer

    TH2D* fJetPtvsPt_2nd[numHistos];       // jet pT vs. leading photon pT histograms for subleading jet, to be filled by LOOP
    TH2D* fJetPtvsPt_2nd_gja[numHistos];   // jet pT vs. leading photon pT histograms for subleading jet, to be filled by GammaJetAnalyzer

    TH2D* fJetEtavsEta_2nd[numHistos];       // jet eta vs. leading photon eta histograms for subleading jet, to be filled by LOOP
    TH2D* fJetEtavsEta_2nd_gja[numHistos];   // jet eta vs. leading photon eta histograms for subleading jet, to be filled by GammaJetAnalyzer

    TH2D* fJetPhivsPhi_2nd[numHistos];       // jet phi vs. leading photon phi histograms for subleading jet, to be filled by LOOP
    TH2D* fJetPhivsPhi_2nd_gja[numHistos];   // jet phi vs. leading photon phi histograms for subleading jet, to be filled by GammaJetAnalyzer

    for (int i=0; i<numHistos; ++i)
    {
        // TH1D
        // photons
        fPt[i] = new TH1D(Form("fPt%s", histoSuffix[i]),"leading photon;p_{T} (GeV)",nBins,0,maxPt);
        fPt_gja[i] = (TH1D*)fPt[i]->Clone(Form("%s_gja",fPt[i]->GetName()));

        fSigmaIetaIeta[i] = new TH1D(Form("fSigmaIetaIeta%s", histoSuffix[i]),"leading photon;#sigma_{#eta #eta}",nBins,0,maxSigmaIetaIeta);
        fSigmaIetaIeta_gja[i] = (TH1D*)fSigmaIetaIeta[i]->Clone(Form("%s_gja",fSigmaIetaIeta[i]->GetName()));

        fPhi[i] = new TH1D(Form("fPhi%s", histoSuffix[i]),"leading photon;#phi",nBins,-maxPhi,maxPhi);
        fPhi_gja[i] = (TH1D*)fPhi[i]->Clone(Form("%s_gja",fPhi[i]->GetName()));

        fIndex[i] = new TH1D(Form("fIndex%s", histoSuffix[i]),"leading photon;index of photon",maxIndex,0,maxIndex);
        fIndex_gja[i] = (TH1D*)fIndex[i]->Clone(Form("%s_gja",fIndex[i]->GetName()));

        fPt_2nd[i] = new TH1D(Form("fPt_2nd%s", histoSuffix[i]),"subleading photon;p_{T} (GeV)",nBins,0,maxPt);
        fPt_2nd_gja[i] = (TH1D*)fPt_2nd[i]->Clone(Form("%s_gja",fPt_2nd[i]->GetName()));

        fSigmaIetaIeta_2nd[i] = new TH1D(Form("fSigmaIetaIeta_2nd%s", histoSuffix[i]),"subleading photon;#sigma_{#eta #eta}",nBins,0,maxSigmaIetaIeta);
        fSigmaIetaIeta_2nd_gja[i] = (TH1D*)fSigmaIetaIeta_2nd[i]->Clone(Form("%s_gja",fSigmaIetaIeta_2nd[i]->GetName()));

        fPhi_2nd[i] = new TH1D(Form("fPhi_2nd%s", histoSuffix[i]),"subleading photon;#phi",nBins,-maxPhi,maxPhi);
        fPhi_2nd_gja[i] = (TH1D*)fPhi_2nd[i]->Clone(Form("%s_gja",fPhi_2nd[i]->GetName()));

        // jets
        fJetPt[i] = new TH1D(Form("fJetPt%s", histoSuffix[i]),"leading jet;p_{T} (GeV)",nBins,0,maxPt);
        fJetPt_gja[i] = (TH1D*)fJetPt[i]->Clone(Form("%s_gja",fJetPt[i]->GetName()));

        fJetPhi[i] = new TH1D(Form("fJetPhi%s", histoSuffix[i]),"leading jet;#phi",nBins,-maxPhi,maxPhi);
        fJetPhi_gja[i] = (TH1D*)fJetPhi[i]->Clone(Form("%s_gja",fJetPhi[i]->GetName()));

        fJetIndex[i] = new TH1D(Form("fJetIndex%s", histoSuffix[i]),"leading jet;index of jet",maxIndex,0,maxIndex);
        fJetIndex_gja[i] = (TH1D*)fJetIndex[i]->Clone(Form("%s_gja",fJetIndex[i]->GetName()));

        fJetPt_2nd[i] = new TH1D(Form("fJetPt_2nd%s", histoSuffix[i]),"subleading jet;p_{T} (GeV)",nBins,0,maxPt);
        fJetPt_2nd_gja[i] = (TH1D*)fJetPt_2nd[i]->Clone(Form("%s_gja",fJetPt_2nd[i]->GetName()));

        fJetPhi_2nd[i] = new TH1D(Form("fJetPhi_2nd%s", histoSuffix[i]),"subleading jet;#phi",nBins,-maxPhi,maxPhi);
        fJetPhi_2nd_gja[i] = (TH1D*)fJetPhi_2nd[i]->Clone(Form("%s_gja",fJetPhi_2nd[i]->GetName()));

        // TH2D
        // photons
        fPtvsSigmaIetaIeta[i] = new TH2D(Form("fPtvsSigmaIetaIeta%s", histoSuffix[i]),"leading photon;p_{T} (GeV);#sigma_{#eta #eta}",nBins2D,0,maxPt,nBins2D,0,maxSigmaIetaIeta);
        fPtvsSigmaIetaIeta_gja[i] = (TH2D*)fPtvsSigmaIetaIeta[i]->Clone(Form("%s_gja",fPtvsSigmaIetaIeta[i]->GetName()));

        fPtvsEta[i] = new TH2D(Form("fPtvsEta%s", histoSuffix[i]),"leading photon;p_{T} (GeV);#eta",nBins2D,0,maxPt,nBins2D,-maxEta,maxEta);
        fPtvsEta_gja[i] = (TH2D*)fPtvsEta[i]->Clone(Form("%s_gja",fPtvsEta[i]->GetName()));

        fPtvsSigmaIetaIeta_2nd[i] = new TH2D(Form("fPtvsSigmaIetaIeta_2nd%s", histoSuffix[i]),"subleading photon;p_{T} (GeV);#sigma_{#eta #eta}",nBins2D,0,maxPt,nBins2D,0,maxSigmaIetaIeta);
        fPtvsSigmaIetaIeta_2nd_gja[i] = (TH2D*)fPtvsSigmaIetaIeta_2nd[i]->Clone(Form("%s_gja",fPtvsSigmaIetaIeta_2nd[i]->GetName()));

        fPtvsEta_2nd[i] = new TH2D(Form("fPtvsEta_2nd%s", histoSuffix[i]),"subleading photon;p_{T} (GeV);#eta",nBins2D,0,maxPt,nBins2D,-maxEta,maxEta);
        fPtvsEta_2nd_gja[i] = (TH2D*)fPtvsEta_2nd[i]->Clone(Form("%s_gja",fPtvsEta_2nd[i]->GetName()));

        // jets
        fJetPtvsEta[i] = new TH2D(Form("fJetPtvsEta%s", histoSuffix[i]),"leading jet;p_{T} (GeV);#eta",nBins2D,0,maxPt,nBins2D,-maxEta,maxEta);
        fJetPtvsEta_gja[i] = (TH2D*)fJetPtvsEta[i]->Clone(Form("%s_gja",fJetPtvsEta[i]->GetName()));

        fJetPtvsPt[i] = new TH2D(Form("fJetPtvsPt%s", histoSuffix[i]),"leading jet;p_{T}^{Jet} (GeV);p_{T}^{#gamma} (GeV)",nBins2D,0,maxPt,nBins2D,0,maxPt);
        fJetPtvsPt_gja[i] = (TH2D*)fJetPtvsPt[i]->Clone(Form("%s_gja",fJetPtvsPt[i]->GetName()));

        fJetEtavsEta[i] = new TH2D(Form("fJetEtavsEta%s", histoSuffix[i]),"leading jet;#eta^{Jet};#eta^{#gamma}",nBins2D,-maxEta,maxEta,nBins2D,-maxEta,maxEta);
        fJetEtavsEta_gja[i] = (TH2D*)fJetEtavsEta[i]->Clone(Form("%s_gja",fJetEtavsEta[i]->GetName()));

        fJetPhivsPhi[i] = new TH2D(Form("fJetPhivsPhi%s", histoSuffix[i]),"leading jet;#phi^{Jet};#phi^{#gamma}",nBins2D,-maxPhi,maxPhi,nBins2D,-maxPhi,maxPhi);
        fJetPhivsPhi_gja[i] = (TH2D*)fJetPhivsPhi[i]->Clone(Form("%s_gja",fJetPhivsPhi[i]->GetName()));

        fJetPtvsEta_2nd[i] = new TH2D(Form("fJetPtvsEta_2nd%s", histoSuffix[i]),"subleading jet;p_{T} (GeV);#eta",nBins2D,0,maxPt,nBins2D,-maxEta,maxEta);
        fJetPtvsEta_2nd_gja[i] = (TH2D*)fJetPtvsEta_2nd[i]->Clone(Form("%s_gja",fJetPtvsEta_2nd[i]->GetName()));

        fJetPtvsPt_2nd[i] = new TH2D(Form("fJetPtvsPt_2nd%s", histoSuffix[i]),"subleading jet;p_{T}^{Jet} (GeV);p_{T}^{#gamma} (GeV)",nBins2D,0,maxPt,nBins2D,0,maxPt);
        fJetPtvsPt_2nd_gja[i] = (TH2D*)fJetPtvsPt_2nd[i]->Clone(Form("%s_gja",fJetPtvsPt_2nd[i]->GetName()));

        fJetEtavsEta_2nd[i] = new TH2D(Form("fJetEtavsEta_2nd%s", histoSuffix[i]),"subleading jet;#eta^{Jet};#eta^{#gamma}",nBins2D,-maxEta,maxEta,nBins2D,-maxEta,maxEta);
        fJetEtavsEta_2nd_gja[i] = (TH2D*)fJetEtavsEta_2nd[i]->Clone(Form("%s_gja",fJetEtavsEta_2nd[i]->GetName()));

        fJetPhivsPhi_2nd[i] = new TH2D(Form("fJetPhivsPhi_2nd%s", histoSuffix[i]),"subleading jet;#phi^{Jet};#phi^{#gamma}",nBins2D,-maxPhi,maxPhi,nBins2D,-maxPhi,maxPhi);
        fJetPhivsPhi_2nd_gja[i] = (TH2D*)fJetPhivsPhi_2nd[i]->Clone(Form("%s_gja",fJetPhivsPhi_2nd[i]->GetName()));
    }

    std::cout << "GammaJetAnalyzer is being initialized." << std::endl;

    GammaJetAnalyzer* gja = new GammaJetAnalyzer(inputFile);
    if(collision == pp)   {
        gja->setJetTree(ak3PFJets);
    }
    else {
        gja->setJetTree(akPu3PFJets);
    }
    TString cond_eta_spike     = GammaJetAnalyzer::mergeSelections(gja->cond_pt_eta,  gja->cond_spike);
    TString cond_eta_spike_iso = GammaJetAnalyzer::mergeSelections(cond_eta_spike, gja->cond_iso);
    int skip_event = -1;
    gja->cut_skip_event = skip_event;
    gja->updateEventList();
    std::cout << "GammaJetAnalyzer is being initialized : DONE" << std::endl;
    std::cout << "GammaJetAnalyzer is making plots ..." << std::endl;

    std::clock_t    start_gja, end_gja;
    start_gja = std::clock();

    // TH1D
    // draw leading photon
    gja->drawMax("pt", "pt", "1==1",                              fPt_gja[0]);
    gja->drawMax("pt", "pt", "1==1",             gja->cond_event, fPt_gja[1]);
    gja->drawMax("pt", "pt", gja->cond_pt_eta,   gja->cond_event, fPt_gja[2]);
    gja->drawMax("pt", "pt", cond_eta_spike,     gja->cond_event, fPt_gja[3]);
    gja->drawMax("pt", "pt", cond_eta_spike_iso, gja->cond_event, fPt_gja[4]);
    gja->drawMax("pt", "pt", gja->cond_photon,   gja->cond_event, fPt_gja[5]);

    gja->drawMax("sigmaIetaIeta", "pt", "1==1",                              fSigmaIetaIeta_gja[0]);
    gja->drawMax("sigmaIetaIeta", "pt", "1==1",             gja->cond_event, fSigmaIetaIeta_gja[1]);
    gja->drawMax("sigmaIetaIeta", "pt", gja->cond_pt_eta,   gja->cond_event, fSigmaIetaIeta_gja[2]);
    gja->drawMax("sigmaIetaIeta", "pt", cond_eta_spike,     gja->cond_event, fSigmaIetaIeta_gja[3]);
    gja->drawMax("sigmaIetaIeta", "pt", cond_eta_spike_iso, gja->cond_event, fSigmaIetaIeta_gja[4]);
    gja->drawMax("sigmaIetaIeta", "pt", gja->cond_photon,   gja->cond_event, fSigmaIetaIeta_gja[5]);

    gja->drawMax("phi", "pt", "1==1",                              fPhi_gja[0]);
    gja->drawMax("phi", "pt", "1==1",             gja->cond_event, fPhi_gja[1]);
    gja->drawMax("phi", "pt", gja->cond_pt_eta,   gja->cond_event, fPhi_gja[2]);
    gja->drawMax("phi", "pt", cond_eta_spike,     gja->cond_event, fPhi_gja[3]);
    gja->drawMax("phi", "pt", cond_eta_spike_iso, gja->cond_event, fPhi_gja[4]);
    gja->drawMax("phi", "pt", gja->cond_photon,   gja->cond_event, fPhi_gja[5]);

    gja->drawMax("Iteration$", "pt", "1==1",                              fIndex_gja[0]);
    gja->drawMax("Iteration$", "pt", "1==1",             gja->cond_event, fIndex_gja[1]);
    gja->drawMax("Iteration$", "pt", gja->cond_pt_eta,   gja->cond_event, fIndex_gja[2]);
    gja->drawMax("Iteration$", "pt", cond_eta_spike,     gja->cond_event, fIndex_gja[3]);
    gja->drawMax("Iteration$", "pt", cond_eta_spike_iso, gja->cond_event, fIndex_gja[4]);
    gja->drawMax("Iteration$", "pt", gja->cond_photon,   gja->cond_event, fIndex_gja[5]);

    // draw subleading photon
    gja->drawMax2nd("pt", "pt", "1==1",                              fPt_2nd_gja[0]);
    gja->drawMax2nd("pt", "pt", "1==1",             gja->cond_event, fPt_2nd_gja[1]);
    gja->drawMax2nd("pt", "pt", gja->cond_pt_eta,   gja->cond_event, fPt_2nd_gja[2]);
    gja->drawMax2nd("pt", "pt", cond_eta_spike,     gja->cond_event, fPt_2nd_gja[3]);
    gja->drawMax2nd("pt", "pt", cond_eta_spike_iso, gja->cond_event, fPt_2nd_gja[4]);
    gja->drawMax2nd("pt", "pt", gja->cond_photon,   gja->cond_event, fPt_2nd_gja[5]);

    gja->drawMax2nd("sigmaIetaIeta", "pt", "1==1",                              fSigmaIetaIeta_2nd_gja[0]);
    gja->drawMax2nd("sigmaIetaIeta", "pt", "1==1",             gja->cond_event, fSigmaIetaIeta_2nd_gja[1]);
    gja->drawMax2nd("sigmaIetaIeta", "pt", gja->cond_pt_eta,   gja->cond_event, fSigmaIetaIeta_2nd_gja[2]);
    gja->drawMax2nd("sigmaIetaIeta", "pt", cond_eta_spike,     gja->cond_event, fSigmaIetaIeta_2nd_gja[3]);
    gja->drawMax2nd("sigmaIetaIeta", "pt", cond_eta_spike_iso, gja->cond_event, fSigmaIetaIeta_2nd_gja[4]);
    gja->drawMax2nd("sigmaIetaIeta", "pt", gja->cond_photon,   gja->cond_event, fSigmaIetaIeta_2nd_gja[5]);

    gja->drawMax2nd("phi", "pt", "1==1",                              fPhi_2nd_gja[0]);
    gja->drawMax2nd("phi", "pt", "1==1",             gja->cond_event, fPhi_2nd_gja[1]);
    gja->drawMax2nd("phi", "pt", gja->cond_pt_eta,   gja->cond_event, fPhi_2nd_gja[2]);
    gja->drawMax2nd("phi", "pt", cond_eta_spike,     gja->cond_event, fPhi_2nd_gja[3]);
    gja->drawMax2nd("phi", "pt", cond_eta_spike_iso, gja->cond_event, fPhi_2nd_gja[4]);
    gja->drawMax2nd("phi", "pt", gja->cond_photon,   gja->cond_event, fPhi_2nd_gja[5]);

    // draw leading jet
    gja->drawMaxJet("jtpt", "jtpt", gja->cond_jet, "nPhotons>0", fJetPt_gja[0]);
    gja->drawMaxJet("jtpt", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPt_gja[1]);
    gja->drawMaxJet("jtpt", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPt_gja[2]);
    gja->drawMaxJet("jtpt", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPt_gja[3]);
    gja->drawMaxJet("jtpt", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPt_gja[4]);
    gja->drawMaxJet("jtpt", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPt_gja[5]);

    gja->drawMaxJet("jtphi", "jtpt", gja->cond_jet, "nPhotons>0", fJetPhi_gja[0]);
    gja->drawMaxJet("jtphi", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPhi_gja[1]);
    gja->drawMaxJet("jtphi", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPhi_gja[2]);
    gja->drawMaxJet("jtphi", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPhi_gja[3]);
    gja->drawMaxJet("jtphi", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPhi_gja[4]);
    gja->drawMaxJet("jtphi", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPhi_gja[5]);

    gja->drawMaxJet("Iteration$", "jtpt", gja->cond_jet, "nPhotons>0", fJetIndex_gja[0]);
    gja->drawMaxJet("Iteration$", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetIndex_gja[1]);
    gja->drawMaxJet("Iteration$", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetIndex_gja[2]);
    gja->drawMaxJet("Iteration$", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetIndex_gja[3]);
    gja->drawMaxJet("Iteration$", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetIndex_gja[4]);
    gja->drawMaxJet("Iteration$", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetIndex_gja[5]);

    // draw subleading jet
    gja->drawMaxJet2nd("jtpt", "jtpt", gja->cond_jet, "nPhotons>0", fJetPt_2nd_gja[0]);
    gja->drawMaxJet2nd("jtpt", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPt_2nd_gja[1]);
    gja->drawMaxJet2nd("jtpt", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPt_2nd_gja[2]);
    gja->drawMaxJet2nd("jtpt", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPt_2nd_gja[3]);
    gja->drawMaxJet2nd("jtpt", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPt_2nd_gja[4]);
    gja->drawMaxJet2nd("jtpt", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPt_2nd_gja[5]);

    gja->drawMaxJet2nd("jtphi", "jtpt", gja->cond_jet, "nPhotons>0", fJetPhi_2nd_gja[0]);
    gja->drawMaxJet2nd("jtphi", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPhi_2nd_gja[1]);
    gja->drawMaxJet2nd("jtphi", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPhi_2nd_gja[2]);
    gja->drawMaxJet2nd("jtphi", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPhi_2nd_gja[3]);
    gja->drawMaxJet2nd("jtphi", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPhi_2nd_gja[4]);
    gja->drawMaxJet2nd("jtphi", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPhi_2nd_gja[5]);

    // TH2D
    // draw leading photon
    gja->drawMax("sigmaIetaIeta:pt", "pt", "1==1",                              fPtvsSigmaIetaIeta_gja[0]);
    gja->drawMax("sigmaIetaIeta:pt", "pt", "1==1",             gja->cond_event, fPtvsSigmaIetaIeta_gja[1]);
    gja->drawMax("sigmaIetaIeta:pt", "pt", gja->cond_pt_eta,   gja->cond_event, fPtvsSigmaIetaIeta_gja[2]);
    gja->drawMax("sigmaIetaIeta:pt", "pt", cond_eta_spike,     gja->cond_event, fPtvsSigmaIetaIeta_gja[3]);
    gja->drawMax("sigmaIetaIeta:pt", "pt", cond_eta_spike_iso, gja->cond_event, fPtvsSigmaIetaIeta_gja[4]);
    gja->drawMax("sigmaIetaIeta:pt", "pt", gja->cond_photon,   gja->cond_event, fPtvsSigmaIetaIeta_gja[5]);

    gja->drawMax("eta:pt", "pt", "1==1",                              fPtvsEta_gja[0]);
    gja->drawMax("eta:pt", "pt", "1==1",             gja->cond_event, fPtvsEta_gja[1]);
    gja->drawMax("eta:pt", "pt", gja->cond_pt_eta,   gja->cond_event, fPtvsEta_gja[2]);
    gja->drawMax("eta:pt", "pt", cond_eta_spike,     gja->cond_event, fPtvsEta_gja[3]);
    gja->drawMax("eta:pt", "pt", cond_eta_spike_iso, gja->cond_event, fPtvsEta_gja[4]);
    gja->drawMax("eta:pt", "pt", gja->cond_photon,   gja->cond_event, fPtvsEta_gja[5]);

    // draw subleading photon
    gja->drawMax2nd("sigmaIetaIeta:pt", "pt", "1==1",                              fPtvsSigmaIetaIeta_2nd_gja[0]);
    gja->drawMax2nd("sigmaIetaIeta:pt", "pt", "1==1",             gja->cond_event, fPtvsSigmaIetaIeta_2nd_gja[1]);
    gja->drawMax2nd("sigmaIetaIeta:pt", "pt", gja->cond_pt_eta,   gja->cond_event, fPtvsSigmaIetaIeta_2nd_gja[2]);
    gja->drawMax2nd("sigmaIetaIeta:pt", "pt", cond_eta_spike,     gja->cond_event, fPtvsSigmaIetaIeta_2nd_gja[3]);
    gja->drawMax2nd("sigmaIetaIeta:pt", "pt", cond_eta_spike_iso, gja->cond_event, fPtvsSigmaIetaIeta_2nd_gja[4]);
    gja->drawMax2nd("sigmaIetaIeta:pt", "pt", gja->cond_photon,   gja->cond_event, fPtvsSigmaIetaIeta_2nd_gja[5]);

    gja->drawMax2nd("eta:pt", "pt", "1==1",                              fPtvsEta_2nd_gja[0]);
    gja->drawMax2nd("eta:pt", "pt", "1==1",             gja->cond_event, fPtvsEta_2nd_gja[1]);
    gja->drawMax2nd("eta:pt", "pt", gja->cond_pt_eta,   gja->cond_event, fPtvsEta_2nd_gja[2]);
    gja->drawMax2nd("eta:pt", "pt", cond_eta_spike,     gja->cond_event, fPtvsEta_2nd_gja[3]);
    gja->drawMax2nd("eta:pt", "pt", cond_eta_spike_iso, gja->cond_event, fPtvsEta_2nd_gja[4]);
    gja->drawMax2nd("eta:pt", "pt", gja->cond_photon,   gja->cond_event, fPtvsEta_2nd_gja[5]);

    // draw leading jet
    gja->drawMaxJet("jteta:jtpt", "jtpt", gja->cond_jet, "nPhotons>0", fJetPtvsEta_gja[0]);
    gja->drawMaxJet("jteta:jtpt", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPtvsEta_gja[1]);
    gja->drawMaxJet("jteta:jtpt", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPtvsEta_gja[2]);
    gja->drawMaxJet("jteta:jtpt", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPtvsEta_gja[3]);
    gja->drawMaxJet("jteta:jtpt", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPtvsEta_gja[4]);
    gja->drawMaxJet("jteta:jtpt", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPtvsEta_gja[5]);

    gja->drawMaxJet("PHOTONPT:jtpt", "jtpt", gja->cond_jet, "nPhotons>0", fJetPtvsPt_gja[0]);
    gja->drawMaxJet("PHOTONPT:jtpt", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPtvsPt_gja[1]);
    gja->drawMaxJet("PHOTONPT:jtpt", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPtvsPt_gja[2]);
    gja->drawMaxJet("PHOTONPT:jtpt", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPtvsPt_gja[3]);
    gja->drawMaxJet("PHOTONPT:jtpt", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPtvsPt_gja[4]);
    gja->drawMaxJet("PHOTONPT:jtpt", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPtvsPt_gja[5]);

    gja->drawMaxJet("PHOTONETA:jteta", "jtpt", gja->cond_jet, "nPhotons>0", fJetEtavsEta_gja[0]);
    gja->drawMaxJet("PHOTONETA:jteta", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetEtavsEta_gja[1]);
    gja->drawMaxJet("PHOTONETA:jteta", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetEtavsEta_gja[2]);
    gja->drawMaxJet("PHOTONETA:jteta", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetEtavsEta_gja[3]);
    gja->drawMaxJet("PHOTONETA:jteta", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetEtavsEta_gja[4]);
    gja->drawMaxJet("PHOTONETA:jteta", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetEtavsEta_gja[5]);

    gja->drawMaxJet("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, "nPhotons>0", fJetPhivsPhi_gja[0]);
    gja->drawMaxJet("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPhivsPhi_gja[1]);
    gja->drawMaxJet("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPhivsPhi_gja[2]);
    gja->drawMaxJet("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPhivsPhi_gja[3]);
    gja->drawMaxJet("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPhivsPhi_gja[4]);
    gja->drawMaxJet("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPhivsPhi_gja[5]);

    // draw subleading jet
    gja->drawMaxJet2nd("jteta:jtpt", "jtpt", gja->cond_jet, "nPhotons>0", fJetPtvsEta_2nd_gja[0]);
    gja->drawMaxJet2nd("jteta:jtpt", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPtvsEta_2nd_gja[1]);
    gja->drawMaxJet2nd("jteta:jtpt", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPtvsEta_2nd_gja[2]);
    gja->drawMaxJet2nd("jteta:jtpt", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPtvsEta_2nd_gja[3]);
    gja->drawMaxJet2nd("jteta:jtpt", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPtvsEta_2nd_gja[4]);
    gja->drawMaxJet2nd("jteta:jtpt", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPtvsEta_2nd_gja[5]);

    gja->drawMaxJet2nd("PHOTONPT:jtpt", "jtpt", gja->cond_jet, "nPhotons>0", fJetPtvsPt_2nd_gja[0]);
    gja->drawMaxJet2nd("PHOTONPT:jtpt", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPtvsPt_2nd_gja[1]);
    gja->drawMaxJet2nd("PHOTONPT:jtpt", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPtvsPt_2nd_gja[2]);
    gja->drawMaxJet2nd("PHOTONPT:jtpt", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPtvsPt_2nd_gja[3]);
    gja->drawMaxJet2nd("PHOTONPT:jtpt", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPtvsPt_2nd_gja[4]);
    gja->drawMaxJet2nd("PHOTONPT:jtpt", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPtvsPt_2nd_gja[5]);

    gja->drawMaxJet2nd("PHOTONETA:jteta", "jtpt", gja->cond_jet, "nPhotons>0", fJetEtavsEta_2nd_gja[0]);
    gja->drawMaxJet2nd("PHOTONETA:jteta", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetEtavsEta_2nd_gja[1]);
    gja->drawMaxJet2nd("PHOTONETA:jteta", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetEtavsEta_2nd_gja[2]);
    gja->drawMaxJet2nd("PHOTONETA:jteta", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetEtavsEta_2nd_gja[3]);
    gja->drawMaxJet2nd("PHOTONETA:jteta", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetEtavsEta_2nd_gja[4]);
    gja->drawMaxJet2nd("PHOTONETA:jteta", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetEtavsEta_2nd_gja[5]);

    gja->drawMaxJet2nd("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, "nPhotons>0", fJetPhivsPhi_2nd_gja[0]);
    gja->drawMaxJet2nd("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, "nPhotons>0",       gja->cond_event, fJetPhivsPhi_2nd_gja[1]);
    gja->drawMaxJet2nd("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, gja->cond_pt_eta,   gja->cond_event, fJetPhivsPhi_2nd_gja[2]);
    gja->drawMaxJet2nd("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, cond_eta_spike,     gja->cond_event, fJetPhivsPhi_2nd_gja[3]);
    gja->drawMaxJet2nd("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, cond_eta_spike_iso, gja->cond_event, fJetPhivsPhi_2nd_gja[4]);
    gja->drawMaxJet2nd("PHOTONPHI:jtphi", "jtpt", gja->cond_jet, gja->cond_photon,   gja->cond_event, fJetPhivsPhi_2nd_gja[5]);

    end_gja = std::clock();
    std::cout << "GammaJetAnalyzer is making plots : DONE" << std::endl;

    std::cout << "entering event loop" << std::endl;
    Long64_t entries = photonTree->GetEntries();
    std::cout << "number of entries = " << entries << std::endl;

    std::clock_t    start_loop, end_loop;
    start_loop = std::clock();
    for(Long64_t j = 0; j < entries; ++j)
    {
        if (j % 100000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries<<" : "<<std::setprecision(2)<<(double)j/entries*100<<" %"<<std::endl;
        }
        if(skip_event > 0 && j % skip_event != 0) continue;

        evtTree->GetEntry(j);
        skimTree->GetEntry(j);
        photonTree->GetEntry(j);
        jetTree->GetEntry(j);

        int max_photon_index[numHistos];
        float max_photon_pt[numHistos];
        float max_photon_sigmaIetaIeta[numHistos];
        float max_photon_phi[numHistos];
        float max_photon_eta[numHistos];
        float max_photon_2nd_pt[numHistos];
        float max_photon_2nd_sigmaIetaIeta[numHistos];
        float max_photon_2nd_phi[numHistos];
        float max_photon_2nd_eta[numHistos];
        for(int i=0; i<numHistos; ++i)  {
            max_photon_index[i] = -1;
            max_photon_pt[i] = -1;
            max_photon_sigmaIetaIeta[i] = -998;
            max_photon_phi[i] = -999;
            max_photon_eta[i] = -999;
            max_photon_2nd_pt[i] = -1;
            max_photon_2nd_sigmaIetaIeta[i] = -998;
            max_photon_2nd_phi[i] = -999;
            max_photon_2nd_eta[i] = -999;
        }

        bool passed[numHistos];
        passed[0] = true;                               // no selection
        passed[1] = (TMath::Abs(vz) < gja->cut_vz);     // event selection

        for(int i = 0; i < nPhoton; ++i)
        {
            // eta cut
            // eta cut includes photon pT > 40 GeV cut as well.
            passed[2] = (TMath::Abs(photon_eta[i]) < gja->cut_eta && photon_pt[i] > gja->cut_pt);
            // spike rejection
            passed[3] = (            swissCrx[i] < gja->cut_swissCross       &&
                         TMath::Abs(seedTime[i]) < gja->cut_seedTime         &&
                                sigmaIetaIeta[i] > gja->cut_sigmaIetaIeta_gt &&
                                sigmaIphiIphi[i] > gja->cut_sigmaIphiIphi);
            // isolation
            passed[4] = (ecalRecHitSumEtConeDR04[i]  < gja->cut_ecalIso   &&
                           hcalTowerSumEtConeDR04[i] < gja->cut_hcalIso   &&
                           trkSumPtHollowConeDR04[i] < gja->cut_trackIso  &&
                                   hadronicOverEm[i] < gja->cut_hadronicOverEm);
            // purity
            passed[5] = (sigmaIetaIeta[i] < gja->cut_sigmaIetaIeta_lt) ;


            passed[1] = passed[1] && passed[0];     // event selection
            passed[2] = passed[2] && passed[1];     // event selection + eta cut
            passed[3] = passed[3] && passed[2];     // event selection + eta cut + spike rejection
            passed[4] = passed[4] && passed[3];     // event selection + eta cut + spike rejection + isolation
            passed[5] = passed[5] && passed[4];     // event selection + eta cut + spike rejection + isolation + purity

            for(int k = 0; k<numHistos; ++k)
            {
                // check if this photon can be subleading photon
                if (photon_pt[i] > max_photon_2nd_pt[k] && passed[k])  {

                    max_photon_2nd_pt[k] = photon_pt[i];
                    max_photon_2nd_sigmaIetaIeta[k] = sigmaIetaIeta[i];
                    max_photon_2nd_phi[k] = photon_phi[i];
                    max_photon_2nd_eta[k] = photon_eta[i];
                }
                // check if this photon is leading photon
                if (photon_pt[i] > max_photon_pt[k]     && passed[k])   {
                    // current leading photon becomes subleading photon
                    max_photon_2nd_pt[k] = max_photon_pt[k];
                    max_photon_2nd_sigmaIetaIeta[k] = max_photon_sigmaIetaIeta[k];
                    max_photon_2nd_phi[k] = max_photon_phi[k];
                    max_photon_2nd_eta[k] = max_photon_eta[k];

                    max_photon_index[k] = i;
                    max_photon_pt[k] = photon_pt[i];
                    max_photon_sigmaIetaIeta[k] = sigmaIetaIeta[i];
                    max_photon_phi[k] = photon_phi[i];
                    max_photon_eta[k] = photon_eta[i];
                }
            }
        }

        int max_jet_index[numHistos];
        float max_jet_pt[numHistos];
        float max_jet_phi[numHistos];
        float max_jet_eta[numHistos];
        float max_jet_2nd_pt[numHistos];
        float max_jet_2nd_phi[numHistos];
        float max_jet_2nd_eta[numHistos];
        for(int i=0; i<numHistos; ++i)
        {
            max_jet_index[i] = -1;
            max_jet_pt[i] = -1;
            max_jet_phi[i] = -999;
            max_jet_eta[i] = -999;
            max_jet_2nd_pt[i] = -1;
            max_jet_2nd_phi[i] = -999;
            max_jet_2nd_eta[i] = -999;
        }

        for(int i=0; i<nJet; ++i)
        {
            bool passed_jet;
            passed_jet =         (jet_pt[i] > gja->cut_jet_pt   &&
                     TMath::Abs(jet_eta[i]) < gja->cut_jet_eta);

            for (int k=0; k<numHistos; ++k)
            {
                // there must be a leading photon for the corresponding selection
                if(!(max_photon_index [k] > -1 )) continue;

                bool passed_jet_dphi = (TMath::Abs(getDPHI(jet_phi[i],max_photon_phi[k])) >= gja->cut_jet_photon_deltaPhi);
                bool passed_jet_dR   = (TMath::Abs(getDR  (jet_eta[i],jet_phi[i],max_photon_eta[k],max_photon_phi[k])) >= gja->cut_jet_photon_deltaR);

                // check if this jet can be subleading jet
                if (jet_pt[i] > max_jet_2nd_pt[k] && passed_jet
                                                  && passed_jet_dphi
                                                  && passed_jet_dR)   {

                    max_jet_2nd_pt[k]  = jet_pt[i];
                    max_jet_2nd_phi[k] = jet_phi[i];
                    max_jet_2nd_eta[k] = jet_eta[i];
                }
                // check if this jet is leading jet
                if (jet_pt[i] > max_jet_pt[k]     && passed_jet
                                                  && passed_jet_dphi
                                                  && passed_jet_dR)   {
                    // current leading jet becomes subleading jet
                    max_jet_2nd_pt[k]  = max_jet_pt[k];
                    max_jet_2nd_phi[k] = max_jet_phi[k];
                    max_jet_2nd_eta[k] = max_jet_eta[k];

                    max_jet_index[k] = i;
                    max_jet_pt[k]  = jet_pt[i];
                    max_jet_phi[k] = jet_phi[i];
                    max_jet_eta[k] = jet_eta[i];
                }
            }
        }

        // fill histograms for different selections
        for(int i=0; i<numHistos; ++i) {
            // TH1D
            // leading photon
            if(max_photon_index[i]>-1) {
                fPt[i]->Fill(max_photon_pt[i]);
                fSigmaIetaIeta[i]->Fill(max_photon_sigmaIetaIeta[i]);
                fPhi[i]->Fill(max_photon_phi[i]);
                fIndex[i]->Fill(max_photon_index[i]);
            }
            // subleading photon
            if(max_photon_2nd_pt[i]>-1) {
                fPt_2nd[i]->Fill(max_photon_2nd_pt[i]);
                fSigmaIetaIeta_2nd[i]->Fill(max_photon_2nd_sigmaIetaIeta[i]);
                fPhi_2nd[i]->Fill(max_photon_2nd_phi[i]);
            }
            // leading jet
            if(max_jet_index[i]>-1) {
                fJetPt[i]->Fill(max_jet_pt[i]);
                fJetPhi[i]->Fill(max_jet_phi[i]);
                fJetIndex[i]->Fill(max_jet_index[i]);
            }
            // subleading jet
            if(max_jet_2nd_pt[i]>-1) {
                fJetPt_2nd[i]->Fill(max_jet_2nd_pt[i]);
                fJetPhi_2nd[i]->Fill(max_jet_2nd_phi[i]);
            }

            //TH2D
            // leading photon
            if(max_photon_index[i]>-1) {
                fPtvsSigmaIetaIeta[i]->Fill(max_photon_pt[i],max_photon_sigmaIetaIeta[i]);
                fPtvsEta[i]->Fill(max_photon_pt[i],max_photon_eta[i]);
            }

            // subleading photon
            if(max_photon_2nd_pt[i]>-1) {
                fPtvsSigmaIetaIeta_2nd[i]->Fill(max_photon_2nd_pt[i],max_photon_2nd_sigmaIetaIeta[i]);
                fPtvsEta_2nd[i]->Fill(max_photon_2nd_pt[i],max_photon_2nd_eta[i]);
            }

            // leading jet
            if(max_jet_index[i]>-1) {
                fJetPtvsEta[i]->Fill(max_jet_pt[i],max_jet_eta[i]);
                fJetPtvsPt[i]->Fill(max_jet_pt[i],max_photon_pt[i]);
                fJetEtavsEta[i]->Fill(max_jet_eta[i],max_photon_eta[i]);
                fJetPhivsPhi[i]->Fill(max_jet_phi[i],max_photon_phi[i]);
            }

            // subleading jet
            if(max_jet_2nd_pt[i]>-1) {
                fJetPtvsEta_2nd[i]->Fill(max_jet_2nd_pt[i], max_jet_2nd_eta[i]);
                fJetPtvsPt_2nd[i]->Fill(max_jet_2nd_pt[i],max_photon_pt[i]);
                fJetEtavsEta_2nd[i]->Fill(max_jet_2nd_eta[i],max_photon_eta[i]);
                fJetPhivsPhi_2nd[i]->Fill(max_jet_2nd_phi[i],max_photon_phi[i]);
            }
        }
    }
    end_loop = std::clock();
    std::cout.precision(6);      // get back to default precision
    std::cout << "GammaJetAnalyzer made plots in : " << (end_gja - start_gja) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "LOOP made plots in             : " << (end_loop - start_loop) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;

    // compare histograms
    // TH1D
    bool histogramsAreSame_fPt[numHistos];
    bool histogramsAreSame_fSigmaIetaIeta[numHistos];
    bool histogramsAreSame_fPhi[numHistos];
    bool histogramsAreSame_fIndex[numHistos];
    bool histogramsAreSame_fPt_2nd[numHistos];
    bool histogramsAreSame_fSigmaIetaIeta_2nd[numHistos];
    bool histogramsAreSame_fPhi_2nd[numHistos];

    bool histogramsAreSame_fJetPt[numHistos];
    bool histogramsAreSame_fJetPhi[numHistos];
    bool histogramsAreSame_fJetIndex[numHistos];
    bool histogramsAreSame_fJetPt_2nd[numHistos];
    bool histogramsAreSame_fJetPhi_2nd[numHistos];
    // TH2D
    bool histogramsAreSame_fPtvsSigmaIetaIeta[numHistos];
    bool histogramsAreSame_fPtvsEta[numHistos];
    bool histogramsAreSame_fPtvsSigmaIetaIeta_2nd[numHistos];
    bool histogramsAreSame_fPtvsEta_2nd[numHistos];

    bool histogramsAreSame_fJetPtvsEta[numHistos];
    bool histogramsAreSame_fJetPtvsPt[numHistos];
    bool histogramsAreSame_fJetEtavsEta[numHistos];
    bool histogramsAreSame_fJetPhivsPhi[numHistos];
    bool histogramsAreSame_fJetPtvsEta_2nd[numHistos];
    bool histogramsAreSame_fJetPtvsPt_2nd[numHistos];
    bool histogramsAreSame_fJetEtavsEta_2nd[numHistos];
    bool histogramsAreSame_fJetPhivsPhi_2nd[numHistos];
    for(int i = 0; i<numHistos; ++i)
    {
        // TH1D
        // leading photon
        histogramsAreSame_fPt[i]=compareHistograms(fPt[i],fPt_gja[i]);
        std::cout << "comparison of " << fPt[i]->GetName() << " = " << histogramsAreSame_fPt[i] <<std::endl;

        histogramsAreSame_fSigmaIetaIeta[i]=compareHistograms(fSigmaIetaIeta[i],fSigmaIetaIeta_gja[i]);
        std::cout << "comparison of " << fSigmaIetaIeta[i]->GetName() << " = " << histogramsAreSame_fSigmaIetaIeta[i] <<std::endl;

        histogramsAreSame_fPhi[i]=compareHistograms(fPhi[i],fPhi_gja[i]);
        std::cout << "comparison of " << fPhi[i]->GetName() << " = " << histogramsAreSame_fPhi[i] <<std::endl;

        histogramsAreSame_fIndex[i]=compareHistograms(fIndex[i],fIndex_gja[i]);
        std::cout << "comparison of " << fIndex[i]->GetName() << " = " << histogramsAreSame_fIndex[i] <<std::endl;

        // subleading photon
        histogramsAreSame_fPt_2nd[i]=compareHistograms(fPt_2nd[i],fPt_2nd_gja[i]);
        std::cout << "comparison of " << fPt_2nd[i]->GetName() << " = " << histogramsAreSame_fPt_2nd[i] <<std::endl;

        histogramsAreSame_fSigmaIetaIeta_2nd[i]=compareHistograms(fSigmaIetaIeta_2nd[i],fSigmaIetaIeta_2nd_gja[i]);
        std::cout << "comparison of " << fSigmaIetaIeta_2nd[i]->GetName() << " = " << histogramsAreSame_fSigmaIetaIeta_2nd[i] <<std::endl;

        histogramsAreSame_fPhi_2nd[i]=compareHistograms(fPhi_2nd[i],fPhi_2nd_gja[i]);
        std::cout << "comparison of " << fPhi_2nd[i]->GetName() << " = " << histogramsAreSame_fPhi_2nd[i] <<std::endl;

        // leading jet
        histogramsAreSame_fJetPt[i]=compareHistograms(fJetPt[i],fJetPt_gja[i]);
        std::cout << "comparison of " << fJetPt[i]->GetName() << " = " << histogramsAreSame_fJetPt[i] <<std::endl;

        histogramsAreSame_fJetPhi[i]=compareHistograms(fJetPhi[i],fJetPhi_gja[i]);
        std::cout << "comparison of " << fJetPhi[i]->GetName() << " = " << histogramsAreSame_fJetPhi[i] <<std::endl;

        histogramsAreSame_fJetIndex[i]=compareHistograms(fJetIndex[i],fJetIndex_gja[i]);
        std::cout << "comparison of " << fJetIndex[i]->GetName() << " = " << histogramsAreSame_fJetIndex[i] <<std::endl;

        // subleading jet
        histogramsAreSame_fJetPt_2nd[i]=compareHistograms(fJetPt_2nd[i],fJetPt_2nd_gja[i]);
        std::cout << "comparison of " << fJetPt_2nd[i]->GetName() << " = " << histogramsAreSame_fJetPt_2nd[i] <<std::endl;

        histogramsAreSame_fJetPhi_2nd[i]=compareHistograms(fJetPhi_2nd[i],fJetPhi_2nd_gja[i]);
        std::cout << "comparison of " << fJetPhi_2nd[i]->GetName() << " = " << histogramsAreSame_fJetPhi_2nd[i] <<std::endl;

        // TH2D
        // leading photon
        histogramsAreSame_fPtvsSigmaIetaIeta[i]=compareHistograms(fPtvsSigmaIetaIeta[i],fPtvsSigmaIetaIeta_gja[i]);
        std::cout << "comparison of " << fPtvsSigmaIetaIeta[i]->GetName() << " = " << histogramsAreSame_fPtvsSigmaIetaIeta[i] <<std::endl;

        histogramsAreSame_fPtvsEta[i]=compareHistograms(fPtvsEta[i],fPtvsEta_gja[i]);
        std::cout << "comparison of " << fPtvsEta[i]->GetName() << " = " << histogramsAreSame_fPtvsEta[i] <<std::endl;

        // subleading photon
        histogramsAreSame_fPtvsSigmaIetaIeta_2nd[i]=compareHistograms(fPtvsSigmaIetaIeta_2nd[i],fPtvsSigmaIetaIeta_2nd_gja[i]);
        std::cout << "comparison of " << fPtvsSigmaIetaIeta_2nd[i]->GetName() << " = " << histogramsAreSame_fPtvsSigmaIetaIeta_2nd[i] <<std::endl;

        histogramsAreSame_fPtvsEta_2nd[i]=compareHistograms(fPtvsEta_2nd[i],fPtvsEta_2nd_gja[i]);
        std::cout << "comparison of " << fPtvsEta_2nd[i]->GetName() << " = " << histogramsAreSame_fPtvsEta_2nd[i] <<std::endl;

        // leading jet
        histogramsAreSame_fJetPtvsEta[i]=compareHistograms(fJetPtvsEta[i],fJetPtvsEta_gja[i]);
        std::cout << "comparison of " << fJetPtvsEta[i]->GetName() << " = " << histogramsAreSame_fJetPtvsEta[i] <<std::endl;

        histogramsAreSame_fJetPtvsPt[i]=compareHistograms(fJetPtvsPt[i],fJetPtvsPt_gja[i]);
        std::cout << "comparison of " << fJetPtvsPt[i]->GetName() << " = " << histogramsAreSame_fJetPtvsPt[i] <<std::endl;

        histogramsAreSame_fJetEtavsEta[i]=compareHistograms(fJetEtavsEta[i],fJetEtavsEta_gja[i]);
        std::cout << "comparison of " << fJetEtavsEta[i]->GetName() << " = " << histogramsAreSame_fJetEtavsEta[i] <<std::endl;

        histogramsAreSame_fJetPhivsPhi[i]=compareHistograms(fJetPhivsPhi[i],fJetPhivsPhi_gja[i]);
        std::cout << "comparison of " << fJetPhivsPhi[i]->GetName() << " = " << histogramsAreSame_fJetPhivsPhi[i] <<std::endl;

        // subleading jet
        histogramsAreSame_fJetPtvsEta_2nd[i]=compareHistograms(fJetPtvsEta_2nd[i],fJetPtvsEta_2nd_gja[i]);
        std::cout << "comparison of " << fJetPtvsEta_2nd[i]->GetName() << " = " << histogramsAreSame_fJetPtvsEta_2nd[i] <<std::endl;

        histogramsAreSame_fJetPtvsPt_2nd[i]=compareHistograms(fJetPtvsPt_2nd[i],fJetPtvsPt_2nd_gja[i]);
        std::cout << "comparison of " << fJetPtvsPt_2nd[i]->GetName() << " = " << histogramsAreSame_fJetPtvsPt_2nd[i] <<std::endl;

        histogramsAreSame_fJetEtavsEta_2nd[i]=compareHistograms(fJetEtavsEta_2nd[i],fJetEtavsEta_2nd_gja[i]);
        std::cout << "comparison of " << fJetEtavsEta_2nd[i]->GetName() << " = " << histogramsAreSame_fJetEtavsEta_2nd[i] <<std::endl;

        histogramsAreSame_fJetPhivsPhi_2nd[i]=compareHistograms(fJetPhivsPhi_2nd[i],fJetPhivsPhi_2nd_gja[i]);
        std::cout << "comparison of " << fJetPhivsPhi_2nd[i]->GetName() << " = " << histogramsAreSame_fJetPhivsPhi_2nd[i] <<std::endl;
    }

    // save histograms
    outputFile->cd();
    for(int i = 0; i<numHistos; ++i)
    {
        // TH1D
        // leading photon
        fPt[i]->Write();
        fPt_gja[i]->Write();

        fSigmaIetaIeta[i]->Write();
        fSigmaIetaIeta_gja[i]->Write();

        fPhi[i]->Write();
        fPhi_gja[i]->Write();

        fIndex[i]->Write();
        fIndex_gja[i]->Write();

        // subleading photon
        fPt_2nd[i]->Write();
        fPt_2nd_gja[i]->Write();

        fSigmaIetaIeta_2nd[i]->Write();
        fSigmaIetaIeta_2nd_gja[i]->Write();

        fPhi_2nd[i]->Write();
        fPhi_2nd_gja[i]->Write();

        // leading jet
        fJetPt[i]->Write();
        fJetPt_gja[i]->Write();

        fJetPhi[i]->Write();
        fJetPhi_gja[i]->Write();

        fJetIndex[i]->Write();
        fJetIndex_gja[i]->Write();

        // subleading jet
        fJetPt_2nd[i]->Write();
        fJetPt_2nd_gja[i]->Write();

        fJetPhi_2nd[i]->Write();
        fJetPhi_2nd_gja[i]->Write();

        // TH2D
        // leading photon
        fPtvsSigmaIetaIeta[i]->Write();
        fPtvsSigmaIetaIeta_gja[i]->Write();

        fPtvsEta[i]->Write();
        fPtvsEta_gja[i]->Write();

        // subleading photon
        fPtvsSigmaIetaIeta_2nd[i]->Write();
        fPtvsSigmaIetaIeta_2nd_gja[i]->Write();

        fPtvsEta_2nd[i]->Write();
        fPtvsEta_2nd_gja[i]->Write();

        // leading jet
        fJetPtvsEta[i]->Write();
        fJetPtvsEta_gja[i]->Write();

        fJetPtvsPt[i]->Write();
        fJetPtvsPt_gja[i]->Write();

        fJetEtavsEta[i]->Write();
        fJetEtavsEta_gja[i]->Write();

        fJetPhivsPhi[i]->Write();
        fJetPhivsPhi_gja[i]->Write();

        // subleading jet
        fJetPtvsEta_2nd[i]->Write();
        fJetPtvsEta_2nd_gja[i]->Write();

        fJetPtvsPt_2nd[i]->Write();
        fJetPtvsPt_2nd_gja[i]->Write();

        fJetEtavsEta_2nd[i]->Write();
        fJetEtavsEta_2nd_gja[i]->Write();

        fJetPhivsPhi_2nd[i]->Write();
        fJetPhivsPhi_2nd_gja[i]->Write();
    }
    outputFile->Close();
    inputFile->Close();
}

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        test_GammaJetAnalyzer();
        return 0;
    }
    else if(argc == 2)
    {
        test_GammaJetAnalyzer(argv[1]);
        return 0;
    }
    else if(argc == 3)
    {
        test_GammaJetAnalyzer(argv[1], argv[2]);
        return 0;
    }
    else if(argc == 4)
    {
        test_GammaJetAnalyzer(argv[1], argv[2], (collisionType)atoi(argv[3]));
        return 0;
    }
    else
    {
        std::cout<<"wrong input"<<std::endl;
    }
}
