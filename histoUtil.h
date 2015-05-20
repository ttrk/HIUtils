/*
 * utilities related to TH1 objects.
 */

#ifndef HISTOUTIL_H_
#define HISTOUTIL_H_


//#include <TCut.h>               // compiling macros give error if this is not included.

void     mergeCuts(TCut cut, TCut* cuts, int len);
void     mergeCuts(TCut cut, TCut* cuts);
int      getNumBins(double xmin, double xmax, int numBinsPerUnitX);
bool     compareHistograms(TH1* h1, TH1* h2);
TList*   divideHistogramList(TList* histoList1   , TList* histoList2,    int rebinFactor=1, bool DoScale=true);
TList*   divideHistogramList(TDirectoryFile* dir1, TDirectoryFile* dir2, int rebinFactor=1, bool DoScale=true);
TList*   getListOfSOMEKeys(TDirectoryFile* dir, const char* pattern);
TList*   getListOfSOMEKeys(TDirectoryFile* dir, const char* pattern, const char* type);
TList*   getListOfALLKeys (TDirectoryFile* dir);
TList*   getListOfALLKeys (TDirectoryFile* dir, const char* type);
TList*   getListOfHistograms   (TDirectoryFile* dir, const char* pattern="");
TList*   getListOfALLHistograms(TDirectoryFile* dir);
void     saveAllHistogramsToFile(const char* fileName, TList* histos);
void     saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType="gif", const char* directoryToBeSavedIn="", int styleIndex=0, int rebin=1);
void     saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType="gif", int dirType=0                        , int styleIndex=0, int rebin=1);
void     saveAllCanvasesToPicture(TList* canvases      , const char* fileType="gif", const char* directoryToBeSavedIn="");
//void     saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType="gif");     // this line gives error if I use default value option (here const char* fileType="gif") in the function definition.

using  std::string;
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
 * save recursively all the histograms inside a TDirectoryFile "dir" to images
 */
void saveAllHistogramsToPicture(TDirectoryFile* dir, const char* fileType /* ="gif" */, const char* directoryToBeSavedIn /* ="" */, int styleIndex /* =0 */, int rebin /* =1 */)
{
    TList* keysHisto = getListOfALLKeys(dir, "TH1D");

    TH1D*  h;
    TKey*  key;
    TIter* iter = new TIter(keysHisto);
    TCanvas* c1=new TCanvas();
    while ((key=(TKey*)iter->Next()))
    {
        h = (TH1D*)key->ReadObj();

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
 *  function to save histogram pictures into directories that are mostly likely to be preferred.
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
//11          TSystem* sys=new TSystem();
          TString dirName(dir->GetName());
          dirName.ReplaceAll(".root","");
//11          sys->mkdir(dirName,true);     // does not work. must override "int MakeDirectory(const char* name)"
          gSystem->mkdir(dirName,true);
          directoryToBeSavedIn=dirName;
    }

    saveAllHistogramsToPicture(dir,fileType,directoryToBeSavedIn, styleIndex, rebin);
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

#endif /* HISTOUTIL_H_ */
