#pragma once 


#include <rviz/tool.h>


    
namespace Ogre
{
class SceneNode;
class Vector3;
}

namespace rviz
{
class VectorProperty;
class VisualizationManager;
class ViewportMouseEvent;
}
    
namespace Waypoints_Plugin {
class StarTool: public ::rviz::Tool
{
Q_OBJECT
public:
  StarTool();
  ~StarTool();

  virtual void onInitialize();

  virtual void activate();
  virtual void deactivate();

  virtual int processMouseEvent( rviz::ViewportMouseEvent& event );

  virtual void load( const ::rviz::Config& config );
  virtual void save( ::rviz::Config config ) const;

private:
  std::string tool_resource_; 
  std::vector<Ogre::SceneNode*> flag_nodes_;
 ::Ogre::SceneNode* moving_flag_node_;
  std::string flag_resource_;
  rviz::VectorProperty* current_flag_property_;
};

}
