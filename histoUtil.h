/*
 * histoUtil.h
 *
 *  Created on: Mar 19, 2015
 *      Author: kaya
 */

#ifndef HISTOUTIL_H_
#define HISTOUTIL_H_


//#include <TCut.h>               // compiling macros give error if this is not included.

int      getNumBins(double xmin, double xmax, int numBinsPerUnitX);
bool     compareHistograms(TH1* h1, TH1* h2);
TList*   divideHistogramList(TList* histoList1   , TList* histoList2,    int rebinFactor=1, bool DoScale=true);
TList*   divideHistogramList(TDirectoryFile* dir1, TDirectoryFile* dir2, int rebinFactor=1, bool DoScale=true);
void     saveAllHistogramsToFile(const char* fileName, TList* histos);

using  std::string;
using  std::cout;
using  std::endl;

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
	int numBins=h1->GetNbinsX();
	if(numBins != h2->GetNbinsX())
		return false;

	for(int i=0; i<numBins; i++)
	{
		if(h1->GetBinContent(i)!=h2->GetBinContent(i))
			return false;
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

//		cout<<h1->GetNbinsX()<<endl;
//		cout<<h2->GetNbinsX()<<endl;

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

//		cout<<h1->GetNbinsX()<<endl;
//		cout<<h2->GetNbinsX()<<endl;

		h_division=(TH1D*)h1->Clone(h1->GetName());
//		h_division=new TH1D(h1->GetName(),h1->GetTitle(),h1->GetNbinsX(),h1->GetXaxis()->GetXmin(),h1->GetXaxis()->GetXmax());
//		h_division=new TH1D();
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

#endif /* HISTOUTIL_H_ */
