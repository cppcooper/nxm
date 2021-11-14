# Nexus Mods CLI (NXM)
NXM is a command line interface for [Nexus Mods](https://www.nexusmods.com/) .

### Table of Contents
- [Getting Started](#getting-started)
    - [Clone](#clone)
        - [Submodules](#submodules)
    - [Building](#build)

This software has been developed as a plugin to run on Orthanc DICOM servers.
## Getting Started
### Clone
Don't forget to populate submodules.
```bash
# clone repo
git clone https://github.com/BCCF-UBCO-AD/Orthanc-TMI.git orthanc-tmi
cd orthanc-tmi
# develop is where all the action is
git checkout develop
# we need to populate submodules (googletest)
git submodule init
git submodule update
```
#### Submodules
The submodules you need to initialize.
| Library | Purpose | URI |
|---------|---------|-----|
| [gumbo-query](extern/gumbo-query) | HTML parser | <ul><li>[external repo](https://github.com/jtv/libpqxx.git) <li>[docs - API](https://libpqxx.readthedocs.io/en/stable/a01382.html) |
| [nlohmann/json](extern/nlohmann-json) | json API | <ul><li>[external repo](https://github.com/nlohmann/json.git) <li>[docs - integration](https://github.com/nlohmann/json#integration) <li>[docs - API](https://nlohmann.github.io/json/api/basic_json/) |
| [cpr](extern/cpr) | curl | <ul><li>[external repo](https://github.com/google/googletest.git) |
| [cli11](extern/cli11) | command line parser | <ul><li>[external repo](https://github.com/google/googletest.git) |

### Build
The project is configured to build a plugin (dll/so) (target `'data-anonymizer'`) binary, then copy it to `docker/plugins` where the orthanc docker server can read it.

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