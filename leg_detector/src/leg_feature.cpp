/*
 * features.cpp
 *
 *  Created on: Apr 15, 2015
 *      Author: frm-ag
 */


// Own includes
#include <leg_detector/leg_feature.h>
#include <people_tracking_filter/advanced_tracker_particle.h>

int LegFeature::nextid = 0;

static std::string fixed_frame              = "odom_combined";  // The fixed frame in which ? //TODO find out

static int NumberOfParticles = 600;

/*LegFeature::LegFeature(tf::Stamped<tf::Point> loc, tf::TransformListener& tfl, OcclusionModelPtr ocm)
  :LegFeature(loc,tfl),
   occlusion_model_(ocm)
{

}*/

// The is the one leg tracker
LegFeature::LegFeature(tf::Stamped<tf::Point> loc, tf::TransformListener& tfl)
  : tfl_(tfl),
    leg_feature_predict_pos_cov_(0.1), // Around 0.05
    leg_feature_predict_vel_cov_(1.0),  // Around 1.0 should be fine
    sys_sigma_(tf::Vector3(leg_feature_predict_pos_cov_, leg_feature_predict_pos_cov_, 0.0), tf::Vector3(leg_feature_predict_vel_cov_, leg_feature_predict_vel_cov_, 0.0)), // The initialized system noise(the variance)
    filter_("tracker_name", NumberOfParticles, sys_sigma_), // Name, NumberOfParticles, Noise
    //reliability(-1.), p(4),
    use_filter_(true),
    is_valid_(true), // On construction the leg feature is always valid
    leg_feature_update_cov_(0.0025), // The update measurement cov (should be around 0.0025)
    is_static_(true) // At the beginning the leg feature is considered static
{
  // Increase the id
  int_id_ = nextid++;

  // Generate the string id
  char id[100];
  snprintf(id, 100, "legtrack%d", int_id_);
  id_ = std::string(id);

  // Configuration server
  // dynamic_reconfigure::Server<leg_detector::DualTrackerConfig>::CallbackType f;
  // f = boost::bind(&LegFeature::configure, this, _1, _2);



  ROS_DEBUG_COND(DEBUG_LEG_TRACKER,"LegFeature::%s Created <NEW_LEGFEATURE %s> at %f - %f - %f", __func__, id_.c_str(), loc.getX(), loc.getY(), loc.getZ());

  object_id = "";
  time_ = loc.stamp_;
  meas_time_ = loc.stamp_;
  //other = NULL;

  try
  {
    tfl_.transformPoint(fixed_frame, loc, loc);
  }
  catch (...)
  {
    ROS_WARN("TF exception spot 6.");
  }

  // Create transform to the tracker position
  tf::StampedTransform pose(tf::Pose(tf::Quaternion(0.0, 0.0, 0.0, 1.0), loc), loc.stamp_, id_, loc.frame_id_);
  tfl_.setTransform(pose);

  // Initialize the filter (Create the initial particles)
  BFL::StatePosVel prior_sigma(tf::Vector3(0.1, 0.1, 0.0), tf::Vector3(0.0000001, 0.0000001, 0.000000));
  BFL::StatePosVel mu(loc);
  filter_.initialize(mu, prior_sigma, time_.toSec());

  // Get the first estimation of the particle state
  BFL::StatePosVel est;
  filter_.getEstimate(est);

  // Set the initial position
  initial_position_[0] = loc.getX();
  initial_position_[1] = loc.getY();
  initial_position_[2] = loc.getZ();

  //leg_feature_update_cov_    = config.leg_feature_update_cov;     ROS_DEBUG_COND(DEBUG_LEG_TRACKER, "DEBUG_LEG_TRACKER::%s - leg_feature_update_cov_ %f", __func__, leg_feature_update_cov_ );
  //leg_feature_predict_pos_cov_    = config.leg_feature_predict_pos_cov;     ROS_DEBUG_COND(DEBUG_LEG_TRACKER, "DEBUG_LEG_TRACKER::%s - leg_feature_predict_pos_cov_ %f", __func__, leg_feature_predict_pos_cov_ );
  //leg_feature_predict_vel_cov_    = config.leg_feature_predict_vel_cov;     ROS_DEBUG_COND(DEBUG_LEG_TRACKER, "DEBUG_LEG_TRACKER::%s - leg_feature_predict_vel_cov_ %f", __func__, leg_feature_predict_vel_cov_ );


  // Update the position of this leg feature
  updatePosition();
}

/**
 * Destructor
 */
LegFeature::~LegFeature(){
  //peopleTrackerList_.clear();

  ROS_DEBUG_COND(DEBUG_LEG_TRACKER,"LegFeature::%s <DELETE_LEGFEATURE %s>", __func__, id_.c_str());
}

/**
 * Propagate the Position of the Feature using the Particle Filter
 */
