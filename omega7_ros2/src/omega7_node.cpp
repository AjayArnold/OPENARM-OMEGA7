#include <cmath>
#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "std_msgs/msg/float64.hpp"

#include "dhdc.h"

using namespace std::chrono_literals;

class Omega7Node : public rclcpp::Node
{
public:
    Omega7Node()
    : Node("omega7_node")
    {
        // Connect to the Omega.7
        if (dhdOpen() < 0)
        {
            RCLCPP_ERROR(
                this->get_logger(),
                "Failed to open Omega.7: %s",
                dhdErrorGetLastStr());

            throw std::runtime_error("Could not open Omega.7");
        }

        RCLCPP_INFO(
            this->get_logger(),
            "Connected to: %s",
            dhdGetSystemName());

        // ROS publishers
        pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>(
            "/omega/pose", 10);

        gripper_pub_ = this->create_publisher<std_msgs::msg::Float64>(
            "/omega/gripper", 10);

        // Read/publish at 100 Hz
        timer_ = this->create_wall_timer(
            10ms,
            std::bind(&Omega7Node::read_omega, this));
    }

    ~Omega7Node()
    {
        dhdClose();
    }

private:
    void read_omega()
    {
        double x, y, z;
        double roll_deg, pitch_deg, yaw_deg;
        double gripper;

        // Read Omega position + orientation
        if (dhdGetPositionAndOrientationDeg(
                &x, &y, &z,
                &roll_deg, &pitch_deg, &yaw_deg) < 0)
        {
            RCLCPP_ERROR(
                this->get_logger(),
                "Failed to read Omega.7 pose: %s",
                dhdErrorGetLastStr());
            return;
        }

        // Read gripper opening
        if (dhdGetGripperGap(&gripper) < 0)
        {
            RCLCPP_ERROR(
                this->get_logger(),
                "Failed to read Omega.7 gripper: %s",
                dhdErrorGetLastStr());
            return;
        }

        geometry_msgs::msg::PoseStamped pose_msg;

        pose_msg.header.stamp = this->now();
        pose_msg.header.frame_id = "world";

        pose_msg.pose.position.x = x;
        pose_msg.pose.position.y = y;
        pose_msg.pose.position.z = z;

	// Convert Omega wrist angles from degrees to radians.
	const double roll  = roll_deg  * M_PI / 180.0;
	const double pitch = pitch_deg * M_PI / 180.0;
	const double yaw   = yaw_deg   * M_PI / 180.0;

	// Convert roll-pitch-yaw to quaternion.
	const double cy = cos(yaw * 0.5);
	const double sy = sin(yaw * 0.5);
	const double cp = cos(pitch * 0.5);
	const double sp = sin(pitch * 0.5);
	const double cr = cos(roll * 0.5);
	const double sr = sin(roll * 0.5);

	pose_msg.pose.orientation.w =
    		cr * cp * cy + sr * sp * sy;

	pose_msg.pose.orientation.x =
    		sr * cp * cy - cr * sp * sy;

	pose_msg.pose.orientation.y =
    		cr * sp * cy + sr * cp * sy;

	pose_msg.pose.orientation.z =
    		cr * cp * sy - sr * sp * cy;

        std_msgs::msg::Float64 gripper_msg;
        gripper_msg.data = gripper;

        pose_pub_->publish(pose_msg);
        gripper_pub_->publish(gripper_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_pub_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr gripper_pub_;

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<Omega7Node>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
