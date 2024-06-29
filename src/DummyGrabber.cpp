#include <Huenicorn/DummyGrabber.hpp>

#include <glm/trigonometric.hpp>

#include <Huenicorn/Config.hpp>


namespace Huenicorn
{
  DummyGrabber::DummyGrabber(Config* config):
  IGrabber(config)
  {
    m_startTime = Timing::ClockType::now();
  }


  DummyGrabber::~DummyGrabber()
  {
  }


  glm::ivec2 DummyGrabber::displayResolution() const
  {
    return m_resolution;
  }


  void DummyGrabber::grabFrameSubsample(cv::Mat& cvImage)
  {
    Timing::TimePoint now = Timing::ClockType::now();
    Timing::Duration duration = now - m_startTime;
    double seconds = duration.count();
    double max = 255.0;
    int rCoeff = static_cast<int>(((glm::sin(seconds / 2.0) + 1.0) / 2.0) * max);
    int gCoeff = static_cast<int>(((glm::sin(seconds / 3.0) + 1.0) / 2.0) * max);
    int bCoeff = static_cast<int>(((glm::sin(seconds / 5.0) + 1.0) / 2.0) * max);

    cv::Mat image(m_resolution.x, m_resolution.y, CV_8UC3, cv::Scalar(bCoeff, gCoeff, rCoeff));
    cvImage = std::move(image);
  }


  IGrabber::RefreshRate DummyGrabber::displayRefreshRate() const
  {
    return m_refreshRate;
  }
}
