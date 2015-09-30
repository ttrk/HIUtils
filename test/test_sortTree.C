/*
 * test_sortTree.C
 *
 * code to test function sortTree();
 */

#include "../treeUtil.h"

#include <iostream>
#include <iomanip>
#include <vector>

void test_sortTree();
void test_sortTree_TreeIndex();
void test_sortTree_selection();
void test_sortTree_selection2();
void test_sortTree_selection3();
void test_sortTreeTMath_selection();

void test_sortTree()
{
    const char* filePath1 = "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_CMSSW_7_6_0_pre4.root";
    const char* filePath2 = "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_mnguyen.root";

    TFile* inputFile1 = new TFile(filePath1, "READ");
    TFile* inputFile2 = new TFile(filePath2, "READ");
    TFile* dummyFile  = new TFile("test_sortTree.root", "RECREATE");    // delete this file after the test.
    // "dummyFile" is for the sorted trees. otherwise will get error
/*
    Error in <TBranchElement::Fill>: Failed filling branch:mcPID, nbytes=-1
    Error in <TTree::Fill>: Failed filling branch:EventTree.mcPID, nbytes=-1, entry=464
     This error is symptomatic of a Tree created as a memory-resident Tree
*/

    TTree* tree1Ref = (TTree*)inputFile1->Get("ggHiNtuplizer/EventTree");
    TTree* tree2Ref = (TTree*)inputFile1->Get("ggHiNtuplizerGED/EventTree");
    TTree* tree1 = (TTree*)inputFile2->Get("ggHiNtuplizer/EventTree");
    TTree* tree2 = (TTree*)inputFile2->Get("ggHiNtuplizerGED/EventTree");

    Int_t run1, lumis1;
    Long64_t event1;
    tree1Ref->SetBranchAddress("run", &run1);
    tree1Ref->SetBranchAddress("event", &event1);
    tree1Ref->SetBranchAddress("lumis", &lumis1);

    Int_t run2, lumis2;
    Long64_t event2;
    tree2Ref->SetBranchAddress("run", &run2);
    tree2Ref->SetBranchAddress("event", &event2);
    tree2Ref->SetBranchAddress("lumis", &lumis2);

    Long64_t entries = tree1Ref->GetEntries();  // tree1Ref and tree2Ref have same number of entries
    Long64_t sortedEntries1[entries];
    std::vector<Long64_t> sortedEntries2;

    const char* selection1;
    const char* selection2;
    std::cout<< "Loop : tree1Ref and tree2Ref" <<std::endl;
    for (int j=0; j<entries; ++j)
    {
        if (j % 2000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries<<" : "<<std::setprecision(2)<<(double)j/entries*100<<" %"<<std::endl;
        }

        tree1Ref->GetEntry(j);
        tree2Ref->GetEntry(j);

        selection1 = Form("run == %d && event == %d && lumis == %d", run1, (int)event1, lumis1);
        selection2 = Form("run == %d && event == %d && lumis == %d", run2, (int)event2, lumis2);

        tree1->Draw("Entry$", selection1);
        tree2->Draw("Entry$", selection2);

        Double_t* values1  = tree1->GetV1();
        Double_t* values2  = tree2->GetV1();
        // values1 and values2 will have exactly one element

        sortedEntries1[j] = (Long64_t)values1[0];
        sortedEntries2.push_back((Long64_t)values2[0]);
    }
    std::cout<<"sortedEntries1 has been set."<<std::endl;
    std::cout<<"sortedEntries2 has been set."<<std::endl;

    TTree* tree1_sorted = tree1->CloneTree(0);   // do not copy any entry yet
    TTree* tree2_sorted = tree2->CloneTree(0);   // do not copy any entry yet

    std::cout<<"sortTree() started."<<std::endl;
    sortTree(tree1, tree1_sorted, sortedEntries1, entries);
    sortTree(tree2, tree2_sorted, sortedEntries2);

    std::cout<<"tree1 has been sorted."<<std::endl;
    std::cout<<"tree2 has been sorted."<<std::endl;

    std::cout<<"test the sorting"<<std::endl;
    int lenBranchNames = 3;
    const char* branchNames[] = {"run",
                                 "event",
                                 "lumis"};

    bool sortTree_1_passed = compareTrees(tree1Ref, tree1_sorted, lenBranchNames, branchNames);
    bool sortTree_2_passed = compareTrees(tree2Ref, tree2_sorted, lenBranchNames, branchNames);

    std::cout<< "sortTree_1_passed = " << sortTree_1_passed <<std::endl;
    std::cout<< "sortTree_2_passed = " << sortTree_2_passed <<std::endl;

    std::cout<<"number of branches that changed position after sorting"<<std::endl;
    Long64_t numDifferentBranches1 = compareTreesVerbose(tree1, tree1_sorted, lenBranchNames, branchNames, true);
    Long64_t numDifferentBranches2 = compareTreesVerbose(tree2, tree2_sorted, lenBranchNames, branchNames, true);

    std::cout<< "numDifferentBranches1 = " << numDifferentBranches1 <<std::endl;
    std::cout<< "numDifferentBranches2 = " << numDifferentBranches2 <<std::endl;

    inputFile1->Close();
    inputFile2->Close();
    dummyFile->Close();
}

