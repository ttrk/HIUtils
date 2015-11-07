/*
 * test_eventMatcher.C
 *
 *
 * This is getting more and more confusing to me. I've asked Lindsey for more help debugging on the github page.

Kaya, can you try to re-run the photon producer step during foresting (with your patch)? You should just be able to add "process.photons" to ana_step before the ggHiNtuplizer, and make sure to set useValMapIso in ggHiNtuplizer to False.
Then, run on the parent RECO for the 75X HiForest you made:
/mnt/hadoop/cms/store/user/tatar/HIHighPt/HiForest_HIHighPt_photon30_HIRun2011-v1/0.root
(I *think* that the parent RECO is this one :
/HIHighPt/dgulhan-HIHighPt_photon20and30_HIRun2011-v1_RECO_753_patch1-fd44351629dd155a25de2b4c109c824c/USER could you please confirm?)
Compare the 53X sigmaIetaIeta from this forest : /mnt/hadoop/cms/store/user/luck/L1Emulator/HiForest_PbPb_photon2030.root to the new results you get from full5x5_sigmaIetaIeta, and see if they match better than the new sigmaIetaIeta. We're trying to see if the full5x5 as calculated by your patch is a better match to the 53X sieie for the same photons. I think you'll only need to run on a few hundred events (so no crab needed), and then do the usual Event Matching.

Please keep me updated via email, and I'll jump in to help when I can find some time tomorrow.

Alex
 *
 * version 2 : match events using "EventMatcher" class. This macro is also considered a test for "EventMatcher" class.
 */

#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TH1.h>
#include <TH1D.h>
#include <TList.h>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

#include "../EventMatcher.h"
#include "../treeUtil.h"
#include "../histoUtil.h"

const int MAXPHOTONS = 500;
const float cutPt = 10;
const float cutEta = 1.4791;

void test_eventMatcher(const char* filePath1, const char* filePath2);
void test_eventMatcher2(const char* filePath);

/*
 * test EventMatcher::addEvent() and EventMatcher::getEntry()
 */
