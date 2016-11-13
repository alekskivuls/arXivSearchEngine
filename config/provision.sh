#!/bin/bash
sudo apt-add-repository -y ppa:ubuntu-toolchain-r/test 
sudo apt-add-repository -y ppa:george-edison55/precise-backports
sudo apt-add-repository -y ppa:boost-latest/ppa
sudo sh -c 'echo "deb http://llvm.org/apt/precise/ llvm-toolchain-precise main" >> /etc/apt/sources.list'
sudo apt-get update -qq

sudo apt-get install -y cmake cmake-data g++-5 gcc-5 git libboost1.55-all-dev #g++-4.8 valgrind doxygen graphviz 
#sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 30
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 20



#WT install
#sudo apt-get install -y witty

#Set corpus dir environment variable
echo "export CORPUS_TEST_DIR=/home/vagrant/test/documents/testCorpus" >> ~/.profile


##Custom boost build for later versions
#sudo apt-get install -y python-dev 
#wget https://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.bz2/download -O boost_1_58_0.tar.bz2
#tar --bzip2 -xf boost_1_58_0.tar.bz2
#cd boost_1_58_0
#sudo ./bootstrap.sh
#sudo ./b2 install
