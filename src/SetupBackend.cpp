#include <Huenicorn/SetupBackend.hpp>

#include <chrono>

#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/PlatformSelector.hpp>
#include <Huenicorn/Logger.hpp>

using namespace std::chrono_literals;

namespace Huenicorn
{
  SetupBackend::SetupBackend(HuenicornCore* huenicornCore):
  IRestServer("setup.html"),
  m_huenicornCore(huenicornCore)
  {
    m_server.Post("/api/finishSetup",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _finish(res);
    });

    m_server.Post("/api/abort",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _abort(res);
    });

    m_server.Get("/api/autodetectBridge",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _autodetectBridge(res);
    });
  
    m_server.Get("/api/configFilePath",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _configFilePath(res);
    });

    m_server.Put("/api/validateBridgeAddress",
    [this](const httplib::Request& req, httplib::Response& res){
      _validateBridgeAddress(req, res);
    });

    m_server.Put("/api/validateCredentials",
    [this](const httplib::Request& req, httplib::Response& res){
      _validateCredentials(req, res);
    });

    m_server.Put("/api/registerNewUser",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _registerNewUser(res);
    });

    // Register static files
    _registerStaticFile("setup.html");
    _registerStaticFile("index.html");
    _registerStaticFile("404.html");
    _registerStaticFile("style.css");
    _registerStaticFile("Channel.js");
    _registerStaticFile("Rainbow.js");
    _registerStaticFile("ScreenWidget.js");
    _registerStaticFile("Utils.js");
    _registerStaticFile("Version.js");
    _registerStaticFile("WebUI.js");
    _registerStaticFile("mainSetup.js");
    _registerStaticFile("mainWebUI.js");
    _registerStaticFile("logo.svg");

    m_webfileBlackList.insert("index.html");
  }


  SetupBackend::~SetupBackend()
  {}


  bool SetupBackend::aborted() const
  {
    return m_aborted;
  }


  void SetupBackend::_onStart()
  {
    std::thread spawnBrowserThread([this](){_spawnBrowser();});
    spawnBrowserThread.detach();
  }


  void SetupBackend::_spawnBrowser()
  {
    while (!running()){
      std::this_thread::sleep_for(100ms);
    }

    std::stringstream serviceUrlStream;
    serviceUrlStream << "http://127.0.0.1:" << port();
    std::string serviceURL = serviceUrlStream.str();
    Logger::log("Setup WebUI is ready and available at ", serviceURL);

    platformAdapter.openWebBrowser(serviceURL);
  }


  void SetupBackend::_getVersion(httplib::Response& res) const
  {
    nlohmann::json jsonResponse = {
      {"version", m_huenicornCore->version()},
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void SetupBackend::_finish(httplib::Response& res)
  {
    std::string response = "{}";
    res.set_content(response, "application/json");

    stop();
  }


  void SetupBackend::_abort(httplib::Response& res)
  {
    std::string response = "{}";
    res.set_content(response, "application/json");

    m_aborted = true;

    stop();
  }


  void SetupBackend::_autodetectBridge(httplib::Response& res)
  {
    nlohmann::json jsonResponse = m_huenicornCore->autodetectedBridge();
    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void SetupBackend::_configFilePath(httplib::Response& res)
  {
    nlohmann::json jsonResponse = {{"configFilePath", m_huenicornCore->configFilePath()}};
    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void SetupBackend::_validateBridgeAddress(const httplib::Request& req, httplib::Response& res)
  {
    const std::string& data = req.body;
    nlohmann::json jsonBridgeAddressData = nlohmann::json::parse(data);

    std::string bridgeAddress = jsonBridgeAddressData.at("bridgeAddress");

    nlohmann::json jsonResponse = {{"succeeded", m_huenicornCore->validateBridgeAddress(bridgeAddress)}};

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void SetupBackend::_validateCredentials(const httplib::Request& req, httplib::Response& res)
  {
    const std::string& data = req.body;
    nlohmann::json jsonCredentials = nlohmann::json::parse(data);

    Credentials credentials(jsonCredentials.at("username"), jsonCredentials.at("clientkey"));

    nlohmann::json jsonResponse = {{"succeeded", m_huenicornCore->validateCredentials(credentials)}};

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void SetupBackend::_registerNewUser(httplib::Response& res)
  {
    nlohmann::json jsonResponse = m_huenicornCore->registerNewUser();
    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }
}
