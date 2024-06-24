#include <Huenicorn/ApiTools.hpp>

#include <nlohmann/json.hpp>

#include <Huenicorn/RequestUtils.hpp>


namespace Huenicorn
{
  namespace ApiTools
  {
    EntertainmentConfigurations loadEntertainmentConfigurations(const std::string& username, const std::string& bridgeAddress)
    {
      // I don't always abbreviate variable names
      // but when I do, it's because I don't have a 32:9 monitor
      // (If someones has such a display, please tell me about Huenicorn's performance)
      EntertainmentConfigurations entConfs;

      RequestUtils::Headers headers = {{"hue-application-key", username}};
      std::string entConfUrl = "https://" + bridgeAddress + "/clip/v2/resource/entertainment_configuration";
      auto entConfResponse = RequestUtils::sendRequest(entConfUrl, "GET", "", headers);

      if(entConfResponse.at("errors").size() == 0){
        // Listing entertainment configurations
        for(const nlohmann::json& jsonEntConf : entConfResponse.at("data")){

          EntertainmentConfiguration entConf = jsonEntConf.get<EntertainmentConfiguration>();

          for(auto& [lightId, device] : entConf.devices){
            std::string lightUrl = "https://" + bridgeAddress + "/clip/v2/resource/light/" + lightId;

            auto jsonLightData = RequestUtils::sendRequest(lightUrl, "GET", "", headers);
            auto deviceId = device.id;
            device = jsonLightData.at("data").at(0).at("metadata").get<Device>();
            device.id = deviceId;
          }

          const auto& jsonChannels = jsonEntConf.at("channels");
          for (const auto& jsonChannel : jsonChannels) {
            uint8_t channelId = jsonChannel.at("channel_id").get<uint8_t>();
            entConf.channels.insert({channelId, Channel{false, {}, 0.f}});
          }

          entConfs.insert({jsonEntConf.at("id").get<std::string>(), entConf});
        }
      }

      return entConfs;
    }


    Devices loadDevices(const std::string& username, const std::string& bridgeAddress)
    {
      RequestUtils::Headers headers = {{"hue-application-key", username}};
      std::string resourceUrl = "https://" + bridgeAddress + "/clip/v2/resource";
      auto jsonResource = RequestUtils::sendRequest(resourceUrl, "GET", "", headers);

      Devices devices;

      for(const auto& jsonData : jsonResource.at("data")){
        if(jsonData.at("type") == "device"){
          const auto& jsonServices = jsonData.at("services");

          for(const auto& service : jsonServices){
            if(service.at("rtype") == "entertainment"){
              std::string deviceId = service.at("rid");

              auto device = jsonData.at("metadata").get<Device>();
              device.id = deviceId;
              devices.emplace(deviceId, device);
            }
          }
        }
      }

      return devices;
    }


    EntertainmentConfigurationsChannels loadEntertainmentConfigurationsChannels(const std::string& username, const std::string& bridgeAddress)
    {
      RequestUtils::Headers headers = {{"hue-application-key", username}};
      std::string resourceUrl = "https://" + bridgeAddress + "/clip/v2/resource/entertainment_configuration";

      auto jsonEntertainmentConfigurations = RequestUtils::sendRequest(resourceUrl, "GET", "", headers);

      EntertainmentConfigurationsChannels entConfsChannels;

      for(const auto& entConf : jsonEntertainmentConfigurations.at("data")){
        std::string configurationId = entConf.at("id");
        for(const auto& jsonChannel : entConf.at("channels")){
          uint8_t channelId = jsonChannel.at("channel_id");
          for(const auto& jsonMember : jsonChannel.at("members")){
            std::string jsonMemberId = jsonMember.at("service").at("rid");
            entConfsChannels[configurationId][channelId].push_back(jsonMemberId);
          }
        }
      }

      return entConfsChannels;
    }


    std::vector<Device> matchDevices(const MembersIds& membersIds, const Devices& devices)
    {
      std::vector<Device> matchedDevices;
      for(const auto& memberId : membersIds){
        const auto& it = devices.find(memberId);
        matchedDevices.push_back(it->second);
      }

      return matchedDevices;
    }


    void setStreamingState(const EntertainmentConfigurationEntry& entertainmentConfigurationEntry, const std::string& username, const std::string& bridgeAddress, bool active)
    {
      nlohmann::json jsonBody = {
        {"action", active ? "start" : "stop"},
        {"metadata", {{"name", entertainmentConfigurationEntry.second.name}}}
      };

      RequestUtils::Headers headers = {{"hue-application-key", username}};

      std::string url = "https://" + bridgeAddress + "/clip/v2/resource/entertainment_configuration/" + entertainmentConfigurationEntry.first;

      RequestUtils::sendRequest(url, "PUT", jsonBody.dump(), headers);
    }


    bool streamingActive(const EntertainmentConfigurationEntry& entertainmentConfigurationEntry, const std::string& username, const std::string& bridgeAddress)
    {
      std::string status;

      RequestUtils::Headers headers = {{"hue-application-key", username}};
      std::string url = "https://" + bridgeAddress + "/clip/v2/resource/entertainment_configuration/" + entertainmentConfigurationEntry.first;
      auto response = RequestUtils::sendRequest(url, "GET", "", headers);
      if(response.at("errors").size() == 0){
        status = response.at("data").front().at("status");
      }

      return status == "active";
    }
  }
}
