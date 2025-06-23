#include <gui/screen1_screen/Screen1View.hpp>
#include "cmsis_os.h"
#include <cstdio>
#include <cmath>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include "usb_device.h"
#include "main.h"

uint32_t startTime;

extern USBD_HandleTypeDef hUsbDeviceHS;
extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t mouseEventQueueHandle;
extern osThreadId_t mouseTaskHandle;

Screen1View::Screen1View()
{
    for(int i = 0; i < SMOOTHING_BUFFER_SIZE; i++) {
        smoothing_x[i] = 0;
        smoothing_y[i] = 0;
    }
    smoothing_index = 0;
    lastDragTime = 0;
    velocity_x = 0.0f;
    velocity_y = 0.0f;
    
    isPotentialClick = false;
    isDragging = false;
    pressStartTime = 0;
    press_start_x = 0;
    press_start_y = 0;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    circlePainter.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));

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
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 10);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
    
    if(mouseTaskHandle != NULL) {
        osThreadTerminate(mouseTaskHandle);
        mouseTaskHandle = NULL;
    }
    
    if(mouseEventQueueHandle != NULL) {
        osMessageQueueDelete(mouseEventQueueHandle);
        mouseEventQueueHandle = NULL;
    }
}

void Screen1View::applySmoothing(int& deltaX, int& deltaY)
{
    smoothing_x[smoothing_index] = deltaX;
    smoothing_y[smoothing_index] = deltaY;
    smoothing_index = (smoothing_index + 1) % SMOOTHING_BUFFER_SIZE;
    
    int sum_x = 0, sum_y = 0;
    for(int i = 0; i < SMOOTHING_BUFFER_SIZE; i++) {
        sum_x += smoothing_x[i];
        sum_y += smoothing_y[i];
    }
    
    deltaX = sum_x / SMOOTHING_BUFFER_SIZE;
    deltaY = sum_y / SMOOTHING_BUFFER_SIZE;
}

void Screen1View::resetTouchState()
{
    isPotentialClick = false;
    isDragging = false;
    velocity_x = 0.0f;
    velocity_y = 0.0f;
    
    for(int i = 0; i < SMOOTHING_BUFFER_SIZE; i++) {
        smoothing_x[i] = 0;
        smoothing_y[i] = 0;
    }
    smoothing_index = 0;
    
    lastDragTime = 0;
    pressStartTime = 0;
}

void Screen1View::updateVelocity(int deltaX, int deltaY, uint32_t timeDelta)
{
    if(timeDelta > 0) {
        float current_vel_x = (float)deltaX / timeDelta * 1000.0f;
        float current_vel_y = (float)deltaY / timeDelta * 1000.0f;
        
        if((velocity_x > 0 && current_vel_x > 0) || (velocity_x < 0 && current_vel_x < 0)) {
            velocity_x = velocity_x * DAMPING_FACTOR + current_vel_x * (1 - DAMPING_FACTOR);
            if(abs(velocity_x) > abs(current_vel_x)) {
                velocity_x *= ACCELERATION_FACTOR;
            }
        } else {
            velocity_x = current_vel_x;
        }
        
        if((velocity_y > 0 && current_vel_y > 0) || (velocity_y < 0 && current_vel_y < 0)) {
            velocity_y = velocity_y * DAMPING_FACTOR + current_vel_y * (1 - DAMPING_FACTOR);
            if(abs(velocity_y) > abs(current_vel_y)) {
                velocity_y *= ACCELERATION_FACTOR;
            }
        } else {
            velocity_y = current_vel_y;
        }
        
        if(abs(velocity_x) < MIN_VELOCITY_THRESHOLD) velocity_x = 0;
        if(abs(velocity_y) < MIN_VELOCITY_THRESHOLD) velocity_y = 0;
    }
}

