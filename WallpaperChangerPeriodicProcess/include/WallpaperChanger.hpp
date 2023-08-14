#ifndef WALLPAPERCHANGER_H
#define WALLPAPERCHANGER_H

#include <string>
#include <windows.h>

class WallpaperChanger{
public:
    static bool ChangeWallpaper(std::string path);
};

#endif 
