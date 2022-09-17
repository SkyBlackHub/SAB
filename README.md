### Snap Assistant Bot

A small utility for quick and convenient saving the images from the Windows clipboard.  
Primarily, the screen snapshots.

![Alt text](/resources/images/screenshot.png?raw=true "Snap Assistant Bot - Main Window Screenshot")

The utility monitors the clipboard, and when a new image appears in it, saves it in the specified directory and format, with the ability to naming using information of the currently active window.  
The saving performs in parallel threads and doesn't block the main process.  
The utility can also check that the **PrintScreen** key was previously pressed in order to avoid false triggering. This allows to quickly perform the screen capture, because it'll be made directly by OS.

The utility also allows to prevent the triggering by filtering the path of the executable file of the active window, and automatically cut the image to the client region of the active window.