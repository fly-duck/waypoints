#include <waypoints_tool.h>

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreEntity.h>

#include <ros/console.h>

#include <rviz/viewport_mouse_event.h>
#include <rviz/visualization_manager.h>
#include <rviz/mesh_loader.h>
#include <rviz/geometry.h>
#include <rviz/properties/vector_property.h>


namespace Waypoints_Plugin{


    StarTool::StarTool()
  : moving_flag_node_( NULL )
  , current_flag_property_( NULL ){
    }

StarTool::~StarTool()
{
  for( unsigned i = 0; i < flag_nodes_.size(); i++ )
  {
    scene_manager_->destroySceneNode( flag_nodes_[ i ]);
  }
}
    void StarTool::onInitialize() {
    
        tool_resource_="package://waypoints_rviz_plugins/free_model/IPHONESE1.dae";
        
       if( ::rviz::loadMeshFromResource( tool_resource_ ).isNull() )
       {
         ROS_ERROR( "StarTool: failed to load model resource '%s'.", tool_resource_.c_str() );
         return;
       }

       moving_flag_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
       ::Ogre::Entity* entity = scene_manager_->createEntity( tool_resource_ );
       moving_flag_node_->attachObject( entity );
       moving_flag_node_->setVisible( false );
    }
    void StarTool::activate()
    {
      if( moving_flag_node_ )
      {
        moving_flag_node_->setVisible( true );
    
        current_flag_property_ = new rviz::VectorProperty( "Flag " + QString::number( flag_nodes_.size() ));
        current_flag_property_->setReadOnly( true );
        getPropertyContainer()->addChild( current_flag_property_ );
      }
    }
    void StarTool::deactivate()
    {
      if( moving_flag_node_ )
      {
        moving_flag_node_->setVisible( false );
        delete current_flag_property_;
        current_flag_property_ = NULL;
      }
    }
int StarTool::processMouseEvent( rviz::ViewportMouseEvent& event )
{
  if( !moving_flag_node_ )
  {
    return Render;
  }
  Ogre::Vector3 intersection;
  Ogre::Plane ground_plane( Ogre::Vector3::UNIT_Z, 0.0f );
  if( rviz::getPointOnPlaneFromWindowXY( event.viewport,
                                         ground_plane,
                                         event.x, event.y, intersection ))
  {
    moving_flag_node_->setVisible( true );
    moving_flag_node_->setPosition( intersection );
    current_flag_property_->setVector( intersection );

    if( event.leftDown() )
    {
      current_flag_property_ = NULL; // Drop the reference so that deactivate() won't remove it.
      return Render | Finished;
    }
  }
  else
  {
    moving_flag_node_->setVisible( false ); // If the mouse is not pointing at the ground plane, don't show the flag.
  }
  return Render;
}
   void StarTool::load( const rviz::Config& config )
    {
      // Here we get the "Flags" sub-config from the tool config and loop over its entries:
      rviz::Config flags_config = config.mapGetChild( "Flags" );
      int num_flags = flags_config.listLength();
      for( int i = 0; i < num_flags; i++ )
      {
        rviz::Config flag_config = flags_config.listChildAt( i );
        // At this point each ``flag_config`` represents a single flag.
        //
        // Here we provide a default name in case the name is not in the config file for some reason:
        QString name = "Flag " + QString::number( i + 1 );
        // Then we use the convenience function mapGetString() to read the
        // name from ``flag_config`` if it is there.  (If no "Name" entry
        // were present it would return false, but we don't care about
        // that because we have already set a default.)
        flag_config.mapGetString( "Name", &name );
        // Given the name we can create an rviz::VectorProperty to display the position:
        rviz::VectorProperty* prop = new rviz::VectorProperty( name );
        // Then we just tell the property to read its contents from the config, and we've read all the data.
        prop->load( flag_config );
        // We finish each flag by marking it read-only (as discussed
        // above), adding it to the property container, and finally making
        // an actual visible flag object in the 3D scene at the correct
        // position.
        prop->setReadOnly( true );
        getPropertyContainer()->addChild( prop );
      }
    }
}
#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(Waypoints_Plugin::StarTool,rviz::Tool )
