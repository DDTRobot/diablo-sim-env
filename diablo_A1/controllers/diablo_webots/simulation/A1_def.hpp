#ifndef ROBOT_DEF_HPP
#define ROBOT_DEF_HPP
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/position_sensor.h>
#include <webots/inertial_unit.h>
#include <webots/accelerometer.h>
#include <webots/touch_sensor.h>
#include <webots/keyboard.h>
#include <webots/mouse.h>
#include <webots/joystick.h>
#include <webots/gyro.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath> 
#include <unistd.h>


#define USE_RVIZ 1
// #define USE_GNU_PLOT 0
#define TIME_STEP 5

#define ROBOT_NAME "DIABLO"  // position of the robot node with respect to root node children
#define ACCELEROMETER "accl"
#define GYROSCOPE "gyro"
#define INETIA_UNIT "imu"
#define LEFT_WHEEL "LEFT_WHEEL"
#define RIGHT_WHEEL "RIGHT_WHEEL"
#define USE_CLOSECHAIN 0


#define MOTOR_NUM 6
#define LINK_NUM 9U

const double init_position[MOTOR_NUM] = { 1.43, 2.87f, 0.0, 1.43, 2.87f, 0.0};
#endif
const char LINK_DEF[LINK_NUM][15] = {ROBOT_NAME,"LEFT_HIP","LEFT_THIGH","LEFT_CALF","LEFT_WHEEL","RIGHT_HIP","RIGHT_THIGH","RIGHT_CALF","RIGHT_WHEEL"};
const char motors_c[MOTOR_NUM][40] = {"left_j4", "left_j1", "left_j3",  "right_j4", "right_j1", "right_j3"};
const char pos_c[MOTOR_NUM][40] = { "left_j4_sensor", "left_j1_sensor", "left_j3_sensor", "right_j4_sensor", "right_j1_sensor", "right_j3_sensor"};

const float torque_max[MOTOR_NUM] = { 90, 20, 90, 90, 90, 20};
const float rpm_max[MOTOR_NUM] = {180,1000,180,180,180,1000};
const float torque_limit[MOTOR_NUM] = {53, 10, 53, 53, 53, 10};
