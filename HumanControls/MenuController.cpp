#include "MenuController.h"

MenuController::MenuController(unsigned int encoderPinA,
                               unsigned int encoderPinB,
                               unsigned int angleIncrement,
                               unsigned int angleMin,
                               unsigned int angleMax,
                               unsigned int pressureIncrement,
                               unsigned int pressureMin,
                               unsigned int pressureMax,
                               unsigned int durationIncrement,
                               unsigned int durationMin,
                               unsigned int durationMax,
                               unsigned int hangTimerDuration)
    : m_rotaryKnob(encoderPinA, encoderPinB), m_display(),
      m_dashPage(),
      m_elevatorPage(angleIncrement, angleMin, angleMax),
      m_shotPage(pressureIncrement, pressureMin, pressureMax),
      m_valvePage(durationIncrement, durationMin, durationMax),
      m_debugPage()
{
    this->m_isActive = false;

    this->m_rotation = 0;

    // Set previous pages
    m_dashPage.setPreviousPage(m_debugPage);
    m_elevatorPage.setPreviousPage(m_dashPage);
    m_shotPage.setPreviousPage(m_elevatorPage);
    m_valvePage.setPreviousPage(m_shotPage);
    m_debugPage.setPreviousPage(m_valvePage);

    // Set next pages
    m_dashPage.setNextPage(m_elevatorPage);
    m_elevatorPage.setNextPage(m_shotPage);
    m_shotPage.setNextPage(m_valvePage);
    m_valvePage.setNextPage(m_debugPage);
    m_debugPage.setNextPage(m_dashPage);

    m_currentPage = &m_dashPage;

    this->m_hangTimerDuration = hangTimerDuration;
    this->m_time = millis();
}

void MenuController::init(TShirtCannonPayload &payload)
{
    this->m_display.init();
    payload.setFiringTime(this->m_valvePage.rangeFilter(payload.getFiringTime()));
    payload.setAngle(this->m_elevatorPage.rangeFilter(payload.getAngle()));
    payload.setFiringPressure(this->m_shotPage.rangeFilter(payload.getFiringPressure()));
    this->m_currentPage->paint(m_display, this->m_isActive, payload);
}

void MenuController::menuRefresh(TShirtCannonPayload &payload)
{
    if (this->m_currentPage->getName() != this->m_dashPage.getName())
    {
        m_isActive = false;
    }
    this->m_currentPage->paint(m_display, m_isActive, payload);
}

void MenuController::menuUpdate(TShirtCannonPayload &payload, bool isEnabled)
{
    this->m_rotation = this->m_rotaryKnob.getValue();

    if (m_isActive && isEnabled)
    {
        if (this->m_rotation == 1)
        {
            this->m_currentPage->clockwise(payload);
        }

        if (this->m_rotation == -1)
        {
            this->m_currentPage->counterClockwise(payload);
        }

        if (this->m_rotation != 0)
        {
            this->m_currentPage->paint(m_display, m_isActive, payload);
        }
    }
    else
    {
        if (this->m_rotation == 1)
        {
            this->m_currentPage = this->m_currentPage->getNextPage();
        }

        if (this->m_rotation == -1)
        {
            this->m_currentPage = this->m_currentPage->getPreviousPage();
        }
    }

    if (this->m_rotation != 0 && payload.getStatus() == Utils::ControllerStatus::DISABLED)
    {
        this->m_time = millis();
        this->m_currentPage->cleanUp(m_display);
        this->m_currentPage->paint(m_display, m_isActive, payload);
    }

    if (this->m_currentPage->applyHang() && millis() > (this->m_time + this->m_hangTimerDuration))
    {
        this->m_currentPage = &m_dashPage;
        this->m_isActive = false;
        this->m_currentPage->cleanUp(m_display);
        this->m_currentPage->paint(m_display, m_isActive, payload);
    }

    m_display.print();
}

void MenuController::menuPress(TShirtCannonPayload &payload, bool isEnabled)
{
    if ((isEnabled && m_currentPage->canActivate()) || this->m_currentPage->getName() == this->m_dashPage.getName())
    {
        this->m_isActive = !this->m_isActive;
        this->m_time = millis();
    }
    else
    {
        this->m_currentPage = &m_dashPage;
        this->m_isActive = false;
        this->m_currentPage->cleanUp(m_display);
        this->m_currentPage->paint(m_display, m_isActive, payload);
    }
    this->m_currentPage->paint(m_display, m_isActive, payload);
}