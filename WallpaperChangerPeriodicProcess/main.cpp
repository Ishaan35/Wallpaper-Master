#include <iostream>
#include "include/WallpaperChanger.hpp"
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <windows.h>
#include <Shlwapi.h>


std::string removeQuotes(const std::string& input) {
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
        return input.substr(1, input.size() - 2);
    }
    return input;
}

int main(){

    //now we need to make sure the exe is only looking at its relative folder for the text file
    //hence, we look in the same folder where the exe is located in
    char szPath[MAX_PATH * 100];
    GetModuleFileNameA(NULL, szPath, MAX_PATH * 100); //get file path of exe
    PathRemoveFileSpecA(szPath); //remove exe file name
    std::string folderPath = szPath;
    std::string filePath = folderPath + "\\config\\" + "fileList.txt"; //append the file name to the folder path


    //open filename list file
    std::ifstream inputFile(filePath);
    if(!inputFile.is_open()){
        //std::cerr << "Error: \"fileList.txt\" was not found!";
            WallpaperChanger::ChangeWallpaper("C:\\Users\\Ishaan\\Pictures\\Saved Pictures\\Wallpapers\\2d31d5b4b142e2aef348b9f94dd1cd34095e97e0a9b1379fd73f93b70852693c.jpg");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        inputFile.close();
        return 1;
    }

    //load file names
    std::vector<std::string> fileList;
    std::string line;
    while(std::getline(inputFile, line))
        fileList.push_back(removeQuotes(line));
    int fileListSize = fileList.size();

    //set backgrounds to all those files periodically
    int fileIndex = 0;
    while (true) {
        WallpaperChanger::ChangeWallpaper(fileList[fileIndex++]);
        if(fileIndex >= fileListSize)
            fileIndex = 0;
        
        // Sleep for 2 seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }







    inputFile.close();
    return 0;
}