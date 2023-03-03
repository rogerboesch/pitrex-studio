
## pitrex_studio
#### BASIC Studio for teh PiTrex (Multiplatform)

*More coming soon*

### How to build

```bash
git remote add origin https://rogerboesch@bitbucket.org/rogerboesch/pitrex_studio.git
cd pitrex_studio
mkdir build
cd build
```


**Build for Windows (Visual Studio Code)**

```bash
cmake ..
cmake --build . --config MinSizeRel
```


**Build for macOS (XCode)**

```bash
cmake -GXcode ..
```


**Build for Linux**

```bash
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
cmake --build .
```
