#include <windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>

int closeAllExistingProcesses();
int writeProcessIdToFile(DWORD pid);
int redirectLogs();

int main() {
    redirectLogs();
    TCHAR szPath[MAX_PATH * 100];
    GetModuleFileName(NULL, szPath, MAX_PATH * 100); // Get file path of exe
    PathRemoveFileSpec(szPath); // Remove exe file name
    PathRemoveFileSpec(szPath); //remove current directory name, resulting in a path to parent directory

    // Construct the full path to the subfolder wallpaperchangerperiodicprocess executable
    TCHAR executablePath[MAX_PATH * 100]; //we use TCHAR or wchar_t (wide character) in Windows programming for unicode support (2 byte characters)
    _tcscpy_s(executablePath, szPath);
    _tcscat_s(executablePath, _T("\\WallpaperChangerPeriodicProcess\\WallpaperChanger.exe"));

    //std::wcout << executablePath << std::endl; // Use wcout for wide-character output

    STARTUPINFO startupInfo = { sizeof(startupInfo) }; //initialize a startupInfo object of the correct size. contains information about how the new process should be displayed (window appearance) and is used when creating a new process
    PROCESS_INFORMATION processInfo; //this structure is used to receive information about the newly created process, such as its handles and identifiers.

    // Hide the console window and create the process without a new console window (background task)
    startupInfo.dwFlags = STARTF_USESHOWWINDOW; //This line sets the dwFlags member of the STARTUPINFO structure to indicate that the wShowWindow field is being used to control the visibility of the new process's window 
    startupInfo.wShowWindow = SW_HIDE; //hide the window

    if(closeAllExistingProcesses() == 1)
        return 1;


    if (CreateProcess(NULL, executablePath, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo)) {
        writeProcessIdToFile(processInfo.dwProcessId);
        CloseHandle(processInfo.hProcess); // Close the process handle immediately
        CloseHandle(processInfo.hThread);  // Close the thread handle immediately
    } else {
        DWORD error = GetLastError(); //A DWORD is a 32-bit unsigned integer
    }

    return 0;
}

std::string getLogsFilePath(){
    //now we need to make sure the exe is only looking at its relative folder for the text file
    //hence, we look in the same folder where the exe is located in
    char szPath[MAX_PATH * 100];
    GetModuleFileNameA(NULL, szPath, MAX_PATH * 100); //get file path of exe
    PathRemoveFileSpecA(szPath); //remove exe file name
    std::string folderPath = szPath;
    std::string filePath = folderPath + "\\" + "logs.txt"; //append the file name to the folder path

    return filePath;
}
std::string getPidFilePath(){
    //now we need to make sure the exe is only looking at its relative folder for the text file
    //hence, we look in the same folder where the exe is located in
    char szPath[MAX_PATH * 100];
    GetModuleFileNameA(NULL, szPath, MAX_PATH * 100); //get file path of exe
    PathRemoveFileSpecA(szPath); //remove exe file name
    std::string folderPath = szPath;
    std::string filePath = folderPath + "\\" + "pid.txt"; //append the file name to the folder path

    return filePath;
}

int writeProcessIdToFile(DWORD pid){

    

    std::ofstream pidFile(getPidFilePath(), std::ios::app);
    if (!pidFile.is_open()) {
        std::cout << "Failed to open pid.txt for recording new process instance" << std::endl;
        return 1;
    }

    pidFile << pid << std::endl;

    pidFile.close();
    return 0;
}

int closeAllExistingProcesses(){

    std::cout << "Closing..." << std::endl;
    std::ifstream inputFile(getPidFilePath());
    if(!inputFile.is_open()){
        std::cout << "Error: \"pid.txt\" was not found!";
        inputFile.close();
        return 1;
    }
    
    std::vector<DWORD> processList;
    std::string line;
    while(std::getline(inputFile, line)){
        try {
            DWORD result = std::stoul(line);
            processList.push_back(result);
            std::cout << "Successfully added pid to deletion queue" << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid argument: " << e.what() << std::endl;
            return 1;
        } catch (const std::out_of_range& e) {
            std::cout << "Out of range: " << e.what() << std::endl;
            return 1;
        }
    }


    for(int i = 0; i < processList.size(); i++){
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processList[i]);
        if (hProcess != NULL) {
            if (TerminateProcess(hProcess, 0)) {
                std::cout << "Process terminated successfully." << std::endl;
            } else {
                DWORD error = GetLastError();
                std::cout << "Failed to terminate process. Error code: " << error << std::endl;
            }
            CloseHandle(hProcess);
        } else {
            DWORD error = GetLastError();
            std::cout << "Failed to open process. Error code: " << error << std::endl;
        }
        
    }
    std::ofstream ofs;
    ofs.open("pid.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    inputFile.close();
    std::cout << "Cleared old processes" << std::endl;

    return 0;
}
int redirectLogs(){
    freopen(getLogsFilePath().c_str(),"w",stdout);
    freopen(getLogsFilePath().c_str(),"w",stderr);
    return 0;
}