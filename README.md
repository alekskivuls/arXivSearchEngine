[![Build Status](https://travis-ci.org/alekskivuls/arXivSearchEngine.svg?branch=master)](https://travis-ci.org/alekskivuls/arXivSearchEngine) [![Build status](https://ci.appveyor.com/api/projects/status/anywctclrbair138/branch/master?svg=true)](https://ci.appveyor.com/project/alekskivuls/arxivsearchengine/branch/master) [![Coverage Status](https://coveralls.io/repos/github/alekskivuls/arXivSearchEngine/badge.svg?branch=master)](https://coveralls.io/github/alekskivuls/arXivSearchEngine?branch=master)
# arXivSearchEngine
Search Engine to search research papers published in http://arxiv.org/

## Documentation
Documentation being served here https://alekskivuls.github.io/arXivSearchEngine/

## Build Instructions with CMake
From top level directory
```
mkdir build
cd build
cmake ..
make
```
Executables are generated in build directory.

| Program 	 | Executable Name    	    |
| -------------- | ------------------------ |
| Console Engine | `arXivSearchEngine`	    |
| Web Engine	 | `arXivSearchEngineWeb`   |
| Unit tests     | `arXivSearchEngine_test` |

When making changes to source only need to run `make` again to regenerate executables.

## Clean Build
From top level directory
```
rm -r build
```

## Sanitizers
To run with sanitizers pass cmake flags `-DSANITIZE_ADDRESS=On`, `-DSANITIZE_MEMORY=On`, `-DSANITIZE_THREAD=On`, or `-DSANITIZE_UNDEFINED=On`

## Vagrant
Can build and run project from a Vagrant box.
Install [Vagrant](https://www.vagrantup.com/) with [Virtual Box](https://www.virtualbox.org/) as your provider.
#### Command List:
```
vagrant up  		# Run to turn on VM
vagrant ssh 		# Connect to VM, password is vagrant if prompted
vagrant halt 		# Stop VM
vagrant destroy 	# Delete VM from disk
```
#### Explanation: 
* To run, in top level directory run `vagrant up` to provision the VM.
* Then run `vagrant ssh` to login. If prompted for password, default password is `vagrant`.
* To stop VM run `vagrant halt`. Run `vagrant up` to spin up again.
* If you wish to destroy the VM run `vagrant destroy` recommend halting VM first. You can then run `vagrant up` to create a new fresh VM if you wish.
* Can run and build inside vm. Follow respective commands to do so. Running the Web Engine, the port 8080 is forwarded to localhost:8888 to open on your host machine's browser.
