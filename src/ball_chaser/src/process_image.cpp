#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include "sensor_msgs/Image.h"

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z) {
     ROS_INFO_STREAM("Driving the bot towards white ball");

    // Request centered joint angles [1.57, 1.57]
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the ball_chaser service and pass velocities
    if (!client.call(srv))
        ROS_ERROR("Failed to call service ball_chaser");

}


// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int white_pixel = 255;
    // Loop through each pixel in the image and check if its equal to the first one
    //int total lin_img_size = 
    for (int i = 0; i < img.height * img.step; i++) {
        if (img.data[i] == white_pixel) {
            if( i < (int)(img.step * 0.33 + 0.5) ) {
                ROS_INFO("ball is on left side in img!");
                drive_robot(-0.5, 0.5);
            } else if( i < (int)(img.step * 0.66 + 0.5)) {
                ROS_INFO("ball is in the middle in img!");
                drive_robot(0.5, 0.5);
            } else {
                ROS_INFO("ball is on right side in img!");
                drive_robot(0.5, -0.5);
            }
        }
    }
}


int main(int argc, char** argv) {
 // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot    
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}