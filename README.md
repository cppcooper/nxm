# Nexus Mods CLI (NXM)
NXM is a command line interface for [Nexus Mods](https://www.nexusmods.com/).

Unfinished:
 - dependency listing (formatting)

Unimplemented plans:
 - downloads
 - parsing mods by name

Untested:
 - (un)tracking
 - (un)endorsing

### Table of Contents
- [Getting Started](#getting-started)
    - [Clone](#clone)
        - [Dependencies](#dependencies)
- [Install](#install)
- [Usage](#usage)

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

## Install
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