void test_sortTree_TreeIndex()
{
//    const char* inputfileName="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_AllQCDPhoton30_standard_forest_2nd.root";
    const char* inputfileName=    "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_CMSSW_7_6_0_pre4.root";

    TFile* inputFile = new TFile(inputfileName, "READ");
    TFile* dummyFile = new TFile("test_sortTree.root", "RECREATE");    // delete this file after the test.
    // "dummyFile" is for the sorted trees. otherwise will get error

    const char* branchName = "nMC";

    TTree *tree = (TTree*)inputFile->Get("ggHiNtuplizer/EventTree");
    TTree* tree_sorted1 = tree->CloneTree(0);   // do not copy any entry yet
    TTree* tree_sorted2 = tree->CloneTree(0);   // do not copy any entry yet

    sortTree2(tree, tree_sorted1, branchName);
    sortTree2(tree, tree_sorted2, branchName, true);

    Int_t nMC1, nMC1_before;
    tree_sorted1->SetBranchAddress("nMC", &nMC1);
    nMC1_before = -1;

    Int_t nMC2, nMC2_before;
    tree_sorted2->SetBranchAddress("nMC", &nMC2);
    nMC2_before = 999999;

    Long64_t entries1 = tree_sorted1->GetEntries();
    Long64_t entries2 = tree_sorted2->GetEntries();

    std::cout<< "Loop : tree_sorted1" <<std::endl;
    bool sortTree_1_passed = true;
    for (int j=0; j<entries1; ++j)
    {
        if (j % 10000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries1<<" : "<<std::setprecision(2)<<(double)j/entries1*100<<" %"<<std::endl;
        }

        tree_sorted1->GetEntry(j);
        if (nMC1 < nMC1_before){
            sortTree_1_passed = false;
            break;
        }
        nMC1_before = nMC1;
    }

    std::cout<< "Loop : tree_sorted2" <<std::endl;
    bool sortTree_2_passed = true;
    for (int j=0; j<entries2; ++j)
    {
        if (j % 10000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries2<<" : "<<std::setprecision(2)<<(double)j/entries2*100<<" %"<<std::endl;
        }

        tree_sorted2->GetEntry(j);
        if (nMC2 > nMC2_before){
            sortTree_2_passed = false;
            break;
        }
        nMC2_before = nMC2;
    }

    std::cout<< "sortTree_1_passed = " << sortTree_1_passed <<std::endl;
    std::cout<< "sortTree_2_passed = " << sortTree_2_passed <<std::endl;

    inputFile->Close();
    dummyFile->Close();
}

