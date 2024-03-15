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
    virtual void _getVersion(crow::response& res) const override;


  private:
    /**
     * @brief Handler to check the availability of the webUI
     * 
     * @param res Pending HTTP response
     */
    void _getWebUIStatus(crow::response& res) const;


    /**
     * @brief Handler to get available entertainment configurations
     * 
     * @param res Pending HTTP response
     */
    void _getEntertainmentConfigurations(crow::response& res) const;


    /**
     * @brief Handler to get channel data
     * 
     * @param res Pending HTTP response
     */
    void _getChannel(crow::response& res, uint8_t channelId) const;


    /**
     * @brief Handlerto get the channels data
     * 
     * @param res Pending HTTP response
     */
    void _getChannels(crow::response& res) const;


    /**
     * @brief Handler to get the display informations
     * 
     * @param res Pending HTTP response
     */
    void _getDisplayInfo(crow::response& res) const;


    /**
     * @brief Handler to get the interpolation informations
     * 
     * @param res Pending HTTP response
     */
    void _getInterpolationInfo(crow::response& res) const;


    /**
     * @brief Handler to set the current entertainment configuration
     * 
     * @param res Pending HTTP response
     */
    void _setEntertainmentConfiguration(const crow::request& req, crow::response& res) const;


    /**
     * @brief Handler to set the UV coordinates for a given channel
     * 
     * @param res Pending HTTP response
     */
    void _setChannelUV(const crow::request& req, crow::response& res, uint8_t channelId) const;


    /**
     * @brief Handler to set the gamma factor for a given channel
     * 
     * @param res Pending HTTP response
     */
    void _setChannelGammaFactor(const crow::request& req, crow::response& res, uint8_t channelId) const;


    /**
     * @brief Handler to set the image subsample width
     * 
     * @param res Pending HTTP response
     */
    void _setSubsampleWidth(const crow::request& req, crow::response& res) const;


    /**
     * @brief Handler to set the streaming refresh rate
     * 
     * @param res Pending HTTP response
     */
    void _setRefreshRate(const crow::request& req, crow::response& res) const;


    /**
     * @brief Handler to set the interpolation type
     * 
     * @param res Pending HTTP response
     */
    void _setInterpolation(const crow::request& req, crow::response& res) const;


    /**
     * @brief Handler to set the channel streaming state
     * 
     * @param res Pending HTTP response
     */
    void _setChannelActivity(const crow::request& req, crow::response& res, uint8_t channelId) const;


    /**
     * @brief Handler to save the user profile
     * 
     * @param res Pending HTTP response
     */
    void _saveProfile(crow::response& res) const;


    /**
     * @brief Handler to stop Huenicorn
     * 
     * @param res Pending HTTP response
     */
    void _stop(crow::response& res) const;


    // Attributes
    HuenicornCore* m_huenicornCore;
  };
}
