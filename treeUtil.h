/*
 * utilities related to TTree objects.
 */

#ifndef TREEUTIL_H_
#define TREEUTIL_H_

#include <TTree.h>
#include <TEntryList.h>
#include <TEntryListArray.h>
#include <TEventList.h>
#include <TH1.h>
#include <TFile.h>
#include <TTreeIndex.h>
#include <TMath.h>

#include <cstdarg>       // for functions which take a variable number of arguments.
#include <iostream>      // std::cout
#include <algorithm>     // std::sort, std::generate

void drawMaximumGeneral   (TTree* tree, TString formula, TString formulaForMax, TString conditionForMax = "1", TH1* hist = NULL);
void drawMaximumGeneral   (TTree* tree, TString formula, TString formulaForMax, TString conditionForMax = "1", TString cut = "1", TH1* hist = NULL);
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString conditionForMax = "1", TH1* hist = NULL);
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString conditionForMax = "1", TString cut = "1", TH1* hist = NULL);

TEventList* getMaximumEventList   (TTree* tree, TString formulaForMax, TString conditionForMax = "1", TString cut = "1");
TEventList* getMaximum2ndEventList(TTree* tree, TString formulaForMax, TString conditionForMax = "1", TString cut = "1");

bool compareTrees(TTree* tree1, TTree* tree2, int lenBranchNames = 0, const char* branchNames[] = NULL);
bool compareTrees(TFile* file1, const char* tree1Path, TFile* file2, const char* tree2Path, int lenBranchNames = 0, const char* branchNames[] = NULL);
int  compareTreesVerbose(TTree* tree1, TTree* tree2, int lenBranchNames = 0, const char* branchNames[] = NULL, bool verbose = false);
int  compareTreesVerbose(TFile* file1, const char* tree1Path, TFile* file2, const char* tree2Path, int lenBranchNames = 0, const char* branchNames[] = NULL, bool verbose = false);
/// DISCOURAGED
bool compareTrees(TTree* tree1, TTree* tree2, std::string selection1 = "1", std::string selection2 = "1",
                  int lenBranchNames = 0, const char* branchNames[] = NULL );
/// DISCOURAGED - END

void sortTree(TTree* inputTree, TTree* outputTree, Long64_t sortedEntries[], Long64_t lenEntries);
void sortTree(TTree* inputTree, TTree* outputTree, const std::vector<Long64_t>& sortedEntries);
void sortTree     (TTree* inputTree, TTree* outputTree, const char* branchName, const char* selection = "1", bool decreasing = false);
void sortTreeTMath(TTree* inputTree, TTree* outputTree, const char* branchName, const char* selection = "1", bool decreasing = false);

TString mergeCuts(TString cut1, TString cut2);
TString mergeCuts2(int nCuts, ...);

