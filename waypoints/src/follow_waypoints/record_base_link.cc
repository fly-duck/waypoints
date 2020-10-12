#include <ros/ros.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <boost/filesystem/fstream.hpp>
#include "boost/filesystem/operations.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <fstream>


using namespace boost::filesystem;
using int32=std::int32_t;
namespace BenchMark
{



constexpr double Kpi=3.14159265359;

double Cal360(const double &theta) 
{
    return theta*180/Kpi;
}

struct RobotPose
{
    RobotPose():x(0),y(0),theta(0)
    {

    }
    double x;
    double y;
    double theta;
};

class array2D { 
    std::vector<RobotPose> data;
    size_t columns;
public:
    array2D(size_t x, size_t y) : columns(x), data(x*y) {}

    RobotPose &operator()(size_t x, size_t y) {
       return data[y*columns+x];
    }
};
    const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

void processfile()
{
    std::ifstream infile("/home/ecruise2/input.txt");


std::string line;
RobotPose pose;
RobotPose mean_pose;
std::vector<RobotPose> poses;
std::vector<RobotPose> output_poses;
tf::Quaternion q;
q.setX(0);
q.setY(0);
double z=0;
double w=0;

int count=0;
while (std::getline(infile, line))
{
    count++;
    std::istringstream iss(line);
    double a=0,b=0,c=0,d=0;
    
    if (!(iss >> a >> b>>c>>d)) { break; } // error

    pose.x+=a;
    pose.y+=b;
    z+=c;
    w+=d;
    
    if(count==3)
    {
        mean_pose.x=pose.x/count;
        mean_pose.y=pose.y/count;
        q.setZ(z/count);
        q.setW(w/count);
        std::cout<< q.x()<<q.y()<<q.z()<<q.w()<< "\n";
        q.normalize();
        std::cout<< "normalize "<<"\n";
        std::cout<< q.x()<<q.y()<<q.z()<<q.w()<< "\n";
    tf::Matrix3x3 rpy(q);
    double roll,pitch;
    rpy.getRPY(roll,pitch,mean_pose.theta);
    mean_pose.theta=Cal360(mean_pose.theta);
    output_poses.emplace_back(mean_pose);
    pose=RobotPose();
    mean_pose=RobotPose();
    count=0;
    z=0;
    w=0;
        
    }
    
    // process pair (a,b)
}

         std::string filename=currentDateTime()+"initial"+".txt";
          path p{filename};
          ofstream ofs{p};
          for(const auto& ele :output_poses)
          {
              ofs<<ele.x<<" "<<ele.y<<" "<<ele.theta<<"\n";
          }

}

class Recorder
{
public:

    Recorder()
    :private_nh_("~")
    {
    
    private_nh_.param<int32>("click_times",click_times_,3);    
    click_sub_= nh_.subscribe("/clicked_point",1,&Recorder::ClickCallback,this);



    }

    ~Recorder()
    {
          if(!mean_poses_.empty())
          {

          std::string filename=currentDateTime()+".txt";
          path p{filename};
          ofstream ofs{p};
          for(const auto& ele :mean_poses_)
          {
              ofs<<ele.x<<" "<<ele.y<<" "<<ele.theta<<"\n";
          }
          }

    }





    void ClickCallback(const geometry_msgs::PointStampedConstPtr & point)
    {
    tf::StampedTransform transform;
    try{
      listener_.lookupTransform("/map", "/base_link",
                               ros::Time(0), transform);
    }
    catch (tf::TransformException &ex) {
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
     
    }
    std::cout<< "get base_link"<< "\n";
    RobotPose pose;
    pose.x=transform.getOrigin().x();
    pose.y=transform.getOrigin().y();
    tf::Quaternion q=transform.getRotation();
    tf::Matrix3x3 rpy(q);
    double roll,pitch;
    rpy.getRPY(roll,pitch,pose.theta);
    pose.theta=Cal360(pose.theta);
    poses_.emplace_back(pose);
    std::cout<< " stored poses" << poses_.size()<<"\n";
    
    if(poses_.size()==size_t(click_times_))
    {
        RobotPose meanpose;
        for(const auto & ele :poses_)
        {
            meanpose.x+=ele.x;
            meanpose.y+=ele.y;
            meanpose.theta+=ele.theta;
        }
        meanpose.x=meanpose.x/click_times_;
        meanpose.y=meanpose.y/click_times_;
        meanpose.theta=meanpose.theta/click_times_;

        mean_poses_.emplace_back(meanpose);
        poses_.clear();
        ROS_INFO("GET MEAN POSE");
    }

    // double th = tf::getYaw(transform.getRotation());
    
    



    }

private:
    int32 click_times_;

    ros::Subscriber click_sub_;
    ros::NodeHandle nh_;
    ros::NodeHandle private_nh_;
    tf::TransformListener listener_; 
    size_t clicked_times_;
    std::vector<RobotPose> poses_;
    std::vector<RobotPose> mean_poses_;
   


};
}


int main (int argc,char** argv)
{

    ros::init(argc,argv,"recorder");

    BenchMark::Recorder recorder;

    // BenchMark::processfile();
    ros::spin();

}