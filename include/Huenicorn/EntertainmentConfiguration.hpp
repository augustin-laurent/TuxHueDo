#pragma once

#include <unordered_map>

#include <nlohmann/json.hpp>

#include <Huenicorn/Channel.hpp>
#include <Huenicorn/Device.hpp>


namespace Huenicorn
{
  // Type definitions
  class EntertainmentConfiguration;
  using EntertainmentConfigurations = std::unordered_map<std::string, EntertainmentConfiguration>;
  using EntertainmentConfigurationsIterator = EntertainmentConfigurations::iterator;
  using EntertainmentConfigurationEntry = std::pair<std::string, EntertainmentConfiguration>;


  /**
   * @brief Wrapper around Hue Entertainment Configuration
   * 
   */
  struct EntertainmentConfiguration
  {
    std::string name;
    Devices devices;
    Channels channels;
  };


  // Deserialization
  void from_json(const nlohmann::json& jsonEntConf, EntertainmentConfiguration& entConf);

  // Serialization
  void to_json(nlohmann::json& jsonEntConf, const EntertainmentConfiguration& entConf);
  void to_json(nlohmann::json& jsonEntConfs, const EntertainmentConfigurations& entConfs);
}
