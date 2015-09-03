/*
 * utilities related to TH1 objects.
 */

#ifndef HISTOUTIL_H_
#define HISTOUTIL_H_

// compiling macros will give error if these classes are not included.
#include <TCut.h>
#include <TList.h>
#include <TKey.h>
#include <TDirectoryFile.h>
#include <TH1.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>

#include <iostream>
#include <vector>
#include <math.h>

void     mergeCuts(TCut cut, TCut* cuts, int len);
void     mergeCuts(TCut cut, TCut* cuts);
int      getNumBins(double xmin, double xmax, int numBinsPerUnitX);
bool     compareHistograms(TH1* h1, TH1* h2);
TList*   divideHistogramList(TList* histoList1   , TList* histoList2,    int rebinFactor=1, bool DoScale=true);
TList*   divideHistogramList(TDirectoryFile* dir1, TDirectoryFile* dir2, int rebinFactor=1, bool DoScale=true);
TList*   getListOfSOMEKeys (TDirectoryFile* dir, const char* pattern);
TList*   getListOfSOMEKeys (TDirectoryFile* dir, const char* pattern, const char* type);
TList*   getListOfGIVENKeys(TDirectoryFile* dir, std::vector<std::string> keyNames);
TList*   getListOfALLKeys (TDirectoryFile* dir);
TList*   getListOfALLKeys (TDirectoryFile* dir, const char* type);
TList*   getListOfALLKeys (TDirectoryFile* dir, const char* type, bool inheritsFrom);
TList*   getListOfHistograms     (TDirectoryFile* dir, const char* pattern="");
TList*   getListOfGIVENHistograms(TDirectoryFile* dir, std::vector<std::string> histoNames);
TList*   getListOfALLHistograms(TDirectoryFile* dir);
void     saveAllHistogramsToFile(const char* fileName, TList* histos);
void     saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType="gif", const char* directoryToBeSavedIn="", int styleIndex=0, int rebin=1);
void     saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType="gif", int dirType=0                      , int styleIndex=0, int rebin=1);
void     saveAllGraphsToPicture(TDirectoryFile* dir, const char* fileType="gif", const char* directoryToBeSavedIn="", int styleIndex=0);
void     saveAllGraphsToPicture(TDirectoryFile* dir, const char* fileType="gif", int dirType=0                      , int styleIndex=0);
void     saveAllCanvasesToPicture(TList* canvases      , const char* fileType="gif", const char* directoryToBeSavedIn="");
TList*   drawSame(TList* histos1, TList* histos2);
TList*   draw2D(TList* histos);
TH1*     Graph2Histogram(TGraphAsymmErrors graph);

using  std::cout;
using  std::endl;

/*
 * merge a set of cuts into a single one. the resulting cut is the cut that satisfies all individual cuts
 *
 * need to initialize "cut" before this function.
 */
void mergeCuts(TCut cut, TCut* cuts, int len)
{
    if (len>0)
    {
        cut=cuts[0];
        for (int i=1; i<len; i++)
        {
            cut = cut && cuts[i];
        }
    }
    else
    {
        cout << "there are not cuts to merge" <<endl;
    }
}

// NOTE : This function will NOT WORK.
/*
 * merge a set of cuts into a single one. the resulting cut is the cut that satisfies all individual cuts
 *
 * need to initialize "cut" before this function.
 */
void mergeCuts(TCut cut, TCut* cuts)
{
    // http://stackoverflow.com/questions/4108313/how-do-i-find-the-length-of-an-array
    int len = (sizeof (cuts) / sizeof (*cuts));
    mergeCuts(cut,cuts,len);
}

/*
 * get total number of bins corresponding to range [xmin, xmax] and number of bins for 1 unit of x
 *
 */
int getNumBins(double xmin, double xmax, int numBinsPerUnitX)
{
	double width=xmax-xmin;
	int n = (int)ceil(width*numBinsPerUnitX);
	return n;
}

/*
 * compare two histograms bin by bin.
 * */
