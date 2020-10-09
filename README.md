# open
Bring OSX open command to Linux

## Features
- Search apps in common .dekstop based directories
- Ignores NoDisplay entries
- Search apps in flatpak exported binaries
- Does not require editing the PATH
- Strict mode (only checks for desktop and flatpak file names)
- By default tests the file name and additionally checks .desktop Name entry if provided
- Circumvents flatpak restriction on launching applications only by providing full id. Instead open allows to specify only application name
- Case insensitive

## Install
- Build the software
- Inject the "open" generated binary in your /bin
- Enjoy

## Build
- Install meson and ninja (sudo apt install meson ninja-build)
- Make sure you have the GLib installed (it comes pre-installed with any GNOME based desktop environments)
- Run meson and then ninja
