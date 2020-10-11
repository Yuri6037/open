# open
Bring OSX open command to Linux

## Features
- Search apps in common .dekstop based directories
- Ignores NoDisplay entries
- Search apps in flatpak exported binaries
- Does not require editing the PATH
- Strict mode:
    - For desktop files, only checks file name without extension
    - For flatpak, only checks the last component of the app id
- By default:
    - For desktop files tests the file name and the Name entry if provided
    - For flatpak, checks both the last and before last component to account for weird naming convention such as OnlyOffice
- Circumvents flatpak restriction on launching applications only by providing full id. Instead open allows to specify only application name
- Case insensitive

## Install
- Build the software
- Inject the "open" generated binary in your /bin
- Enjoy

## Example usage

```bash
open files .
```
On Ubuntu and derivatives this should open nautilus on the current working directory.

## Build
- Install meson and ninja (sudo apt install meson ninja-build)
- Install gcc
- Make sure you have the GLib installed (it comes pre-installed with any GNOME based desktop environments)
- Run meson and then ninja

