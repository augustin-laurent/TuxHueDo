#pragma once

#include <filesystem>
#include <memory>

#include <Huenicorn/IGrabber.hpp>

namespace Huenicorn
{
  /**
   * @brief Wrapper interface for platform-specific functions and grabber selection
   * 
   */
  class IPlatformAdapter
  {
  public:
    /**
     * @brief Constructor
     * 
     */
    IPlatformAdapter(){}


    /**
     * @brief Destructor
     * 
     */
    virtual ~IPlatformAdapter(){}


    /**
     * @brief Getter for config file configuration path
     * 
     * @return std::filesystem::path Path to config path
     */
    virtual std::filesystem::path getConfigFilePath() const = 0;

    /**
     * @brief Getter for OS username
     * 
     * @return std::string OS username
     */
    virtual std::string getUsername() const = 0;


    /**
     * @brief Method to open default web browser at given URL
     * 
     * @param url Page to open in web browser
     */
    virtual void openWebBrowser(const std::string& url) const = 0;


    /**
     * @brief Getter to instanciate / store valid grabber
     * 
     * @param config Huenicorn current configuration
     * @return SharedGrabber Grabber instance
     */
    SharedGrabber getGrabber(Config* config)
    {
      SharedGrabber sharedGrabber = m_grabber.lock();
      if(!sharedGrabber){
        sharedGrabber = _createGrabber(config);
        m_grabber = sharedGrabber;
      }

      return sharedGrabber;
    }


  protected:
    /**
     * @brief Factory method for grabber
     * 
     * @param config Huenicorn current configuration
     * @return SharedGrabber New grabber
     */
    virtual SharedGrabber _createGrabber(Config* config) const = 0;

    // Attributes
    WeakGrabber m_grabber;
  };
}
