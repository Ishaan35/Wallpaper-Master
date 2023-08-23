#include <iostream>
#include "include/WallpaperChanger.hpp"
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <windows.h>
#include <Shlwapi.h>
#include "json.hpp"
#include <stdexcept>
#include <algorithm>
#include <random>
#include <codecvt>


void writeDefaultDataToConfigFile();
void setDefaultConfigVariables(std::string &INITIAL_WALLPAPER_NAME, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE);
void loadConfigAndProgress(std::string &INITIAL_WALLPAPER_NAME, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE);
int updateJsonProgressConfigDataFile(nlohmann::json jsonData);
void SkipBOM(std::ifstream &in);
int loadFileNamesIntoList(std::vector<std::string> &fileList);
std::string removeQuotes(const std::string& input);
long long currentMillis();
std::string getCurrentExeFolderPath();
void continuouslyChangeWallpaper(std::string &INITIAL_WALLPAPER_NAME, std::vector<std::string> &imageListFileVector, int fileListSize, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE);
std::string getCurrentExeFolderPath();
int PopulateVectorWithAllImagesFromFolderList(std::vector<std::string> &filePathVector);
void AppendFilesInDirectoryToVector(const std::string& directoryPath, std::vector<std::string> &filePathVector);
void ShuffleVector(std::vector<std::string> &fileList);
int redirectLogs();

nlohmann::json defaultConfigData = {
    {"INITIAL_WALLPAPER_NAME", ""},
    {"WALLPAPER_CHANGE_WAIT_DURATION", 10},
    {"NEXT_WALLPAPER_CHANGE_DUE", -1LL}
};


int main(){

    std::setlocale(LC_ALL, "en_US.UTF-8");

    if(redirectLogs() == 1)
        return 1;

    //load the config
    std::string INITIAL_WALLPAPER_NAME = "";
    int WALLPAPER_CHANGE_WAIT_DURATION = -1;
    long long NEXT_WALLPAPER_CHANGE_DUE = -1;
    loadConfigAndProgress(INITIAL_WALLPAPER_NAME, WALLPAPER_CHANGE_WAIT_DURATION, NEXT_WALLPAPER_CHANGE_DUE);
    
    //load file names
    std::vector<std::string> imageListFileVector;
    int fileListSize = PopulateVectorWithAllImagesFromFolderList(imageListFileVector);
    //ShuffleVector(imageListFileVector);
    if(fileListSize <= 0)
        return 1;

    //set backgrounds to all those files periodically
    continuouslyChangeWallpaper(INITIAL_WALLPAPER_NAME, imageListFileVector, fileListSize, WALLPAPER_CHANGE_WAIT_DURATION, NEXT_WALLPAPER_CHANGE_DUE);

    return 0;
}

int PopulateVectorWithAllImagesFromFolderList(std::vector<std::string> &filePathVector){
    std::ifstream folderListFile(getCurrentExeFolderPath() + "\\config\\" + "folderList.txt");
    SkipBOM(folderListFile);

    if (!folderListFile.is_open()) {
        std::cerr << "Unable to open the folderList file." << std::endl;
        return -1;
        folderListFile.close();
    }
    std::string line;
    while (std::getline(folderListFile, line)) 
        AppendFilesInDirectoryToVector(line, filePathVector);
    
    folderListFile.close();
    std::cout << filePathVector.size() << std::endl;
    return filePathVector.size();
}
void AppendFilesInDirectoryToVector(const std::string& directoryPath, std::vector<std::string> &filePathVector) {
    WIN32_FIND_DATAA findFileData; // Note the use of WIN32_FIND_DATAA
    HANDLE hFind = FindFirstFileA((directoryPath + "\\*").c_str(), &findFileData); // Note the use of FindFirstFileA

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open directory." << std::endl;
        return;
    }
    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::string image_path = directoryPath + "\\" + findFileData.cFileName;
            filePathVector.push_back(image_path);
        }
    } while (FindNextFileA(hFind, &findFileData) != 0); // Note the use of FindNextFileA

    FindClose(hFind);
}
void ShuffleVector(std::vector<std::string> &fileList){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (fileList.begin(), fileList.end(), std::default_random_engine(seed));
}

void writeDefaultDataToConfigFile(){
    std::cout << "Writing default data to json file" << std::endl;
    std::ofstream configOutput(getCurrentExeFolderPath() + "\\config\\" + "progressBeforeTermination.json", std::ios::trunc); //truncate the file before writing default json to it
    configOutput << defaultConfigData;
    configOutput.close();
}
void setDefaultConfigVariables(std::string &INITIAL_WALLPAPER_NAME, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE){
    std::cout << "Setting default values for config vars" << std::endl;
    INITIAL_WALLPAPER_NAME = defaultConfigData["INITIAL_WALLPAPER_NAME"];
    WALLPAPER_CHANGE_WAIT_DURATION = defaultConfigData["WALLPAPER_CHANGE_WAIT_DURATION"];
    NEXT_WALLPAPER_CHANGE_DUE = defaultConfigData["NEXT_WALLPAPER_CHANGE_DUE"];
}

