# rpi-pico
Enjoy coding😁😁😁
## what
__I'm__ a greenhand in pico develop. This project includes some Raspiberry PI Pico driver that I have made is based on _Pico c/c++ SDK_. <br/>
__This__ project is also a quickstart template for c/c++ pico develop.

## architecture
- `lib` the dir for underlying implements such as protocol and so on.
-  `src` the dir for middleware implements such as a driver can directly use.
-   main.c aggregate middleware to implement your product's function. This can compile to the final executeble hex.

## quick start
1.  download pico-sdk from official site
1.  edit `./CMakeLists.txt` and modify the "include" section to your own.
1. run fllowing command 
```sh
# build the project
mkdir build
cd build
cmake -DCMAKE_PICO_SDK=path-to-your-pico_sdk_init.cmake ..
make

# flash
mv ./your-hex.uf2 /dir-your-pico-mount
```

## support
- gpio software i2c master

## coming soon
- gpio software i2c slave
- other