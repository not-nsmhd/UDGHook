# UDGHook
~~Utility menu~~ Unfinished external code injection thing for the Steam version of Ultra Despair Girls.

TODO: Write a proper description.

## Building & usage
Fetch the source code either by cloning the repository using `git` or downloading the source code directly.

Open the `UDGHook.sln` file in Visual Studio 2019 or newer and build the solution. Built `UDGHook.asi` file will be placed in the `bin` directory in the same directory as the solution file.

Copy the `UDGHook.asi` file in the `plugins` folder in the root folder of UDG near the executable file (i. e. `C:\Program Files (x86)\Steam\steamapps\common\Danganronpa Another Episode Ultra Despair Girls\plugins`). Note that the 64-bit version of [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/) must also be present at the root folder of UDG.

The final layout of UDG's installation folder should look something like this (assuming you have the English version installed):
- en
- plugins
  - UDGHook.asi
- dinput8.dll (Ultimate ASI Loader)
- game.exe