void test_eventMatcher(const char* filePath1, const char* filePath2)
{
    std::cout << "comparing files" <<std::endl;
    std::cout << "fileName1 = " << filePath1 <<std::endl;
    std::cout << "fileName2 = " << filePath2 <<std::endl;

       TFile* inputFile1= new TFile(filePath1, "READ");
       TFile* inputFile2= new TFile(filePath2, "READ");

       TTree *tree1 = (TTree*)inputFile1->Get("ggHiNtuplizer/EventTree");
       TTree *tree2 = (TTree*)inputFile2->Get("multiPhotonAnalyzer/photon");

       TTree *tree1Event = (TTree*)inputFile1->Get("ggHiNtuplizer/EventTree");
       TTree *tree2Event = (TTree*)inputFile2->Get("hltanalysis/HltTree");

       Int_t run1, lumis1;
       Long64_t event1;
       tree1Event->SetBranchAddress("run", &run1);
       tree1Event->SetBranchAddress("event", &event1);
       tree1Event->SetBranchAddress("lumis", &lumis1);

       Int_t nPho;
       std::vector<float>* phoEt=0;
       std::vector<float>* phoEta=0;
       std::vector<float>* phoSigmaIEtaIEta=0;
       std::vector<float>* phoSigmaIEtaIEta_2012=0;

       tree1->SetBranchAddress("nPho",&nPho);
       tree1->SetBranchAddress("phoEt",&phoEt);
       tree1->SetBranchAddress("phoEta",&phoEta);
       tree1->SetBranchAddress("phoSigmaIEtaIEta",&phoSigmaIEtaIEta);
       tree1->SetBranchAddress("phoSigmaIEtaIEta_2012",&phoSigmaIEtaIEta_2012);

       Int_t run2, lumis2;
       Int_t event2; // Long64_t event2;
       tree2Event->SetBranchAddress("Run", &run2);
       tree2Event->SetBranchAddress("Event", &event2);
       tree2Event->SetBranchAddress("LumiBlock", &lumis2);

       Int_t nPhotons;
       Float_t pt[MAXPHOTONS];
       Float_t eta[MAXPHOTONS];
       Float_t sigmaIetaIeta[MAXPHOTONS];

       tree2->SetBranchAddress("nPhotons",&nPhotons);
       tree2->SetBranchAddress("pt",pt);
       tree2->SetBranchAddress("eta",eta);
       tree2->SetBranchAddress("sigmaIetaIeta",sigmaIetaIeta);

       TFile* outFile = new TFile("out_test_eventMatcher.root","RECREATE");

       TH1D* h[3];
       h[0]=new TH1D("h_phoSigmaIEtaIEta","phoSigmaIEtaIEta pT>10, abs(eta)<1.4791",100,0,0.1);
       h[1]=new TH1D("h_phoSigmaIEtaIEta_2012","phoSigmaIEtaIEta_2012 pT>10, abs(eta)<1.4791",100,0,0.1);
       h[2]=new TH1D("h_sigmaIetaIeta","sigmaIetaIeta",100,0,0.1);

       EventMatcher* em2 = new EventMatcher();
       Long64_t entries2 = tree2Event->GetEntries();
       bool passed_test_eventMatcher = true;
       std::cout<< "Loop : fill EventMatcher em2" <<std::endl;
       for (int j=0; j<entries2; ++j)
       {
           if (j % 20000 == 0)  {
             std::cout << "current entry = " <<j<<" out of "<<entries2<<" : "<<std::setprecision(2)<<(double)j/entries2*100<<" %"<<std::endl;
           }
           tree2Event->GetEntry(j);
           em2->addEvent(run2,lumis2,event2,j);
       }
       std::cout<< "Loop ENDED : fill EventMatcher em2" <<std::endl;

       Long64_t entries = tree1Event->GetEntries();
       Long64_t entries_notMatched = 0;
       const char* selection;
       std::cout<< "Loop : ggHiNtuplizer/EventTree" <<std::endl;
       for (int j=0; j<entries; ++j)
       {
           if (j % 2000 == 0)  {
             std::cout << "current entry = " <<j<<" out of "<<entries<<" : "<<std::setprecision(2)<<(double)j/entries*100<<" %"<<std::endl;
           }

           tree1Event->GetEntry(j);
           tree1->GetEntry(j);

           Long64_t j2 = em2->getEntry(run1,lumis1,event1);
           if (j2 == -1)
           {
               std::cout<< "mismatched event : " << selection << std::endl;
               std::cout<<Form("run == %d && event == %lld && lumis == %d", run1, event1, lumis1)<< std::endl;
               entries_notMatched++;
               continue;
           }

           tree2Event->GetEntry(j2);
           tree2->GetEntry(j2);

           std::cout << "j  = " << j <<std::endl;
           std::cout << "j2 = " << j2 <<std::endl;
           std::cout<<Form("run == %d && event == %lld && lumis     == %d", run1, event1, lumis1)<< std::endl;
           std::cout<<Form("Run == %d && Event == %d && LumiBlock == %d", run2, event2, lumis2)<< std::endl;
           if(run1 != run2 || event1!=event2 || lumis1!=lumis2) {
               passed_test_eventMatcher = false;
           }

           for(int i=0; i<nPho; ++i)
           {
               if(!(phoEt->at(i) > cutPt && TMath::Abs(phoEta->at(i))<cutEta)) continue;
               h[0]->Fill(phoSigmaIEtaIEta->at(i));
               h[1]->Fill(phoSigmaIEtaIEta_2012->at(i));
           }

           for(int i=0; i<nPhotons; ++i)
           {
               if(!(pt[i] > cutPt && TMath::Abs(eta[i])<cutEta)) continue;
               h[2]->Fill(sigmaIetaIeta[i]);
           }
       }
       std::cout<< "Loop ENDED : ggHiNtuplizer/EventTree" <<std::endl;

       std::cout << "entries            = " << entries << std::endl;
       std::cout << "entries_notMatched = " << entries_notMatched << std::endl;

       std::cout << "passed_test_eventMatcher = " << passed_test_eventMatcher << std::endl;

       TCanvas* c1 = drawSame(h[0],h[2]);
       TCanvas* c2 = drawSame(h[1],h[2]);

       c1->Write();
       c2->Write();

       outFile->Write();
       outFile->Close();

       inputFile1->Close();
       inputFile2->Close();
}

