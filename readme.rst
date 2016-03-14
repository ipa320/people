People
======
Algorithms related to detecting and tracking people using various robot sensors.

Documentation
^^^^^^^^^^^^^
http://ros.org/wiki/people

Requirements & building
^^^^^^^^^^^^^^^^^^^^^^^
This project depends on ROS and can be built using catkin.

Branches
^^^^^^^^
fuerte_trunk - Most recent version for Fuerte. This version is not maintained.

groovy-devel - Most recent version for Groovy. Bug fixes only.

groovy - Catkinized version for Groovy - Released as deb. 

hydro-devel - Most recent version for Hydro. Bug fixes only.

indigo-devel - Most recent version for Indigo. Bug fixes and stable new features.

master - Really new features, may not be stable. Currently developing against Hydro.

USAGE
^^^^^
Training Set Creator (Create Labels using rviz, stores results inside a bagfile)
	> rosrun leg_detector training_set_creator --input-file ../SampleData/people.bag --output-file output.bag
	Inside rviz you will see the segmentation along with labels.
	Use rqt_bag to inspect the page and see what exactly was happening
	
	During the label creation the following messages are pubished:
		Topic /visualization_marker(visualization_msgs::Marker) - The labels
		Topic /clusters (sensor_msgs::PointCloud) - The clusters color highlighted
		Topic /scan (sensor_msgs::LaserScan) - The original scan
		Topic /clock                         - The simulation time
		Topic /tf			     - The Transformations
	A rviz configuration file is stored under leg_detector/config/leg_detector.rviz
	
Dual Tracker (Advanced Training)
  > roslaunch leg_detector dual_tracker.launch
  


