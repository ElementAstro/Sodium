#ifndef __APP_H__
#define __APP_H__

#include "crow.h"

extern crow::SimpleApp app;
extern "C" void run();
extern "C" void stop();

#endif