bool compareHistograms(TH1* h1, TH1* h2)
{
	int numBinsX = h1->GetNbinsX();
	if(numBinsX != h2->GetNbinsX())
		return false;

    int numBinsY = h1->GetNbinsY();
    if(numBinsY != h2->GetNbinsY())
        return false;

    int numBinsZ = h1->GetNbinsZ();
    if(numBinsZ != h2->GetNbinsZ())
        return false;

    for(int i=0; i<=numBinsX+1; ++i){
        for(int j=0; j<=numBinsY+1; ++j){
            for(int k=0; k<=numBinsZ+1; ++k){

                if(h1->GetBinContent(i,j,k) != h2->GetBinContent(i,j,k))
                    return false;
                if(h1->GetBinError(i,j,k) != h2->GetBinError(i,j,k))
                    return false;
            }
        }
    }
	return true;
}

/*
 *  divide histograms element wise
 *
 *  "TH1::SetDefaultSumw2()" must have been run before creating the histograms in histoList1 and histoList2
 *
 */
TList* divideHistogramList(TList* histoList1, TList* histoList2, int rebinFactor /* =1 */, bool DoScale /* =true */)
{
	TList* histos_Division=new TList();

	TH1D*  h1;
	TH1D*  h2;
	TH1D*  h_division;
	for(int i=0; i<histoList1->GetEntries(); i++)
	{
		h1=(TH1D*)histoList1->At(i);
		h2=(TH1D*)histoList2->At(i);

		if (rebinFactor != 1)
		{
			h1->Rebin(rebinFactor);
			h2->Rebin(rebinFactor);
		}

		if(DoScale)
		{
			h1->Scale(1/(h1->GetEntries()));
			h2->Scale(1/(h2->GetEntries()));
		}

		h_division=(TH1D*)h1->Clone(h1->GetName());
		h_division->Divide(h1,h2);
		h_division->SetName(Form("%s_ratio",h1->GetName()));
		h_division->SetTitle(Form("ratio of %s",h1->GetTitle()));

		histos_Division->Add(h_division);
	}

	return histos_Division;
}

/*
 *  divide histograms from 2 directories element wise
 */
TList* divideHistogramList(TDirectoryFile* dir1, TDirectoryFile* dir2, int rebinFactor /* =1 */, bool DoScale /* =true */)
{
	TList* histoList1=getListOfALLHistograms(dir1);
	TList* histoList2=getListOfALLHistograms(dir2);

	return divideHistogramList(histoList1, histoList2, rebinFactor, DoScale);
}

/*
 * get list of all keys under a directory "dir" whose name contains "pattern"
 * pattern = "" means any pattern, hence getListOfSOMEKeys(dir, "") is the same as getListOfALLKeys(dir).
 */
TList* getListOfSOMEKeys(TDirectoryFile* dir, const char* pattern)
{
    TList* keysInDir = dir->GetListOfKeys();
    TIter* iter = new TIter(keysInDir);

    TDirectoryFile *subdir;
    TKey*  key;
    TList* keys=new TList();
    TList* newKeys=new TList();
    TString keyName;

    while ((key=(TKey*)iter->Next())) {

        keyName=key->GetName();
        if(keyName.Contains(pattern))
        {
            keys->Add(key);
        }

        // traverse directories in a DFS manner (recursively)
        if(key->IsFolder())
        {
            subdir=(TDirectoryFile*)key->ReadObj();
            newKeys=getListOfSOMEKeys(subdir, pattern);
            keys->AddAll(newKeys);
        }
    }
    return keys;
}

/*
 * get list of all keys under a directory "dir" for objects of a given "type" and whose name contains "pattern"
 * type    = "" means any type   , hence getListOfSOMEKeys(dir, pattern, "") is the same as getListOfSOMEKeys(dir, pattern).
 * pattern = "" means any pattern, hence getListOfSOMEKeys(dir, "", "") is the same as getListOfALLKeys(dir).
 */
TList* getListOfSOMEKeys(TDirectoryFile* dir, const char* pattern, const char* type /* ="" */ )
{
    TList* keysInDir = dir->GetListOfKeys();
    TIter* iter = new TIter(keysInDir);

    TDirectoryFile *subdir;
    TKey*  key;
    TList* keys=new TList();
    TList* newKeys=new TList();
    TString keyName;

    while ((key=(TKey*)iter->Next())) {

        keyName=key->GetName();
        if(keyName.Contains(pattern) && strcmp(key->GetClassName(), type) == 0)
        {
            keys->Add(key);
        }

        // traverse directories in a DFS manner (recursively)
        if(key->IsFolder())
        {
            subdir=(TDirectoryFile*)key->ReadObj();
            newKeys=getListOfSOMEKeys(subdir, pattern, type);
            keys->AddAll(newKeys);
        }
    }
    return keys;
}

