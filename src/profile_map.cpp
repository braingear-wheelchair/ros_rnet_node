#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int32.h>
#include <math.h>

//Speed and pwm limit
float maxVel=1;
int minVal=255;

std_msgs::Int32 x_velocity;
std_msgs::Int32 y_velocity;

geometry_msgs::Twist mapped_velocity;
float linear_vel = 0;
float angular_vel = 0;


bool neg_lin_vel = false;
bool neg_ang_vel = false;

int x = minVal;
int y = minVal;

int map_values_linear(float x){
	if(!neg_lin_vel){	//avanti
		if(x>=0.56) return maxVel;
		else if(x==0) return minVal;

		if(x <= 0.38)
			return round(36.6667 - 0.235702*sqrt(600*x+11));
		else
			return round(30.25 + 0.25*sqrt(449-800*x));
	}
	else{				//indietro
		if(x>=0.27) return maxVel;
		else if(x==0) return minVal;
		
		if(x <= 0.13)
			return round(17.5353 + 0.000137255*sqrt(2.7246*pow(10,10) - 7.28571*pow(10,10)*x));
		else
			return round(29.4999 + 0.000177087*sqrt(2.49005*pow(10,9) - 8.92858*pow(10,9)*x));
	}
}
int map_values_angular(float x){
	if(!neg_ang_vel){	//sinistra
		if(x>=0.806) return maxVel;
		else if(x==0) return minVal;
		
		if(x <= 0.73) 
			return round(41.8148 - 0.0000734499*sqrt(2.72294*pow(10,10)*x + 9.99985*pow(10,8)));
		else
			return round(27.6 + 0.0000347833*sqrt(1.03944*pow(10,11) - 1.28571*pow(10,11)*x));
	}
	else{			
		if(x>=0.74) return maxVel;
		else if(x==0) return minVal;
		
		if(x <= 0.34)
			return round(44.5203 - 0.000216842*sqrt(7.29168*pow(10,9)*x + 9.03179*pow(10,8)));
		else
			return round(27.9062 + 0.0000978282*sqrt(3.41492*pow(10,9) - 4.57142*pow(10,9)*x));
	}
}


void callback(const geometry_msgs::Twist& velocity){
	linear_vel = velocity.linear.x;
	angular_vel = velocity.angular.z;
	
	if(linear_vel < 0)
		neg_lin_vel = true;
	else
		neg_lin_vel = false;
		
	if(angular_vel < 0)
		neg_ang_vel = true;
	else
		neg_ang_vel = false;

	x = map_values_linear(abs(linear_vel));
	y = map_values_angular(abs(angular_vel));

	if(neg_lin_vel)
		x *=-1;
	if(neg_ang_vel)
		y *= -1; 

	x_velocity.data = x ;
	y_velocity.data = y;
	
}

int main(int argc, char** argv){
	ros::init(argc,argv,"profile_mapping");
	ros::NodeHandle nh;
	ros::Subscriber sub = nh.subscribe("cmd_vel", 10, callback);

	ros::Publisher pub_l = nh.advertise<std_msgs::Int32>("arduino_linear", 1);
	ros::Publisher pub_a = nh.advertise<std_msgs::Int32>("arduino_angular", 1);
	
	x_velocity.data=minVal;
	y_velocity.data=minVal;
	
	ros::Rate rate(50);
	
	while(ros::ok()){
		pub_l.publish(x_velocity);
		pub_a.publish(y_velocity);

		ros::spinOnce();
		rate.sleep();
	}

	return 0;	
}
