/*
 * test_compareTrees.C
 *
 * code to test function compareTrees();
 */

#include "../treeUtil.h"

#include <iostream>

void test_compareTrees();

void test_compareTrees()
{
    const char* inputfileName;
    const char* inputfileName2;
    inputfileName="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_AllQCDPhoton30_standard_forest_2nd.root";
    inputfileName2="/mnt/hadoop/cms/store/user/tatar/GEDPhotonCommissioning/merged_EmEnrichedDijet30_standard_forest.root";

    TFile *inputFile = new TFile(inputfileName, "READ");

    int lenBranchNames = 7;
    const char* branchNames[] = {"nMC",
                                 "mcPt",
                                 "mcEta",
                                 "mcPID",
                                 "mcMomPID",
                                 "mcStatus",
                                 "mcCalIsoDR04"};

    TTree *tree1 = (TTree*)inputFile->Get("ggHiNtuplizer/EventTree");
    TTree *tree2 = (TTree*)inputFile->Get("ggHiNtuplizerGED/EventTree");

    // test 1, passed
    bool treesAreSame = compareTrees(tree1, tree2, lenBranchNames, branchNames);

    std::cout << "treesAreSame = " << treesAreSame << std::endl;  // treesAreSame = 1

    // test 2, passed
    bool treesAreSame2 = compareTrees(new TFile(inputfileName2), "ggHiNtuplizer/EventTree",
                                      new TFile(inputfileName2), "ggHiNtuplizerGED/EventTree",
                                      lenBranchNames,branchNames);

    std::cout << "treesAreSame2 = " << treesAreSame2 << std::endl;  // treesAreSame2 = 1

    // test 3, passed
    bool treesAreSame3 = compareTrees(new TFile(inputfileName2), "ggHiNtuplizer/EventTree",
                                      new TFile(inputfileName2), "ggHiNtuplizerGED/EventTree",
                                      0,branchNames);

    std::cout << "treesAreSame3 = " << treesAreSame3 << std::endl;  // treesAreSame3 = 0

    // test 4, passed
    int numDifferentBranches = compareTreesVerbose(tree1, tree2, lenBranchNames, branchNames);  // verbose = false

    std::cout << "numDifferentBranches = " << numDifferentBranches << std::endl;  // numDifferentBranches = 0

    // test 5, passed
    int numDifferentBranches2 = compareTreesVerbose(new TFile(inputfileName2), "ggHiNtuplizer/EventTree",
                                                    new TFile(inputfileName2), "ggHiNtuplizerGED/EventTree",
                                                    lenBranchNames,branchNames);      // verbose = false

    std::cout << "numDifferentBranches2 = " << numDifferentBranches2 << std::endl;  // numDifferentBranches2 = 0

    // test 6, passed
    int numDifferentBranches3 = compareTreesVerbose(new TFile(inputfileName2), "ggHiNtuplizer/EventTree",
                                                    new TFile(inputfileName2), "ggHiNtuplizerGED/EventTree",
                                                    0,branchNames);       // verbose = false

    std::cout << "numDifferentBranches3 = " << numDifferentBranches3 << std::endl;  // numDifferentBranches3 = 33 > 0
}

int main()
{
    test_compareTrees();
}
