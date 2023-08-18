#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <Shlwapi.h>
#include <locale>
#include <codecvt>
#include <chrono>
#include <thread>
#include <tchar.h>

std::string getExePath(){
    char szPath[MAX_PATH * 100];
    GetModuleFileNameA(NULL, szPath, MAX_PATH * 100); //get file path of exe
    PathRemoveFileSpecA(szPath); //remove exe file name (1 byte chars)
    PathRemoveFileSpecA(szPath); //remove current folder name to make it represent parent folder path


    std::string folderPath = szPath;
    std::string filePath = folderPath + "\\WallpaperChangerBackgroundInit\\WallpaperBackgroundProcessInit.exe"; //append the file name to the folder path

    return filePath;
}
std::wstring getWExePath(){
    wchar_t szPath[MAX_PATH * 100];
    GetModuleFileNameW(NULL, szPath, MAX_PATH * 100); //get file path of exe
    PathRemoveFileSpecW(szPath); //remove exe file name (1 byte chars)
    PathRemoveFileSpecW(szPath); //remove current folder name to make it represent parent folder path


    std::wstring folderPath = szPath;
    std::wstring filePath = folderPath + L"\\WallpaperChangerBackgroundInit\\WallpaperBackgroundProcessInit.exe"; //append the file name to the folder path

    return filePath;
}

void stringToWcharArray(const std::string& str, wchar_t* wcharArray, size_t maxLength) {
    // Copy characters from std::string to wchar_t array manually
    size_t i;
    for (i = 0; i < maxLength - 1 && i < str.length(); ++i) {
        wcharArray[i] = static_cast<wchar_t>(str[i]);
    }
    wcharArray[i] = L'\0'; // Null-terminate the wchar_t array
}
void WstringToWcharArray(const std::wstring& str, wchar_t* wcharArray, size_t maxLength) {
    // Copy characters from std::string to wchar_t array manually
    size_t i;
    for (i = 0; i < maxLength - 1 && i < str.length(); ++i) {
        wcharArray[i] = str[i];
    }
    wcharArray[i] = L'\0'; // Null-terminate the wchar_t array
}

int addExeToRegistry(){
    HKEY hKey;
    LONG openResult = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);

    if (openResult == ERROR_SUCCESS) {
        // Check if the entry already exists
        wchar_t existingValue[MAX_PATH * 100];
        DWORD dataSize = sizeof(existingValue);
        LONG queryResult = RegQueryValueExW(hKey, L"Wallpaper_Master_App", NULL, NULL, (BYTE*)existingValue, &dataSize); ///queries the regisry for the program name as the key. stores the exe path value in the existingValue wchar_t array (std::wstring in a c-style format. like how char* is the c-style of std::string)

        std::wstring newExecutablePath = getWExePath(); //current path of the exe we are trying to run/invoke though here
        if (queryResult != ERROR_SUCCESS || wcslen(existingValue) == 0) {
            // If the registry entry doesn't exist, or its value is empty (length of wide string is 0), create it
            wchar_t new_executablePath[MAX_PATH * 100];
            WstringToWcharArray(newExecutablePath, new_executablePath, MAX_PATH*100); //

            //the function that sets a registry entry
                //1. the handle to open the registry key
                //2. Long Pointer to Constant Wide String. in this case, the constant wide string is the name of the program. Note that in c++, string literals in general are constant by default
                //3. must be 0
                //4. type of the value being stored with this registry key. in this case, fixed length regisry string
                //5. value of registry key (in this case, it is the value or the path of the exe)
                //6. the value (exe path). Must be byte* to allow for generic pointers to be passed in. Essentially how it works behind the scenes (not easily seen here) is that a byte* pointer is just a pointer to a sequence of bytes. then given the size of the individual elements in the sequence themselves (for example, sizeOf(wchar_t)), we can increment the pointer using pointer arithmetic by sizeOf(wchar_t) to move the pointer that many bytes ahead in memory. by doing this, we would ensure we look at all 'sizeof(wchar_t)' bytes and interpret the correct data of an element, and move the correct number of bytes forward to accurately reach an entirely new next element in the sequence
            RegSetValueExW(hKey, L"Wallpaper_Master_App", 0, REG_SZ, (BYTE*)new_executablePath, (wcslen(new_executablePath) + 1) * sizeof(wchar_t));
            std::cout << "Successfully added to registry" << std::endl;
        } else {
            // Compare the existing path with the new path
            if (newExecutablePath.compare(existingValue) != 0) {
                // Paths are different, update the registry entry
                wchar_t executablePath[MAX_PATH * 100];
                WstringToWcharArray(newExecutablePath, executablePath, MAX_PATH*100);
                RegSetValueExW(hKey, L"Wallpaper_Master_App", 0, REG_SZ, (BYTE*)executablePath, (wcslen(executablePath) + 1) * sizeof(wchar_t));
                std::cout << "Successfully updated registry entry" << std::endl;
            } else {
                std::cout << "Registry entry already exists and points to the same executable" << std::endl;
            }
        }

        RegCloseKey(hKey);

    }
    return 0;
}
void startProcess(){
    
    TCHAR szPath[MAX_PATH * 100];
    GetModuleFileName(NULL, szPath, MAX_PATH * 100); // Get file path of exe
    PathRemoveFileSpec(szPath); // Remove exe file name
    PathRemoveFileSpec(szPath);

    // Construct the full path to the subfolder wallpaperchangerperiodicprocess executable
    TCHAR executablePath[MAX_PATH * 100]; //we use TCHAR or wchar_t (wide character) in Windows programming for unicode support (2 byte characters)
    _tcscpy_s(executablePath, szPath);
    _tcscat_s(executablePath, _T("\\WallpaperChangerBackgroundInit\\WallpaperBackgroundProcessInit.exe"));

    // additional information
    STARTUPINFO si;     
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if (CreateProcess(NULL, executablePath, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess); // Close the process handle immediately
        CloseHandle(pi.hThread);  // Close the thread handle immediately
    } else {
        DWORD error = GetLastError(); //A DWORD is a 32-bit unsigned integer
    }
}
int main(){
    addExeToRegistry();
    startProcess();

}