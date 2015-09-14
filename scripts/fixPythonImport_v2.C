/*
 * modify the "import" statements in CMSSW modules
 * so that Python interpreter can resolve them.
 *
 * CMSSW has the convention that the folder "python" is omitted
 * in import statements.
 *
 * This code inserts an intermediate directory called "python"
 * into the import statement of every .py file
 *
 * version 2 - try different locations in the import statement for insertion of "python".
 *             insert "python" if the resulting imported path exists.
 */

#include <TList.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>      // ifstream, ofstream

std::vector<std::string> getFileNames    (const char* dirName=".", const char* ext="", bool recursive = false);
int         fixImport(const char* filePath, const char* dirName);\
std::string fixImportedPath(std::string importedPath, const char* dirName);
std::string replaceAll(std::string str, std::string oldString, std::string newString);
bool fileExists(const char *filename);

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
        int result = fixImport(filePath, dirName);

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

int fixImport(const char* filePath, const char* dirName)
{
    std::string s         = filePath;
    const char* filePath2 = s.c_str();

    std::ifstream inFile(filePath2);      // file to be updated

    std::string strLine;
    std::vector<std::string> importedPaths;
    if (inFile.is_open())
    {
        while( getline(inFile,strLine))
        {
            std::size_t found = strLine.find("import ");
            if(found != std::string::npos)
            {
                // fix "import" statement
                std::string key=".";
                std::size_t keyPosLast = strLine.rfind(key);    // find the last "." in the line

                // strip off the imported path and check whether it exists,
                // if not insert ".python." into different places in the imported path
                // and check whether this new imported path exists.
                std::string importedPathStart = " ";
                std::string importedPathEnd   = " ";
                // imported path begins with the last whitespace before the "."
                std::size_t foundStart = strLine.rfind(importedPathStart, keyPosLast);
                // imported path ends with the first whitespace after the "."
                std::size_t foundEnd   = strLine.find(importedPathEnd, keyPosLast);

                std::string importedPath;
                std::string importedPathFile;
                if (foundStart!=std::string::npos && foundEnd!=std::string::npos) {
                    std::size_t len = foundEnd-foundStart-1;
                    importedPath = strLine.substr (foundStart+1, len);
                    importedPaths.push_back(importedPath);
                }
            }
        }
    }

    // do not check the existence of the imported paths file the original file is open.
    // make sure there is no more than 1 file open at a time for reading.
    inFile.close();

    std::vector<std::string> fixedImportedPaths;
    std::string fixedImportedPath;
    for (std::vector<std::string>::iterator it = importedPaths.begin() ; it != importedPaths.end(); ++it) {
        fixedImportedPath = fixImportedPath(*it, dirName);
        fixedImportedPaths.push_back(fixedImportedPath);
    }

    std::ifstream inFile2(filePath2);      // file to be updated, REOPEN it.
    const char* tmpFilePath = Form("%sTEMP", filePath2);
    std::ofstream outFile(tmpFilePath);    // temporary output file

    std::vector<std::string>::iterator it = fixedImportedPaths.begin();
    if (inFile2.is_open() && outFile.is_open())
       {
           while( getline(inFile2,strLine))
           {
               std::size_t found = strLine.find("import ");
               if(found != std::string::npos)
               {
                   // fix "import" statement
                   std::string key=".";
                   std::size_t keyPosLast = strLine.rfind(key);    // find the last "." in the line

                   // strip off the imported path and check whether it exists,
                   // if not insert ".python." into different places in the imported path
                   // and check whether this new imported path exists.
                   std::string importedPathStart = " ";
                   std::string importedPathEnd   = " ";
                   // imported path begins with the last whitespace before the "."
                   std::size_t foundStart = strLine.rfind(importedPathStart, keyPosLast);
                   // imported path ends with the first whitespace after the "."
                   std::size_t foundEnd   = strLine.find(importedPathEnd, keyPosLast);

                   std::string importedPath;
                   std::string importedPathFile;
                   if (foundStart!=std::string::npos && foundEnd!=std::string::npos) {
                       std::size_t len = foundEnd-foundStart-1;
                       importedPath = strLine.substr (foundStart+1, len);
                       // importedPaths.push_back(importedPath);

                       // now we have the fixed version of the imported path
                       std::string importedPath_fixed = *it;    // assume current object in the vector corresponds
                       // to the line we are in. did not do any check on index etc.

                       strLine.replace(foundStart+1, len, importedPath_fixed.c_str());
                       // import statement for this line is fixed.
                       ++it;
                   }
               }
               outFile << strLine << "\n";
           }
       }

    inFile2.close();
    outFile.close();

    // overwrite the original file.
    return rename( tmpFilePath , filePath2 );
}

std::string fixImportedPath(std::string importedPath, const char* dirName)
{
    std::string importedPathFile = replaceAll(importedPath.c_str(),".","/");

    bool pathExists = fileExists(Form("%s/%s.py",dirName,importedPathFile.c_str()));

    std::size_t searchEnd  = importedPath.length();
    std::string key=".";
    while (!pathExists)
    {
        std::size_t keyPos = importedPath.rfind(key, searchEnd);    // find the last not tried "." in the imported path
        if (keyPos!=std::string::npos) {

            std::string importedPath_tmp = importedPath.c_str();

            importedPath_tmp.replace (keyPos,key.length(),".python.");

            // check if this new imported path exists
            std::string importedPathFile_tmp = replaceAll(importedPath_tmp.c_str(),".","/");
            pathExists = fileExists(Form("%s/%s.py",dirName,importedPathFile_tmp.c_str()));

            if (pathExists) {
                importedPath = importedPath_tmp.c_str();
            }
            else {
                searchEnd = keyPos-1;   // if this path does not exist, then try to insert
                                        // ".python." to in place of the "." before this one.
            }
        }
        else {
            pathExists = true;      // just to prevent infinite loop
        }
    }

    return importedPath;
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

bool fileExists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}


