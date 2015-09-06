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
#include <fstream>      // ifstream, ofstream

TList*                   getFileNamesList(const char* dirname=".", const char* ext="");
std::vector<std::string> getFileNames    (const char* dirName=".", const char* ext="", bool recursive = false);
int                      replaceStringInFile(const char* file, std::string oldString, std::string newString);
std::string              replaceAll(std::string str, std::string oldString, std::string newString);

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

   return outFileNames;
}

std::vector<std::string> getFileNames(const char* dirName, const char* ext, bool recursive)
{
    std::string dirNameStr = dirName;
    const char* dirName2   = dirNameStr.c_str();  // looks dummy, but very important.
    // if dirName is used, things may get messed up in the recursion step.

    TSystemDirectory dir(dirName, dirName);
    TList* files = dir.GetListOfFiles();
    std::vector<std::string> outFileNames;

    if (files) {
        TSystemFile* file;
        TString fname;
        TIter next(files);
        while ((file=(TSystemFile*)next())) {

            fname = file->GetName();
            const char* fnameChar = fname.Data();

            if (strcmp(fnameChar, ".") == 0 || strcmp(fnameChar, "..") == 0)    continue;

            if(recursive && file->IsFolder())
            {
                const char* subdirname = Form("%s/%s", dirName2, fnameChar);

                std::vector<std::string> outFileNamesTmp = getFileNames(subdirname, ext, true);
                for (std::vector<std::string>::iterator it = outFileNamesTmp.begin() ; it != outFileNamesTmp.end(); ++it)
                {
                    outFileNames.push_back(Form("%s/%s",fnameChar,(*it).c_str()));
                }
            }
            else if (fname.EndsWith(ext) && !file->IsFolder()) {
                outFileNames.push_back(fnameChar);
            }

        }
        files->Delete();
    }

    return outFileNames;
}

/*
 * replace each occurence of "oldString" in file "filePath" with "newString"
 */
int replaceStringInFile(const char* file, std::string oldString, std::string newString)
{
    ifstream inFile(file);      // file to be updated
    const char* tmpFilePath = Form("%sTEMP", file);
    ofstream outFile(tmpFilePath);  // temporary output file

    std::string strLine;
    if (inFile.is_open() && outFile.is_open())
    {
        while( getline(inFile,strLine))
        {
            std::string  newStrLine = replaceAll(strLine, oldString, newString);
            outFile << newStrLine << "\n";
        }
    }

    inFile.close();
    outFile.close();

    // overwrite the original file.
    return rename(tmpFilePath , file);
}

/*
 * replace each occurence of "oldString" in string "str" with "newString"
 * http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string/3418285#3418285
 */
std::string replaceAll(std::string str, std::string oldString, std::string newString)
{
    if(str.empty())
        return str;

    size_t pos = 0;
    while ((pos = str.find(oldString, pos)) != std::string::npos) {
        str.replace(pos, oldString.length(), newString);
        pos += newString.length();
    }
    return str;
}

#endif /* SYSTEMUTIL_H_ */
