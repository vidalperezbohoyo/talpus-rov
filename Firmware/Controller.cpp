#include "Controller.hpp"

Controller::Controller()
{

}

void Controller::init()
{
    static Thread control_task;
    control_task.onRun(controlTask);
    control_task.setInterval(100);


    // 
    ThreadController.add(&control_task);
}

void Controller::controlTask()
{
    Command cmd;
    cmd.code = CMD_CONTROL;

    // Convert joysticks to motor thrust
    
    // Horizontal movement
    
    
    // Vertical movement

}

void Controller::lightsTask()
{
    // Check if lights knob has changed

}

void Controller::loop()
{
    // Check "tasks" execution

}