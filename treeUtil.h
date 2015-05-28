/*
 * utilities related to TTree objects.
 *
 */

#ifndef TREEUTIL_H_
#define TREEUTIL_H_

#include <TTree.h>

void drawMaximum(TTree* tree, TString formula, TString condition = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum(TTree* tree, TString formula, TString condition = "1", TString cut = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximumGeneral   (TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TH1* hist = NULL);
void drawMaximumGeneral   (TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TString cut = "1", TH1* hist = NULL);
void drawMaximum2nd(TTree* tree, TString formula, TString condition = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum2nd(TTree* tree, TString formula, TString condition = "1", TString cut = "1", TH1* hist = NULL, bool plotZero = false);
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TH1* hist = NULL);
void drawMaximum2ndGeneral(TTree* tree, TString formula, TString formulaForMax, TString condition = "1", TString cut = "1", TH1* hist = NULL);

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

#endif /* TREEUTIL_H_ */
