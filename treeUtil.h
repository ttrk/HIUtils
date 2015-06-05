/*
 * utilities related to TTree objects.
 *
 */

#ifndef TREEUTIL_H_
#define TREEUTIL_H_

#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TFile.h>

void drawMaximum(TTree* tree, TString formula, TString condition = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum(TTree* tree, TString formula, TString condition = "1", TString cut = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximumGeneral   (TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TH1* hist = NULL);
void drawMaximumGeneral   (TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TString cut = "1", TH1* hist = NULL);
void drawMaximum2nd(TTree* tree, TString formula, TString condition = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum2nd(TTree* tree, TString formula, TString condition = "1", TString cut = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TH1* hist = NULL);
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TString cut = "1", TH1* hist = NULL);

bool compareTrees(TTree* tree1, TTree* tree2, int lenBranchNames = 0, const char* branchNames[] = NULL);
bool compareTrees(TFile* file1, const char* tree1Path, TFile* file2, const char* tree2Path, int lenBranchNames = 0, const char* branchNames[] = NULL);

/*
 * plot the maximum value of the elements of a "formula" where the elements satisfy the "condition".
 * If no element satisfies "condition" : if plotZero is true, then 0 is plotted. Otherwise nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
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
 * plot "formula" corresponding to the element with maximum value for "formulaForMax" where the element satisfies the "condition".
 * If no element satisfies "condition", then nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * Example : plotting the eta for the maximum pT photon that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
void drawMaximumGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition, TH1* hist)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }
    tree->Draw(Form("%s >> %s", formula.Data(), hname) ,Form("%s == Max$(%s*(%s))",formulaForMax.Data() ,formulaForMax.Data() ,condition.Data()));
}

/*
 * plot "formula" corresponding to the element with maximum value for "formulaForMax" where the element satisfies the "condition".
 * If no element satisfies "condition", then nothing is plotted.
 * Additional cuts to the maximum element after it is found are given by "cut".
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * Example : plotting the eta for the maximum pT photon that satisfy a "condition"
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
void drawMaximumGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition, TString cut, TH1* hist)
{
    const char* hname="hDrawMax";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }
    tree->Draw(Form("%s >> %s", formula.Data(), hname) ,Form("%s == Max$(%s*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,condition.Data() ,cut.Data()));
}

/*
 * plot the 2nd maximum value of the elements of a "formula" where the elements satisfy the "condition".
 * If no element satisfies "condition" : if plotZero is true, then 0 is plotted. Otherwise nothing is plotted.
 * The plot will be stored in histogram "hist" if it is supplied.
 *
 * https://root.cern.ch/root/html/TTree.html#TTree:Draw@2
 */
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
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition, TH1* hist)
{
    const char* hname="hDrawMax2nd";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }
    tree->Draw(Form("%s >> %s", formula.Data(), hname) ,Form("%s == Max$(%s*(%s < Max$(%s*(%s)))*(%s))",formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,
                                                                                                              condition.Data() ,condition.Data()));
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
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition, TString cut, TH1* hist)
{
    const char* hname="hDrawMax2nd";
    if(hist!=NULL)
    {
       hname = hist->GetName();
    }
    tree->Draw(Form("%s >> %s", formula.Data(), hname) ,Form("%s == Max$(%s*(%s < Max$(%s*(%s)))*(%s)) && %s",formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,formulaForMax.Data() ,
                                                                                                              condition.Data() ,condition.Data() ,cut.Data()));
}

/*
 * general function to compare "TTree"s. A comparison is based on a list of branches
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

    int notEqual=0;
    TString selection;
    for(int i=0; i<len; ++i)
    {
        selection = Form("abs(%s - %s.%s)>0", branches[i], alias, branches[i]);
        notEqual = tree1->GetEntries(selection.Data());
        if(notEqual>0)
        {
/*
            // report
            std::cout<< "branch = " << branches[i] << "is not same." << " different entries = " << notEqual <<std::endl;
*/
            return false;
        }
    }

    return true;
}

bool compareTrees(TFile* file1, const char* tree1Path, TFile* file2, const char* tree2Path, int lenBranchNames, const char* branchNames[])
{
    TTree *t1 = (TTree*)file1->Get(tree1Path);
    TTree *t2 = (TTree*)file2->Get(tree2Path);

    return compareTrees(t1, t2, lenBranchNames, branchNames);
}

#endif /* TREEUTIL_H_ */
