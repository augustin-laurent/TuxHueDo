#include <Huenicorn/WebUIBackend.hpp>

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Logger.hpp>


namespace Huenicorn
{
  WebUIBackend::WebUIBackend(HuenicornCore* huenicornCore):
  IRestServer("index.html"),
  m_huenicornCore(huenicornCore)
  {
    m_server.Get("/api/webUIStatus",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _getWebUIStatus(res);
    });

    m_server.Get("/api/entertainmentConfigurations",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _getEntertainmentConfigurations(res);
    });

    m_server.Get(R"(/api/channel/(\d+))",
    [this](const httplib::Request& req, httplib::Response& res){
      int channelId = std::stoi(req.matches[1]);
      _getChannel(res, channelId);
    });

    m_server.Get("/api/channels",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _getChannels(res);
    });

    m_server.Get("/api/displayInfo",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _getDisplayInfo(res);
    });

    m_server.Get("/api/interpolationInfo",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _getInterpolationInfo(res);
    });

    m_server.Put("/api/setEntertainmentConfiguration",
    [this](const httplib::Request& req, httplib::Response& res){
      _setEntertainmentConfiguration(req, res);
    });

    m_server.Put(R"(/api/setChannelUV/(\d+))",
    [this](const httplib::Request& req, httplib::Response& res){
      int channelId = std::stoi(req.matches[1]);
      _setChannelUV(req, res, channelId);
    });

    m_server.Put(R"(/api/setChannelGammaFactor/(\d+))",
    [this](const httplib::Request& req, httplib::Response& res){
      int channelId = std::stoi(req.matches[1]);
      _setChannelGammaFactor(req, res, channelId);
    });

    m_server.Put("/api/setSubsampleWidth",
    [this](const httplib::Request& req, httplib::Response& res){
      _setSubsampleWidth(req, res);
    });

    m_server.Put("/api/setRefreshRate",
    [this](const httplib::Request& req, httplib::Response& res){
      _setRefreshRate(req, res);
    });

    m_server.Put("/api/setInterpolation",
    [this](const httplib::Request& req, httplib::Response& res){
      _setInterpolation(req, res);
    });

    m_server.Post(R"(/api/setChannelActivity/(\d+))",
    [this](const httplib::Request& req, httplib::Response& res){
      int channelId = std::stoi(req.matches[1]);
      _setChannelActivity(req, res, channelId);
    });

    m_server.Post("/api/saveProfile",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _saveProfile(res);
    });

    m_server.Post("/api/stop",
    [this](const httplib::Request& /*req*/, httplib::Response& res){
      _stop(res);
    });

    // Register static files
    _registerStaticFile("index.html");
    _registerStaticFile("setup.html");
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

    m_webfileBlackList.insert("setup.html");
  }


  void WebUIBackend::_onStart()
  {
    std::stringstream ss;
    ss << "Huenicorn management panel is now available at http://localhost:" << port();
    Logger::log(ss.str());

    if(m_readyWebUIPromise.has_value()){
      m_readyWebUIPromise.value().set_value(true);
    }
  }


  void WebUIBackend::_getVersion(httplib::Response& res) const
  {
    nlohmann::json jsonResponse = {
      {"version", m_huenicornCore->version()},
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_getWebUIStatus(httplib::Response& res) const
  {
    nlohmann::json jsonResponse = {
      {"ready", true},
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_getEntertainmentConfigurations(httplib::Response& res) const
  {
    auto entertainmentConfigurations = nlohmann::json(m_huenicornCore->entertainmentConfigurations());
    std::string currentEntertainmentConfigurationId = m_huenicornCore->currentEntertainmentConfigurationId().value();

    nlohmann::json jsonResponse = {
      {"entertainmentConfigurations", entertainmentConfigurations},
      {"currentEntertainmentConfigurationId", currentEntertainmentConfigurationId}
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_getChannel(httplib::Response& res, uint8_t channelId) const
  {
    std::string response = nlohmann::json(m_huenicornCore->channels().at(channelId)).dump();

    res.set_content(response, "application/json");
  }


  void WebUIBackend::_getChannels(httplib::Response& res) const
  {
    std::string response = nlohmann::json(m_huenicornCore->channels()).dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_getDisplayInfo(httplib::Response& res) const
  {
    auto displayResolution = m_huenicornCore->displayResolution();

    nlohmann::json jsonSubsampleCandidates = nlohmann::json::array();
    for(const auto& candidate : m_huenicornCore->subsampleResolutionCandidates()){
      jsonSubsampleCandidates.push_back({
        {"x", candidate.x},
        {"y", candidate.y}
      });
    }

    nlohmann::json jsonDisplayInfo{
      {"x", displayResolution.x},
      {"y", displayResolution.y},
      {"subsampleWidth", m_huenicornCore->subsampleWidth()},
      {"subsampleResolutionCandidates", jsonSubsampleCandidates},
      {"selectedRefreshRate", m_huenicornCore->refreshRate()},
      {"maxRefreshRate", m_huenicornCore->maxRefreshRate()}
    };

    std::string response = jsonDisplayInfo.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_getInterpolationInfo(httplib::Response& res) const
  {
    nlohmann::json jsonAvailableInterpolations = nlohmann::json::array();
    for(const auto& [key, value] : m_huenicornCore->availableInterpolations()){
      jsonAvailableInterpolations.push_back({
        {key, value},
      });
    }

    nlohmann::json jsonInterpolationInfo = {
      {"available", jsonAvailableInterpolations},
      {"current", m_huenicornCore->interpolation()}
    };

    std::string response = jsonInterpolationInfo.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setEntertainmentConfiguration(const httplib::Request& req, httplib::Response& res) const
  {
    const std::string& data = req.body;
    std::string entertainmentConfigurationId = nlohmann::json::parse(data);

    bool succeeded = m_huenicornCore->setEntertainmentConfiguration(entertainmentConfigurationId);

    nlohmann::json jsonResponse = {
      {"succeeded", succeeded},
      {"entertainmentConfigurationId", entertainmentConfigurationId},
      {"channels", nlohmann::json(m_huenicornCore->channels())}
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setChannelUV(const httplib::Request& req, httplib::Response& res, uint8_t channelId) const
  {
    const std::string& data = req.body;
    nlohmann::json jsonUV = nlohmann::json::parse(data);

    float x = jsonUV.at("x");
    float y = jsonUV.at("y");
    UVCorner uvCorner = static_cast<UVCorner>(jsonUV.at("type").get<int>());

    const auto& clampedUVs = m_huenicornCore->setChannelUV(channelId, {x, y}, uvCorner);

    // TODO : Serialize from JsonSerializer
    nlohmann::json jsonResponse = {
      {"uvA", {{"x", clampedUVs.min.x}, {"y", clampedUVs.min.y}}},
      {"uvB", {{"x", clampedUVs.max.x}, {"y", clampedUVs.max.y}}}
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setChannelGammaFactor(const httplib::Request& req, httplib::Response& res, uint8_t channelId) const
  {
    const std::string& data = req.body;
    nlohmann::json jsonGammaFactorData = nlohmann::json::parse(data);
    float gammaFactor = jsonGammaFactorData.at("gammaFactor");

    if(!m_huenicornCore->setChannelGammaFactor(channelId, gammaFactor)){
      std::string response = nlohmann::json{
        {"succeeded", false},
        {"error", "invalid channel id"}
      }.dump();
      
      res.set_content(response, "application/json");
      return;
    }

    nlohmann::json jsonResponse = nlohmann::json{
      {"succeeded", true},
      {"gammaFactor", gammaFactor}
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setSubsampleWidth(const httplib::Request& req, httplib::Response& res) const
  {
    const std::string& data = req.body;
    int subsampleWidth = nlohmann::json::parse(data).get<int>();
    m_huenicornCore->setSubsampleWidth(subsampleWidth);

    glm::ivec2 displayResolution = m_huenicornCore->displayResolution();
    nlohmann::json jsonDisplay{
      {"x", displayResolution.x},
      {"y", displayResolution.y},
      {"subsampleWidth", m_huenicornCore->subsampleWidth()}
    };

    std::string response = jsonDisplay.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setRefreshRate(const httplib::Request& req, httplib::Response& res) const
  {
    const std::string& data = req.body;
    unsigned refreshRate = nlohmann::json::parse(data).get<unsigned>();
    m_huenicornCore->setRefreshRate(refreshRate);

    nlohmann::json jsonRefreshRate{
      {"refreshRate", m_huenicornCore->refreshRate()}
    };

    std::string response = jsonRefreshRate.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setInterpolation(const httplib::Request& req, httplib::Response& res) const
  {
    const std::string& data = req.body;
    unsigned interpolation = nlohmann::json::parse(data).get<unsigned>();

    m_huenicornCore->setInterpolation(interpolation);

    nlohmann::json jsonInterpolation{
      {"interpolation", m_huenicornCore->interpolation()}
    };

    std::string response = jsonInterpolation.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_setChannelActivity(const httplib::Request& req, httplib::Response& res, uint8_t channelId) const
  {
    const std::string& data = req.body;
    nlohmann::json jsonChannelData = nlohmann::json::parse(data);
    bool active = jsonChannelData.at("active");

    if(!m_huenicornCore->setChannelActivity(channelId, active)){
      std::string response = nlohmann::json{
        {"succeeded", false},
        {"error", "invalid channel id"}
      }.dump();
      
      res.set_content(response, "application/json");
      return;
    }

    nlohmann::json jsonResponse = nlohmann::json{
      {"succeeded", true},
      {"channels", nlohmann::json(m_huenicornCore->channels())},
    };
    
    if(active){
      jsonResponse["newActiveChannelId"] = channelId;
    }

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_saveProfile(httplib::Response& res) const
  {
    m_huenicornCore->saveProfile();

    nlohmann::json jsonResponse = {
      "succeeded", true
    };

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");
  }


  void WebUIBackend::_stop(httplib::Response& res) const
  {
    nlohmann::json jsonResponse = {{
      "succeeded", true
    }};

    std::string response = jsonResponse.dump();
    res.set_content(response, "application/json");

    m_huenicornCore->stop();
  }
}