void Screen1View::handleDragEvent(const DragEvent& evt)
{
	Screen1ViewBase::handleDragEvent(evt);

	int deltaX = evt.getDeltaX();
	int deltaY = evt.getDeltaY();
	
	uint32_t currentTime = HAL_GetTick();
	uint32_t timeDelta = currentTime - lastDragTime;
	lastDragTime = currentTime;	if(isPotentialClick && !isDragging) {
		uint32_t timeSincePress = currentTime - pressStartTime;
		int totalMovement = abs(evt.getNewX() - press_start_x) + abs(evt.getNewY() - press_start_y);
		
		if(timeSincePress >= DRAG_MIN_TIME && totalMovement >= DRAG_THRESHOLD) {
			isDragging = true;
			isPotentialClick = false;
			
			char msg[50];
			snprintf(msg, sizeof(msg), "Drag started: time=%lums, move=%dpx\r\n", timeSincePress, totalMovement);
			uartPrint(msg);
		}
		else {
			return;
		}
	}
	
	if(!isDragging) {
		return;
	}

	applySmoothing(deltaX, deltaY);
	
	updateVelocity(deltaX, deltaY, timeDelta);

	if(abs(deltaX) <= 0 && abs(deltaY) <= 0){
		return;
	}
	if(timeDelta < 50) {
		deltaX = (int)(deltaX * 1.1f);
		deltaY = (int)(deltaY * 1.1f);
	}

	char msg[50];
	snprintf(msg, sizeof(msg), "Drag: %d, %d\r\n", deltaX, deltaY);
	uartPrint(msg);

	//Gui thong tin
	enqueueMouseEvent(deltaX, deltaY, 0);
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
    	resetTouchState();
    	
    	lastDragTime = HAL_GetTick();
    	isPotentialClick = true;
    	isDragging = false;
    	pressStartTime = HAL_GetTick();
    	press_start_x = touch_x;
    	press_start_y = touch_y;
    	
    	myCircle.setPosition(touch_x - currentRadius, touch_y - currentRadius, currentRadius * 2, currentRadius * 2);
    	myCircle.setCenter(currentRadius, currentRadius);
    	myCircle.setRadius(currentRadius);
    	myCircle.setVisible(true);

    	//Xu ly hien thi
    	isScaling = true;
    	startTime = HAL_GetTick();

    	char msg[50];
    	snprintf(msg, sizeof(msg), "Touch started at: x=%d, y=%d\r\n", touch_x, touch_y);
    	uartPrint(msg);
    }    
    else if(event.getType() == ClickEvent::RELEASED)
    {
        uint32_t pressDuration = HAL_GetTick() - pressStartTime;
        int totalMovement = abs(touch_x - press_start_x) + abs(touch_y - press_start_y);
        if(isPotentialClick && !isDragging && 
           pressDuration <= CLICK_MAX_DURATION && 
           totalMovement < DRAG_THRESHOLD)        {
            char msg[50];
            snprintf(msg, sizeof(msg), "Click detected: duration=%lums, movement=%dpx\r\n", pressDuration, totalMovement);
            uartPrint(msg);
            
            enqueueMouseEvent(0, 0, 1);
            enqueueMouseEvent(0, 0, 2);
        }        else if(isDragging)
        {
            char msg[50];
            snprintf(msg, sizeof(msg), "Drag ended: duration=%lums, movement=%dpx\r\n", pressDuration, totalMovement);
            uartPrint(msg);
        }
        else
        {
            char msg[50];
            snprintf(msg, sizeof(msg), "Gesture cancelled: duration=%lums, movement=%dpx\r\n", pressDuration, totalMovement);
            uartPrint(msg);
        }
        resetTouchState();

        // Đặt circle tại vị trí touch
        myCircle.setPosition(touch_x - currentRadius, touch_y - currentRadius, currentRadius * 2, currentRadius * 2);
        myCircle.setCenter(currentRadius, currentRadius);
        myCircle.setRadius(currentRadius);
        myCircle.setVisible(true);

        isScaling = true;
        startTime = HAL_GetTick();
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
    		snprintf(msg1, sizeof(msg1), "lastTime = %lu\n", lastTime);
    		uartPrint(msg1);
    	}
    }
}


