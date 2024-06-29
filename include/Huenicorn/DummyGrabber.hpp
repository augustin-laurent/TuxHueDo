#pragma once

#include <Huenicorn/IGrabber.hpp>

#include <Huenicorn/TimingDefinitions.hpp>

namespace Huenicorn
{
  /**
   * @brief Dummy grabber returning a color shift based on time
   * 
   */
  class DummyGrabber : public IGrabber
  {
  public:
    // Constructor / destructor
    /**
     * @brief DummyGrabber constructor
     * 
     * @param config Huenicorn configuration
     */
    DummyGrabber(Config* config);


    /**
     * @brief DummyGrabber destructor
     * 
     */
    virtual ~DummyGrabber();


    // Getters
    /**
     * @brief Returns a dummy resolution
     * 
     * @return Resolution Dummy resolution
     */
    virtual glm::ivec2 displayResolution() const override;


    /**
     * @brief Returns the refresh rate of the display
     * 
     * @return RefreshRate Dummy refresh rate
     */
    virtual RefreshRate displayRefreshRate() const override;


    // Methods
    /**
     * @brief Returns a bitmap filled with color based on time-based color gradient
     * 
     * @param imageData Subsample of screen capture
     */
    virtual void grabFrameSubsample(cv::Mat& imageData) override;


  private:

    // Attributes
    glm::ivec2 m_resolution{16, 9};
    RefreshRate m_refreshRate{60};
    Timing::TimePoint m_startTime;
  };
}