/*
 * get list of all keys under a directory "dir" where key names are listed in "keyNames"
 */
TList* getListOfGIVENKeys(TDirectoryFile* dir, std::vector<std::string> keyNames)
{
    TList* keysInDir = dir->GetListOfKeys();
    TIter* iter = new TIter(keysInDir);

    TDirectoryFile *subdir;
    TKey*  key;
    TList* keys=new TList();
    TList* newKeys=new TList();
    TString keyName;


    while ((key=(TKey*)iter->Next())) {

        keyName=key->GetName();
        for (std::vector<std::string>::iterator itergivenNames = keyNames.begin() ; itergivenNames != keyNames.end(); ++itergivenNames){
            std::string givenName = (std::string)(*itergivenNames);
            if(keyName.EqualTo(givenName.data()))
            {
                keys->Add(key);
            }
        }

        // traverse directories in a DFS manner (recursively)
        if(key->IsFolder())
        {
            subdir=(TDirectoryFile*)key->ReadObj();
            newKeys=getListOfGIVENKeys(subdir, keyNames);
            keys->AddAll(newKeys);
        }
    }

    return keys;
}

TList* getListOfALLKeys(TDirectoryFile* dir)
{
    TList* keysInDir = dir->GetListOfKeys();
    TIter* iter = new TIter(keysInDir);

    TDirectoryFile *subdir;
    TKey*  key;
    TList* keys=new TList();
    TList *newKeys=new TList();

    while ((key=(TKey*)iter->Next())) {

        keys->Add(key);

        // traverse directories in a DFS manner (recursively)
        if(key->IsFolder())
        {
            subdir=(TDirectoryFile*)key->ReadObj();
            newKeys=getListOfALLKeys(subdir);
            keys->AddAll(newKeys);
        }
    }
    return keys;
}

/*
 * get list of all keys under a directory "dir" for objects of a given "type"
 */
TList* getListOfALLKeys(TDirectoryFile* dir, const char* type)
{

    TList* keysInDir = dir->GetListOfKeys();
    TIter* iter = new TIter(keysInDir);

    TDirectoryFile *subdir;
    TKey*  key;
    TList* keysOfType=new TList();
    TList *newKeys=new TList();

    while ((key=(TKey*)iter->Next())) {

        //      http://www.cplusplus.com/reference/cstring/strcmp/
        if(strcmp(key->GetClassName(), type) == 0)
        {
            keysOfType->Add(key);
        }

        // traverse directories in a DFS manner (recursively)
        if(key->IsFolder())
        {
            subdir=(TDirectoryFile*)key->ReadObj();
            newKeys=getListOfALLKeys(subdir, type);
            keysOfType->AddAll(newKeys);
        }
    }

    return keysOfType;
}

/*
 * get list of all keys under a directory "dir" for objects of a given "type"
 *
 * if inheritsFrom is true,  then get list of keys for objects that inherit from   "type"
 * if inheritsFrom is false, then get list of keys for objects that are exactly of "type"
 */
TList* getListOfALLKeys(TDirectoryFile* dir, const char* type, bool inheritsFrom)
{
    if(!inheritsFrom)
        return getListOfALLKeys(dir, type);

    TList* keysInDir = dir->GetListOfKeys();
    TIter* iter = new TIter(keysInDir);

    TDirectoryFile *subdir;
    TKey*  key;
    TList* keysOfType=new TList();
    TList *newKeys=new TList();

    while ((key=(TKey*)iter->Next())) {

        if(key->ReadObj()->InheritsFrom(type))
        {
            keysOfType->Add(key);
        }

        // traverse directories in a DFS manner (recursively)
        if(key->IsFolder())
        {
            subdir=(TDirectoryFile*)key->ReadObj();
            newKeys=getListOfALLKeys(subdir, type);
            keysOfType->AddAll(newKeys);
        }
    }

    return keysOfType;
}

