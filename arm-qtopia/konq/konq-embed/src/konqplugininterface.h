#ifndef __plugininterface_h__
#define __plugininterface_h__

#include <qglobal.h>

#if defined(_QT_QPE_)

#include <qpe/qcom.h>

// {F3DB61DC-B4A0-482A-8D2C-681571A6A480}
#ifndef IID_KonqPluginInterface
#define IID_KonqPluginInterface QUuid( 0xf3db61dc, 0xb4a0, 0x482a, 0x8d, 0x2c, 0x68, 0x15, 0x71, 0xa6, 0xa4, 0x80)
#endif
struct KonqPluginInterface : public QUnknownInterface
{
    virtual void setup() = 0;
};

#endif

#endif

