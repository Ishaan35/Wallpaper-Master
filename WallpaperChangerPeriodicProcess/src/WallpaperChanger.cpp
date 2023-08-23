#include "../include/WallpaperChanger.hpp"
#include <string>
#include <iostream>
#include <windows.h>



bool WallpaperChanger::ChangeWallpaper(std::string path){
    int result = SystemParametersInfo(SPI_SETDESKWALLPAPER,0,(void*)path.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    if(result){
        std::cout << "Successfully changed Wallpaper to: " << path << std::endl;
        return true;
    }
    std::cout << "Failed to change Wallpaper" << std::endl;
    return false;
}
