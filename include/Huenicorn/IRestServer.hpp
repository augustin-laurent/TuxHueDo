#pragma once

#include <filesystem>
#include <fstream>
#include <future>
#include <optional>
#include <thread>
#include <unordered_set>
#include <atomic>

#include <httplib.h>

#include <Huenicorn/EmbeddedWebrootFiles.hpp>

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
     * @param indexFile Default file to show
     */
    IRestServer(const std::string& indexFile = "index.html"):
    m_indexFile(indexFile)
    {
      m_contentTypes = {
        {".js", "text/javascript"},
        {".html", "text/html"},
        {".css", "text/css"},
        {".svg", "image/svg+xml"}
      };

      // Route for index
      m_server.Get("/", [this](const httplib::Request& /*req*/, httplib::Response& res){
        _getWebFile(m_indexFile, res);
      });

      // Route for version API
      m_server.Get("/api/version", [this](const httplib::Request& /*req*/, httplib::Response& res){
        _getVersion(res);
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
      return m_running.load();
    }


    /**
     * @brief Returns the port the server is listening on
     * 
     * @return unsigned Port number
     */
    unsigned port() const
    {
      return m_port;
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

      m_port = port;
      m_boundIP = boundBackendIP;
      m_running.store(true);

      _onStart();
      
      // This is blocking - runs until server is stopped
      m_server.listen(boundBackendIP, static_cast<int>(port));
      
      m_running.store(false);
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

      m_server.stop();
      m_running.store(false);
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


    /**
     * @brief Registers a static file route
     * 
     * @param filename Name of the file to serve
     */
    void _registerStaticFile(const std::string& filename)
    {
      std::string route = "/" + filename;
      m_server.Get(route, [this, filename](const httplib::Request& /*req*/, httplib::Response& res){
        _getWebFile(filename, res);
      });
    }


    // Handlers
    /**
     * @brief Sends the version of the backend project
     * 
     * @param res Pending HTTP connection
     */
    virtual void _getVersion(httplib::Response& res) const = 0;


    /**
     * @brief Web files handler
     * 
     * @param res Pending HTTP connection
     */
    void _getWebFile(const std::string& filePath, httplib::Response& res) const
    {
      std::filesystem::path webFilePath = filePath;
      if(webFilePath.empty()){
        webFilePath = m_indexFile;
      }

      std::filesystem::path webFileFullPath = webFilePath;

      if(Webroot::embeddedFiles.find(webFileFullPath) == Webroot::embeddedFiles.end() || m_webfileBlackList.contains(webFilePath)){
        webFileFullPath = "404.html";
      }

      std::string extension = webFileFullPath.extension();
      std::string contentType = "text/plain";

      if(m_contentTypes.find(extension) != m_contentTypes.end()){
        contentType = m_contentTypes.at(extension);
      }

      auto it = Webroot::embeddedFiles.find(webFileFullPath);
      if(it != Webroot::embeddedFiles.end()){
        res.set_content(it->second, contentType);
      } else {
        res.status = 404;
        res.set_content("Not Found", "text/plain");
      }
    }


    // Attributes
    std::string m_indexFile;
    std::atomic<bool> m_running{false};
    unsigned m_port{0};
    std::string m_boundIP;
    httplib::Server m_server;
    std::unordered_map<std::string, std::string> m_contentTypes;
    std::unordered_set<std::string> m_webfileBlackList;
    std::optional<std::promise<bool>> m_readyWebUIPromise;
  };
}
