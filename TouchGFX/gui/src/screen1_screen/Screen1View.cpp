#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os.h"
#include <cstdio>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include "usb_device.h"
#include "usbd_hid.h"

uint32_t startTime;

typedef struct {
	uint8_t button;
	int8_t mouse_x;
	int8_t mouse_y;
	int8_t wheel;
} mouseHID;

float scale_x = 1080.0f / 240.0f;
float scale_y = 1920.0f / 320.0f;

extern USBD_HandleTypeDef hUsbDeviceHS;
extern UART_HandleTypeDef huart1;
extern mouseHID mousehid;

Screen1View::Screen1View()
{
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    circlePainter.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));

    // Setup animated circle
    myCircle.setPosition(0, 0, currentRadius * 2, currentRadius * 2);
    myCircle.setCenter(currentRadius, currentRadius);
    myCircle.setRadius(currentRadius);
    myCircle.setLineWidth(0);
    myCircle.setPainter(circlePainter);
    myCircle.setVisible(false);

    add(myCircle);
}

void uartPrint(const char* msg)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void sendMouse(int8_t dx, int8_t dy)
{
    mousehid.button = 0;
    mousehid.mouse_y = -dx * scale_x;
    mousehid.mouse_x = dy * scale_y;
    mousehid.wheel = 0;

//    mousehid.button = 0;
//    mousehid.mouse_y = -dx;
//    mousehid.mouse_x = dy;
//    mousehid.wheel = 0;

    USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t *)&mousehid, sizeof(mousehid));

    mousehid.button = 0;
    mousehid.mouse_x = 0;
    mousehid.mouse_y = 0;
    mousehid.wheel = 0;
}

void Screen1View::handleDragEvent(const DragEvent& evt)
{
	Screen1ViewBase::handleDragEvent(evt);

	int dentaX = evt.getDeltaX();
	int dentaY = evt.getDeltaY();

	if(dentaX <= 1 && dentaX >= -1){
		dentaX = 0;
	}

	if(dentaY <= 1 && dentaY >= -1){
		dentaY = 0;
	}

    char msg[50];
    snprintf(msg, sizeof(msg), "%d, %d\r\n", dentaX, dentaY);
    uartPrint(msg);

    //Gui thong tin
    sendMouse(dentaX, dentaY);
}

void Screen1View::handleClickEvent(const ClickEvent& event)
{
    Screen1ViewBase::handleClickEvent(event);
    currentRadius = 20;
    touch_x = event.getX();
    touch_y = event.getY();

    //Xu ly nhan
    if (event.getType() == ClickEvent::PRESSED)
    {
    	myCircle.setPosition(touch_x - currentRadius, touch_y - currentRadius, currentRadius * 2, currentRadius * 2);
    	myCircle.setCenter(currentRadius, currentRadius);
    	myCircle.setRadius(currentRadius);
    	myCircle.setVisible(true);

    	//Xu ly hien thi
    	isScaling = true;
    	startTime = HAL_GetTick();

    	char msg[50];
    	snprintf(msg, sizeof(msg), "Pressed at: x=%d, y=%d\r\n", touch_x, touch_y);
    	uartPrint(msg);
    }

    //Xu ly tha
    else if(event.getType() == ClickEvent::RELEASED){

        // Đặt circle tại vị trí touch
        myCircle.setPosition(touch_x - currentRadius, touch_y - currentRadius, currentRadius * 2, currentRadius * 2);
        myCircle.setCenter(currentRadius, currentRadius);
        myCircle.setRadius(currentRadius);
        myCircle.setVisible(true);

        isScaling = true;
        startTime = HAL_GetTick();

        char msg[50];
        snprintf(msg, sizeof(msg), "Released at: x=%d, y=%d\r\n", touch_x, touch_y);
        uartPrint(msg);
    }

}

void Screen1View::handleTickEvent()
{

	//Xu ly hinh tron
    if (isScaling){
    	int dentaTick = HAL_GetTick() - startTime;

    	//Xu ly voi denta
    	if(dentaTick <= 1000){
    		int newRadius = currentRadius - currentRadius * dentaTick / 1000;
    		myCircle.setRadius(newRadius);
    		myCircle.invalidate();
    	}
    	else{
    		isScaling = false;
    		int lastTime = HAL_GetTick() - startTime;
    		myCircle.setVisible(false);
    		myCircle.invalidate();

    		char msg1[50];
    		snprintf(msg1, sizeof(msg1), "lastTime = %d\n", lastTime);
    		uartPrint(msg1);
    	}
    }
}


