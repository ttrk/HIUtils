/*
 * treeDraw.C
 *
 * macro to quickly draw objects from a TTree*
 */

#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TH1.h>
#include <TH1D.h>
#include <TCanvas.h>

#include <string>
#include <iostream>

void treeDraw();

void treeDraw()
{
    ///// enter input /////
    const char* inputFilePath = "HiForest.root";
    TFile* inputFile = new TFile(inputFilePath,"READ");

    const char* treePath = "ggHiNtuplizer/EventTree";
    const int numFriends = 3;
    const char* treeFriendList[numFriends] = {"akPu3PFJetAnalyzer/t", "hltanalysis/HltTree", "skimanalysis/HltTree"};

    std::string formula = "phoEt";
    TCut selection = "abs(phoEta) < 1.44";

    // input for TH1D
    std::string hName  = "h1";
    std::string hTitle = "";
    int nBins   = 100;
    double xLow = 0;
    double xUp  = 100;

    // initialize histograms, graphs etc.
    // NOTE : this step must be done after the input file is opened.
    TH1::SetDefaultSumw2();

    TH1D* h = new TH1D(hName.c_str() , hTitle.c_str() ,nBins ,xLow ,xUp);
    h->SetMarkerStyle(kFullCircle);
    h->SetMarkerColor(kBlack);

    // input for TCanvas
    std::string cnvName  = "cnv";
    std::string cnvTitle = "";

    TCanvas* c = new TCanvas(cnvName.c_str() , cnvTitle.c_str() ,600 ,600 );
    ///// enter input - END /////
    ///// verbose /////
    std::cout << "inputFile = " << inputFilePath << std::endl;
    std::cout << "treePath  = " << treePath      << std::endl;
    ///// verbose - END/////
    TTree* tree = (TTree*)inputFile->Get(treePath);
    TTree* treeFriends[numFriends];
    for (int i=0; i<numFriends; ++i)
    {
        treeFriends[i] = (TTree*)inputFile->Get(treeFriendList[i]);
        tree->AddFriend(treeFriends[i],Form("t%d",i));
        std::cout<<"added as friend : "<<treeFriendList[i]<<std::endl;
    }

    // drawing
    c->cd();
    tree->Draw(Form("%s>>%s", formula.c_str(), h->GetName()),selection.GetTitle(),"goff");
    h->Draw("e same");

    c->SaveAs(Form("cnv_%s.png", h->GetName()));
    c->SaveAs(Form("cnv_%s.pdf", h->GetName()));
    c->SaveAs(Form("cnv_%s.C", h->GetName()));
    c->SaveAs(Form("cnv_%s.root", h->GetName()));
    c->Close();

    inputFile->Close();
}

int main()
{
    treeDraw();

    return 0;
}