/// DEPRECATED
void drawMaximum(TTree* tree, TString formula, TString condition = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum(TTree* tree, TString formula, TString condition = "1", TString cut = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum2nd(TTree* tree, TString formula, TString condition = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum2nd(TTree* tree, TString formula, TString condition = "1", TString cut = "1", TH1* hist = NULL, bool plotZero = false);
TEntryList* getMaximumEntryList   (TTree* tree, TString formulaForMax, TString conditionForMax = "1", TString cut = "1");
TEntryList* getMaximum2ndEntryList(TTree* tree, TString formulaForMax, TString conditionForMax = "1", TString cut = "1");
TEntryListArray* getMaximumEntryListArray(TTree* tree, TString formulaForMax, TString conditionForMax = "1", TString cut = "1");
void sortTree2(TTree* inputTree, TTree* outputTree, const char* branchName, bool decreasing = false);
/// DEPRECATED - END

/*
 * plot the maximum value of the elements of a "formula" where the elements satisfy the "condition".
 * If no element satisfies "condition" : if plotZero is true, then 0 is plotted. Otherwise nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
void drawMaximum(TTree* tree, TString formula, TString condition, TH1* hist, bool plotZero)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    if(plotZero) {
        tree->Draw(Form("Max$(%s*(%s)) >> %s",formula.Data(), condition.Data(), hname));
    }
    else {
        tree->Draw(Form("Max$(%s*(%s)) >> %s",formula.Data(), condition.Data(), hname), Form("%s == Max$(%s*(%s))",
                                                                                        formula.Data(), formula.Data(), condition.Data()));
//        drawMaximumGeneral(tree, formula, formula, condition, hist);
    }
}

/*
 * plot the maximum value of the elements of a "formula" where the elements satisfy the "condition".
 * If no element satisfies "condition" : if plotZero is true, then 0 is plotted. Otherwise nothing is plotted.
 * Additional cuts to the maximum element after it is found are given by "cut".
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * Example : plotting the maximum pT for photons that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
void drawMaximum(TTree* tree, TString formula, TString condition, TString cut, TH1* hist, bool plotZero)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    if(plotZero){
        tree->Draw(Form("Max$(%s*(%s)) >> %s",formula.Data(), condition.Data(), hname), cut.Data());
    }
    else {
        tree->Draw(Form("Max$(%s*(%s)) >> %s",formula.Data(), condition.Data(), hname), Form("%s == Max$(%s*(%s)) && %s",
                                                                                        formula.Data(), formula.Data(), condition.Data(),
                                                                                        cut.Data()));
//        drawMaximumGeneral(tree, formula, formula, condition, cut, hist);
    }
}

/*
 * plot "formula" corresponding to the element with maximum value for "formulaForMax" where the element satisfies the "conditionForMax".
 * If no element satisfies "condition", then nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 * this function uses "TEventList" to get the list of events. Using "TEntryList" does not work.
 *
 * Example : plotting the eta for the maximum pT photon that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 *
 */
void drawMaximumGeneral(TTree* tree, TString formula, TString formulaForMax, TString conditionForMax, TH1* hist)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    TEventList* elist_original = tree->GetEventList();
    TEventList* elist = getMaximumEventList(tree, formulaForMax, conditionForMax);
    elist->SetReapplyCut(true); // among the elements in the event pick only the one that passes the selection
    tree->SetEventList(elist);
    tree->Draw(Form("%s >> %s", formula.Data(), hname));

    tree->SetEventList(elist_original);   // restore the original event list after making the histogram
                                          // tree->SetEntryList(elist_original);  --> this approach does not work
    elist->Delete();
}

/*
 * plot "formula" corresponding to the element with maximum value for "formulaForMax" where the element satisfies the "conditionForMax".
 * If no element satisfies "conditionForMax", then nothing is plotted.
 * Additional cuts to the maximum element after it is found are given by "cut".
 * The plot will be stored in histogram "hist" if it is supplied.
 * this function uses "TEventList" to get the list of events. Using "TEntryList" does not work.
 *
 * Example : plotting the eta for the maximum pT photon that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 *
 */
void drawMaximumGeneral(TTree* tree, TString formula, TString formulaForMax, TString conditionForMax, TString cut, TH1* hist)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    TEventList* elist_original = tree->GetEventList();
    TEventList* elist = getMaximumEventList(tree, formulaForMax, conditionForMax, cut);
    elist->SetReapplyCut(true); // among the elements in the event pick only the one that passes the selection
    tree->SetEventList(elist);
    tree->Draw(Form("%s >> %s", formula.Data(), hname));

    tree->SetEventList(elist_original);   // restore the original event list after making the histogram
                                          // tree->SetEntryList(elist_original);  --> this approach does not work
    elist->Delete();
}

/*
 * plot the 2nd maximum value of the elements of a "formula" where the elements satisfy the "condition".
 * If no element satisfies "condition" : if plotZero is true, then 0 is plotted. Otherwise nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
void drawMaximum2nd(TTree* tree, TString formula, TString condition, TH1* hist, bool plotZero)
{
    const char* hname="hDrawMax2nd";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    if(plotZero) {
        tree->Draw(Form("Max$(%s*(%s < Max$(%s*(%s)))*(%s)) >> %s",formula.Data(),formula.Data(),formula.Data(),
                                                                    condition.Data(), condition.Data(), hname));
    }
    else {
        drawMaximum2ndGeneral(tree, formula, formula, condition, hist);
    }
}

/*
 * plot the 2nd maximum value of the elements of a "formula" where the elements satisfy the "condition".
 * If no element satisfies "condition" : if plotZero is true, then 0 is plotted. Otherwise nothing is plotted.
 * Additional cuts to the 2nd maximum element after it is found are given by "cut".
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
void drawMaximum2nd(TTree* tree, TString formula, TString condition, TString cut, TH1* hist, bool plotZero)
{
    const char* hname="hDrawMax2nd";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    if(plotZero) {
        tree->Draw(Form("Max$(%s*(%s < Max$(%s*(%s)))*(%s)) >> %s",formula.Data(),formula.Data(),formula.Data(),
                                                                    condition.Data(), condition.Data(), hname),
                                                                    cut.Data());
    }
    else {
        drawMaximum2ndGeneral(tree, formula, formula, condition, cut, hist);
    }
}

/*
 * plot "formula" corresponding to the element with 2nd maximum value for "formulaForMax" where the element satisfies the "condition".
 * If no element satisfies "condition", then nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * Example : plotting the eta for the subleading pT photon that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString conditionForMax, TH1* hist)
{
    const char* hname="hDrawMax2nd";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    TEventList* elist_original = tree->GetEventList();
    TEventList* elist = getMaximum2ndEventList(tree, formulaForMax, conditionForMax);
    elist->SetReapplyCut(true); // among the elements in the event pick only the one that passes the selection
    tree->SetEventList(elist);
    tree->Draw(Form("%s >> %s", formula.Data(), hname));

    tree->SetEventList(elist_original);   // restore the original event list after making the histogram
                                          // tree->SetEntryList(elist_original);  --> this approach does not work
    elist->Delete();
}

/*
 * plot "formula" corresponding to the element with 2nd maximum value for "formulaForMax" where the element satisfies the "condition".
 * If no element satisfies "condition", then nothing is plotted.
 * Additional cuts to the 2nd maximum element after it is found are given by "cut".
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * Example : plotting the eta for the subleading pT photon that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString conditionForMax, TString cut, TH1* hist)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }

    TEventList* elist_original = tree->GetEventList();
    TEventList* elist = getMaximum2ndEventList(tree, formulaForMax, conditionForMax, cut);
    elist->SetReapplyCut(true); // among the elements in the event pick only the one that passes the selection
    tree->SetEventList(elist);
    tree->Draw(Form("%s >> %s", formula.Data(), hname));

    tree->SetEventList(elist_original);   // restore the original event list after making the histogram
                                          // tree->SetEntryList(elist_original);  --> this approach does not work
    elist->Delete();
}

/*
 * get list of entries where there is an element with maximum value for "formulaForMax" and this element satisfies the "conditionForMax".
 * Additional cuts to the maximum element after it is found are given by "cut".
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
TEntryList* getMaximumEntryList(TTree* tree, TString formulaForMax, TString conditionForMax, TString cut)
{
    const char* entrylist_name="entrylistMax";

    tree->Draw(Form(">> %s", entrylist_name) ,Form("%s == Max$(%s*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,conditionForMax.Data() ,cut.Data()), "entrylist");

    TEntryList* entrylist = (TEntryList*)gDirectory->Get(entrylist_name);
    return entrylist;
}

/*
 * get list of events where there is an element with maximum value for "formulaForMax" and this element satisfies the "conditionForMax".
 * Additional cuts to the maximum element after it is found are given by "cut".
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
TEventList* getMaximumEventList(TTree* tree, TString formulaForMax, TString conditionForMax, TString cut)
{
    const char* eventlist_name="eventlistMax";

    tree->Draw(Form(">> %s", eventlist_name) ,Form("%s == Max$(%s*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,conditionForMax.Data() ,cut.Data()));

    TEventList* eventlist = (TEventList*)gDirectory->Get(eventlist_name);
    return eventlist;
}

/*
 * get list of entries where there is an element with 2nd maximum value for "formulaForMax" and this element satisfies the "conditionForMax".
 * Additional cuts to the maximum element after it is found are given by "cut".
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
TEntryList* getMaximum2ndEntryList(TTree* tree, TString formulaForMax, TString conditionForMax, TString cut)
{
    const char* entrylist_name="entrylistMax2nd";

    tree->Draw(Form(">> %s", entrylist_name) ,Form("%s == Max$(%s*(%s < Max$(%s*(%s)))*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,
                                                                                                              conditionForMax.Data() ,conditionForMax.Data() ,cut.Data()));

    TEntryList* entrylist = (TEntryList*)gDirectory->Get(entrylist_name);
    return entrylist;
}

/*
 * get list of events where there is an element with 2nd maximum value for "formulaForMax" and this element satisfies the "conditionForMax".
 * Additional cuts to the maximum element after it is found are given by "cut".
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
TEventList* getMaximum2ndEventList(TTree* tree, TString formulaForMax, TString conditionForMax, TString cut)
{
    const char* eventlist_name="eventlistMax2nd";

    tree->Draw(Form(">> %s", eventlist_name) ,Form("%s == Max$(%s*(%s < Max$(%s*(%s)))*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,
                                                                                                              conditionForMax.Data() ,conditionForMax.Data() ,cut.Data()));

    TEventList* eventlist = (TEventList*)gDirectory->Get(eventlist_name);
    return eventlist;
}

/*
 * get list of arrays for entries where there is an element with maximum value for "formulaForMax" and this element satisfies the "conditionForMax".
 * Additional cuts to the maximum element after it is found are given by "cut".
 *
 * Normally "TEntryListArray" should have an array for entries where there is an element that passes a selection.
 * Elements of the array are the elements of the entry that passes a selection. Since we look for a maximum element
 * and assume that it is unique, the length of such an array can be at most one.
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
/// DEPRECATED
TEntryListArray* getMaximumEntryListArray(TTree* tree, TString formulaForMax, TString conditionForMax, TString cut)
{
    const char* entrylist_name="entrylistarrayMax";

    tree->Draw(Form(">> %s", entrylist_name) ,Form("%s == Max$(%s*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,conditionForMax.Data() ,cut.Data()), "entrylistarray");

    TEntryListArray* entrylistarray = (TEntryListArray*)gDirectory->Get(entrylist_name);
    return entrylistarray;
}

/*
 * general function to compare "TTree"s. A comparison is based on a list of branches.
 *
 * returns false if comparison does not succeed
 *
 * If a list of branches is given then the comparison will be based on that list.
 * If no list is given, the comparison will be based on all the branches. In that case tree1 and tree2 must have the same number of branches.
 *
 * */
bool compareTrees(TTree* tree1, TTree* tree2, int lenBranchNames, const char* branchNames[])
{
    TString aliasStr = Form("%s_2",tree1->GetName());
    const char* alias = aliasStr.Data();
    tree1->AddFriend(tree2, alias);   // "tree2" will be referred with "alias"

    bool useAll = false;
    TObjArray* branchList = tree1->GetListOfBranches();

    if(lenBranchNames==0)
    {
        // do comparison over all the branches
        useAll=true;

        lenBranchNames = branchList->GetSize();

        if(lenBranchNames != tree2->GetListOfBranches()->GetSize())    // tree1 and tree2 must have the same number of branches
        {
            return false;
        }
    }

    const int len = lenBranchNames;     // use a "const" variable for array dimension to avoid the following error in a ROOT session
                                        // Error: Non-static-const variable in array dimension
    const char* branches[len];
    // set the array "branches"
    // comparison will be based on this list of branches.
    if(useAll) {
        for(int i=0 ; i<len; ++i)
        {
            branches[i]=branchList->At(i)->GetName();
        }
    }
    else {
        for(int i=0 ; i<len; ++i)
        {
            branches[i]=branchNames[i];
        }
    }
    // the array "branches" has been set.

    TString selection;
    for(int i=0; i<len; ++i)
    {
        selection = Form("abs(%s - %s.%s)>0", branches[i], alias, branches[i]);
        int notEqual = tree1->GetEntries(selection.Data());
        if(notEqual>0)
        {
/*
            // report
            std::cout<< "branch is not same : " << branches[i] << ", different entries = " << notEqual <<std::endl;
*/
            return false;
        }
    }

    return true;
}

bool compareTrees(TFile* file1, const char* tree1Path, TFile* file2, const char* tree2Path,
                  int lenBranchNames, const char* branchNames[])
{
    TTree *t1 = (TTree*)file1->Get(tree1Path);
    TTree *t2 = (TTree*)file2->Get(tree2Path);

    return compareTrees(t1, t2, lenBranchNames, branchNames);
}

/*
 * general function to compare "TTree"s. A comparison is based on a list of branches.
 * this is more detailed version of function
 * bool compareTrees(TTree* tree1, TTree* tree2, int lenBranchNames, const char* branchNames[])
 *
 * returns the number of branches that are not same.
 *
 * If a list of branches is given then the comparison will be based on that list.
 * If no list is given, the comparison will be based on all the branches. In that case tree1 and tree2 must have the same number of branches.
 *
 * if "verbose" is true, then for each branch that are not same,
 *                       it prints the number entries where these branches differ.
 *
 * */
int compareTreesVerbose(TTree* tree1, TTree* tree2, int lenBranchNames, const char* branchNames[], bool verbose)
{
    TString aliasStr = Form("%s_2",tree1->GetName());
    const char* alias = aliasStr.Data();
    tree1->AddFriend(tree2, alias);   // "tree2" will be referred with "alias"

    bool useAll = false;
    TObjArray* branchList = tree1->GetListOfBranches();

    int differentBranches = 0;
    if(lenBranchNames==0)
    {
        // do comparison over all the branches
        useAll=true;

        lenBranchNames = branchList->GetSize();

        if(lenBranchNames != tree2->GetListOfBranches()->GetSize())    // tree1 and tree2 must have the same number of branches
        {
            if (verbose)
            {
                std::cout<<"tree1 and tree2 must have the same number of branches" <<std::endl;
                std::cout<<"tree1, number of branches = " << lenBranchNames <<std::endl;
                std::cout<<"tree2, number of branches = " << tree2->GetListOfBranches()->GetSize() <<std::endl;
            }
            differentBranches = abs(lenBranchNames - tree2->GetListOfBranches()->GetSize());
            return differentBranches;
        }
    }

    const int len = lenBranchNames;     // use a "const" variable for array dimension to avoid the following error in a ROOT session
                                        // Error: Non-static-const variable in array dimension
    const char* branches[len];
    // set the array "branches"
    // comparison will be based on this list of branches.
    if(useAll) {
        for(int i=0 ; i<len; ++i)
        {
            branches[i]=branchList->At(i)->GetName();
        }
    }
    else {
        for(int i=0 ; i<len; ++i)
        {
            branches[i]=branchNames[i];
        }
    }
    // the array "branches" has been set.

    TString selection;
    for(int i=0; i<len; ++i)
    {
        selection = Form("abs(%s - %s.%s)>0", branches[i], alias, branches[i]);
        int notEqual = tree1->GetEntries(selection.Data());
        if(notEqual>0)
        {
            differentBranches++;
            if (verbose) {
                // report
                std::cout<< "branch not same : " << branches[i] << ", different entries = " << notEqual <<std::endl;
            }
        }
        else if (notEqual == 0)
        {
            if (verbose) {
                // report
                std::cout<< "branch is same  : " << branches[i] <<std::endl;
            }
        }
    }

    return differentBranches;
}

int compareTreesVerbose(TFile* file1, const char* tree1Path, TFile* file2, const char* tree2Path,
                        int lenBranchNames, const char* branchNames[], bool verbose)
{
    TTree *t1 = (TTree*)file1->Get(tree1Path);
    TTree *t2 = (TTree*)file2->Get(tree2Path);

    return compareTreesVerbose(t1, t2, lenBranchNames, branchNames, verbose);
}

/*
 * WARNING : this version is significantly slower than other versions of the function
 *
 * general function to compare "TTree"s. A comparison is based on a list of branches
 * and given selections on tree entries.
 *
 * returns false if comparison does not succeed
 *
 * If a list of branches is given then the comparison will be based on that list.
 * If no list is given, the comparison will be based on all the branches. In that case tree1 and tree2 must have the same number of branches.
 *
 * selection1(2) : selection that picks entries from Tree1(2)
 *                 if the entries from the two trees are not in the same order, then
 *                 make sure "selection" picks not more than two entries, otherwise comparison order may be wrong.
 *
 *                 if the entries from the two trees are in the same order, then
 *                 "selection" can pick any number of entries.
 * */
/// DISCOURAGED
bool compareTrees(TTree* tree1, TTree* tree2, std::string selection1, std::string selection2,
                  int lenBranchNames, const char* branchNames[])
{
        bool useAll = false;
        TObjArray* branchList = tree1->GetListOfBranches();

        if(lenBranchNames==0)
        {
            // do comparison over all the branches
            useAll=true;

            lenBranchNames = branchList->GetSize();

            if(lenBranchNames != tree2->GetListOfBranches()->GetSize())    // tree1 and tree2 must have the same number of branches
            {
                return false;
            }
        }

        const int len = lenBranchNames;     // use a "const" variable for array dimension to avoid the following error in a ROOT session
                                            // Error: Non-static-const variable in array dimension
        const char* branches[len];
        // set the array "branches"
        // comparison will be based on this list of branches.
        if(useAll) {
            for(int i=0 ; i<len; ++i)
            {
                branches[i]=branchList->At(i)->GetName();
            }
        }
        else {
            for(int i=0 ; i<len; ++i)
            {
                branches[i]=branchNames[i];
            }
        }
        // the array "branches" has been set.

        for(int i=0; i<len; ++i)
        {
            tree1->Draw(Form("%s", branches[i]), selection1.c_str());
            tree2->Draw(Form("%s", branches[i]), selection2.c_str());

            Long64_t selected1 = tree1->GetSelectedRows();
            Long64_t selected2 = tree2->GetSelectedRows();

            Double_t* values1 = tree1->GetV1();
            Double_t* values2 = tree2->GetV1();

            /*
            // report
            std::cout << "branch    = " << branches[i] << std::endl;
            std::cout << "selected1 = " << selected1 << std::endl;
            std::cout << "selected2 = " << selected2 << std::endl;
            */

            if (selected1 != selected2)
            {
                /*
                // report
                std::cout<< "branch = " << branches[i] << "is not same." <<std::endl;
                std::cout<< "selected1 = " << selected1 <<std::endl;
                std::cout<< "selected2 = " << selected2 <<std::endl;
                */

                return false;
            }

            for (int j=0; j<selected1; ++j)
            {
                /*
                // report
                std::cout << "values1[j] = " << values1[j] << std::endl;
                std::cout << "values2[j] = " << values2[j] << std::endl;
                 */
                if (values1[j] != values2[j])
                {
                    /*
                    // report
                    std::cout<< "branch = " << branches[i] << "is not same." <<std::endl;
                    std::cout<< "selected elements = " << selected1 <<std::endl;
                    std::cout << "values1[j] = " << values1[j] << std::endl;
                    std::cout << "values2[j] = " << values2[j] << std::endl;
                    */

                    return false;
                }
            }
        }

        return true;
}

/*
 * function to sort a TTree* with respect to given array of entry indices
 * if sortedEntries[j] = k, then
 * entry k of "inputTree" will appear at entry j of "outputTree"
 *
 * NOTE : initialize "outputTree" before function call via
 * TTree* outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
 * otherwise will get :
 * may be used uninitialized in this function [-Werror=maybe-uninitialized]
 *
 * If "lenEntries" is less than number of entries in "inputTree", then
 * the function does not exit and sorts "lenEntries" number of entries.
 * If "lenEntries" is greater than number of entries in "inputTree", then
 * the function exits after initializing "outputTree".
 * */
void sortTree(TTree* inputTree, TTree* outputTree, Long64_t sortedEntries[], Long64_t lenEntries)
{
//    outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
    if (inputTree->GetEntries() < lenEntries)
    {
        std::cout << "mismatch of number of entries" <<std::endl;
        std::cout << "lenEntries              = " << lenEntries <<std::endl;
        std::cout << "inputTree->GetEntries() = " << inputTree->GetEntries() <<std::endl;
    }
    else{
        // LOOP : sorting
        for (Long64_t j = 0; j<lenEntries; ++j)
        {
            inputTree->GetEntry(sortedEntries[j]);
            outputTree->Fill();
        }
        // LOOP : sorting - END
    }
}

/*
 * function to sort a TTree* with respect to given vector of entry indices
 * if sortedEntries[j] = k, then
 * entry k of "inputTree" will appear at entry j of "outputTree"
 *
 * NOTE : initialize "outputTree" before function call via
 * TTree* outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
 * otherwise will get :
 * may be used uninitialized in this function [-Werror=maybe-uninitialized]
 *
 * If length of "sortedEntries" is less than number of entries in "inputTree", then
 * the function does not exit and sorts that number of entries.
 * If length of "sortedEntries" is greater than number of entries in "inputTree", then
 * the function exits after initializing "outputTree".
 * */
void sortTree(TTree* inputTree, TTree* outputTree, const std::vector<Long64_t>& sortedEntries)
{
//    outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
    Long64_t lenEntries = sortedEntries.size();
    if (inputTree->GetEntries() < lenEntries)
    {
        std::cout << "mismatch of number of entries" <<std::endl;
        std::cout << "sortedEntries.size()    = " << lenEntries <<std::endl;
        std::cout << "inputTree->GetEntries() = " << inputTree->GetEntries() <<std::endl;
    }
    else
    {
        // LOOP : sorting
        for (std::vector<Long64_t>::const_iterator it = sortedEntries.begin() ; it != sortedEntries.end(); ++it)
        {
            inputTree->GetEntry(*it);
            outputTree->Fill();
        }
        // LOOP : sorting - END
    }
}

/*
 * function to sort a TTree* with respect to "branchName" by increasing order
 * sorted entries should satisfy "selection"
 * if decreasing = true, then sort order will be decreasing
 *
 * NOTE : initialize "outputTree" before function call via
 * TTree* outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
 * otherwise will get :
 * may be used uninitialized in this function [-Werror=maybe-uninitialized]
 * */
void sortTree(TTree* inputTree, TTree* outputTree, const char* branchName, const char* selection, bool decreasing)
{
    inputTree->Draw(Form("%s:Entry$", branchName), selection, "goff");  // "goff" = graphics off

    Long64_t nentries = (Int_t)inputTree->GetSelectedRows();
    // selected values of the branch, comparison is based on double values
    std::vector<double> vecBranch  (inputTree->GetV1(), inputTree->GetV1()+nentries);
    // indices of selected entries
    std::vector<Long64_t> vecEntry (inputTree->GetV2(), inputTree->GetV2()+nentries);
    std::vector<Long64_t> indices  (nentries);

    // fill "indices"
    std::size_t n(0);
    std::generate(indices.begin(), indices.end(), [&]{ return n++; });

    // sort indices which is a list from [0, nentries-1]
    if (!decreasing)
        std::sort(indices.begin(), indices.end(),
                [&vecBranch](int i1, int i2) { return (double)vecBranch[i1] < (double)vecBranch[i2]; } );
    else
        std::sort(indices.begin(), indices.end(),
                [&vecBranch](int i1, int i2) { return (double)vecBranch[i1] > (double)vecBranch[i2]; } );

    // sort vecEntry using sorted "indices"
    // LOOP : sorting
    for (std::vector<Long64_t>::const_iterator it = indices.begin() ; it != indices.end(); ++it)
    {
        inputTree->GetEntry(vecEntry.at(*it));
        outputTree->Fill();
    }
    // LOOP : sorting - END
}

/*
 * function to sort a TTree* with respect to "branchName" by increasing order
 * sorted entries should satisfy "selection"
 * if decreasing = true, then sort order will be decreasing
 *
 * NOTE : initialize "outputTree" before function call via
 * TTree* outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
 * otherwise will get :
 * may be used uninitialized in this function [-Werror=maybe-uninitialized]
 *
 * uses TMath::Sort()
 * https://root.cern.ch/root/roottalk/roottalk01/3646.html
 * */
void sortTreeTMath(TTree* inputTree, TTree* outputTree, const char* branchName, const char* selection, bool decreasing)
{
    inputTree->Draw(Form("%s:Entry$", branchName), selection, "goff");  // "goff" = graphics off

    Long64_t nentries = (Int_t)inputTree->GetSelectedRows();
    // indices of selected entries
    std::vector<Long64_t> vecEntry (inputTree->GetV2(), inputTree->GetV2()+nentries);

    // sort indices which is a list from [0, nentries-1]
    Long64_t *indices = new Long64_t[nentries];
    TMath::Sort(nentries,inputTree->GetV1(),indices, decreasing);

    // sort vecEntry using sorted "indices"
    // LOOP : sorting
    for (Long64_t i=0;i<nentries;i++) {
        inputTree->GetEntry(vecEntry.at(indices[i]));
        outputTree->Fill();
    }
    // LOOP : sorting - END
}

/*
 * function to sort a TTree* with respect to "branchName" by increasing order
 * if decreasing = true, then sort order will be decreasing
 *
 * NOTE : initialize "outputTree" before function call via
 * TTree* outputTree = inputTree->CloneTree(0);   // do not copy any entry yet
 * otherwise will get :
 * may be used uninitialized in this function [-Werror=maybe-uninitialized]
 * */
/// DEPRECATED
void sortTree2(TTree* inputTree, TTree* outputTree, const char* branchName, bool decreasing)
{
    inputTree->BuildIndex(branchName);
    TTreeIndex *index = (TTreeIndex*)inputTree->GetTreeIndex();
//    Long64_t Nindex = index->GetN();
    Long64_t Nindex = inputTree->GetEntries();

    // LOOP : sorting
    if (!decreasing){
        for(Long64_t j = 0; j < Nindex; ++j) {
            inputTree->GetEntry(index->GetIndex()[j]);
            outputTree->Fill();
        }
    }
    else {
        for(Long64_t j = Nindex-1; j>=0; --j) {
            inputTree->GetEntry(index->GetIndex()[j]);
            outputTree->Fill();
        }
    }
    // LOOP : sorting - END
}

TString mergeCuts(TString cut1, TString cut2)
{
    TString cut = Form("%s && %s", cut1.Data(), cut2.Data());
    return cut;
}

/*
 * function to merge a variable number of cuts.
 * The cuts are of type "const char*"
 * assumes that "nCuts" matches the number of cuts entered in the function call
 *
 * http://en.cppreference.com/w/cpp/utility/variadic
 * http://en.cppreference.com/w/cpp/language/variadic_arguments
 * http://www.cplusplus.com/reference/cstdarg/va_arg/
 * http://www.cplusplus.com/reference/cstdarg/
 *
 * */
TString mergeCuts2(int nCuts, ...)
{
    va_list vl;
    va_start(vl,nCuts);

    TString cutNext = va_arg(vl, const char*);
    TString cut = cutNext.Data();
    if(nCuts == 1 && cutNext.Length() > 0)
        return cut;

    for (int i=1; i<nCuts; ++i)
    {
        cutNext = va_arg(vl, const char*);
        if(cutNext.Length() > 0){
            cut = Form("%s && %s", cut.Data(), cutNext.Data());
        }
    }
    va_end(vl);

    return cut;
}

#endif /* TREEUTIL_H_ */
