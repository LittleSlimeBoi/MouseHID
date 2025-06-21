#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleClickEvent(const ClickEvent& event);
    virtual void handleTickEvent();
protected:
    touchgfx::Circle myCircle;
    touchgfx::PainterRGB565 circlePainter;

    int currentRadius = 20;
    int scaleStep = 0;
    bool isScaling = false;

    static const uint32_t CANVAS_BUFFER_SIZE = 3600;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
};

#endif // SCREEN1VIEW_HPP
