#include "webots_handler.hpp"

template<typename T>
static inline void bound(T& input, const T max, const T min)
{
    if(input > max)       input = max;
    else if(input < min)  input = min;
}

void testGetfield()
{
  WbNodeRef node;
  WbFieldRef field;
  int i;

  wb_robot_init();

  // get the root children field
  const WbNodeRef root_node = wb_supervisor_node_get_root();
  const WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  const int n = wb_supervisor_field_get_count(root_children_field);
  printf("This world contains %d nodes:\n", n);

  // check what type of nodes are present in the world
  for (i = 0; i < n; i++) {
    node = wb_supervisor_field_get_mf_node(root_children_field, i);
    printf("-> %s\n", wb_supervisor_node_get_type_name(node));
  }
  printf("\n");

  // get the content of the 'gravity' field of the 'WorldInfo' node
  node = wb_supervisor_field_get_mf_node(root_children_field, 0);
  field = wb_supervisor_node_get_field(node, "gravity");
  const double gravity = wb_supervisor_field_get_sf_float(field);
  printf("WorldInfo.gravity = %g\n\n", gravity);
}

double getWholebodyMass()
{
    WbNodeRef link_node, physics_node;
    WbFieldRef link_field_physics;
    WbFieldRef physics_mass, physics_com;
    double mass = 0;
    double com_product_mass[3] = {0};
    for(int id = 0; id < LINK_NUM; id++)
    {     
        char physics[100];
        strcpy(physics,LINK_DEF[id]);
        strcat(physics, "_PHYSICS"); 
        physics_node = wb_supervisor_node_get_from_def(physics);
        physics_mass = wb_supervisor_node_get_field(physics_node,"mass");
        const double mass_i = wb_supervisor_field_get_sf_float(physics_mass);
        mass += mass_i;
    }
    #if PRINT_LOG
        std::cout << "whole body mass is " << mass << std::endl;
    #endif
    return mass;
}
// get the link position of its base coordinate in world coordinate
void getGlobalPositon(const char *def, double *pos)
{
    WbNodeRef link;
    link = wb_supervisor_node_get_from_def(def);
    const double *position = wb_supervisor_node_get_position(link);
    for(int id = 0; id < 3; id++)
    {
        pos[id] = position[id];
    }
    #if PRINT_LOG
        std::cout << "link " << def << " position is " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    #endif
}
// get the link position of its base coordinate in robot coordinate
void getRelativePositon(const char *def, double *pos)
{
    double robot_pos[3], link_pos[3], rotation[9];
    getRobotGlobalPositon(robot_pos);
    getGlobalPositon(def, link_pos);
    getRobotGlobalOrientation(rotation);
    calcRelative(robot_pos,link_pos,rotation,pos);
    #if PRINT_LOG
        std::cout << "link " << def << " relative position is " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    #endif
}
// use in supervisor mode ,and get the center of mass position
void getRobotGlobalPositon(double *pos)
{
    WbNodeRef robot;
    robot = wb_supervisor_node_get_from_def(ROBOT_NAME);
    const double *position = wb_supervisor_node_get_position(robot);
    for(int id = 0; id < 3; id++)
    {
        pos[id] = position[id];
    }
    #if PRINT_LOG
        std::cout << "robot position is " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    #endif    
}

void getRobotGlobalOrientation(double *ori)
{
    WbNodeRef robot;
    robot = wb_supervisor_node_get_from_def(ROBOT_NAME);
    const double *rotation = wb_supervisor_node_get_orientation(robot);
    for(int id = 0; id < 9; id++)
    {
        ori[id] = rotation[id];
    }    
}
// zyx sequence eular angle,but eular[0] is x angle, eular[1] is y angle, eular[2] is z angle
void getRobotGlobalEularAngle(double *eular)
{
    double ori[9];
    getRobotGlobalOrientation(ori);
    eular[0] = atan2(ori[7],ori[8]);
    eular[1] = atan2(-ori[6],sqrt(ori[7]*ori[7] + ori[8]*ori[8]));
    eular[2] = atan2(ori[3],ori[0]);
}


void getRobotGlobalWholeBodyComPos(double *com)
{
    WbNodeRef robot;
    robot = wb_supervisor_node_get_from_def(ROBOT_NAME);
    const double *centerofmass = wb_supervisor_node_get_center_of_mass(robot);
    for(int id = 0; id < 3; id++)
    {
        com[id] = centerofmass[id];
    }
    #if PRINT_LOG
        std::cout << "robot center of mass is " << com[0] << " " << com[1] << " " << com[2] << std::endl;
    #endif
}
// relative com calc 
void getRobotRelativeWholeBodyComPos(double *relative_com)
{
   // Computing the image of com by the robot's inverse transform
    double position[3], com[3], rotation[9];
    getRobotGlobalPositon(position);
    getRobotGlobalWholeBodyComPos(com);
    getRobotGlobalOrientation(rotation);
    calcRelative(position,com,rotation,relative_com);
    #if PRINT_LOG
        std::cout << "robot relative center of mass is " << relative_com[0] << " " << relative_com[1] << " " << relative_com[2] << std::endl;
    #endif
}

