LightGuider
========
Light Guider for LightAPT , modified from PHD2

#### Build Guider

In this project , we use PHD2 as the guider , but we modified it and made many differences. Build with following steps.

```
sudo apt-get install build-essential gcc git cmake pkg-config libwxgtk3.0-gtk3-dev \
   wx-common wx3.0-i18n libindi-dev libnova-dev gettext zlib1g-dev libx11-dev \
   libcurl4-gnutls-dev libcfitsio-dev libeigen3-dev
```

```
mkdir build && cd build
cmake ..
make -j4 (depends on the computer)
cd server/guider
./guider
```

## Support

+ QQ Group 710622107
+ Email astro_air@126.com
