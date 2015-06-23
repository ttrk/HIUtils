/*
 * test_GammaJetAnalyzer.C
 *
 * macro to test GammaJetAnalyzer class
 *  1. histograms by GammayJetAnalyzer
 */

#include "../GammaJetAnalyzer.h"
#include "../GammaJetAnalyzer.cc"   // need to use this include if this macro and "GammaJetAnalyzer.h" are not in the same directory.
#include "../histoUtil.h"
//#include "../treeUtil.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TMath.h>

#include <iostream>
#include <iomanip>

void test_GammaJetAnalyzer(const char* inputfileName = "/mnt/hadoop/cms/store/user/luck/2014-photon-forests/pPb_DATA_photon30trig_localJEC_v2.root", const char* outputFileName = "test_GammaJetAnalyzer.root");

const int MAXPHOTONS = 500;

void test_GammaJetAnalyzer(const char* inputfileName , const char* outputFileName)
{
    TFile *inputFile = new TFile(inputfileName, "READ");
    std::cout << "input HiForest : " << inputFile->GetName() << std::endl;

    TTree *photonTree = (TTree*)inputFile->Get("multiPhotonAnalyzer/photon");
    TTree *evtTree = (TTree*)inputFile->Get("hiEvtAnalyzer/HiTree");
    TTree *skimTree = (TTree*)inputFile->Get("skimanalysis/HltTree");

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

//    inputFile->Close();
    TFile* outputFile=new TFile(outputFileName, "RECREATE");

    const int numHistos = 6;
    const int nBins = 500;
    const int maxPt = 500;

    TH1D* fPt[numHistos];       // maximum photon pT histograms to be filled by LOOP
    fPt[0] = new TH1D("fPt"        ,";p_{T} (GeV)",nBins,0,maxPt);      // no selection
    fPt[1] = new TH1D("fPt_event"  ,";p_{T} (GeV)",nBins,0,maxPt);      // event selection
    fPt[2] = new TH1D("fPt_eta"    ,";p_{T} (GeV)",nBins,0,maxPt);      // event selection + eta cut
    fPt[3] = new TH1D("fPt_spike"  ,";p_{T} (GeV)",nBins,0,maxPt);      // event selection + eta cut + spike rejection
    fPt[4] = new TH1D("fPt_iso"    ,";p_{T} (GeV)",nBins,0,maxPt);      // event selection + eta cut + spike rejection + isolation
    fPt[5] = new TH1D("fPt_purity" ,";p_{T} (GeV)",nBins,0,maxPt);      // event selection + eta cut + spike rejection + isolation + purity

    TH1D *fPt_gja[numHistos];   // maximum photon pT histograms to be filled by GammaJetAnalyzer
    for (int i=0; i<numHistos ; ++i)
    {
        fPt_gja[i] = (TH1D*)fPt[i]->Clone(Form("%s_gja",fPt[i]->GetName()));
    }

    std::cout << "GammaJetAnalyzer is being initialized." << std::endl;
    GammaJetAnalyzer* gja = new GammaJetAnalyzer(inputFile);
    TString cond_eta_spike     = GammaJetAnalyzer::mergeSelections(gja->cond_eta,  gja->cond_spike);
    TString cond_eta_spike_iso = GammaJetAnalyzer::mergeSelections(cond_eta_spike, gja->cond_iso);
    std::cout << "GammaJetAnalyzer is being initialized : DONE" << std::endl;
    std::cout << "GammaJetAnalyzer is making plots ..." << std::endl;
//    drawMaximum(gja->tree, "pt", "1==1",             "1==1",          fPt_gja[0]);
//    drawMaximum(gja->tree, "pt", "1==1",             gja->cond_event, fPt_gja[1]);
//    drawMaximum(gja->tree, "pt", gja->cond_eta ,     gja->cond_event, fPt_gja[2]);
//    drawMaximum(gja->tree, "pt", cond_eta_spike,     gja->cond_event, fPt_gja[3]);
//    drawMaximum(gja->tree, "pt", cond_eta_spike_iso, gja->cond_event, fPt_gja[4]);
//    drawMaximum(gja->tree, "pt", gja->cond_photon,   gja->cond_event, fPt_gja[5]);
    gja->drawMax("pt", "pt", "1==1",             "1==1",          fPt_gja[0]);
    gja->drawMax("pt", "pt", "1==1",             gja->cond_event, fPt_gja[1]);
    gja->drawMax("pt", "pt", gja->cond_eta ,     gja->cond_event, fPt_gja[2]);
    gja->drawMax("pt", "pt", cond_eta_spike,     gja->cond_event, fPt_gja[3]);
    gja->drawMax("pt", "pt", cond_eta_spike_iso, gja->cond_event, fPt_gja[4]);
    gja->drawMax("pt", "pt", gja->cond_photon,   gja->cond_event, fPt_gja[5]);
    std::cout << "GammaJetAnalyzer is making plots : DONE" << std::endl;

    std::cout << "entering event loop" << std::endl;
    Long64_t entries = photonTree->GetEntries();
    std::cout << "number of entries = " << entries << std::endl;
    for(Long64_t j = 0; j < entries; ++j)
    {
        if (j % 100000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries<<" : "<<std::setprecision(2)<<(double)j/entries*100<<" %"<<std::endl;
        }

        evtTree->GetEntry(j);
        skimTree->GetEntry(j);
        photonTree->GetEntry(j);

        bool passed_event  = false;
        bool passed_eta    = false;
        bool passed_spike  = false;
        bool passed_iso    = false;
        bool passed_purity = false;

        float max_photon_pt[numHistos];
        for(int i=0; i<numHistos; ++i)  {
            max_photon_pt[i] = -1;
        }

        passed_event = (TMath::Abs(vz) < gja->cut_vz);

        for(int i = 0; i < nPhoton; ++i)
        {
            passed_eta = (TMath::Abs(photon_eta[i]) < gja->cut_eta);
            passed_spike = (            swissCrx[i] < gja->cut_swissCross       &&
                            TMath::Abs(seedTime[i]) < gja->cut_seedTime         &&
                                   sigmaIetaIeta[i] > gja->cut_sigmaIetaIeta_gt &&
                                   sigmaIphiIphi[i] > gja->cut_sigmaIphiIphi);

            passed_iso = (ecalRecHitSumEtConeDR04[i] < gja->cut_ecalIso   &&
                           hcalTowerSumEtConeDR04[i] < gja->cut_hcalIso   &&
                           trkSumPtHollowConeDR04[i] < gja->cut_trackIso  &&
                                   hadronicOverEm[i] < gja->cut_hadronicOverEm);

            passed_purity = (sigmaIetaIeta[i] < gja->cut_sigmaIetaIeta_lt) ;

            if (photon_pt[i] > max_photon_pt[0])   {
                max_photon_pt[0] = photon_pt[i];
            }
            if (photon_pt[i] > max_photon_pt[1] && passed_event)   {
                max_photon_pt[1] = photon_pt[i];
            }
            if (photon_pt[i] > max_photon_pt[2] && passed_event && passed_eta)   {
                max_photon_pt[2] = photon_pt[i];
            }
            if (photon_pt[i] > max_photon_pt[3] && passed_event && passed_eta
                                                && passed_spike)   {
                max_photon_pt[3] = photon_pt[i];
            }
            if (photon_pt[i] > max_photon_pt[4] && passed_event && passed_eta
                                                && passed_spike && passed_iso)   {
                max_photon_pt[4] = photon_pt[i];
            }
            if (photon_pt[i] > max_photon_pt[5] && passed_event && passed_eta
                                                && passed_spike && passed_iso && passed_purity)   {
                max_photon_pt[5] = photon_pt[i];
            }
        }

        // fill maximum photon pT for different selections
        for(int i=0; i<numHistos; ++i) {
            if(max_photon_pt[i]>-1) {
                fPt[i]->Fill(max_photon_pt[i]);
            }
        }
    }

    // compare histograms
    bool histogramsAreSame[numHistos];
    for(int i = 0; i<numHistos; ++i)
    {
        histogramsAreSame[i]=compareHistograms(fPt[i],fPt_gja[i]);
        std::cout << "comparison of " << fPt[i]->GetName() << " = " << histogramsAreSame[i] <<std::endl;
    }

    // save histograms
    outputFile->cd();
    for(int i = 0; i<numHistos; ++i)
    {
        fPt[i]->Write();
        fPt_gja[i]->Write();
    }
    outputFile->Close();
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
    else
    {
        std::cout<<"wrong input"<<std::endl;
    }
}
