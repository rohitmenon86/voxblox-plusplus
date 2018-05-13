#ifndef VOXBLOX_GSM_SLIDING_WINDOW_CONTROLLER_H
#define VOXBLOX_GSM_SLIDING_WINDOW_CONTROLLER_H

#include "controller.h"

#include <tf/transform_broadcaster.h>

namespace voxblox {
namespace voxblox_gsm {
class SlidingWindowController : public Controller {
 public:
  explicit SlidingWindowController(ros::NodeHandle* node_handle);

  /**
   * Checks whether the segments have already been extracted for the current
   * window. If so, it returns the already extracted ones. If not, the base
   * method is called and the segments are extracted.
   * @param labels
   * @param label_layers_map
   * @param labels_list_is_complete
   */
  void extractSegmentLayers(
      const std::vector<Label>& labels,
      std::unordered_map<Label, LayerPair>* label_layers_map,
      bool labels_list_is_complete = false) override;

 private:
  /**
   * Sets the sliding window to the new position, removes segments which are
   * completely outide of its volume and publishes the remaining scene and
   * segments.
   * @param new_center center position of the sliding window
   */
  void updateAndPublishWindow(const Point& new_center);

  /**
   * Called by the timer. Checks whether the camera has moved more than a
   * certain distance from the current window center. If so, the window is
   * updated.
   */
  void checkTfCallback(const ros::TimerEvent&);

  /**
   * Removes segments from the gsm which are outside the ball volume defined
   * by the radius and center.
   * @param radius
   * @param center
   */
  void removeSegmentsOutsideOfRadius(float radius, Point center);

  /**
   * Looks up current transform of camera
   * @param position output transform
   */
  void getCurrentPosition(tf::StampedTransform* position);

  /**
   * Publishes position of new sliding window. Useful for debugging or display.
   * @param position
   */
  void publishPosition(const Point& position);

  /**
   * Calls base method and adds the position of the sliding window to the
   * message.
   * @param publisher
   * @param gsm_update
   */
  void publishGsmUpdate(const ros::Publisher& publisher,
                        modelify_msgs::GsmUpdate& gsm_update) override;

  /**
   * Calls the base method and then removes the elements from the list, which
   * were outside of the sliding window.
   * @param labels
   * @param get_all
   */
  void getLabelsToPublish(std::vector<Label>* labels, bool get_all) override;

  ros::Timer tf_check_timer_;
  tf::TransformBroadcaster position_broadcaster_;
  tf::StampedTransform current_window_position_;

  std::unordered_map<Label, LayerPair> label_to_layers_;
  Point current_window_position_point_;
  std::vector<Label> removed_segments_;
  float window_radius_ = 1.0f;
  float update_fraction_ = 0.5f;
  bool window_has_moved_first_time_ = false;
};
}  // namespace voxblox_gsm
}  // namespace voxblox

#endif  // VOXBLOX_GSM_SLIDING_WINDOW_CONTROLLER_H