cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_CXX_STANDARD 11)
project(follow_waypoints)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(catkin REQUIRED COMPONENTS
  actionlib
  geometry_msgs
  move_base_msgs
  rospy
  roscpp
  smach
  std_msgs
  tf
  
)

catkin_python_setup()

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS actionlib geometry_msgs move_base_msgs rospy smach std_msgs
)
include_directories(include ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS} )

add_executable(follower src/follow_waypoints/waypoints_follower.cc)
target_link_libraries(follower ${catkin_LIBRARIES})
add_dependencies(follower ${catkin_EXPORTED_TARGETS})


add_executable(recorder src/follow_waypoints/record_base_link.cc)
target_link_libraries(recorder ${catkin_LIBRARIES})
add_dependencies(recorder ${catkin_EXPORTED_TARGETS})

catkin_install_python(
  PROGRAMS nodes/follow_waypoints
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# Install launch files
install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)

# This step install waypoints
install(TARGETS follower
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

install(TARGETS recorder
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})


install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
        PATTERN ".svn" EXCLUDE)
# install(TARGETS explore_costmap
#         ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#         LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#         RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

# install(DIRECTORY include/${PROJECT_NAME}/
#         DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#         PATTERN ".svn" EXCLUDE)

# install(DIRECTORY launch/
#         DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
#         PATTERN ".svn" EXCLUDE)
