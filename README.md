LightGuider
===========

Light Guider for LightAPT , modified from PHD2

### Features

+ Fully open source based on GPL3, ensuring transparency and community-driven development
+ Multiple interface access, allowing for flexible and convenient control from various devices
+ Highly optimized asynchronous architecture, ensuring stable and efficient performance
+ Extensive platform support, enabling use on all mainstream operating systems
+ Device driver based on INDI and ASCOM, supporting a wide range of astronomical devices
+ Built-in guide tool (modified PHD2) and platesolving engines (asap and astrometry) for improved accuracy and precision.

#### Build Guider

On Ubuntu / Debian
```
sudo apt-get install build-essential gcc git cmake pkg-config libwxgtk3.2-dev \
   wx-common wx3.2-i18n libindi-dev libnova-dev gettext zlib1g-dev libx11-dev \
   libcurl4-gnutls-dev libcfitsio-dev libeigen3-dev libspdlog-dev libgsl-dev libboost-dev \
   libasio-dev libboost-system-dev libboost-thread-dev libssl-dev
```

```
mkdir build && cd build
cmake ..
make -j4 (depends on the computer)
./lightguider
./lightguider_logviewer

make package # build deb package
```
