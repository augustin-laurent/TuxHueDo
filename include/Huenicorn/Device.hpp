#pragma once

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace Huenicorn
{
  /**
   * @brief Device data structure
   * 
   */
  struct Device
  {
    std::string id;
    std::string name;
    std::string type;
  };

  using Devices = std::unordered_map<std::string, Device>;

  // Deserialization
  void from_json(const nlohmann::json& jsonDevice, Device& device);

  // Serialization
  void to_json(nlohmann::json& jsonDevice, const Device& device);
  void to_json(nlohmann::json& jsonDevices, const std::vector<Device>& devices);
}
