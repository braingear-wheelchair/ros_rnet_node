#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int32.h>
#include <chrono>
#include<thread>

//Speed and pwm limit
geometry_msgs::Twist velocity;


float max = 1;
float min = 0;

float l = min;
int a = min;

std_msgs::Int32 x_velocity;
std_msgs::Int32 y_velocity;

/*
void forward(){
    velocity.angular.z = min;
    l=0;
    while(l < max){
        velocity.linear.x = l;
        prof_pub.publish(velocity);
        l+=0.01;
        std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));
    }
}
void backward(){
    velocity.angular.z = min;
    l=0;
    while(l > -max){
        velocity.linear.x = l;
        prof_pub.publish(velocity);
        l-=0.01;
        std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));
    }
}
void left(){
    velocity.linear.x = min;
    a=0;
    while(a < max){
        velocity.angular.z = a;
        prof_pub.publish(velocity);
        a+=0.01;
        std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));
    }
}
void right(){
    velocity.linear.x = min;
    a=0;
    while(a > -max){
        velocity.angular.z = l;
        prof_pub.publish(velocity);
        a-=0.01;
        std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));
    }
}
*/

int main(int argc, char** argv){
    ros::init(argc,argv,"profile_test");
    ros::NodeHandle nh;
    
    ros::Publisher prof_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);

	
	x_velocity.data=min;
	y_velocity.data=min;


	ros::Rate rate(50);


//AVANTI ------------------------------------------------------------------------------------------------------------------------------
    /*l=0;
    velocity.linear.x = l;
	std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
    velocity.angular.z = min;

    l=0.6;
    velocity.linear.x = l;
    std::this_thread::sleep_for(std::chrono::nanoseconds(4000000000));
    while(l > 0){
            velocity.linear.x = l;
            std::this_thread::sleep_for(std::chrono::nanoseconds(500000000));
            prof_pub.publish(velocity);
            l-=0.01;
            //std::cout << l << std::endl;
            
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
        velocity.linear.x = 0;
        prof_pub.publish(velocity);*/

//INDIETRO ------------------------------------------------------------------------------------------------------------------------------
    /*l=0;
    velocity.linear.x = l;
	std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
    velocity.angular.z = min;

    l=-0.4;
    velocity.linear.x = l;
    std::this_thread::sleep_for(std::chrono::nanoseconds(4000000000));
    while(l < 0){
            velocity.linear.x = l;
            std::this_thread::sleep_for(std::chrono::nanoseconds(500000000));
            prof_pub.publish(velocity);
            l+=0.01;
            //std::cout << l << std::endl;
            
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
        velocity.linear.x = 0;
        prof_pub.publish(velocity);*/

//SINISTRA ------------------------------------------------------------------------------------------------------------------------------
    l=0;
    velocity.angular.z = l;
	std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
    velocity.linear.x = 0;

    l=0.9;
    velocity.linear.x = l;
    std::this_thread::sleep_for(std::chrono::nanoseconds(4000000000));
    while(l > 0){
            velocity.angular.z = l;
            velocity.linear.x = 0;
            std::this_thread::sleep_for(std::chrono::nanoseconds(500000000));
            prof_pub.publish(velocity);
            l-=0.01;
            //std::cout << l << std::endl;
            
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
        velocity.angular.z = 0;
        velocity.linear.x = 0;
        prof_pub.publish(velocity);
    
//DESTRA ------------------------------------------------------------------------------------------------------------------------------
    l=0;
    velocity.angular.z = l;
	std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
    velocity.linear.x = 0;

    l=-0.9;
    velocity.linear.x = l;
    std::this_thread::sleep_for(std::chrono::nanoseconds(4000000000));
    while(l < 0){
            velocity.angular.z = l;
            velocity.linear.x = 0;
            std::this_thread::sleep_for(std::chrono::nanoseconds(500000000));
            prof_pub.publish(velocity);
            l+=0.01;
            //std::cout << l << std::endl;
            
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(3000000000));
        velocity.angular.z = 0;
        velocity.linear.x = 0;
        prof_pub.publish(velocity);






	while(ros::ok()){
        //forward();
        //backward();
        //left();
        //right();
        
        ros::spinOnce();
		rate.sleep();
	}

	return 0;	
}