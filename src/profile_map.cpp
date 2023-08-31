#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>
#include <math.h>

//enum class Profile {One, Two, Three, Four, Five, Eight};
//static Profile profile = Profile::One;

//static int profile = 1;
/*
const std::vector<float> velocities = {0.55, 0.264, 	//One
					0.88, 0.264, 	//Two
					1.32, 0.44,	//Three
					2.20, 0.44,	//Four
					2.20, 0.44,	//Five
					1.76, 0.44,	//Eight
					};*/

float max_lin_vel = 0.55*4;
float max_ang_vel = 0.27*4;

float vel_min = 0.83; // Velocità minima (in percentuale) rispetto alla mappa

//Speed and pwm limit
float maxVel=1;
int minVal=255;

std_msgs::Int32 x_velocity;
std_msgs::Int32 y_velocity;

geometry_msgs::Twist mapped_velocity;
float linear_vel = 0;
float angular_vel = 0;

/*
void profile_setting(const std_msgs::Int32& p){
	profile = p.data;
	max_lin_vel = velocities[profile*2];
	max_ang_vel = velocities[profile*2+1];
}*/

bool neg_lin_vel = false;
bool neg_ang_vel = false;

int x = minVal;
int y = minVal;
/*
int map_pwm(float x){ 
	return (int)(minVal*(x-maxVel)/(-maxVel));
}

int map_values_profile(float x, float a1, float a2, float b1, float b2){
	float i=b1+((x-a1)*(b2-b1)/(a2-a1)) ;
	//return map_pwm(i);
	return (int) i;
}*/

int map_values_linear(float v){
	if(!neg_lin_vel){	//avanti
		if(v>=0.56) return maxVel;
		else if(v==0) return minVal;
		return (int) 1 + 27.795 + 0.000171168* sqrt(2.15845* pow(10, 9) - 3.373* pow(10,9)*v);
	}
	else{				//indietro
		if(v>=0.27) return maxVel;
		else if(v==0) return minVal;
		return (int) 362.096 -1 - 0.000973882 * sqrt(2.29604*pow(10,10)*v + 1.09047*pow(10,11));
	}
}
int map_values_angular(float v){
	if(!neg_ang_vel){	//sinistra
		if(v>=0.81) return maxVel;
		else if(v==0) return minVal;
		return (int) 777.297 - 0.000798868 * sqrt(3.23206 * pow(10,10) * v + 8.52686 * pow(10,11));
	}
	else{			
		if(v>=0.74) return maxVel;
		else if(v==0) return minVal;
		return (int) 62.5333-0.000383892 * sqrt(5.2098*pow(10,9)*v + 4.7583* pow(10,9));
	}
}



void callback(const geometry_msgs::Twist& velocity){
	linear_vel = velocity.linear.x;
	angular_vel = velocity.angular.z;
	
	if(linear_vel > max_lin_vel)
		linear_vel = max_lin_vel;
	if(linear_vel < -max_lin_vel)
		linear_vel = -max_lin_vel;
		
	if(angular_vel > max_ang_vel)
		angular_vel = max_ang_vel;
	if(angular_vel < -max_ang_vel)
		angular_vel = -max_ang_vel;
	
	if(linear_vel < 0)
		neg_lin_vel = true;
	else
		neg_lin_vel = false;
		
	if(angular_vel < 0)
		neg_ang_vel = true;
	else
		neg_ang_vel = false;
	
	//mappa da [-0.55,0.55] a [0,1]
	//mappa da [-1,1] a [x,1] -> x il valore che poi sarà 40 in arduino = 0.843
	//x = map_values_profile(abs(linear_vel), 0, max_lin_vel, vel_min,1);
	//y = map_values_profile(abs(angular_vel), 0, max_ang_vel, vel_min,1) + 1;
	
	//x = map_values_profile(abs(linear_vel), 0, max_lin_vel, minVal,0);
	//y = map_values_profile(abs(angular_vel), 0, max_ang_vel, minVal,0);
	
	x = map_values_linear(abs(linear_vel));
	y = map_values_angular(abs(angular_vel));

	//se negative le velocità in input torna a valori negativi
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
	//ros::Subscriber profile_set = nh.subscribe("set_profile", 10, profile_setting);

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