void LegFeature::propagate(ros::Time time)
{
  ROS_DEBUG_COND(DEBUG_LEG_TRACKER,"LegFeature::%s ID:%i", __func__, int_id_);

  // Update the time
  time_ = time;

  // Get the associated people tracker with the highest probability
  PeopleTrackerPtr mostProbableAssociatedPPL;
  double max_prob = 0.0;

  std::vector<PeopleTrackerPtr> associatedPT = this->getPeopleTracker();

  for(std::vector<PeopleTrackerPtr>::iterator pplIt = associatedPT.begin();
      pplIt != associatedPT.end();
      pplIt++)
  {
    if((*pplIt)->getTotalProbability() > max_prob){
      mostProbableAssociatedPPL = *pplIt;
      max_prob = (*pplIt)->getTotalProbability();
    }
    //std::cout << "\t" << **pplIt << std::endl;
  }


  MatrixWrapper::SymmetricMatrix cov(6);
  cov = 0.0;
  cov(1, 1) = leg_feature_predict_pos_cov_;//conf.leg_feature_predict_pos_cov;
  cov(2, 2) = leg_feature_predict_pos_cov_;//conf.leg_feature_predict_pos_cov;
  cov(3, 3) = 0.0;
  cov(4, 4) = leg_feature_predict_vel_cov_;//conf.leg_feature_predict_vel_cov;
  cov(5, 5) = leg_feature_predict_vel_cov_;//conf.leg_feature_predict_vel_cov;
  cov(6, 6) = 0.0;



  // If there exists a relevant high level filter
  if(mostProbableAssociatedPPL && mostProbableAssociatedPPL->getTotalProbability() > 0.5){ // TODO Make the configurable
    //std::cout << "\t Highest is:" << *mostProbableAssociatedPPL << std::endl;

    // Get estimation for itself
    // StatePosVel est = mostProbableAssociatedPPL->getLegEstimate(int_id_);

    // TODO THIS SHOULD DEPEND ON THE ESTIMATION OF THE LEG; NOT THE PERSON!!!
    filter_.updatePrediction(time.toSec(), cov, mostProbableAssociatedPPL->getEstimate().vel_, mostProbableAssociatedPPL->getHipVec());

  }
  // If there is no relevant people tracker assigned-> Consider only the low level filter
  else
  {

    filter_.updatePrediction(time.toSec(),cov);
  }





  Eigen::Matrix<double,3,3> pos_cov;





  MatrixWrapper::SymmetricMatrix debug_cov(6);
  debug_cov(1,1) = 0.0001;
  debug_cov(2,2) = 0.01;
  debug_cov(4,4) = 1.0;
  debug_cov(5,5) = 1.0;

  // Calculate the covariance of the

  //server.updateConfig(conf);
  //std::cout << "conf.leg_feature_predict_pos_cov " << conf.leg_feature_predict_pos_cov << std::endl;
  //std::cout << "conf.leg_feature_predict_vel_cov " << conf.leg_feature_predict_vel_cov << std::endl;


  // Do the Prediction in the filter
  //filter_.updatePrediction(time.toSec());



  //assert(false); // Continue work here: set the prediction from the current motion model
  // update the Position
  updatePosition();

  ROS_DEBUG_COND(DEBUG_LEG_TRACKER,"LegFeature::%s Done Propagating leg_tracker with ID %s", __func__, id_.c_str());

}

// Here the measurement is used to update the filter location
void LegFeature::update(tf::Stamped<tf::Point> loc, double probability)
{
  ROS_DEBUG_COND(DEBUG_LEG_TRACKER,"LegFeature::%s",__func__);

  // Set the tf to represent this
  tf::StampedTransform pose(tf::Pose(tf::Quaternion(0.0, 0.0, 0.0, 1.0), loc), loc.stamp_, id_, loc.frame_id_);
  tfl_.setTransform(pose);

  // Update the measurement time
  meas_time_ = loc.stamp_;
  time_ = meas_time_;

  // Covariance of the Measurement
  MatrixWrapper::SymmetricMatrix cov(3);
  cov = 0.0;
  cov(1, 1) = leg_feature_update_cov_;
  cov(2, 2) = leg_feature_update_cov_;
  cov(3, 3) = leg_feature_update_cov_;

  filter_.updateCorrection(loc, cov);

  // Update the position based on the latest measurements
  updatePosition();

  // Update history
  BFL::StatePosVel est;
  filter_.getEstimate(est);

  boost::shared_ptr<tf::Stamped<tf::Point> > point(new tf::Stamped<tf::Point>());
  point->setX( est.pos_[0]);
  point->setY( est.pos_[1]);
  point->setZ( est.pos_[2]);
  point->stamp_ = time_;

  position_history_.push_back(point);

}

// Update own position based on the Estimation of the Filter

/**
 * Update the Position(position_) of the Leg Feature
 */
void LegFeature::updatePosition()
{
  ROS_DEBUG_COND(DEBUG_LEG_TRACKER,"LegFeature::%s",__func__);

  BFL::StatePosVel est;
  filter_.getEstimate(est);

  pos_vel_ = est;

  position_[0] = est.pos_[0];
  position_[1] = est.pos_[1];
  position_[2] = est.pos_[2];
  position_.stamp_ = time_;
  position_.frame_id_ = fixed_frame_;
  double nreliability = fmin(1.0, fmax(0.1, est.vel_.length() / 0.5)); //TODO ???????
  //reliability = fmax(reliability, nreliability);

  // Check if static
  double static_threshold = 0.8; // TODO make this configurable
  if((initial_position_-position_).length() > static_threshold){
    this->is_static_ = false;
  }
}

// TODO do this static
/**
 *  @brief The distance between two legs.
 *
 *  Calculates the euclidian distance between to features(legs)
 */
double LegFeature::distance(LegFeaturePtr leg0,  LegFeaturePtr leg1){
    tf::Stamped<tf::Point> one = leg0->position_;
    tf::Stamped<tf::Point> two = leg1->position_;

    double distance = (one-two).length();
    return distance;
}

void LegFeature::removeInvalidAssociations(){
  peopleTrackerList_.erase(std::remove_if(peopleTrackerList_.begin(),peopleTrackerList_.end(), isValidPeopleTracker),peopleTrackerList_.end());
}

/**
 * Add a People Tracker to associated with this leg, every leg holds its associated People Trackers
 * @param peopleTracker
 */
void LegFeature::addPeopleTracker(PeopleTrackerPtr peopleTracker){
  // Add this tracker to the list
  this->peopleTrackerList_.push_back(peopleTracker);
}
