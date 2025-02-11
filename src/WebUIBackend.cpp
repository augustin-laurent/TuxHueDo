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
    CROW_ROUTE(m_app, "/api/webUIStatus").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _getWebUIStatus(res);
    });

    CROW_ROUTE(m_app, "/api/entertainmentConfigurations").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _getEntertainmentConfigurations(res);
    });

    CROW_ROUTE(m_app, "/api/channel/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res, int channelId){
      _getChannel(res, channelId);
    });

    CROW_ROUTE(m_app, "/api/channels").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _getChannels(res);
    });

    CROW_ROUTE(m_app, "/api/displayInfo").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _getDisplayInfo(res);
    });

    CROW_ROUTE(m_app, "/api/interpolationInfo").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& /*req*/, crow::response& res){
      _getInterpolationInfo(res);
    });

    CROW_ROUTE(m_app, "/api/setEntertainmentConfiguration").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res){
      _setEntertainmentConfiguration(req, res);
    });

    CROW_ROUTE(m_app, "/api/setChannelUV/<int>").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res, int channelId){
      _setChannelUV(req, res, channelId);
    });

    CROW_ROUTE(m_app, "/api/setChannelGammaFactor/<int>").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res, int channelId){
      _setChannelGammaFactor(req, res, channelId);
    });

    CROW_ROUTE(m_app, "/api/setSubsampleWidth").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res){
      _setSubsampleWidth(req, res);
    });

    CROW_ROUTE(m_app, "/api/setRefreshRate").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res){
      _setRefreshRate(req, res);
    });

    CROW_ROUTE(m_app, "/api/setInterpolation").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, crow::response& res){
      _setInterpolation(req, res);
    });

    CROW_ROUTE(m_app, "/api/setChannelActivity/<int>").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req, crow::response& res, int channelId){
      _setChannelActivity(req, res, channelId);
    });

    CROW_ROUTE(m_app, "/api/saveProfile").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& /*req*/, crow::response& res){
      _saveProfile(res);
    });

    CROW_ROUTE(m_app, "/api/stop").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& /*req*/, crow::response& res){
      _stop(res);
    });

    m_webfileBlackList.insert("setup.html");
  }


  void WebUIBackend::_onStart()
  {
    std::stringstream ss;
    ss << "Huenicorn management panel is now available at http://localhost:" <<  m_app.port();
    Logger::log(ss.str());

    if(m_readyWebUIPromise.has_value()){
      m_readyWebUIPromise.value().set_value(true);
    }
  }


  void WebUIBackend::_getVersion(crow::response& res) const
  {
    nlohmann::json jsonResponse = {
      {"version", m_huenicornCore->version()},
    };

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_getWebUIStatus(crow::response& res) const
  {
    nlohmann::json jsonResponse = {
      {"ready", true},
    };

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_getEntertainmentConfigurations(crow::response& res) const
  {
    auto entertainmentConfigurations = nlohmann::json(m_huenicornCore->entertainmentConfigurations());
    std::string currentEntertainmentConfigurationId = m_huenicornCore->currentEntertainmentConfigurationId().value();

    nlohmann::json jsonResponse = {
      {"entertainmentConfigurations", entertainmentConfigurations},
      {"currentEntertainmentConfigurationId", currentEntertainmentConfigurationId}
    };

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_getChannel(crow::response& res, uint8_t channelId) const
  {
    std::string response = nlohmann::json(m_huenicornCore->channels().at(channelId)).dump();

    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_getChannels(crow::response& res) const
  {
    std::string response = nlohmann::json(m_huenicornCore->channels()).dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_getDisplayInfo(crow::response& res) const
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
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_getInterpolationInfo(crow::response& res) const
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
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setEntertainmentConfiguration(const crow::request& req, crow::response& res) const
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
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setChannelUV(const crow::request& req, crow::response& res, uint8_t channelId) const
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
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setChannelGammaFactor(const crow::request& req, crow::response& res, uint8_t channelId) const
  {
    const std::string& data = req.body;
    nlohmann::json jsonGammaFactorData = nlohmann::json::parse(data);
    float gammaFactor = jsonGammaFactorData.at("gammaFactor");

    if(!m_huenicornCore->setChannelGammaFactor(channelId, gammaFactor)){
      std::string response = nlohmann::json{
        {"succeeded", false},
        {"error", "invalid channel id"}
      }.dump();
      
      res.set_header("Content-Type", "application/json");
      res.write(response);
      res.end();
      return;
    }

    nlohmann::json jsonResponse = nlohmann::json{
      {"succeeded", true},
      {"gammaFactor", gammaFactor}
    };

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setSubsampleWidth(const crow::request& req, crow::response& res) const
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
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setRefreshRate(const crow::request& req, crow::response& res) const
  {
    const std::string& data = req.body;
    unsigned refreshRate = nlohmann::json::parse(data).get<unsigned>();
    m_huenicornCore->setRefreshRate(refreshRate);

    nlohmann::json jsonRefreshRate{
      {"refreshRate", m_huenicornCore->refreshRate()}
    };

    std::string response = jsonRefreshRate.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setInterpolation(const crow::request& req, crow::response& res) const
  {
    const std::string& data = req.body;
    unsigned interpolation = nlohmann::json::parse(data).get<unsigned>();

    m_huenicornCore->setInterpolation(interpolation);

    nlohmann::json jsonInterpolation{
      {"interpolation", m_huenicornCore->interpolation()}
    };

    std::string response = jsonInterpolation.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_setChannelActivity(const crow::request& req, crow::response& res, uint8_t channelId) const
  {
    const std::string& data = req.body;
    nlohmann::json jsonChannelData = nlohmann::json::parse(data);
    bool active = jsonChannelData.at("active");

    if(!m_huenicornCore->setChannelActivity(channelId, active)){
      std::string response = nlohmann::json{
        {"succeeded", false},
        {"error", "invalid channel id"}
      }.dump();
      
      res.set_header("Content-Type", "application/json");
      res.write(response);
      res.end();
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
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_saveProfile(crow::response& res) const
  {
    m_huenicornCore->saveProfile();

    nlohmann::json jsonResponse = {
      "succeeded", true
    };

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();
  }


  void WebUIBackend::_stop(crow::response& res) const
  {
    nlohmann::json jsonResponse = {{
      "succeeded", true
    }};

    std::string response = jsonResponse.dump();
    res.set_header("Content-Type", "application/json");
    res.write(response);
    res.end();

    m_huenicornCore->stop();
  }
}
