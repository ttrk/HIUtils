/*
 * treeDrawArr.C
 *
 * macro to quickly draw an array of objects from a TTree*
 * saves canvases for each individual histogram : c[i]
 * saves a joint canvas where all histograms are plotted : cAll
 */

#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TH1.h>
#include <TH1D.h>
#include <TCanvas.h>

#include <string>
#include <iostream>

void treeDrawArr();

void treeDrawArr()
{
    ///// enter input /////
    const char* inputFilePath = "HiForest.root";
    TFile* inputFile = new TFile(inputFilePath,"READ");

    const char* treePath = "ggHiNtuplizer/EventTree";
    const int numFriends = 3;
    const char* treeFriendList[numFriends] = {"akPu3PFJetAnalyzer/t", "hltanalysis/HltTree", "skimanalysis/HltTree"};

    // input for array of TH1D
    const int numHistos = 2;

    std::string formula[numHistos] = {"phoEt",              "phoEt"};
    TCut selection[numHistos]      = {"abs(phoEta) < 1.44", "abs(phoEta) < 1.44 && phoSigmaIEtaIEta > 0.002"};

    std::string hName[numHistos]  = {"h1",     "h2"};
    std::string hTitle[numHistos] = {"title1", "title2"};
    int nBins[numHistos]   = {100, 100};
    double xLow[numHistos] = {0,   0} ;
    double xUp[numHistos]  = {100, 100};
    int markerStyles[numHistos] = {kFullCircle, kFullSquare};
    int markerColors[numHistos] = {kBlack,      kRed};

    // input for TCanvas
    std::string cnvAllName  = "cnvAll";
    std::string cnvAllTitle = "";

    std::string cnvName[numHistos]  = {"cnv1",      "cnv2"};
    std::string cnvTitle[numHistos] = {"cnv1Title", "cnv2Title"};

    // initialize histograms, graphs etc.
    // NOTE : this step must be done after the input file is opened.
    TH1::SetDefaultSumw2();

    TH1D* h[numHistos];
    TCanvas* c[numHistos];
    for (int i=0; i<numHistos; ++i)
    {
        h[i] = new TH1D(hName[i].c_str() , hTitle[i].c_str() ,nBins[i] ,xLow[i] ,xUp[i]);
        h[i]->SetMarkerStyle(markerStyles[i]);
        h[i]->SetMarkerColor(markerColors[i]);

        c[i] = new TCanvas(cnvName[i].c_str() , cnvTitle[i].c_str() ,600 ,600 );
    }

    TCanvas* cAll = new TCanvas(cnvAllName.c_str() , cnvAllTitle.c_str() ,600 ,600 );
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

    ///// drawing /////
    // drawing individual canvases
    for (int i=0; i<numHistos; ++i)
    {
        tree->Draw(Form("%s>>%s", formula[i].c_str(), h[i]->GetName()),selection[i].GetTitle(),"goff");
        // draw to individual canvases
        c[i]->cd();
        h[i]->Draw("e same");

        // save individual canvases
        c[i]->SaveAs(Form("cnv%d_%s.png", i+1, h[i]->GetName()));
        c[i]->SaveAs(Form("cnv%d_%s.pdf", i+1,h[i]->GetName()));
        c[i]->SaveAs(Form("cnv%d_%s.C", i+1,h[i]->GetName()));
        c[i]->SaveAs(Form("cnv%d_%s.root", i+1,h[i]->GetName()));
        c[i]->Close();
    }

    // drawing to the same canvas
    cAll->cd();
    for (int i=0; i<numHistos; ++i)
    {
        h[i]->Draw("e same");
    }
    cAll->SaveAs(Form("%s.png",  cnvAllName.c_str()));
    cAll->SaveAs(Form("%s.pdf",  cnvAllName.c_str()));
    cAll->SaveAs(Form("%s.C",    cnvAllName.c_str()));
    cAll->SaveAs(Form("%s.root", cnvAllName.c_str()));
    cAll->Close();

    inputFile->Close();
}

int main()
{
    treeDrawArr();

    return 0;
}
