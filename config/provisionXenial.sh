#!/bin/bash
sudo apt-get -qq update

sudo apt-get install -y cmake g++-5 gcc-5 libboost1.58-all-dev witty witty-dev

sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 30
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 30

echo "export CORPUS_TEST_DIR=/home/vagrant/test/documents/testCorpus" >> ~/.profile
