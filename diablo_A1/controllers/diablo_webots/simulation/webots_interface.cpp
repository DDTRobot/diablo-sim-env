#include "webots_handler.hpp"


double position_last[MOTOR_NUM] = {0}, position[MOTOR_NUM] = {0}, velocity[MOTOR_NUM] = {0},
       torque[MOTOR_NUM] = {0};
void Get_Robot(void)
{
  getMotorPositon(position);
  getMotorTorque(torque);

  for (int id = 0; id < 8; id++)
  {
    if(id > 0 && id < 4)
    {
      velocity[id-1] = (position[id-1] - position_last[id-1]) / (float)TIME_STEP * 1000.f;
      position_last[id-1] = position[id-1];
    }
    else if(id > 4 && id < 8)
    {
      velocity[id-2] = (position[id-2] - position_last[id-2]) / (float)TIME_STEP * 1000.f;
      position_last[id-2] = position[id-2];      
    }
  }
  const double *qua = getInertialUnit();
  const double *accl = getAccleration();
  const double *gyro = getGyro();
}

void Drive_Robot(void)
{ // here apply motor torque limit
  const float torque_max[8] = {90, 90, 90, 9.5, 90, 90, 90, 9.5};
  const float rpm_max[8] = {180, 180, 180, 520, 180, 180, 180, 520};
  const float torque_limit[8] = {53, 53, 53, 9.5, 53, 53, 53, 9.5};
  float torque_receive[8] = {0};
  float torque_set[MOTOR_NUM] = {0};
  // printf("tau: ");
  double torset[MOTOR_NUM] = {0};
  for (int id = 0; id < 8; id++)
  {
    //Get_Motor_Torque(torque_receive[id], id);//8 motors out
  }
  torque_set[0] = torque_receive[1];
  torque_set[1] = torque_receive[2];
  torque_set[2] = torque_receive[3];
  torque_set[3] = torque_receive[5];
  torque_set[4] = torque_receive[6];
  torque_set[5] = torque_receive[7];
  for (int id = 0; id < MOTOR_NUM; id++)
  {
    setMotorTorqueConstraint(
            torque_set[id], velocity[id] * 60 / (2 * M_PI), torque_max[id],
            rpm_max[id]);
        if (fabs(torque_set[id]) > torque_limit[id])
        {
          torque_set[id] = torque_limit[id] * torque_set[id] / fabs(torque_set[id]);
        }
        torset[id] = torque_set[id];
     
  }
  setMotorTorque(torset);
}



float ax = 10,bx = 70;
int main(int argc, char **argv)
{
  float dt = TIME_STEP / 1000.f;
  float frequency = 30.0;
  float stamp = 0.0f;
  wb_robot_init();
  initDevices();
  std::cout << "----------Sim Start--------" << std::endl;
  while (wb_robot_step(TIME_STEP) != -1)
  {
    stamp += dt;
    Get_Robot();
    if (stamp > 1.0 / frequency)
    {
      stamp = 0.0f;
    }

    Drive_Robot();
  }

  wb_robot_cleanup();
  return 0;
}

