#include <rclcpp/rclcpp.hpp>
#include <tf2_ros/buffer_server.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <tf2_ros/buffer.h>


class TransformNode : public rclcpp::Node
{
public:
    TransformNode()
        : Node("transform_node")
    {
        // Initialize the TF2 Buffer and Listener
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        // Timer to perform the lookup transform periodically
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            [this]() { this->lookup_transform(); }
        );
    }

private:
    void lookup_transform()
    {
        try
        {
            // Lookup the transform from LiDAR to Camera
            geometry_msgs::msg::TransformStamped transform = tf_buffer_->lookupTransform(
                "arm_camera_color_frame",  // Target frame
                "lidar_link",   // Source frame
                tf2::TimePointZero // Use the latest available transform
            );

            RCLCPP_INFO(this->get_logger(), "Transform from LiDAR to Camera: [%f, %f, %f]",
                        transform.transform.translation.x,
                        transform.transform.translation.y,
                        transform.transform.translation.z);
        }
        catch (const tf2::TransformException &ex)
        {
            RCLCPP_WARN(this->get_logger(), "Transform warning: %s", ex.what());
        }
    }

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TransformNode>());
    rclcpp::shutdown();
    return 0;
}
