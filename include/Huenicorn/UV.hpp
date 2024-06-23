#pragma once

#include <glm/vec2.hpp>

#include <nlohmann/json.hpp>

namespace Huenicorn
{
  using UV = glm::vec2;

  /**
   * @brief Normalized screen coordinates
   * 
   */
  struct UVs
  {
    UV min;
    UV max;
  };


  /**
   * @brief Flag to identify corner
   * 
   */
  enum UVCorner
  {
    TopLeft = 0,
    TopRight = 1,
    BottomLeft = 2,
    BottomRight = 3
  };


  // Serialization
  void to_json(nlohmann::json& jsonUv, const UV& uv);
  void to_json(nlohmann::json& jsonUvs, const UVs& uvs);
}
