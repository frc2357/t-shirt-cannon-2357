#ifndef DASH_PAGE_H
#define DASH_pAGE_h

#include "Page.h"

class DashPage : public Page
{
public:
    DashPage();
    void paint(DisplayController &display, bool isActivated);
};

#endif