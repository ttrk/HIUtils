/*
 * modify the "import" statements in CMSSW modules
 * so that Python interpreter can resolve them.
 *
 * CMSSW has the convetion that the folder "python" is omitted
 * in import statements.
 *
 * This code inserts an intermediate directory called "python"
 * into the import statement of every .py file
 */

#include <TList.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>      // ifstream, ofstream

std::vector<std::string> getFileNames    (const char* dirName=".", const char* ext="", bool recursive = false);
int fixImport(const char* filePath);

int main(int argc, char** argv)
{
    const char* dirName;
    if(argc == 2) {
        dirName = argv[1];
    }
    else {
        dirName = "";
        std::cout<<"wrong input"<<std::endl;
        return 0;
    }

    std::vector<std::string> fileNames = getFileNames(dirName, ".py", true);    // all python files
    for (std::vector<std::string>::iterator it = fileNames.begin() ; it != fileNames.end(); ++it) {

        const char* filePath = Form("%s/%s", dirName, (*it).c_str());
        int result = fixImport(filePath);

        if (result != 0)
        {
            std::cout<<"possible error for file : "<<filePath<<std::endl;
        }
    }

    return 0;
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

            if(recursive && file->IsFolder()) {
                const char* subdirname = Form("%s/%s", dirName2, fnameChar);

                std::vector<std::string> outFileNamesTmp = getFileNames(subdirname, ext, true);
                for (std::vector<std::string>::iterator it = outFileNamesTmp.begin() ; it != outFileNamesTmp.end(); ++it) {
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

int fixImport(const char* filePath)
{
    ifstream inFile(filePath);      // file to be updated
    const char* tmpFilePath = Form("%sTEMP", filePath);
    ofstream outFile(tmpFilePath);  // temporary output file

    std::string strLine;
    if (inFile.is_open() && outFile.is_open())
    {
        while( getline(inFile,strLine))
        {
            std::size_t found = strLine.find("import ");
            if(found != std::string::npos)
            {
                // fix "import" statement
                std::string key=".";
                std::size_t found2 = strLine.rfind(key);    // find the last "." in the line
                if (found2!=std::string::npos) {
                    strLine.replace (found2,key.length(),".python.");
                }
            }
            outFile << strLine << "\n";
        }
    }

    inFile.close();
    outFile.close();

    // overwrite the original file.
    return rename( tmpFilePath , filePath );
}