// without wheel mass
void getRobotRelativeUpperBodyComPos(double *relative_upper_com)
{
    WbNodeRef  link_node, physics_node;
    WbFieldRef link_field_physics;
    WbFieldRef physics_mass, physics_com;
    double mass = 0;
    double com_product_mass[3] = {0};
    const char WHEEL_DEF[2][15] = {"LEFT_WHEEL","RIGHT_WHEEL"};
    for(int id = 0; id < 2; id++)
    {
        link_node = wb_supervisor_node_get_from_def(WHEEL_DEF[id]);
        char physics[100];
        strcpy(physics,WHEEL_DEF[id]);
        strcat(physics, "_PHYSICS"); 
        physics_node = wb_supervisor_node_get_from_def(physics);
     
        physics_mass = wb_supervisor_node_get_field(physics_node,"mass");
        physics_com = wb_supervisor_node_get_field(physics_node,"centerOfMass");
        // for test if node of field is null    
        if(physics_mass == NULL)
        {
            std::cout<< "err in " << id << std::endl; 
        }
        const double mass_i = wb_supervisor_field_get_sf_float(physics_mass);
        const double *com_i = wb_supervisor_node_get_center_of_mass(link_node);
    
        mass += mass_i;
        for(int j = 0; j < 3; j++)
        {
            com_product_mass[j] += mass_i * com_i[j];
        }
    }
    
    double position[3], com[3], rotation[9];
    getRobotGlobalPositon(position);
    getRobotGlobalWholeBodyComPos(com);
    getRobotGlobalOrientation(rotation);
    const double whole_mass = getWholebodyMass();
    double upper_com[3];
    for(int id = 0; id < 3; id++)
    {
        upper_com[id] = (com[id] * whole_mass - com_product_mass[id])/(whole_mass - mass);
    }
    calcRelative(position,upper_com,rotation,relative_upper_com);
    #if PRINT_LOG
        std::cout << "robot relative upper body center of mass is " << relative_upper_com[0] << " " << relative_upper_com[1] << " " << relative_upper_com[2] << std::endl;
    #endif
}

const double* getAccleration()
{
    WbDeviceTag ACCL = wb_robot_get_device(ACCELEROMETER);
    return wb_accelerometer_get_values(ACCL);
}

const double* getGyro()
{
    WbDeviceTag GYRO = wb_robot_get_device(GYROSCOPE);
    return wb_gyro_get_values(GYRO);
}

const double* getInertialUnit()
{
    WbDeviceTag IMU = wb_robot_get_device(INETIA_UNIT);
    return wb_inertial_unit_get_quaternion(IMU);
}
//return all positon by index
void getMotorPositon(double *position)
{
    for(int id = 0; id < 6; id++)
    {
        WbDeviceTag motor = wb_robot_get_device(motors_c[id]);
        WbDeviceTag pos_sensor = wb_motor_get_position_sensor(motor);
        position[id] = wb_position_sensor_get_value(pos_sensor) ;
        position[id] += init_position[id];
    }
} 

void getMotorTorque(double *torque)
{
    for(int id = 0; id < 6; id++)
    {
        WbDeviceTag motor = wb_robot_get_device(motors_c[id]);
        // WbDeviceTag pos_sensor = wb_motor_get_position_sensor(motor);
        torque[id] = wb_motor_get_torque_feedback(motor);
    }
}

void setMotorTorque(double *torque_set)
{ 
    for(int id = 0; id < 6; id++)
    {
        WbDeviceTag motor = wb_robot_get_device(motors_c[id]);
        // if(id == 0 || id == 4)
        //     wb_motor_set_position(motor,0);
        // else
            wb_motor_set_torque(motor,torque_set[id]);
    }   
}

void initDevices()
{
    // 设置仿真时间步长


    WbDeviceTag motor[6]; 
    WbDeviceTag pos_sensor[6];
    for(int id = 0; id < 6; id++)
    {
        motor[id] = wb_robot_get_device(motors_c[id]);
        wb_motor_enable_torque_feedback(motor[id], TIME_STEP);
        pos_sensor[id] = wb_motor_get_position_sensor(motor[id]);
        wb_position_sensor_enable(pos_sensor[id], TIME_STEP);
    }
    WbDeviceTag ACCL = wb_robot_get_device(ACCELEROMETER);
    WbDeviceTag GYRO = wb_robot_get_device(GYROSCOPE);
    WbDeviceTag IMU = wb_robot_get_device(INETIA_UNIT);
    wb_accelerometer_enable(ACCL,TIME_STEP);
    wb_gyro_enable(GYRO,TIME_STEP);
    wb_inertial_unit_enable(IMU,TIME_STEP);
    wb_mouse_enable(TIME_STEP);
    //enable the webots keyboard
    wb_keyboard_enable(TIME_STEP);
}

void setMotorTorqueConstraint(float &torque_set, const float rpm_now, const float torque_limit, const float rpm_limit)
{
    float b = rpm_limit;
    float k = -b / torque_limit;
    float maxtorque = (rpm_now - b) / k;
    float mintorque = (rpm_now + b) / k;
    bound(maxtorque, torque_limit, 0.0f);
    bound(mintorque, 0.0f, -torque_limit);
    bound(torque_set, maxtorque, mintorque);
}


// privite
static void calcRelative(const double *base, const double *target,  const double *rotation, double *relative)
{
    const double delta[3] = {target[0] - base[0], target[1] - base[1], target[2] - base[2]};
    for (int i = 0; i < 3; ++i)
    {
        double s = 0.0;
        for (int j = 0; j < 3; ++j)
        {
            const int k = 3 * j;
            s += rotation[k + i] * delta[j];
        }
        relative[i] = s;
    }
}