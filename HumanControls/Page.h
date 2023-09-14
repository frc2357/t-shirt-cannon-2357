#ifndef PAGE_H
#define PAGE_H
#include <Arduino.h>
#include <JsonElement.h>
#include "DisplayInterface.h"
#include "RotaryKnobController.h"
#include "DisplayController.h"
#include "TShirtCannonPayload.h"

class Page : DisplayInterface
{
public:
    enum class PageType
    {
        DASH_PAGE = 0,
        ELEVATOR_PAGE = 1,
        SHOT_PAGE = 2,
        VALVE_PAGE = 3,
        DEBUG_PAGE = 4
    };

    Page(bool isActive, bool applyHang, Page::PageType name);
    void cleanUp(DisplayController &display);
    virtual void paint(DisplayController &display, bool isActivated, TShirtCannonPayload &payload) = 0;
    virtual void clockwise(TShirtCannonPayload &payload) = 0;
    virtual void counterClockwise(TShirtCannonPayload &payload) = 0;
    virtual int rangeFilter(int value) = 0;
    bool canActivate();
    void setNextPage(Page &page);
    void setPreviousPage(Page &page);
    bool applyHang();
    Page::PageType getName();
    Page *getNextPage();
    Page *getPreviousPage();

private:
    bool m_isActive, m_applyHang;
    PageType m_name;
    Page *m_previousPage;
    Page *m_nextPage;
};
#endif