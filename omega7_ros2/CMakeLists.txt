cmake_minimum_required(VERSION 3.8)
project(omega7_ros2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(std_srvs REQUIRED)

set(FD_SDK_DIR "/home/qcr-openarm/ray_ws/force_dimension/sdk-3.17.8")

include_directories(
  ${FD_SDK_DIR}/include
)

link_directories(
  ${FD_SDK_DIR}/lib/release/lin-x86_64-gcc
)

add_executable(omega7_node src/omega7_node.cpp)

ament_target_dependencies(
  omega7_node
  rclcpp
  geometry_msgs
  std_msgs
)

target_link_libraries(
  omega7_node
  dhd
  pthread
  usb-1.0
  rt
  dl
)


add_executable(omega7_teleop src/omega7_teleop.cpp)

ament_target_dependencies(
  omega7_teleop
  rclcpp
  geometry_msgs
  trajectory_msgs
  moveit_msgs
  tf2
  tf2_geometry_msgs
  sensor_msgs
  std_msgs
  tf2_ros
  std_srvs
)

install(
  TARGETS
	omega7_node
	omega7_teleop
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()

