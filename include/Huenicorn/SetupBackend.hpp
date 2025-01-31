#pragma once

#include <Huenicorn/IRestServer.hpp>

#include <filesystem>
#include <functional>

#include <nlohmann/json.hpp>


namespace Huenicorn
{
  class HuenicornCore;

  /**
   * @brief REST service handling requests for Huenicorn initial setup
   * 
   */
  class SetupBackend : public IRestServer
  {
    using Callback = std::function<void(const nlohmann::json& data, crow::response& res)>;

  public:
    // Constructor / destructor
    /**
     * @brief SetupBackend constructor
     * 
     * @param huenicornCore Pointer to Huenicorn core
     */
    SetupBackend(HuenicornCore* huenicornCore);


    /**
     * @brief SetupBackend destructor
     * 
     */
    ~SetupBackend();


  // Getters
    /**
     * @brief Returns whether the setup was aborted or not
     * 
     * @return true Setup was aborted
     * @return false Setup was completed
     */
    bool aborted() const;


  protected:
    /**
     * @brief Overriden method to call _spawnBrowser
     * 
     */
    void _onStart() override;


  private:
    /**
     * @brief Opens user default browser on Huenicorn setup page
     * 
     */
    void _spawnBrowser();


  protected:
    // Handlers
    /**
     * @brief Returns the version of the backend project
     * 
     * @param res Pending HTTP response
     */
    virtual void _getVersion(crow::response& res) const override;

  private:
    /**
     * @brief Completes Huenicorn setup and stops setup backend server
     * 
     * @param res Pending HTTP response
     */
    void _finish(crow::response& res);


    /**
     * @brief Aborts Huenicorn setup and stops setup backend server
     * 
     * @param res Pending HTTP response
     */
    void _abort(crow::response& res);


    /**
     * @brief Handles call for Hue bridge auto-detection
     * 
     * @param res Pending HTTP response
     */
    void _autodetectBridge(crow::response& res);


    /**
     * @brief Returns the path to the config file
     * 
     * @param res Pending HTTP response
     */
    void _configFilePath(crow::response& res);


    /**
     * @brief Handles a requests for a Hue bridge address validation
     * 
     * @param req Pending HTTP request
     * @param res Pending HTTP response
     */
    void _validateBridgeAddress(const crow::request& req, crow::response& res);


    /**
     * @brief Handles a requests for Hue bridge credentials validation
     * 
     * @param req Pending HTTP request
     * @param res Pending HTTP response
     */
    void _validateCredentials(const crow::request& req, crow::response& res);


    /**
     * @brief Handles a request for a new Hue bridge user registration
     * 
     * @param res Pending HTTP response
     */
    void _registerNewUser(crow::response& res);


    // Attributes
    std::unordered_map<std::string, Callback> m_callbacks;
    HuenicornCore* m_huenicornCore;
    const std::filesystem::path m_webroot;
    std::unordered_map<std::string, std::string> m_contentTypes;
    bool m_aborted{false};
  };
}
