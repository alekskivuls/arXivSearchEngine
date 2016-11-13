[![Build Status](https://travis-ci.org/alekskivuls/arXivSearchEngine.svg?branch=master)](https://travis-ci.org/alekskivuls/arXivSearchEngine)

# arXivSearchEngine
Search Engine to search reasearch papers published in http://arxiv.org/

##Documentation
Documentation being served here https://alekskivuls.github.io/arXivSearchEngine/

##Build Instructions with CMake
From top level directory
```
mkdir build
cd build
cmake ..
make
```
Executables are generated in build directory 

Console Engine: `arXivSearchEngine`
Web Engine: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; `arXivSearchEngineWeb`
Unit tests: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`arXivSearchEngine_test`
When making changes to source only need to run `make` again to regenerate executables

##Clean Build
From top level directory
```
rm -r build
```

##Vagrant
