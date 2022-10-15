
```
git clone https://github.com/google/googletest/
cmake .
make
sudo make install

refer to https://github.com/edenhill/librdkafka
./configure
  make
  sudo make install

git clone https://github.com/gabime/spdlog.git
cd spdlog && mkdir build && cd build
cmake .. && make -j
sudo make install

https://github.com/Tencent/rapidjson
git clone https://github.com/Tencent/rapidjson --depth 1 --branch 27c3a8dc0e2c9218fe94986d249a12b5ed838f1d
cd rapidjson && mkdir build && cd build
cmake ..
make
sudo make install

git clone https://github.com/usdot-fhwa-stol/carma-streets
cd kafka_clients && mkdir build && cd build
cmake .. && make -j
sudo make install
```
