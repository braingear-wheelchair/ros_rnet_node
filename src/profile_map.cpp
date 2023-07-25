#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>

//enum class Profile {One, Two, Three, Four, Five, Eight};
//static Profile profile = Profile::One;

static int profile = 1;

const std::vector<float> velocities = {0.55, 0.264, 	//One
					0.88, 0.264, 	//Two
					1.32, 0.44,	//Three
					2.20, 0.44,	//Four
					2.20, 0.44,	//Five
					1.76, 0.44,	//Eight
					};

float max_lin_vel = 0.55*4;
float max_ang_vel = 0.27*4;

float vel_min = 0.83; // Velocità minima (in percentuale) rispetto alla mappa

//Speed and pwm limit
float maxVel=1;
int minVal=40;

std_msgs::Int32 x_velocity;
std_msgs::Int32 y_velocity;

geometry_msgs::Twist mapped_velocity;
float linear_vel = 0;
float angular_vel = 0;


void profile_setting(const std_msgs::Int32& p){
	profile = p.data;
	max_lin_vel = velocities[profile*2];
	max_ang_vel = velocities[profile*2+1];
}

bool neg_lin_vel = false;
bool neg_ang_vel = false;

int x = minVal;
int y = minVal;

int map_pwm(float x){ 
	return (int)(minVal*(x-maxVel)/(-maxVel));
}

int map_values_profile(float x, float a1, float a2, float b1, float b2){
	float i=b1+((x-a1)*(b2-b1)/(a2-a1)) ;
	//return map_pwm(i);
	return (int) i;
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
	
	x = map_values_profile(abs(linear_vel), 0, max_lin_vel, minVal,0);
	y = map_values_profile(abs(angular_vel), 0, max_ang_vel, minVal,0);

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
	ros::Subscriber profile_set = nh.subscribe("set_profile", 10, profile_setting);

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
