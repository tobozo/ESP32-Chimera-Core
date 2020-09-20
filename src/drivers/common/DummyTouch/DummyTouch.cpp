// TODO: fire real touch events (e.g. M5Core2 emulation) when button is pressed

#include "DummyTouch.h"

touch::touch(/* args */)
{
}

touch::~touch(void)
{

}

void touch::begin(void)
{

}

bool touch::ispressed(void)
{
    return false;
}

const TouchPoint_t& touch::getPressPoint(void)
{
    _TouchPoint.x = _TouchPoint.y = -1;
    return _TouchPoint;
}

int touch::readTouchtoBuff(void)
{
  return -1;
}
