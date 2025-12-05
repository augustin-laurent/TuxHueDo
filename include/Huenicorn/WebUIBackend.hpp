#pragma once

#include <Huenicorn/IRestServer.hpp>

#include <filesystem>


namespace Huenicorn
{
  class HuenicornCore;

  /**
   * @brief REST service handling requests for light management
   * 
   */
  class WebUIBackend : public IRestServer
  {
    // Type definitions

  public:
    // Constructor
    /**
     * @brief WebUIBackend constructor
     * 
     * @param huenicornCore Pointer to Huenicorn core
     */
    WebUIBackend(HuenicornCore* huenicornCore);


  protected:
    // Overrides
    /**
     * @brief Overriden routine to prompt message
     * 
     */
    void _onStart() override;


    // Handlers
    /**
     * @brief Returns the version of the backend project
     * 
     * @param res Pending HTTP response
     */
    virtual void _getVersion(httplib::Response& res) const override;


  private:
    /**
     * @brief Handler to check the availability of the webUI
     * 
     * @param res Pending HTTP response
     */
    void _getWebUIStatus(httplib::Response& res) const;


    /**
     * @brief Handler to get available entertainment configurations
     * 
     * @param res Pending HTTP response
     */
    void _getEntertainmentConfigurations(httplib::Response& res) const;


    /**
     * @brief Handler to get channel data
     * 
     * @param res Pending HTTP response
     */
    void _getChannel(httplib::Response& res, uint8_t channelId) const;


    /**
     * @brief Handlerto get the channels data
     * 
     * @param res Pending HTTP response
     */
    void _getChannels(httplib::Response& res) const;


    /**
     * @brief Handler to get the display informations
     * 
     * @param res Pending HTTP response
     */
    void _getDisplayInfo(httplib::Response& res) const;


    /**
     * @brief Handler to get the interpolation informations
     * 
     * @param res Pending HTTP response
     */
    void _getInterpolationInfo(httplib::Response& res) const;


    /**
     * @brief Handler to set the current entertainment configuration
     * 
     * @param res Pending HTTP response
     */
    void _setEntertainmentConfiguration(const httplib::Request& req, httplib::Response& res) const;


    /**
     * @brief Handler to set the UV coordinates for a given channel
     * 
     * @param res Pending HTTP response
     */
    void _setChannelUV(const httplib::Request& req, httplib::Response& res, uint8_t channelId) const;


    /**
     * @brief Handler to set the gamma factor for a given channel
     * 
     * @param res Pending HTTP response
     */
    void _setChannelGammaFactor(const httplib::Request& req, httplib::Response& res, uint8_t channelId) const;


    /**
     * @brief Handler to set the image subsample width
     * 
     * @param res Pending HTTP response
     */
    void _setSubsampleWidth(const httplib::Request& req, httplib::Response& res) const;


    /**
     * @brief Handler to set the streaming refresh rate
     * 
     * @param res Pending HTTP response
     */
    void _setRefreshRate(const httplib::Request& req, httplib::Response& res) const;


    /**
     * @brief Handler to set the interpolation type
     * 
     * @param res Pending HTTP response
     */
    void _setInterpolation(const httplib::Request& req, httplib::Response& res) const;


    /**
     * @brief Handler to set the channel streaming state
     * 
     * @param res Pending HTTP response
     */
    void _setChannelActivity(const httplib::Request& req, httplib::Response& res, uint8_t channelId) const;


    /**
     * @brief Handler to save the user profile
     * 
     * @param res Pending HTTP response
     */
    void _saveProfile(httplib::Response& res) const;


    /**
     * @brief Handler to stop Huenicorn
     * 
     * @param res Pending HTTP response
     */
    void _stop(httplib::Response& res) const;


    // Attributes
    HuenicornCore* m_huenicornCore;
  };
}