void test_sortTree_selection()
{
//    const char* inputfileName="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_AllQCDPhoton30_standard_forest_2nd.root";
    const char* inputfileName=    "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_CMSSW_7_6_0_pre4.root";

    TFile* inputFile = new TFile(inputfileName, "READ");
    TFile* dummyFile = new TFile("test_sortTree.root", "RECREATE");    // delete this file after the test.
    // "dummyFile" is for the sorted trees. otherwise will get error

    const char* branchName = "nMC";

    TTree *tree = (TTree*)inputFile->Get("ggHiNtuplizer/EventTree");
    TTree* tree_sorted1 = tree->CloneTree(0);   // do not copy any entry yet
    TTree* tree_sorted2 = tree->CloneTree(0);   // do not copy any entry yet

    sortTree(tree, tree_sorted1, branchName);
    sortTree(tree, tree_sorted2, branchName, "1", true);

    Int_t nMC1, nMC1_before;
    tree_sorted1->SetBranchAddress("nMC", &nMC1);
    nMC1_before = -1;

    Int_t nMC2, nMC2_before;
    tree_sorted2->SetBranchAddress("nMC", &nMC2);
    nMC2_before = 999999;

    Long64_t entries1 = tree_sorted1->GetEntries();
    Long64_t entries2 = tree_sorted2->GetEntries();

    std::cout<< "Loop : tree_sorted1" <<std::endl;
    bool sortTree_1_passed = true;
    for (int j=0; j<entries1; ++j)
    {
        if (j % 10000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries1<<" : "<<std::setprecision(2)<<(double)j/entries1*100<<" %"<<std::endl;
        }

        tree_sorted1->GetEntry(j);
        if (nMC1 < nMC1_before){
            sortTree_1_passed = false;
            break;
        }
        nMC1_before = nMC1;
    }

    std::cout<< "Loop : tree_sorted2" <<std::endl;
    bool sortTree_2_passed = true;
    for (int j=0; j<entries2; ++j)
    {
        if (j % 10000 == 0)  {
          std::cout << "current entry = " <<j<<" out of "<<entries2<<" : "<<std::setprecision(2)<<(double)j/entries2*100<<" %"<<std::endl;
        }

        tree_sorted2->GetEntry(j);
        if (nMC2 > nMC2_before){
            sortTree_2_passed = false;
            break;
        }
        nMC2_before = nMC2;
    }

    std::cout<< "sortTree_1_passed = " << sortTree_1_passed <<std::endl;
    std::cout<< "sortTree_2_passed = " << sortTree_2_passed <<std::endl;

    inputFile->Close();
    dummyFile->Close();
}

void test_sortTree_selection2()
{
//    const char* inputfileName="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_AllQCDPhoton30_standard_forest_2nd.root";
    const char* inputfileName=    "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_CMSSW_7_6_0_pre4.root";

    TFile* inputFile = new TFile(inputfileName, "READ");
    TFile* dummyFile = new TFile("test_sortTree.root", "RECREATE");    // delete this file after the test.
    // "dummyFile" is for the sorted trees. otherwise will get error

    const char* branchName = "nMC";
    const char* selection  = "phoEt[0]>20";

    TTree *tree = (TTree*)inputFile->Get("ggHiNtuplizer/EventTree");
    TTree* tree_sorted1 = tree->CloneTree(0);   // do not copy any entry yet
    TTree* tree_sorted2 = tree->CloneTree(0);   // do not copy any entry yet

    tree_sorted1->SetName("EventTree_tree_sorted1");
    tree_sorted2->SetName("EventTree_tree_sorted2");

    sortTree(tree, tree_sorted1, branchName, selection);
    sortTree(tree, tree_sorted2, branchName, selection, true);

    Int_t nMC1, nMC1_before;
    std::vector<float>* phoEt1=0;
    tree_sorted1->SetBranchAddress("nMC",   &nMC1);
    tree_sorted1->SetBranchAddress("phoEt", &phoEt1);
    nMC1_before = -1;

    Int_t nMC2, nMC2_before;
    std::vector<float>* phoEt2=0;
    tree_sorted2->SetBranchAddress("nMC",   &nMC2);
    tree_sorted2->SetBranchAddress("phoEt", &phoEt2);
    nMC2_before = 999999;

    Long64_t entriesSelected = (Int_t)tree->GetEntries(selection);
    Long64_t entries1 = tree_sorted1->GetEntries();
    Long64_t entries2 = tree_sorted2->GetEntries();

    std::cout<< "entriesSelected = " << entriesSelected <<std::endl;
    std::cout<< "entries1 = " << entries1 <<std::endl;
    std::cout<< "entries2 = " << entries2 <<std::endl;

    std::cout<< "Loop : tree_sorted1" <<std::endl;
    bool sortTree_1_passed = true;
    if(entriesSelected != entries1){
        sortTree_1_passed = false;
    }
    else {
        for (int j=0; j<entries1; ++j)
        {
            if (j % 10000 == 0)  {
                std::cout << "current entry = " <<j<<" out of "<<entries1<<" : "<<std::setprecision(2)<<(double)j/entries1*100<<" %"<<std::endl;
            }

            tree_sorted1->GetEntry(j);
            if (nMC1 < nMC1_before || phoEt1->at(0) <= 20){
                sortTree_1_passed = false;
                break;
            }
            nMC1_before = nMC1;
        }
    }

    std::cout<< "Loop : tree_sorted2" <<std::endl;
    bool sortTree_2_passed = true;
    if(entriesSelected != entries2){
        sortTree_2_passed = false;
    }
    else {
        for (int j=0; j<entries2; ++j)
        {
            if (j % 10000 == 0)  {
                std::cout << "current entry = " <<j<<" out of "<<entries2<<" : "<<std::setprecision(2)<<(double)j/entries2*100<<" %"<<std::endl;
            }

            tree_sorted2->GetEntry(j);
            if (nMC2 > nMC2_before || phoEt2->at(0) <= 20){
                sortTree_2_passed = false;
                break;
            }
            nMC2_before = nMC2;
        }
    }

    std::cout<< "sortTree_1_passed = " << sortTree_1_passed <<std::endl;
    std::cout<< "sortTree_2_passed = " << sortTree_2_passed <<std::endl;

    inputFile->Close();
    dummyFile->Close();
}

