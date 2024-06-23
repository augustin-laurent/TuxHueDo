#include <Huenicorn/Device.hpp>

namespace Huenicorn
{
  void from_json(const nlohmann::json& jsonDevice, Device& device)
  {
    jsonDevice.at("name").get_to(device.name);
    jsonDevice.at("archetype").get_to(device.type);
  }


  void to_json(nlohmann::json& jsonDevice, const Device& device)
  {
    jsonDevice = {
      {"id", device.id},
      {"name", device.name},
      {"type", device.type}
    };
  }


  void to_json(nlohmann::json& jsonDevices, const std::vector<Device>& devices)
  {
    jsonDevices = nlohmann::json::array();
    for(const auto& device : devices){
      jsonDevices.push_back(nlohmann::json(device));
    }
  }
}
