# Nexus Mods CLI (NXM)
NXM is a command line interface for [Nexus Mods](https://www.nexusmods.com/) .

### Table of Contents
- [Getting Started](#getting-started)
    - [Clone](#clone)
        - [Submodules](#submodules)
    - [Building](#build)
    - [Install](#install)
- [Usage](#usage)

This software has been developed as a plugin to run on Orthanc DICOM servers.
## Getting Started
### Clone
Don't forget to populate submodules.
```bash
# clone repo
git clone https://github.com/cppcooper/nxm.git
cd nxm
# we need to populate submodules
git submodule init
git submodule update
```

#### Dependencies
The libraries being used. CPR didn't play nice as a submodule, so just install manually `yay -Sy cpr` (for arch users).
| Library | Purpose | Docs |
|---------|---------|------|
| [gumbo-query](https://github.com/cppcooper/gumbo-query) | HTML parser | `make doc` |
| [nlohmann/json](https://github.com/nlohmann/json) | json API | https://nlohmann.github.io/json/api/basic_json/ |
| [cpr](https://github.com/libcpr/cpr) | curl | https://docs.libcpr.org/ |
| [cli11](https://github.com/CLIUtils/CLI11) | command line parser | https://github.com/CLIUtils/CLI11/tree/main#usage |

### Build
Straight forward build process. No installation config yet.
```bash
mkdir build
cd build
# configure build
# on linux
cmake ..
# on windows, pretty sure they've got VS as the default
cmake .. -G "Unix Makefiles"

# perform build
make
```
or if you're a cool ninja
```bash
mkdir build
cd build
cmake .. -G Ninja
ninja
```

### Install
Installing is pretty easy. 
```bash
# clone repo
git clone https://github.com/cppcooper/nxm.git
cd nxm
# we need to populate submodules
git submodule init
git submodule update
# build/install
mkdir build
cd build
cmake .. -G Ninja
sudo ninja install
```

## Usage
coming soon