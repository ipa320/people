/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Wim Meeussen */

#include "people_tracking_filter/advanced_measmodel_pos.h"

using namespace std;
using namespace BFL;
using namespace tf;


static const unsigned int NUM_MEASMODEL_POS_COND_ARGS   = 1;
static const unsigned int DIM_MEASMODEL_POS             = 13;

// Constructor
AdvancedMeasPdfPos::AdvancedMeasPdfPos(const Vector3& sigma)
  : ConditionalPdf<Vector3, StatePosVel>(DIM_MEASMODEL_POS, NUM_MEASMODEL_POS_COND_ARGS),
    meas_noise_(Vector3(0, 0, 0), sigma)
{}


// Destructor
AdvancedMeasPdfPos::~AdvancedMeasPdfPos()
{}


/**
 * Calculate the probability of this measurement(particle)
 * @param measurement The measurement
 * @return Probability to measure this
 */
Probability
AdvancedMeasPdfPos::ProbabilityGet(const Vector3& measurement) const
{

  tf::Vector3 delta = measurement - ConditionalArgumentGet(0).pos_;

  Probability prob = meas_noise_.ProbabilityGet(delta);

/*  std::cout << ConditionalArgumentGet(0).pos_.getX() << " " << ConditionalArgumentGet(0).pos_.getY() << " " << ConditionalArgumentGet(0).pos_.getZ()<< std::endl;
  std::cout << "Delta" << delta.getX() << " " << delta.getY() << " " << delta.getZ() << " Norm:" << delta.length() << std::endl;
  std::cout << prob.getValue() << std::endl;*/


  return prob;
}



bool
AdvancedMeasPdfPos::SampleFrom(Sample<Vector3>& one_sample, int method, void *args) const
{
  cerr << "AdvancedMeasPdfPos::SampleFrom Method not applicable" << endl;
  assert(0);
  return false;
}




Vector3
AdvancedMeasPdfPos::ExpectedValueGet() const
{
  cerr << "AdvancedMeasPdfPos::ExpectedValueGet Method not applicable" << endl;
  Vector3 result;
  assert(0);
  return result;
}




SymmetricMatrix
AdvancedMeasPdfPos::CovarianceGet() const
{
  cerr << "AdvancedMeasPdfPos::CovarianceGet Method not applicable" << endl;
  SymmetricMatrix Covar(DIM_MEASMODEL_POS);
  assert(0);
  return Covar;
}


void
AdvancedMeasPdfPos::CovarianceSet(const MatrixWrapper::SymmetricMatrix& cov)
{
  tf::Vector3 cov_vec(sqrt(cov(1, 1)), sqrt(cov(2, 2)), sqrt(cov(3, 3)));
  meas_noise_.sigmaSet(cov_vec);
}



