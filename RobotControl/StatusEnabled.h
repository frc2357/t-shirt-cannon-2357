#ifndef STATUS_ENABLED_H
#define STATUS_ENABLED_H

#include "RobotStatus.h"
#include "StatusEnum.h"

class StatusEnabled : RobotStatus {
public:
    void update();
    void onTransition();
}