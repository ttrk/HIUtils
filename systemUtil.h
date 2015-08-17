/*
 * utilities related to system operations
 */

#ifndef SYSTEMUTIL_H_
#define SYSTEMUTIL_H_

// compiling macros will give error if these classes are not included.
#include <TList.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TObjString.h>

#include <vector>
#include <string>
#include <iostream>


TList*                   getFileNamesList(const char* dirname=".", const char* ext="");
std::vector<std::string> getFileNames    (const char* dirname=".", const char* ext="");

using  std::cout;
using  std::endl;

/*
 * get a list files and folders in the given directory
 * https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=12793
 *
 * default option : all files in the current directory will be returned
 *
 *
 *
 * how to access the result :
 *
    TObjString* tmpObjStr;
    TString     tmpStr;
	for(int i=0; i<list->GetEntries(); i++)
	{
		tmpObjStr=(TObjString*)list->At(i);
		tmpStr   =tmpObjStr->GetString();
		cout     << tmpStr <<endl;
	}
 */
TList* getFileNamesList(const char* dirname /* ="." */, const char* ext /* ="" */)
{
   // ignore errors of Eclipse about TSystem objects
   TSystemDirectory dir(dirname, dirname);
   TList* files = dir.GetListOfFiles();
   TList* outFileNames=new TList();
   if (files) {
      TSystemFile *file;
      // TString cannot be stored in a TCollection... use TObjString instead.
      // https://root.cern.ch/root/htmldoc/TString.html
      TObjString* fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) {
         fname = new TObjString(file->GetName());
         if (fname->GetString().EndsWith(ext)) {
            	outFileNames->Add(fname);
         }
      }
   }
   else
   {
	   cout << "no files with given extension found in : " << dirname << endl;
   }

   return outFileNames;
}

std::vector<std::string> getFileNames(const char* dirname /* ="." */, const char* ext /* ="" */)
{
	TSystemDirectory dir(dirname, dirname);
	TList* files = dir.GetListOfFiles();
	std::vector<std::string> outFileNames;
	if (files) {
		TSystemFile *file;
		TString fname;
		TIter next(files);
		while ((file=(TSystemFile*)next())) {
			fname = file->GetName();
			if (fname.EndsWith(ext)) {
				outFileNames.push_back(fname.Data());
			}
		}
	}
	else
	{
		cout << "no files with given extension found in : " << dirname << endl;
	}

	return outFileNames;
}


#endif /* SYSTEMUTIL_H_ */
