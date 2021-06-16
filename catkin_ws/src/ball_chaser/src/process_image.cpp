#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <cmath>

using namespace std;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
	ROS_INFO_STREAM("Driving Robot");
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

	if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

	int white_pixel = 255;
	int height = img.height;
	int width = img.width;
	int step = img.step;
	int dummy;
	ROS_INFO_STREAM(img.data[320000]);
	ROS_INFO_STREAM(img.data[320001]);
	ROS_INFO_STREAM(img.data[320002]);
	ROS_INFO_STREAM("Image Processing");

	for(int j = 0; j < height * step; j = j+3){
		ROS_INFO_STREAM("For Loop");
		if(img.data[j] == white_pixel && img.data[j+1] == white_pixel && img.data[j+2] == white_pixel){
			dummy = (j%2400);

			if(dummy < 800){
			drive_robot(3,1);
			}else if(dummy >=800 && dummy < 1600){
			drive_robot(3,0);
			}else{
			drive_robot(3,-1);
			}

			ros::Duration(3).sleep();
			ROS_INFO_STREAM("Ball detected.");
			break;
		}else{
			drive_robot(0,0);
			break;
		}
	}
	ros::Duration(3).sleep();
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/DriveToTarget");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
