#include <Huenicorn/SetupBackend.hpp>

#include <chrono>

#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/PlatformSelector.hpp>
#include <Huenicorn/Logger.hpp>

using namespace std::chrono_literals;

namespace Huenicorn
{
  SetupBackend::SetupBackend(HuenicornCore* huenicornCore):
  IRestServer("webroot", "setup.html"),
  m_huenicornCore(huenicornCore)
  {
    CROW_ROUTE(m_app, "/api/finishSetup").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& /*req*/, crow::response& res){
      _finish(res);
    });

    CROW_ROUTE(m_app, "/api/abort").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& /*req*/, crow::response& res){
      _abort(res);
    });

    CROW_ROUTE(m_app, "/api/autodetectBridge").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _autodetectBridge(res);
    });
  
    CROW_ROUTE(m_app, "/api/configFilePath").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _configFilePath(res);
    });

    CROW_ROUTE(m_app, "/api/validateBridgeAddress").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res){
      _validateBridgeAddress(req, res);
    });

    CROW_ROUTE(m_app, "/api/validateCredentials").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res){
      _validateCredentials(req, res);
    });

    CROW_ROUTE(m_app, "/api/registerNewUser").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& /*req*/, crow::response& res){
      _registerNewUser(res);
    });

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
    serviceUrlStream << "http://127.0.0.1:" << m_app.port();
    std::string serviceURL = serviceUrlStream.str();
    Logger::log("Setup WebUI is ready and available at ", serviceURL);

    platformAdapter.openWebBrowser(serviceURL);
  }


  void SetupBackend::_getVersion(crow::response& res) const
  {
    nlohmann::json jsonResponse = {
      {"version", m_huenicornCore->version()},
    };

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void SetupBackend::_finish(crow::response& res)
  {
    std::string response = "{}";
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();

    stop();
  }


  void SetupBackend::_abort(crow::response& res)
  {
    std::string response = "{}";
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();

    m_aborted = true;

    stop();
  }


  void SetupBackend::_autodetectBridge(crow::response& res)
  {
    nlohmann::json jsonResponse = m_huenicornCore->autodetectedBridge();
    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void SetupBackend::_configFilePath(crow::response& res)
  {
    nlohmann::json jsonResponse = {{"configFilePath", m_huenicornCore->configFilePath()}};
    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void SetupBackend::_validateBridgeAddress(const crow::request& req, crow::response& res)
  {
    const std::string& data = req.body;
    nlohmann::json jsonBridgeAddressData = nlohmann::json::parse(data);

    std::string bridgeAddress = jsonBridgeAddressData.at("bridgeAddress");

    nlohmann::json jsonResponse = {{"succeeded", m_huenicornCore->validateBridgeAddress(bridgeAddress)}};

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void SetupBackend::_validateCredentials(const crow::request& req, crow::response& res)
  {
    const std::string& data = req.body;
    nlohmann::json jsonCredentials = nlohmann::json::parse(data);

    Credentials credentials(jsonCredentials.at("username"), jsonCredentials.at("clientkey"));

    nlohmann::json jsonResponse = {{"succeeded", m_huenicornCore->validateCredentials(credentials)}};

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void SetupBackend::_registerNewUser(crow::response& res)
  {
    nlohmann::json jsonResponse = m_huenicornCore->registerNewUser();
    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }
}
