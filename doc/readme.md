

``` shell

git clone --depth=1 https://github.com/libevent/libevent.git
cmake -DBUILD_SHARED_LIBS=ON -S libevent/ -B libevent/build
cmake --build libevent/build -- -j$(nproc) 
sudo cmake --install libevent/build

git clone --depth=1 https://github.com/openssl/openssl.git
cd openssl/ 
./Configure
make -j12
sudo make install

git clone --depth=1 https://github.com/google/googletest.git
cmake -DBUILD_SHARED_LIBS=ON -S googletest/ -B googletest/build
cmake --build googletest/build -- -j$(nproc) 
sudo cmake --install googletest/build

git clone --depth=1 https://github.com/gflags/gflags.git
cmake -DBUILD_SHARED_LIBS=ON -S gflags -B gflags/build
cmake --build gflags/build -- -j$(nproc) 
sudo cmake --build gflags/build --target install

apt-get install bridge-utils

sudo ldconfig

clang-format -style=google -dump-config > .clang-format


# hellp pkt
echo -n -e "\x01\x01\x00\x08\x00\x00\x00\x0A\xE1\xE2" | nc -u 127.0.0.1 58080

```