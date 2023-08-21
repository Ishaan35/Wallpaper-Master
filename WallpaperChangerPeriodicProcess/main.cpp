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


void writeDefaultDataToConfigFile();
void setDefaultConfigVariables(std::string &INITIAL_WALLPAPER_NAME, int &CURRENT_WALLPAPER_INDEX, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE);
void loadConfigAndProgress(std::string &INITIAL_WALLPAPER_NAME, int &CURRENT_WALLPAPER_INDEX, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE);
void updateJsonProgressConfigDataFile(nlohmann::json jsonData);
void SkipBOM(std::ifstream &in);
int loadFileNamesIntoList(std::vector<std::string> &fileList);
std::string removeQuotes(const std::string& input);
long long currentMillis();
std::string getCurrentExeFolderPath();
void continuouslyChangeWallpaper(int &CURRENT_WALLPAPER_INDEX, std::string &INITIAL_WALLPAPER_NAME, std::vector<std::string> &imageListFileVector, int fileListSize, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE);
std::string getCurrentExeFolderPath();

nlohmann::json defaultConfigData = {
    {"INITIAL_WALLPAPER_NAME", ""},
    {"CURRENT_WALLPAPER_INDEX", 0},
    {"WALLPAPER_CHANGE_WAIT_DURATION", 10},
    {"NEXT_WALLPAPER_CHANGE_DUE", -1LL}
};


int main(){

    //load the config
    std::string INITIAL_WALLPAPER_NAME = "";
    int CURRENT_WALLPAPER_INDEX = -1;
    int WALLPAPER_CHANGE_WAIT_DURATION = -1;
    long long NEXT_WALLPAPER_CHANGE_DUE = -1;
    loadConfigAndProgress(INITIAL_WALLPAPER_NAME, CURRENT_WALLPAPER_INDEX, WALLPAPER_CHANGE_WAIT_DURATION, NEXT_WALLPAPER_CHANGE_DUE);

    //load file names
    std::vector<std::string> imageListFileVector;
    int fileListSize = loadFileNamesIntoList(imageListFileVector);
    if(fileListSize <= 0)
        return 1;

    //set backgrounds to all those files periodically
    continuouslyChangeWallpaper(CURRENT_WALLPAPER_INDEX, INITIAL_WALLPAPER_NAME, imageListFileVector, fileListSize, WALLPAPER_CHANGE_WAIT_DURATION, NEXT_WALLPAPER_CHANGE_DUE);

    return 0;
}

void writeDefaultDataToConfigFile(){
    std::cout << "Writing default data to json file" << std::endl;
    std::ofstream configOutput(getCurrentExeFolderPath() + "\\config\\" + "progressBeforeTermination.json", std::ios::trunc); //truncate the file before writing default json to it
    configOutput << defaultConfigData;
    configOutput.close();
}
void setDefaultConfigVariables(std::string &INITIAL_WALLPAPER_NAME, int &CURRENT_WALLPAPER_INDEX, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE){
    std::cout << "Setting default values for config vars" << std::endl;
    INITIAL_WALLPAPER_NAME = defaultConfigData.at("INITIAL_WALLPAPER_NAME");
    CURRENT_WALLPAPER_INDEX = defaultConfigData.at("CURRENT_WALLPAPER_INDEX");
    WALLPAPER_CHANGE_WAIT_DURATION = defaultConfigData.at("WALLPAPER_CHANGE_WAIT_DURATION");
    NEXT_WALLPAPER_CHANGE_DUE = defaultConfigData.at("NEXT_WALLPAPER_CHANGE_DUE");
}

