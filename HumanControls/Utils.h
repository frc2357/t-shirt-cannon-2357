#ifndef UTILS_H
#define UTILS_H

#include "TShirtCannonPayload.h"

class Utils
{
public:
  enum ControllerStatus
  {
    DISCONNECTED = 0,
    DISABLED = 1,
    ENABLED = 2,
    PRIMED = 4,
    FIRING = 5
  };
  static void setMotors(TShirtCannonPayload &payload, float turn, float speed);
};

#endif
