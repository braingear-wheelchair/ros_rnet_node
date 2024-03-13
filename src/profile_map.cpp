#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int32.h>
#include <math.h>

//Speed and pwm limit
float maxVel=1;
int minVal=255;

std_msgs::Int32 x_velocity;
std_msgs::Int32 y_velocity;

// TODO: add a pid with the odometry

std::vector<float> vel_x = {0.02, 0.15, 0.21, 0.40, 0.56};
std::vector<int>   cmd_x = {255, 40, 39, 38, 37, 36};

std::vector<float> vel_xn = {0.01, 0.03, 0.06, 0.10, 0.14, 0.15, 0.26};
std::vector<int>   cmd_xn = {255, 45, 44, 43, 42, 41, 40, 17};

geometry_msgs::Twist mapped_velocity;
float linear_vel = 0;
float angular_vel = 0;


bool neg_lin_vel = false;
bool neg_ang_vel = false;

int x = minVal;
int y = minVal;

int index_x = 0;

int map_values_linear(float x){
	index_x = 0;
	if(!neg_lin_vel){	//avanti
		while (x > vel_x[index_x])
			index_x++;
		return cmd_x[index_x];
	}
	else{		
		while (x > vel_xn[index_x])
			index_x++;
		return cmd_xn[index_x];
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
