#pragma once

#include <filesystem>
#include <fstream>
#include <future>
#include <optional>
#include <unordered_set>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <crow.h>
#pragma GCC diagnostic pop


namespace Huenicorn
{
  /**
   * @brief Abstract class to implement for REST service
   * 
   */
  class IRestServer
  {
  public:
    // Constructor / Destructor
    /**
     * @brief IRestServer constructor
     * 
     * @param webRoot Path to the web root directory
     */
    IRestServer(const std::filesystem::path& webRoot, const std::string& indexFile = "index.html"):
    m_webroot(webRoot),
    m_indexFile(indexFile)
    {
      m_contentTypes = {
        {".js", "text/javascript"},
        {".html", "text/html"},
        {".css", "text/css"},
        {".svg", "image/svg+xml"}
      };

      m_app.loglevel(crow::LogLevel::Warning);

      CROW_ROUTE(m_app, "/").methods(crow::HTTPMethod::GET)
      ([this](const crow::request& /*req*/, crow::response& res){
        _getWebFile(m_indexFile, res);
      });

      CROW_ROUTE(m_app, "/api/version").methods(crow::HTTPMethod::GET)
      ([this](const crow::request& /*req*/, crow::response& res){
        _getVersion(res);
      });

      CROW_ROUTE(m_app, "/<string>").methods(crow::HTTPMethod::GET)
      ([this](const crow::request& /*req*/, crow::response& res, const std::string& param){
        return _getWebFile(param, res);
      });
    }


    /**
     * @brief IRestServer destructor
     * 
     */
    virtual ~IRestServer(){}


    // Getters
    /**
     * @brief Returns whether the REST server is running or not
     * 
     * @return true REST server is running
     * @return false REST server is not running
     */
    bool running() const
    {
      return m_running;
    }


    // Methods
    /**
     * @brief Starts the REST server and triggers _onStart event
     * 
     * @param port Listening port of the REST server
     * @param boundBackendIP IP to bind the backend to
     * @return true REST server started successfully
     * @return false REST server is already running
     */
    bool start(unsigned port, const std::string& boundBackendIP)
    {
      if(running()){
        return false;
      }

      m_app.bindaddr(boundBackendIP);
      m_app.port(port);
      m_app.signal_clear();

      m_running = true;

      _onStart();
      m_app.run();

      return true;
    }


    /**
     * @brief (Overload) Starts the REST server and triggers _onStart event
     * 
     * @param port Listening port of the REST server
     * @param boundBackendIP IP to bind the backend to
     * @param readyPromise Promise to notify readyness to the parent thread
     * @return true REST server started successfully
     * @return false REST server is already running
     */
    bool start(unsigned port, const std::string& boundBackendIP, std::promise<bool>&& readyPromise)
    {
      m_readyWebUIPromise.emplace(std::move(readyPromise));
      return start(port, boundBackendIP);
    }


    /**
     * @brief Stops the REST server
     * 
     * @return true 
     * @return false 
     */
    bool stop()
    {
      return _stop();
    }

  private:
    // Private method


    /**
     * @brief Stops REST server and triggers _onStop event
     * 
     * @return true REST server stopped successfully
     * @return false REST server was not running
     */
    bool _stop()
    {
      if(!running()){
        return false;
      }

      m_app.stop();
      m_running = false;
      _onStop();

      return true;
    }

  protected:
    // Protected methods

    /**
     * @brief Overridable routine triggering at server start
     * 
     */
    virtual void _onStart(){}


    /**
     * @brief Overridable routine triggering at server stop
     * 
     */
    virtual void _onStop(){}


    // Handlers
    /**
     * @brief Sends the version of the backend project
     * 
     * @param res Pending HTTP connection
     */
    virtual void _getVersion(crow::response& res) const = 0;


    /**
     * @brief Web files handler
     * 
     * @param res Pending HTTP connection
     */
    void _getWebFile(const std::string& fileName, crow::response& res) const
    {
      std::filesystem::path webFileName = fileName;
      if(webFileName == ""){
        webFileName = m_indexFile;
      }

      std::filesystem::path webFileFullPath = m_webroot / webFileName;

      if(!std::filesystem::exists(m_webroot)){
        std::string response = "<h1>Error : Could not locate webroot</h1><p>Make sure that the webroot directory figures in the current working directory</p>";
        std::string contentType = "text/html";

        res.set_header("Content-Type", contentType);
        res.write(response);
        res.end();
        return;
      }

      if(!std::filesystem::exists(webFileFullPath) || m_webfileBlackList.contains(webFileName)){
        webFileName = "404.html";
        webFileFullPath = m_webroot / webFileName;
      }

      std::string extension = webFileName.extension();
      std::string contentType = "text/plain";

      if(m_contentTypes.find(extension) != m_contentTypes.end()){
        contentType = m_contentTypes.at(extension);
      }

      std::ifstream webFile(webFileFullPath);
      std::string response = std::string(std::istreambuf_iterator<char>(webFile), std::istreambuf_iterator<char>());

      res.set_header("Content-Type", contentType);
      res.write(response);
      res.end();
    }


    // Attributes
    const std::filesystem::path m_webroot;
    std::string m_indexFile;
    bool m_running{false};
    crow::SimpleApp m_app;
    std::unordered_map<std::string, std::string> m_contentTypes;
    std::unordered_set<std::string> m_webfileBlackList;
    std::optional<std::promise<bool>> m_readyWebUIPromise;
  };
}
