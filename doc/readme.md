

``` shell

git clone --depth=1 https://github.com/gflags/gflags.git
cmake -DBUILD_SHARED_LIBS=ON -S gflags -B gflags/build
cmake --build gflags/build -- -j$(nproc) 
sudo cmake --build gflags/build --target install
sudo ldconfig

clang-format -style=google -dump-config > .clang-format

```