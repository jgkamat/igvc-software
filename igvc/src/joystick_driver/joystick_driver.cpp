#include <ros/ros.h>
#include <ros/subscriber.h>
#include <ros/publisher.h>
#include <igvc_msgs/velocity_pair.h>
#include <sensor_msgs/Joy.h>

using namespace std;

ros::Publisher cmd_pub;

ros::NodeHandle *nhp;

void joyCallback(const sensor_msgs::Joy::ConstPtr& msg)
{
    double absoluteMaxVel, maxVel, maxVelIncr;
    nhp->param(string("absoluteMaxVel"), absoluteMaxVel, 1.0);
    nhp->param(string("maxVel"), maxVel, 1.0);
    nhp->param(string("maxVelIncr"), maxVelIncr, 0.1);
    
    if(msg->buttons[1])
        maxVel -= maxVelIncr;
    else if(msg->buttons[3])
        maxVel += maxVelIncr;
    maxVel = min(maxVel, absoluteMaxVel);
    maxVel = max(maxVel, 0.0);
    
    nhp->setParam("maxVel", maxVel);
    
    int leftJoyAxis, rightJoyAxis;
    bool leftInverted, rightInverted;
    nhp->param(string("leftAxis"), leftJoyAxis, 1);
    nhp->param(string("rightAxis"), rightJoyAxis, 3);
    nhp->param(string("leftInverted"), leftInverted, false);
    nhp->param(string("rightInverted"), rightInverted, false);
    
    igvc_msgs::velocity_pair cmd;
    cmd.left_velocity = msg->axes[leftJoyAxis]*maxVel * (leftInverted ? -1.0 : 1.0);
    cmd.right_velocity = msg->axes[rightJoyAxis]*maxVel * (rightInverted ? -1.0 : 1.0);
    
    cmd_pub.publish(cmd);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "joystick_driver");
    ros::NodeHandle nh;
    nhp = new ros::NodeHandle("~");
    
    cmd_pub = nh.advertise<igvc_msgs::velocity_pair>("/motors", 1);
    
    ros::Subscriber joy_sub = nh.subscribe("/joy", 1, joyCallback);

    ros::spin();
    
    delete nhp;
    
    return 0;
}