void test_sortTree_selection3()
{
//    const char* inputfileName="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_AllQCDPhoton30_standard_forest_2nd.root";
    const char* inputfileName=    "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_CMSSW_7_6_0_pre4.root";

    TFile* inputFile = new TFile(inputfileName, "READ");
    TFile* dummyFile = new TFile("test_sortTree.root", "RECREATE");    // delete this file after the test.
    // "dummyFile" is for the sorted trees. otherwise will get error

    const char* branchName = "phoEta[0]";
    const char* selection  = "abs(phoPhi[0])<1.25";

    TTree *tree = (TTree*)inputFile->Get("ggHiNtuplizer/EventTree");
    TTree* tree_sorted1 = tree->CloneTree(0);   // do not copy any entry yet
    TTree* tree_sorted2 = tree->CloneTree(0);   // do not copy any entry yet

    tree_sorted1->SetName("EventTree_tree_sorted1");
    tree_sorted2->SetName("EventTree_tree_sorted2");

    sortTree(tree, tree_sorted1, branchName, selection);
    sortTree(tree, tree_sorted2, branchName, selection, true);

    float phoEta1_before = -999;
    std::vector<float>* phoPhi1=0;
    std::vector<float>* phoEta1=0;
    tree_sorted1->SetBranchAddress("phoPhi", &phoPhi1);
    tree_sorted1->SetBranchAddress("phoEta", &phoEta1);

    float phoEta2_before = 999;
    std::vector<float>* phoPhi2=0;
    std::vector<float>* phoEta2=0;
    tree_sorted2->SetBranchAddress("phoPhi", &phoPhi2);
    tree_sorted2->SetBranchAddress("phoEta", &phoEta2);

    Long64_t entriesSelected = (Int_t)tree->GetEntries(selection);
    Long64_t entries1 = tree_sorted1->GetEntries();
    Long64_t entries2 = tree_sorted2->GetEntries();

    std::cout<< "entriesSelected = " << entriesSelected <<std::endl;
    std::cout<< "entries1 = " << entries1 <<std::endl;
    std::cout<< "entries2 = " << entries2 <<std::endl;

    std::cout<< "Loop : tree_sorted1" <<std::endl;
    bool sortTree_1_passed = true;
    if(entriesSelected != entries1){
        sortTree_1_passed = false;
    }
    else {
        for (int j=0; j<entries1; ++j)
        {
            if (j % 10000 == 0)  {
                std::cout << "current entry = " <<j<<" out of "<<entries1<<" : "<<std::setprecision(2)<<(double)j/entries1*100<<" %"<<std::endl;
            }

            tree_sorted1->GetEntry(j);
            if ((phoEta1->at(0) < phoEta1_before) || !(TMath::Abs(phoPhi1->at(0)) < 1.25)){
                sortTree_1_passed = false;
                break;
            }
            phoEta1_before = phoEta1->at(0);
        }
    }

    std::cout<< "Loop : tree_sorted2" <<std::endl;
    bool sortTree_2_passed = true;
    if(entriesSelected != entries2){
        sortTree_2_passed = false;
    }
    else {
        for (int j=0; j<entries2; ++j)
        {
            if (j % 10000 == 0)  {
                std::cout << "current entry = " <<j<<" out of "<<entries2<<" : "<<std::setprecision(2)<<(double)j/entries2*100<<" %"<<std::endl;
            }

            tree_sorted2->GetEntry(j);
            if ((phoEta2->at(0) > phoEta2_before) || !(TMath::Abs(phoPhi2->at(0)) < 1.25)){
                sortTree_2_passed = false;
                break;
            }
            phoEta2_before = phoEta2->at(0);
        }
    }

    std::cout<< "sortTree_1_passed = " << sortTree_1_passed <<std::endl;
    std::cout<< "sortTree_2_passed = " << sortTree_2_passed <<std::endl;

    inputFile->Close();
    dummyFile->Close();
}