void loadConfigAndProgress(std::string &INITIAL_WALLPAPER_NAME, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE){

    std::string configJsonFilePath = getCurrentExeFolderPath() + "\\config\\" + "progressBeforeTermination.json";
    std::ifstream configFile(configJsonFilePath);
    if(!configFile.is_open()){
        std::cout << "Failed to open configuration file! Writing now!" << std::endl;
        configFile.close();
        writeDefaultDataToConfigFile();
        setDefaultConfigVariables(INITIAL_WALLPAPER_NAME,WALLPAPER_CHANGE_WAIT_DURATION,  NEXT_WALLPAPER_CHANGE_DUE);
        return; 
    }
    
    nlohmann::json config_file_json_data;
    try{
        configFile >> config_file_json_data;
    }
    catch(nlohmann::json::exception& e){
        std::cout << "Failed to load configuration data as json. Loading default configuration" << std::endl;
        configFile.close();
        writeDefaultDataToConfigFile();
        setDefaultConfigVariables(INITIAL_WALLPAPER_NAME,WALLPAPER_CHANGE_WAIT_DURATION,  NEXT_WALLPAPER_CHANGE_DUE);
        return;
    }

    try{
        INITIAL_WALLPAPER_NAME = config_file_json_data.at("INITIAL_WALLPAPER_NAME");
        WALLPAPER_CHANGE_WAIT_DURATION = config_file_json_data.at("WALLPAPER_CHANGE_WAIT_DURATION");
        NEXT_WALLPAPER_CHANGE_DUE = config_file_json_data.at("NEXT_WALLPAPER_CHANGE_DUE");
        std::cout << "Successfully set vonfig variables from json file" << std::endl;
    }
    catch(nlohmann::json::exception& e){
        std::cout << "Failed to parse configuration data! Setting default configuration" << std::endl;
        configFile.close();
        writeDefaultDataToConfigFile();
        setDefaultConfigVariables(INITIAL_WALLPAPER_NAME,WALLPAPER_CHANGE_WAIT_DURATION,  NEXT_WALLPAPER_CHANGE_DUE);
        return;
    }
    
    configFile.close();
}
int updateJsonProgressConfigDataFile(nlohmann::json jsonData){
    std::ofstream outputConfigFile(getCurrentExeFolderPath() + "\\config\\" + "progressBeforeTermination.json", std::ios::out | std::ios::binary | std::ios::trunc);    
    try {
        outputConfigFile << jsonData;
    } catch (const std::ios_base::failure& e) {
        std::cerr << "File I/O error: " << e.what() << std::endl;
        outputConfigFile.close();
        return 1;
    } catch (const nlohmann::json::exception& e) {

        //restore previous json
        std::cerr << "JSON error: " << e.what() << std::endl;
        outputConfigFile.close();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Other error: " << e.what() << std::endl;
        outputConfigFile.close();
        return 1;
    }
    
    outputConfigFile.close();
    return 0;
}

//sometimes, text files may have three characters in the beginning indicating the encoding: Byte Order Mark UTF-8 (BOM) which are not a part of the actual content of the file itself. we can use this function to forcefully read in those characters and move the read pointer 3 spaces forward
//if those 3 characters were not the three standard characters used to indicate Byte Order Mark UTF-8, just move the read (get) pointer (seekg) back to position 0
void SkipBOM(std::ifstream &in)
{
    char test[3] = {0};
    in.read(test, 3);
    if ((unsigned char)test[0] == 0xEF && 
        (unsigned char)test[1] == 0xBB && 
        (unsigned char)test[2] == 0xBF)
    {
        return;
    }
    in.seekg(0);
}
int loadFileNamesIntoList(std::vector<std::string> &fileList){

    //open the file (skip BOM first)
    std::string imageListFilePath = getCurrentExeFolderPath() + "\\config\\" + "fileList.txt"; //append the file name to the folder path
    std::ifstream imageListFile(imageListFilePath);
    SkipBOM(imageListFile);

    if(!imageListFile.is_open()){
        std::cerr << "Error: \"fileList.txt\" was not found!";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        imageListFile.close();
        return -1;
    }
    
    //load them into the vector
    std::string line;
    while(std::getline(imageListFile, line))
        fileList.push_back(removeQuotes(line));
    int fileListSize = fileList.size();

    imageListFile.close();
    return fileListSize;
}

std::string removeQuotes(const std::string& input) {
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') 
        return input.substr(1, input.size() - 2);
    return input;
}

long long currentMillis(){
    auto currentTimePoint = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint.time_since_epoch());
    long long millis = duration.count();
    return millis;
}

