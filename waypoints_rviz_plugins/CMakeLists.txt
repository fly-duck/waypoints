cmake_minimum_required(3.02)

project(waypoints_rviz_plugins)

find_package(catkin REQUIRED COMPONENTS rviz)

catkin_package()
include_directories(${catkin_INCLUDE_DIRS})
link_directories(${catkin_LIBRARY_DIRS})
