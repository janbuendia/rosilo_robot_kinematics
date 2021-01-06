#include <rosilo_robot_kinematics/rosilo_robot_kinematics_provider.h>
#include <rosilo_conversions/rosilo_conversions.h>
namespace rosilo
{

void RobotKinematicsProvider::_callback_desired_pose(const geometry_msgs::PoseStamped::ConstPtr &msg)
{
    desired_pose_ = geometry_msgs_pose_to_dq(msg->pose);
    if(! enabled_)
    {
        enabled_ = true;
        ROS_INFO_STREAM(ros::this_node::getName() + "::RobotKinematicsProvider enabled.");
    }
}

void RobotKinematicsProvider::_callback_desired_interpolator_speed(const std_msgs::Float64::ConstPtr &msg)
{
    desired_interpolator_speed_ = std_msgs_float64_to_double(*msg);
}

void RobotKinematicsProvider::_callback_desired_gripper_state(const std_msgs::Float64::ConstPtr &msg)
{
    desired_gripper_state_ = std_msgs_float64_to_double(*msg);
}

RobotKinematicsProvider::RobotKinematicsProvider(ros::NodeHandle &node_handle, const std::string &topic_prefix):
    RobotKinematicsProvider(node_handle, node_handle, topic_prefix)
{
    //Delegated to RobotKinematicsProvider::RobotKinematicsProvider(ros::NodeHandle &nodehandle_publisher, ros::NodeHandle &nodehandle_subscriber, const std::string &topic_prefix)
}

RobotKinematicsProvider::RobotKinematicsProvider(ros::NodeHandle &nodehandle_publisher, ros::NodeHandle &nodehandle_subscriber, const std::string &topic_prefix):
    enabled_(false),
    topic_prefix_(topic_prefix),
    desired_pose_(0)
{
    ROS_INFO_STREAM(ros::this_node::getName() + "::Initializing RobotKinematicsProvider with prefix " + topic_prefix);
    publisher_pose_ = nodehandle_publisher.advertise<geometry_msgs::PoseStamped>(topic_prefix + "get/pose", 1);

    subscriber_desired_pose_ = nodehandle_subscriber.subscribe(topic_prefix + "set/desired_pose", 1, &RobotKinematicsProvider::_callback_desired_pose, this);
    subscriber_desired_interpolator_speed_ = nodehandle_subscriber.subscribe(topic_prefix + "set/desired_interpolator_speed", 1, &RobotKinematicsProvider::_callback_desired_interpolator_speed, this);
    subscriber_desired_gripper_state_ = nodehandle_subscriber.subscribe(topic_prefix + "set/desired_gripper_state", 1, &RobotKinematicsProvider::_callback_desired_gripper_state, this);
}

bool RobotKinematicsProvider::is_enabled() const
{
    return enabled_;
}

DQ RobotKinematicsProvider::get_desired_pose() const
{
    if(enabled_)
    {
        return desired_pose_;
    }
    else
    {
        throw std::runtime_error(ros::this_node::getName() + "::Trying to get_desired_pose of an unitialized RobotKinematicsProvider.");
    }
}

double RobotKinematicsProvider::get_desired_interpolator_speed() const
{
    return desired_interpolator_speed_;
}

void RobotKinematicsProvider::send_pose(const DQ &pose) const
{
    publisher_pose_.publish(dq_to_geometry_msgs_pose_stamped(pose));
}

double RobotKinematicsProvider::get_desired_gripper_state() const
{
    return desired_gripper_state_;
}

}