/*
 * test EventMatcher::addEvent() for duplicate entries, EventMatcher::removeEvent() and EventMatcher::containsEvent()
 */
void test_eventMatcher2(const char* filePath)
{
    TFile *file  = new TFile(filePath, "READ");

    TTree* t1 = (TTree*)file->Get("ggHiNtuplizer/EventTree");

    Int_t run, lumis;
    Long64_t event;
    t1->SetBranchAddress("run", &run);
    t1->SetBranchAddress("event", &event);
    t1->SetBranchAddress("lumis", &lumis);

    EventMatcher* em = new EventMatcher();
    Long64_t duplicateEvents = 0;

    Long64_t entries = t1->GetEntries();
    long long test_run   = -1;
    long long test_lumis = -1;
    long long test_event = -1;
    std::cout<< "Loop : ggHiNtuplizer/EventTree" <<std::endl;
    for (int j=0; j<entries; ++j)
    {
        if (j % 100000 == 0)  {
            std::cout << "current entry = " <<j<<" out of "<<entries<<" : "<<std::setprecision(2)<<(double)j/entries*100<<" %"<<std::endl;
        }

        t1->GetEntry(j);

        bool eventAdded = em->addEvent(run,lumis,event,j);
        if(!eventAdded) // this event is duplicate, skip this one.
        {
            duplicateEvents++;
            continue;
        }

        if(j==1000) // book this event for EventMatcher::removeEvent() and EventMatcher::containsEvent() test
        {
            test_run   = run;
            test_lumis = lumis;
            test_event = event;
        }
    }
    std::cout<< "Loop ENDED : ggHiNtuplizer/EventTree" <<std::endl;

    // test EventMatcher::removeEvent() and EventMatcher::containsEvent()
    bool test_containsEvent  = em->containsEvent(test_run,test_lumis,test_event);
    bool test_containsEvent2 = em->containsEvent(test_run,test_lumis,123454321);   // some random event
    bool test_containsEvent3 = em->containsEvent(113355,test_lumis,225577);   // some random event

    bool test_removeEvent       = em->removeEvent(test_run,test_lumis,test_event);
    bool test_containsEvent_v2  = em->containsEvent(test_run,test_lumis,test_event);
    bool test_removeEvent2      = em->removeEvent(123456789,test_lumis,test_event);   // some random event
    bool test_removeEvent3      = em->removeEvent(test_run,115599,test_event);   // some random event

    std::cout << "entries         = " << entries << std::endl;
    std::cout << "duplicateEvents = " << duplicateEvents << std::endl;

    std::cout << "passed test_containsEvent    = " << test_containsEvent   << std::endl;
    std::cout << "passed test_containsEvent2   = " << !test_containsEvent2 << std::endl;
    std::cout << "passed test_containsEvent3   = " << !test_containsEvent3 << std::endl;

    std::cout << "passed test_removeEvent      = " << test_removeEvent       << std::endl;
    std::cout << "passed test_containsEvent_v2 = " << !test_containsEvent_v2 << std::endl;
    std::cout << "passed test_removeEvent2     = " << !test_removeEvent2      << std::endl;
    std::cout << "passed test_removeEvent3     = " << !test_removeEvent3      << std::endl;

    file->Close();
}

int main(int argc, char** argv)
{
    const char* fileName1="/net/hisrv0001/home/tatar/output/HiForestAOD_full5x5_53X.root";
    const char* fileName2="/mnt/hadoop/cms/store/user/luck/L1Emulator/HiForest_PbPb_photon2030.root";

    const char* fileName="/mnt/hadoop/cms/store/user/luck/HIMinBiasUPC/2011_MB_750_hiForest/0.root";

    std::cout << "running test_eventMatcher()" << fileName2 <<std::endl;
    test_eventMatcher(fileName1, fileName2);

    std::cout << "running test_eventMatcher2()" << fileName2 <<std::endl;
    test_eventMatcher2(fileName);

    return 0;
}