/*
 * get list of histograms under a directory "dir" for objects of a given "type"
 * the name of the histograms should contain the pattern
 */
TList* getListOfHistograms(TDirectoryFile* dir, const char* pattern /* ="" */ )
{
    TList* histos=new TList();
    TList* keysHisto = getListOfSOMEKeys(dir, pattern, "TH1D");

    TIter* iter = new TIter(keysHisto);
    TKey*  key;
    while ((key=(TKey*)iter->Next()))
    {
        histos->Add((TH1D*)key->ReadObj());
    }

    return histos;
}

/*
 * get list of histograms under a directory "dir" where histogram names are listed in "histoNames"
 */
TList* getListOfGIVENHistograms(TDirectoryFile* dir, std::vector<std::string> histoNames)
{
    TList* histos=new TList();
    TList* keysHisto = getListOfGIVENKeys(dir, histoNames);

    TIter* iter = new TIter(keysHisto);
    TKey*  key;
    while ((key=(TKey*)iter->Next()))
    {
        histos->Add((TH1D*)key->ReadObj());
    }

    return histos;
}

/*
 * get list of all histograms under a directory "dir" for objects of a given "type"
 */
TList* getListOfALLHistograms(TDirectoryFile* dir)
{
    TList* histos=new TList();
    TList* keysHisto = getListOfALLKeys(dir, "TH1D");

    TIter* iter = new TIter(keysHisto);
    TKey*  key;
    while ((key=(TKey*)iter->Next()))
    {
        histos->Add((TH1D*)key->ReadObj());
    }

    return histos;
}


void saveAllHistogramsToFile(const char* fileName, TList* histos)
{
    TFile* f=new TFile(fileName, "RECREATE");

    TH1D* h;
    TIter* iter = new TIter(histos);
    while ((h=(TH1D*)iter->Next()))
    {
        h->Write();
    }

    f->Close();
}

/*
 * save recursively all the TH1 histograms inside a TDirectoryFile "dir" to images
 */
void saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType /* ="gif" */, const char* directoryToBeSavedIn /* ="" */, int styleIndex /* =0 */, int rebin /* =1 */)
{
    TList* keysHisto = getListOfALLKeys(dir, "TH1", true);  // all histograms that inherit from "TH1" will be saved to picture.

    TH1*  h;
    TKey*  key;
    TIter* iter = new TIter(keysHisto);
    TCanvas* c1=new TCanvas();
    while ((key=(TKey*)iter->Next()))
    {
        h = (TH1*)key->ReadObj();

        if(rebin!=1)
        {
            h->Rebin(rebin);
        }

        if(styleIndex==1)
        {
            h->Draw("E");
        }
        else
        {
            h->Draw();
            if(h->InheritsFrom("TH2"))
            {
                h->Draw("COLZ");    // default plot style for TH2 histograms
            }
        }

        if(strcmp(directoryToBeSavedIn, "") == 0)   // save in the current directory if no directory is specified
        {
            c1->SaveAs(Form("%s.%s" ,h->GetName(), fileType));  // name of the file is the name of the histogram
        }
        else
        {
            c1->SaveAs(Form("%s/%s.%s", directoryToBeSavedIn ,h->GetName(), fileType));
        }
    }
    c1->Close();
}

/*
 *  function to save TH1 histogram pictures into directories that are mostly likely to be preferred.
 *
 *  absolute path of the macro calling this method = /path/to/macro/myMacro.C
 *  absolute path of the TDirectoryFile = /path/to/file/myFile.root
 *
 *  dirType = 0 (default option) --> save files under   /path/to/macro/
 *  dirType = 1                  --> save files under   /path/to/macro/myFile
 *  dirType = 2                  --> save files under   /path/to/file/
 *  dirType = 3                  --> save files under   /path/to/file/myFile
 *
 * */
void saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType /* ="gif" */, int dirType /* =0 */, int styleIndex /* =0 */, int rebin /* =1 */)
{
    const char* directoryToBeSavedIn="";

    if(dirType==0)
    {
        directoryToBeSavedIn="";
    }
    else if(dirType==1)
    {
          TSystem* sys=new TSystem();
          const char* baseName=sys->BaseName(dir->GetName());   //    const char* baseName=basename(f->GetName());  does NOT work
          TString dirName(baseName);
          dirName.ReplaceAll(".root","");
//        sys->mkdir(dirName);          // does not work. must override "int MakeDirectory(const char* name)"
          gSystem->mkdir(dirName,true);
          directoryToBeSavedIn=dirName;
    }
    else if(dirType==2)
    {
        TSystem* sys=new TSystem();
        directoryToBeSavedIn=sys->DirName(dir->GetName());
    }
    else if(dirType==3)
    {
//        TSystem* sys=new TSystem();
          TString dirName(dir->GetName());
          dirName.ReplaceAll(".root","");
//          sys->mkdir(dirName,true);     // does not work. must override "int MakeDirectory(const char* name)"
          gSystem->mkdir(dirName,true);
          directoryToBeSavedIn=dirName;
    }

    saveAllHistogramsToPicture(dir,fileType,directoryToBeSavedIn, styleIndex, rebin);
}

/*
 * save recursively all the graphs inside a TDirectoryFile "dir" to images
 */
void saveAllGraphsToPicture(TDirectoryFile* dir, const char* fileType /* ="gif" */, const char* directoryToBeSavedIn /* ="" */, int styleIndex /* =0 */)
{
    TList* keysGraph = getListOfALLKeys(dir, "TGraph", true); // all graphs that inherit from "TGraph" will be saved to picture.

    TGraph* graph;
    TKey*  key;
    TIter* iter = new TIter(keysGraph);
    TCanvas* c1=new TCanvas();
    while ((key=(TKey*)iter->Next()))
    {
        graph = (TGraph*)key->ReadObj();

        if(styleIndex==1)
        {
            graph->Draw();
        }
        else
        {
            graph->Draw("a p");
        }

        if(strcmp(directoryToBeSavedIn, "") == 0)   // save in the current directory if no directory is specified
        {
            c1->SaveAs(Form("%s.%s" ,graph->GetName(), fileType));  // name of the file is the name of the histogram
        }
        else
        {
            c1->SaveAs(Form("%s/%s.%s", directoryToBeSavedIn ,graph->GetName(), fileType));
        }
    }
    c1->Close();
}

/*
 *  function to save TGraph pictures into directories that are mostly likely to be preferred.
 *
 *  absolute path of the macro calling this method = /path/to/macro/myMacro.C
 *  absolute path of the TDirectoryFile = /path/to/file/myFile.root
 *
 *  dirType = 0 (default option) --> save files under   /path/to/macro/
 *  dirType = 1                  --> save files under   /path/to/macro/myFile
 *  dirType = 2                  --> save files under   /path/to/file/
 *  dirType = 3                  --> save files under   /path/to/file/myFile
 *
 * */
void saveAllGraphsToPicture(TDirectoryFile* dir, const char* fileType /* ="gif" */, int dirType /* =0 */, int styleIndex /* =0 */)
{
    const char* directoryToBeSavedIn="";

    if(dirType==0)
    {
        directoryToBeSavedIn="";
    }
    else if(dirType==1)
    {
          TSystem* sys=new TSystem();
          const char* baseName=sys->BaseName(dir->GetName());   //    const char* baseName=basename(f->GetName());  does NOT work
          TString dirName(baseName);
          dirName.ReplaceAll(".root","");
//        sys->mkdir(dirName);          // does not work. must override "int MakeDirectory(const char* name)"
          gSystem->mkdir(dirName,true);
          directoryToBeSavedIn=dirName;
    }
    else if(dirType==2)
    {
        TSystem* sys=new TSystem();
        directoryToBeSavedIn=sys->DirName(dir->GetName());
    }
    else if(dirType==3)
    {
//        TSystem* sys=new TSystem();
          TString dirName(dir->GetName());
          dirName.ReplaceAll(".root","");
//          sys->mkdir(dirName,true);     // does not work. must override "int MakeDirectory(const char* name)"
          gSystem->mkdir(dirName,true);
          directoryToBeSavedIn=dirName;
    }

    saveAllGraphsToPicture(dir,fileType,directoryToBeSavedIn, styleIndex);
}

/*
 * MODIFY THIS
 */
