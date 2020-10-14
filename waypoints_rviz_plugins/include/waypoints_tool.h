#pragma once 


#include <rviz/tool.h>


namespace Waypoints_Plugin {
    
using namespace Waypoints_Plugin;
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
    
  std::vector<Ogre::SceneNode*> flag_nodes_;
};

}
