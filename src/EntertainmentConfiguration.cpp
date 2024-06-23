#include <Huenicorn/EntertainmentConfiguration.hpp>


namespace Huenicorn
{
  void from_json(const nlohmann::json& jsonEntConf, EntertainmentConfiguration& entConf)
  {
    jsonEntConf.at("metadata").at("name").get_to(entConf.name);

    const auto& lightServices = jsonEntConf.at("light_services");
    for (const auto& lightService : lightServices) {
      std::string lightId = lightService.at("rid");
      Device device;
      device.id = lightId; // Initialize with default value
      entConf.devices.insert({lightId, device});
    }
  }


  void to_json(nlohmann::json& jsonEntConf, const EntertainmentConfiguration& entConf)
  {
    jsonEntConf = {
      {"name", entConf.name},
      //{"devices", nlohmann::json(entertainmentConfiguration.devices())},
      //{"channels", nlohmann::json(entertainmentConfiguration.channels())},
    };
  }


  void to_json(nlohmann::json& jsonEntConfs, const EntertainmentConfigurations& entConfs)
  {
    jsonEntConfs = nlohmann::json::array();
    for(const auto& entertainmentConfiguration : entConfs){
      auto& it = jsonEntConfs.emplace_back(nlohmann::json(entertainmentConfiguration.second));
      it["entertainmentConfigurationId"] = entertainmentConfiguration.first;
    }
  }
}