void test_sortTreeTMath_selection()
{
//    const char* inputfileName="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_AllQCDPhoton30_standard_forest_2nd.root";
    const char* inputfileName=    "/mnt/hadoop/cms/store/user/tatar/reLinkingTest/RelValZEEMM_13_HI/HiForest_merged_CMSSW_7_6_0_pre4.root";

    TFile* inputFile = new TFile(inputfileName, "READ");
    TFile* dummyFile = new TFile("test_sortTree.root", "RECREATE");    // delete this file after the test.
    // "dummyFile" is for the sorted trees. otherwise will get error

    const char* branchName = "nMC";
    const char* selection  = "phoEt[0]>20";

    TTree *tree = (TTree*)inputFile->Get("ggHiNtuplizer/EventTree");
    TTree* tree_sorted1 = tree->CloneTree(0);   // do not copy any entry yet
    TTree* tree_sorted2 = tree->CloneTree(0);   // do not copy any entry yet

    tree_sorted1->SetName("EventTree_tree_sorted1");
    tree_sorted2->SetName("EventTree_tree_sorted2");

    sortTreeTMath(tree, tree_sorted1, branchName, selection);
    sortTreeTMath(tree, tree_sorted2, branchName, selection, true);

    Int_t nMC1, nMC1_before;
    std::vector<float>* phoEt1=0;
    tree_sorted1->SetBranchAddress("nMC",   &nMC1);
    tree_sorted1->SetBranchAddress("phoEt", &phoEt1);
    nMC1_before = -1;

    Int_t nMC2, nMC2_before;
    std::vector<float>* phoEt2=0;
    tree_sorted2->SetBranchAddress("nMC",   &nMC2);
    tree_sorted2->SetBranchAddress("phoEt", &phoEt2);
    nMC2_before = 999999;

    Long64_t entriesSelected = (Int_t)tree->GetEntries(selection);
    Long64_t entries1 = tree_sorted1->GetEntries();
    Long64_t entries2 = tree_sorted2->GetEntries();

    std::cout<< "entriesSelected = " << entriesSelected <<std::endl;
    std::cout<< "entries1 = " << entries1 <<std::endl;
    std::cout<< "entries2 = " << entries2 <<std::endl;

    std::cout<< "Loop : tree_sorted1" <<std::endl;
    bool sortTree_1_passed = true;
    if(entriesSelected != entries1){
        sortTree_1_passed = false;
    }
    else {
        for (int j=0; j<entries1; ++j)
        {
            if (j % 10000 == 0)  {
                std::cout << "current entry = " <<j<<" out of "<<entries1<<" : "<<std::setprecision(2)<<(double)j/entries1*100<<" %"<<std::endl;
            }

            tree_sorted1->GetEntry(j);
            if (nMC1 < nMC1_before || phoEt1->at(0) <= 20){
                sortTree_1_passed = false;
                break;
            }
            nMC1_before = nMC1;
        }
    }

    std::cout<< "Loop : tree_sorted2" <<std::endl;
    bool sortTree_2_passed = true;
    if(entriesSelected != entries2){
        sortTree_2_passed = false;
    }
    else {
        for (int j=0; j<entries2; ++j)
        {
            if (j % 10000 == 0)  {
                std::cout << "current entry = " <<j<<" out of "<<entries2<<" : "<<std::setprecision(2)<<(double)j/entries2*100<<" %"<<std::endl;
            }

            tree_sorted2->GetEntry(j);
            if (nMC2 > nMC2_before || phoEt2->at(0) <= 20){
                sortTree_2_passed = false;
                break;
            }
            nMC2_before = nMC2;
        }
    }

    std::cout<< "sortTree_1_passed = " << sortTree_1_passed <<std::endl;
    std::cout<< "sortTree_2_passed = " << sortTree_2_passed <<std::endl;

    inputFile->Close();
    dummyFile->Close();
}

int main()
{
//    std::cout<< "running : test_sortTree()" <<std::endl;
//    test_sortTree();
//    std::cout<< "running : test_sortTree_TreeIndex()" <<std::endl;
//    test_sortTree_TreeIndex();
//    std::cout<< "running : test_sortTree_selection()" <<std::endl;
//    test_sortTree_selection();
    std::cout<< "running : test_sortTree_selection2()" <<std::endl;
    test_sortTree_selection2();
    std::cout<< "running : test_sortTreeTMath_selection()" <<std::endl;
    test_sortTreeTMath_selection();
    std::cout<< "running : test_sortTree_selection3()" <<std::endl;
    test_sortTree_selection3();
}