void saveAllCanvasesToPicture(TList* canvases, const char* fileType /* ="gif" */, const char* directoryToBeSavedIn /* ="" */)
{
    TCanvas* c;
    TIter* iter = new TIter(canvases);
    while ((c=(TCanvas*)iter->Next()))
    {
        if(strcmp(directoryToBeSavedIn, "") == 0)   // save in the current directory if no directory is specified
        {
            c->SaveAs(Form("%s.%s" ,c->GetName(), fileType));   // name of the file is the name of the histogram
        }
        else
        {
            c->SaveAs(Form("%s/%s.%s", directoryToBeSavedIn ,c->GetName(), fileType));
        }
    }
//  c->Close();
}

/*
 * for all histograms in "histos1" and "histos2", plot the histograms with the same indices onto the same TCanvas.
 */
TList* drawSame(TList* histos1, TList* histos2)
{
    TList* canvasList = new TList();

    TH1D* h1;
    TH1D* h2;
    TCanvas* c;
    TIter* iter1 = new TIter(histos1);
    TIter* iter2 = new TIter(histos2);
    while((h1=(TH1D*)iter1->Next())) {
        h2=(TH1D*)iter2->Next();

        // default plotting options
        h1->SetMarkerStyle(kFullSquare);
        h2->SetMarkerStyle(kFullCircle);

        h1->SetMarkerColor(kBlack);
        h2->SetMarkerColor(kRed);

        h2->SetMarkerSize(h1->GetMarkerSize()*0.8);     // to distinguish between points when they overlap

        c=new TCanvas(h1->GetName());
        c->Clear();     // clear changes coming from the previous canvas

        h1->Draw("e");
        h2->Draw("e SAME");

        // set Y-axis ranges
        // default Y-axis range is that of h1
        // make sure that both plots will not run out of y-axis
        double max1 = h1->GetMaximum();
        double max2 = h2->GetMaximum();
        double min1 = h1->GetMinimum();
        double min2 = h2->GetMinimum();

        if (max2 > max1)
            h1->SetMaximum(max2+TMath::Abs(max2)*0.2);

        if (min2 < min1)
            h1->SetMinimum(min2-TMath::Abs(min2)*0.2);

        // special cases
        // TO DO

        canvasList->Add(c);
    }

    return canvasList;
}

/*
 * plot list of 2D histograms in "histos" onto TCanvas list.
 */
TList* draw2D(TList* histos)
{
    TList* canvasList = new TList();

    TH2D* h;
    TCanvas* c;
    TIter* iter = new TIter(histos);
    while((h=(TH2D*)iter->Next())) {

        c=new TCanvas(h->GetName());

        h->Draw("colz");

        canvasList->Add(c);
    }

    return canvasList;
}

/*
 * convert a TGraphAsymmErrors to 1D TH1 by setting the data points bin by bin.
 * NOTE : cannot set the asymmetric error bars for TH1. So cannot carry over the asymmetric errors
 * in "graph" to the histogram that is returned.
 * The error in the histogram will be
 * TMath::Sqrt(0.5*(elow*elow + ehigh*ehigh))  which is returned by graph->GetErrorY(i)
 * https://root.cern.ch/root/html/src/TGraphAsymmErrors.cxx.html#rBLO5D
 */
TH1* Graph2Histogram(TGraphAsymmErrors* graph)
{
    int fNpoints = graph->GetN();
    double* fX   = graph->GetX();
    double* fY   = graph->GetY();

    if (fNpoints == 0) {
        return NULL;
    }

    // prepare x-bins for the histograms
    // xbins  : array of low-edges for each bin
    //          This is an array of size nbins+1
    double xbins[fNpoints+1];
    for (int i=0; i<fNpoints; ++i)  {
        xbins[i]=fX[i]-graph->GetErrorXlow(i);
    }
    xbins[fNpoints]=fX[fNpoints-1]+graph->GetErrorXhigh(fNpoints-1);

    TH1* h = new TH1D(graph->GetName(),graph->GetTitle(), fNpoints, xbins);

    for (int i=0; i<fNpoints; ++i)  {
        h->SetBinContent(i+1, fY[i]);
        h->SetBinError(i+1, graph->GetErrorY(i));
    }

    return h;
}

#endif /* HISTOUTIL_H_ */
