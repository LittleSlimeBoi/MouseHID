#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os.h"
#include <gui/screen1_screen/Screen1View.hpp>
#include <cstdio>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include "usb_device.h"
#include "usbd_hid.h"

uint32_t startTime;
extern "C" {
#include "main.h"
}

typedef struct {
	uint8_t button;
	uint16_t mouse_x;
	uint16_t mouse_y;
	int8_t wheel;
} mouseHID;

float scale_x = 1920.0f / 320.0f;
float scale_y = 1080.0f / 240.0f;

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

    circlePainter.setColor(touchgfx::Color::getColorFromRGB(255, 0, 0));

    // Setup animated circle
    myCircle.setPosition(50, 50, 40, 40);
    myCircle.setCenter(20, 20);
    myCircle.setRadius(20);
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

void Screen1View::handleClickEvent(const ClickEvent& event)
{
    Screen1ViewBase::handleClickEvent(event);

    if (event.getType() == ClickEvent::RELEASED)
    {
        int x = event.getX();
        int y = event.getY();


        //Gui thong tin ra man hinh
        if (hUsbDeviceHS.dev_state == USBD_STATE_CONFIGURED) {
        	mousehid.mouse_x = (uint16_t)(x * scale_x);
        	mousehid.mouse_y = (uint16_t)(y * scale_y);
        	mousehid.wheel = 0;
        	mousehid.button = 0;

        	USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t*)&mousehid, sizeof(mousehid));

        	HAL_Delay(20);
        	char *msg = "Mouse moved\r\n";
        	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

        	mousehid.mouse_x = 0;
        	mousehid.mouse_y = 0;
        }

        // Đặt circle tại vị trí touch
        myCircle.setPosition(x - 50, y - 50, 100, 100);
        myCircle.setCenter(50, 50);
        myCircle.setRadius(50);
        myCircle.setVisible(true);

        // Bắt đầu hiệu ứng thu nhỏ
        isScaling = true;
        startTime = HAL_GetTick();
        scaleStep = 0;
        currentRadius = 50; // Bắt đầu từ 50 pixels

        char msg[50];
        snprintf(msg, sizeof(msg), "Touch at: x=%d, y=%d\r\n", x, y);
        uartPrint(msg);
    }
}

void Screen1View::handleTickEvent()
{
    if (isScaling){

    	scaleStep++;
        if (scaleStep <= 63)
        {
            currentRadius = 50 - (scaleStep * 50 / 63);
            myCircle.setRadius(currentRadius);
            myCircle.invalidate();
        }
        else
        {
            isScaling = false;
            int lastTime = HAL_GetTick() - startTime;
            myCircle.setVisible(false); // Ẩn circle

            char msg1[50];
            snprintf(msg1, sizeof(msg1), "lastTime = %d", lastTime);
            uartPrint(msg1);

        }
    }
}


