#pragma once

#include <memory>
#include <optional>

#include <Huenicorn/IGrabber.hpp>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>


namespace Huenicorn
{
  /**
   * @brief Image data structure
   * 
   */
  struct ImageData
  {
    std::vector<uint8_t> pixels;
    int width;
    int height;
    int bitsPerPixel;
  };


  /**
   * @brief X11 implementation of screen grabber
   * 
   */
  class X11Grabber : public IGrabber
  {
  public:
    // Constructor / destructor
    /**
     * @brief X11Grabber constructor
     * 
     * @param config Huenicorn configuration
     */
    X11Grabber(Config* config);


    /**
     * @brief X11Grabber destructor
     * 
     */
    virtual ~X11Grabber();


    // Getters
    /**
     * @brief Returns the resolution of the selected display
     * 
     * @return Resolution Resolution of the selected display
     */
    virtual glm::ivec2 displayResolution() const override;


    /**
     * @brief Returns the refresh rate of the display
     * 
     * @return RefreshRate Refresh rate of the display
     */
    virtual RefreshRate displayRefreshRate() const override;


    // Methods
    /**
     * @brief Takes a screen capture and returns a subsample of it as bitmap
     * 
     * @param imageData Subsample of screen capture
     */
    virtual void grabFrameSubsample(cv::Mat& imageData) override;


  private:
    // Attributes
    std::optional<ImageData> m_imageData;
    std::unique_ptr<XShmSegmentInfo> m_shmInfo;

    XImage* m_ximage{nullptr};
    Display* m_display{nullptr};
  };
}
