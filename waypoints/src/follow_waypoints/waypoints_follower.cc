// ros msg
#include <std_msgs/Empty.h> 
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/Point32.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <geometry_msgs/PointStamped.h>
// actionlib
#include <actionlib/server/simple_action_server.h>
// #include <actionlib/client/simple_action_client.h>
#include <follow_waypoints/client.h>

// visualization
#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/Marker.h>



#include <ros/publisher.h>
#include <ros/ros.h>
// standard library 
#include <queue>
#include <ios>
#include <type_traits>
#include <thread>
#include <boost/thread/mutex.hpp> 



namespace Follow_Waypoints
{
    // using flags = std::ios_base::fmtflags;
   using int32 = std::int32_t;

class FollowPath
{
public:
    FollowPath()
    :move_client_("move_base",true),
     nh_()
    {
    nh_.param<std::string>("frame_id",frame_id_,"map");
        // std::cout<< " hsdhfskd" <<"\n";
    ROS_INFO("connecting!!");
    // if(move_client_.waitForServer())
        // move_client_.sendGoal()
    
    ROS_INFO("connected");


    }
private:



ros::NodeHandle nh_;
std::string frame_id_;
actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_client_;
// actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_client_;

std::queue<geometry_msgs::PoseWithCovariance> waypoints_;

};


class GetPath{


public: 
GetPath(std::string name)
    :
    // move_server_(nh_,name,boost::bind(&GetPath::executecb,this,_1),false),
    move_client_("move_base",true),ids(0),private_nh_("~")
{
    
    path_pub_=nh_.advertise<geometry_msgs::PoseArray>("waypoints",1);
    
    // geometry_msgs::PoseWithCovariance msg =*(ros::topic::waitForMessage<geometry_msgs::PoseWithCovariance>("/path_ready",ros::Duration(10)));
    // std::cout<< msg.pose.position.x <<"\n";
    private_nh_.param<int32>("num_waypoints",num_waypoints_,2);
    arrows_pub_=nh_.advertise<visualization_msgs::MarkerArray>("WaypointsViusalization",10);
    single_arrow_pub_=nh_.advertise<visualization_msgs::Marker>("SingleArrow",10);
    // arrow_timer_=nh_.createWallTimer(ros::WallDuration(0.1),boost::bind(&GetPath::Marker_Publish,this));
    sendgoal_=false;
    triger_num_=0;
    triger_=false;
    pose_sub_= nh_.subscribe("/initialpose",1,&GetPath::wapointscb,this);
    triger_sub_=nh_.subscribe("/flag",1,&GetPath::ProcessTriger,this);

            // if(move_client_.waitForServer())
    

 

    // path_pub_.publish("waypoints",)
}
~GetPath()
{
    
}


void CheckResult(bool* sendgoal)
{
        if(move_client_.waitForResult())
    {
            ROS_INFO("The Robot has arrived at the waypoint successfully");
            *sendgoal=true;
    }
    else 
    { 
        std::cout << " still moving on " << "\n" ;
    }

}


void ProcessTriger (const geometry_msgs::PointStampedConstPtr & triger_msg)
 
{
    boost::mutex::scoped_lock(flag_mutex_);
    // to do : set a plane to allow navigation  
    // will cause race condition 
    // std::cout<< triger_num_<<"number"<< "\n";
    if(++triger_num_&&triger_num_%2==1)
    {
        ROS_INFO("Waypoints has been trigged!");
        // std::cout<< "it has been triger"<<"\n";
        triger_=true;
    }
    else 
    {
        triger_=false;
        ROS_INFO("Waypoints deactivated!");
    }
        if(triger_)
    {
        sendgoal_=true;
    }
       else
       {
           sendgoal_=false;
           move_client_.cancelGoal();
           ROS_INFO("Cancle goalls !");
       }


    
}

void executecb(const move_base_msgs::MoveBaseActionConstPtr& goal)
{

}

void Marker_Publish()
{
    if(!arrows_.empty())
    {
        visualization_msgs::MarkerArray markers;
        markers.markers.reserve(arrows_.size());
        for(size_t i=0; i<arrows_.size();i++)
        {
            // arrows_[i].header.stamp=ros::Time::now();
            markers.markers.push_back(arrows_[i]);                // marker.header=poses_[i].


        }
        std::cout<<"arrow size: " << arrows_.size()<<"\n";
        std::cout<< "markers size" << markers.markers.size() << " \n" ;
        
        arrows_pub_.publish(markers);


    }

}

void wapointscb(const geometry_msgs::PoseWithCovarianceStampedConstPtr& point) 
 
{
    // std::cout<< " receiving " << "\n";

    poses_.push(*point);

    visualization_msgs::Marker arrow;
    
    arrow.ns="waypoint";
    arrow.header=point->header;
    arrow.pose=point->pose.pose;
    arrow.id=ids;
    arrow.type=visualization_msgs::Marker::ARROW;
    arrow.action=visualization_msgs::Marker::ADD;
    arrow.scale.x = 1;
    arrow.scale.y = 0.1;
    arrow.scale.z = 0.1;
    arrow.color.a = 1.0; // Don't forget to set the alpha!
    arrow.color.r = 0.0;
    arrow.color.g = 1.0;
    arrow.color.b = 0.0;    

    line_strip.ns="waypoint";
    line_strip.header=point->header;
    // line_strip.pose=point->pose.pose;
    line_strip.pose.orientation.w=1.0;
    line_strip.id=10;
    line_strip.type=visualization_msgs::Marker::LINE_STRIP;
    line_strip.action=visualization_msgs::Marker::ADD;
    // line_strip.scale.x = 1;
    line_strip.scale.x = 0.1;
    // line_strip.scale.y = 0.1;
    // line_strip.scale.z = 0.1;
    // line_strip.color.a = 1.0; // Don't forget to set the alpha!
    // line_strip.color.r = 0.0;
    // line_strip.color.g = 1.0;
    // line_strip.color.b = 0.0;  
    line_strip.color.b = 1.0;
    line_strip.color.a = 1.0;

    ROS_INFO_STREAM("Waypoint number "<< ids+1);
    ids++;
    geometry_msgs::Point p;
    p.x=point->pose.pose.position.x;
    p.y=point->pose.pose.position.y;
    p.z=point->pose.pose.position.z;

    line_strip.points.push_back(p);
    arrows_.push_back(arrow);
    // ugly and not neccessary  --- use single arrow instead 
    Marker_Publish();
    single_arrow_pub_.publish(line_strip);
    // line_strip.points.

    // single_arrow_pub_.publish(arrow);

    // sendgoal_=move_client.waitForResult(ros::Duration(30));
    // if (poses_.size()>num_waypoints_)



}

void run () 
{
    ros::Rate loop(20);
            while(ros::ok())
    {
        // std::cout<< "hdsfa"<< "\n";
    if(sendgoal_&&!poses_.empty())
    {
        geometry_msgs::PoseWithCovarianceStamped msg =poses_.front();
        poses_.pop();
        // sendgoal_=false;
        move_base_msgs::MoveBaseGoal goal ;
        goal.target_pose.header=msg.header;
        // goal.target_pose.header.frame_id="/map";
        goal.target_pose.pose=msg.pose.pose;
        move_client_.sendGoal(goal);
        sendgoal_=false;



    std::thread t(&GetPath::CheckResult,this,&sendgoal_);
    // t.join();
    t.join();
    // ROS_INFO("MAIN THREAD");
    }
    ros::spinOnce();
    loop.sleep();
}
}



// private:
public:
// c++
bool sendgoal_;
bool triger_;


int32 triger_num_;
int32 ids;
int32 num_waypoints_;

//  ros
ros::WallTimer arrow_timer_;
ros::Publisher path_pub_;
ros::Publisher arrows_pub_;
ros::Publisher single_arrow_pub_;
ros::Subscriber pose_sub_;
ros::Subscriber triger_sub_;
actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_client_;
// c++ stl
std::queue<geometry_msgs::PoseWithCovarianceStamped> poses_;
visualization_msgs::Marker line_strip;

std::vector<visualization_msgs::Marker> arrows_;
// actionlib::SimpleActionServer<move_base_msgs::MoveBaseAction> move_server_;


ros::NodeHandle nh_;
ros::NodeHandle private_nh_;


boost::mutex flag_mutex_;
};

}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "waypoints_follower");

    std::cout<< "?????"<< "\n";
    // Follow_Waypoints::FollowPath server;
    Follow_Waypoints::GetPath path("goals_server");
    // path.sendgoals();

    path.run();
 
    // if(move_client_.waitForResult())
    // {
    //         ROS_INFO("The Robot has arrived at the waypoint successfully");
    //         sendgoal_=true;
    // }
    // else 
    std::cout<<"...."<<"\n";
    // { 
    //     std::cout << " still moving on " << "\n" ;
    // }
    
    // ros::spin();
    return 0;
}
