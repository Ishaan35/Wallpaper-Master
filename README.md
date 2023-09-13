# Wallpaper-Master

This is a project dedicated to allow users to customize their Windows wallpaper experience. It allows users to set a slideshow from multiple folders of their choice, and adjust the duration of each picture. This project uses a save-state feature which brings up the wallpaper previously being shown before a full system shutdown, to ensure each wallpaper is displayed for the accurate time that was set by the user. This application runs on start-up.

This project is written mostly in C++, and is currently under development. For now, the parameters can temporarily be configured in the WallpaperChangerPeriodicProcess/config/progressBeforeTermination.json file, and the WallpaperChangerPeriodicProcess/config/folderList.txt file. I am working on a C# Windows Form application that can provide a user-friendly interface for all the configurations I plan to add.

## Built With  

* [![][C++]][C++-url]
* [![][C#]][C#-url]


[C#]: https://img.shields.io/badge/C%23-9a48b1?style=for-the-badge&logo=c%20sharp&logoColor=white
[C#-url]: https://learn.microsoft.com/en-us/dotnet/csharp/

[C++]: https://img.shields.io/badge/C++-085e9f?style=for-the-badge&logo=cplusplus&logoColor=white
[C++-url]: https://cplusplus.com/


## Usage

1. Download the project and do not change the file or folder structure or names.
2. Navigate to the WallpaperChangerPeriodicProcess/config/folderList.txt file and enter the full path of the folder containing your images (relative to your main drive C, or whatever drive your images are on). You may enter multiple folders, each on a new line.
3. (Optional) Navigate to the WallpaperChangerPeriodicProcess/config/progressBeforeTermination.json file, and change the 'WALLPAPER_CHANGE_WAIT_DURATION' property of the JSON to the number of seconds you would like for each wallpaper in the slideshow.
4. Run the executable at 'WallpaperChangerAddToRegistry/WallpaperChangerRegistryConfig.exe'.
5. If the process has not started yet, try restarting your computer.

## Roadmap

- [ ] Add Windows Form application for an easy-to-use UI
- [ ] Add a feature to download wallpapers from within the app.
- [ ] Similar image identification feature to display wallpapers of a specific type.
