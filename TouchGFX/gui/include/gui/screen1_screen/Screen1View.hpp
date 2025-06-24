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
    virtual void handleClickEvent(const ClickEvent &event);
    virtual void handleTickEvent();
    virtual void handleDragEvent(const DragEvent &evt);

protected:
    touchgfx::Circle myCircle;
    touchgfx::PainterRGB565 circlePainter;

    int touch_x;
    int touch_y;

    int currentRadius = 20;
    bool isScaling = false;

    static const int SMOOTHING_BUFFER_SIZE = 3;
    int smoothing_x[SMOOTHING_BUFFER_SIZE];
    int smoothing_y[SMOOTHING_BUFFER_SIZE];    
    int smoothing_index = 0;
    uint32_t lastDragTime = 0;
    float velocity_x = 0.0f;
    float velocity_y = 0.0f;
    static constexpr float ACCELERATION_FACTOR = 1.2f;
    static constexpr float DAMPING_FACTOR = 0.7f;
    static constexpr float MIN_VELOCITY_THRESHOLD = 0.1f;

    bool isPotentialClick = false;
    bool isDragging = false;
    uint32_t pressStartTime = 0;
    int press_start_x = 0;
    int press_start_y = 0;
    static constexpr uint32_t CLICK_MAX_DURATION = 500;
    static constexpr int DRAG_THRESHOLD = 8;
    static constexpr uint32_t DRAG_MIN_TIME = 100;

    static const uint32_t CANVAS_BUFFER_SIZE = 3600;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];

private:
    void applySmoothing(int &deltaX, int &deltaY);
    void updateVelocity(int deltaX, int deltaY, uint32_t timeDelta);
    void resetTouchState();
};

#endif // SCREEN1VIEW_HPP