void loadConfigAndProgress(std::string &INITIAL_WALLPAPER_NAME, int &CURRENT_WALLPAPER_INDEX, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE){

    std::string configJsonFilePath = getCurrentExeFolderPath() + "\\config\\" + "progressBeforeTermination.json";
    std::ifstream configFile(configJsonFilePath);
    if(!configFile.is_open()){
        std::cout << "Failed to open configuration file! Writing now!" << std::endl;
        configFile.close();
        writeDefaultDataToConfigFile();
        setDefaultConfigVariables(INITIAL_WALLPAPER_NAME, CURRENT_WALLPAPER_INDEX,WALLPAPER_CHANGE_WAIT_DURATION,  NEXT_WALLPAPER_CHANGE_DUE);
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
        setDefaultConfigVariables(INITIAL_WALLPAPER_NAME, CURRENT_WALLPAPER_INDEX,WALLPAPER_CHANGE_WAIT_DURATION,  NEXT_WALLPAPER_CHANGE_DUE);
        return;
    }

    try{
        INITIAL_WALLPAPER_NAME = config_file_json_data.at("INITIAL_WALLPAPER_NAME");
        CURRENT_WALLPAPER_INDEX = config_file_json_data.at("CURRENT_WALLPAPER_INDEX");
        WALLPAPER_CHANGE_WAIT_DURATION = config_file_json_data.at("WALLPAPER_CHANGE_WAIT_DURATION");
        NEXT_WALLPAPER_CHANGE_DUE = config_file_json_data.at("NEXT_WALLPAPER_CHANGE_DUE");
        std::cout << "Successfully set vonfig variables from json file" << std::endl;
    }
    catch(nlohmann::json::exception& e){
        std::cout << "Failed to parse configuration data! Setting default configuration" << std::endl;
        configFile.close();
        writeDefaultDataToConfigFile();
        setDefaultConfigVariables(INITIAL_WALLPAPER_NAME, CURRENT_WALLPAPER_INDEX,WALLPAPER_CHANGE_WAIT_DURATION,  NEXT_WALLPAPER_CHANGE_DUE);
        return;
    }
    
    configFile.close();
}
void updateJsonProgressConfigDataFile(nlohmann::json jsonData){
    std::ofstream outputConfigFile(getCurrentExeFolderPath() + "\\config\\" + "progressBeforeTermination.json", std::ios::trunc);
    outputConfigFile << jsonData;
    outputConfigFile.close();
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

void continuouslyChangeWallpaper(int &CURRENT_WALLPAPER_INDEX, std::string &INITIAL_WALLPAPER_NAME, std::vector<std::string> &imageListFileVector, int fileListSize, int &WALLPAPER_CHANGE_WAIT_DURATION, long long &NEXT_WALLPAPER_CHANGE_DUE){
    
    //now we have to look at the initial image and see how long we have to keep that for
    //basically, if the program terminated but we still have some duration left from the image that was showing before the process terminated, we still have t show that image for the remaining time
    //to get this remaining time, calculate (wallpaper_change_due_time_millis - current_millis_time)
        //if this quantity is <= 0, don't do anything. otherwise we first show the initial image for that time, and then move on
    //also note that we will need to constantly write the next wallpaper change time to the file every time we change the wallpaper. so current_millis_time + WALLPAPER_CHANGE_WAIT_DURATION_converted_to_milliseconds
    nlohmann::json currentConfigData = {
        {"INITIAL_WALLPAPER_NAME", INITIAL_WALLPAPER_NAME},
        {"CURRENT_WALLPAPER_INDEX", CURRENT_WALLPAPER_INDEX},
        {"WALLPAPER_CHANGE_WAIT_DURATION", WALLPAPER_CHANGE_WAIT_DURATION},
        {"NEXT_WALLPAPER_CHANGE_DUE", NEXT_WALLPAPER_CHANGE_DUE}
    };

    //the case of where the file list has changed and the Initial_wallpaper we were on before any system shutdown is taken care of since the file list can only change through the GUI app. and knowing that we are changing the file list from there only, we can make sure that the initial_wallpaper_name property is an empty string, and all other properties are updated properly
    //also check if prior progress config was pointing to a valid image file and the index matches
    if(NEXT_WALLPAPER_CHANGE_DUE > 0 && INITIAL_WALLPAPER_NAME.compare("") != 0){
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
        CURRENT_WALLPAPER_INDEX++;
        if(CURRENT_WALLPAPER_INDEX >= fileListSize)
            CURRENT_WALLPAPER_INDEX = 0;  
    }
    
    while (true && fileListSize > 0) {
        WallpaperChanger::ChangeWallpaper(imageListFileVector[CURRENT_WALLPAPER_INDEX]);
        if(CURRENT_WALLPAPER_INDEX >= fileListSize)
            CURRENT_WALLPAPER_INDEX = 0;

        currentConfigData["NEXT_WALLPAPER_CHANGE_DUE"] = currentMillis() + WALLPAPER_CHANGE_WAIT_DURATION * 1000;
        currentConfigData["INITIAL_WALLPAPER_NAME"] = imageListFileVector[CURRENT_WALLPAPER_INDEX];
        currentConfigData["CURRENT_WALLPAPER_INDEX"] = CURRENT_WALLPAPER_INDEX;


        updateJsonProgressConfigDataFile(currentConfigData);
        CURRENT_WALLPAPER_INDEX++;

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