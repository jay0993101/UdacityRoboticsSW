#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_my_robot(float lin_x, float ang_z)
{ 
     ROS_INFO_STREAM("Driving the bot");  
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    if (!client.call(srv))
        ROS_ERROR("Failed to call service /ball_chaser/command_robot");
    
}
           
void process_image_callback(const sensor_msgs::Image img)
{
    int index = 0;
    int position = -1;
    bool white_ball = false;
    int white_pixel = 255;
    int left_pos = img.width / 3;
    int center_end = 2 * img.width / 3;
    int array_size = img.width * img.height * 3;
    
    
     for (int i = 0; i < (img.height * img.step); i+=3)
    {
        if (img.data[i] == white_pixel && 
            img.data[i+1] == white_pixel && 
            img.data[i+2] == white_pixel)
        {
            white_ball = true;
          	index = int(i/3) % img.width;
            break;
        }
    }
    
   if( white_ball)
   {
        if (index < left_pos)
        {
           drive_my_robot(0.8, 0.45); 
        }
        else if (index >= left_pos && index < center_end)
        {
            drive_my_robot(0.45, 0.0); 
        }
        else if (index >= center_end && index < img.width)
        {
            drive_my_robot(0.8, -0.45); 
        }
          
   }
   else {
    drive_my_robot(0.0, 0.9); 
   }  
}
         
int main(int argc, char** argv)
{
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();

    return 0;
}