void continuouslyChangeWallpaper(std::string &INITIAL_WALLPAPER_NAME, std::vector<std::string> &imageListFileVector, int fileListSize, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE){
    
    //now we have to look at the initial image and see how long we have to keep that for
    //basically, if the program terminated but we still have some duration left from the image that was showing before the process terminated, we still have t show that image for the remaining time
    //to get this remaining time, calculate (wallpaper_change_due_time_millis - current_millis_time)
        //if this quantity is <= 0, don't do anything. otherwise we first show the initial image for that time, and then move on
    //also note that we will need to constantly write the next wallpaper change time to the file every time we change the wallpaper. so current_millis_time + WALLPAPER_CHANGE_WAIT_DURATION_converted_to_milliseconds
    nlohmann::json currentConfigData = {
        {"INITIAL_WALLPAPER_NAME", INITIAL_WALLPAPER_NAME},
        {"WALLPAPER_CHANGE_WAIT_DURATION", WALLPAPER_CHANGE_WAIT_DURATION},
        {"NEXT_WALLPAPER_CHANGE_DUE", NEXT_WALLPAPER_CHANGE_DUE}
    };

    //the case of where the file list has changed and the Initial_wallpaper we were on before any system shutdown is taken care of since the file list can only change through the GUI app. and knowing that we are changing the file list from there only, we can make sure that the initial_wallpaper_name property is an empty string, and all other properties are updated properly
    //also check if prior progress config was pointing to a valid image file and the index matches
    if(NEXT_WALLPAPER_CHANGE_DUE > 0 && INITIAL_WALLPAPER_NAME == ""){
        std::cout << "waiting for previus progress" << std::endl;
        //wait out that initial wallpaper and then go onto next one
        
        long long currMillis = currentMillis();
        long long waitSeconds = (NEXT_WALLPAPER_CHANGE_DUE - currMillis)/1000;
        std::cout << waitSeconds << std::endl;
        std::cout << NEXT_WALLPAPER_CHANGE_DUE << std::endl;
        std::cout << currMillis << std::endl;

        if(waitSeconds > 0){
            WallpaperChanger::ChangeWallpaper(INITIAL_WALLPAPER_NAME);
            std::this_thread::sleep_for(std::chrono::seconds(waitSeconds));
        }
        //if waitseconds was negative, it means that the wallpaper was overdue to change. the program started up dgain after the time the wallpaper was supposed to change. in that case, the old wallpaper will be skipped entirely as this if statement above will return false. Since the index of the wallpaper will be incremented no matter what, the while loop ahead will take care of the following wallpapers.
        //essentially, it will go onto the next wallpaper like nothing happened.  
    }

    int wallpaperIndex = 0;
    
    while (true && fileListSize > 0) {

        WallpaperChanger::ChangeWallpaper(imageListFileVector[wallpaperIndex]);
        currentConfigData["INITIAL_WALLPAPER_NAME"] = imageListFileVector[wallpaperIndex];
        currentConfigData["NEXT_WALLPAPER_CHANGE_DUE"] = currentMillis() + WALLPAPER_CHANGE_WAIT_DURATION * 1000;
        
        int status = updateJsonProgressConfigDataFile(currentConfigData);


        wallpaperIndex++;
        if(wallpaperIndex >= fileListSize)
            wallpaperIndex = 0;

        // Sleep for x seconds
        std::this_thread::sleep_for(std::chrono::seconds(WALLPAPER_CHANGE_WAIT_DURATION));
    }
}

std::string getCurrentExeFolderPath(){
    //get the path of the folder that this exe is in
    //we need to make sure the exe is only looking at its relative folder for the text file
    //hence, we look in the same folder where the exe is located in
    char szPath[MAX_PATH * 100];
    GetModuleFileNameA(NULL, szPath, MAX_PATH * 100); //get file path of exe
    PathRemoveFileSpecA(szPath); //remove exe file name
    std::string folderPath = szPath;
    return folderPath;
}

int redirectLogs() {

    
    std::string logsFilePath = (getCurrentExeFolderPath() + "\\config\\" + "logs.txt");
    FILE* logFile = fopen(logsFilePath.c_str(), "a"); // Open log file in append mode
    if (!logFile) {
        std::cerr << "Failed to open log file for appending." << std::endl;
        return 1;
    }

    // Redirect standard output (stdout) to the log file
    if (freopen(logsFilePath.c_str(), "a", stdout) == nullptr) {
        std::cerr << "Failed to redirect stdout to log file. logs.txt is missing from config folder!" << std::endl;
        fclose(logFile);
        return 1;
    }

    // Redirect standard error (stderr) to the log file
    if (freopen(logsFilePath.c_str(), "a", stderr) == nullptr) {
        std::cerr << "Failed to redirect stderr to log file." << std::endl;
        fclose(logFile);
        return 1;
    }

    // Disable buffering for immediate output
    setvbuf(logFile, nullptr, _IONBF, 0);


    std::cout << "========================================================================================================" << std::endl;
    return 0;
}