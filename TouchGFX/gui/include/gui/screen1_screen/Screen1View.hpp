#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void leftMouse();
    virtual void rightMouse();
    virtual void moveUp();
    virtual void moveDown();
    virtual void moveRight();
    virtual void moveLeft();
protected:
    int16_t touchStartX = -1;
    int16_t touchStartY = -1;
    bool dragging = false;
};

#endif // SCREEN1VIEW_HPP
