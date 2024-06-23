#include <Huenicorn/UV.hpp>

namespace Huenicorn
{
  void to_json(nlohmann::json& jsonUv, const UV& uv)
  {
    jsonUv = {
      {"x", uv.x},
      {"y", uv.y}
    };
  }


  void to_json(nlohmann::json& jsonUvs, const UVs& uvs)
  {
    nlohmann::json jsonMin, jsonMax;
    // Functions had to be called explicitely for mysterious reason...
    to_json(jsonMin, uvs.min);
    to_json(jsonMax, uvs.max);

    jsonUvs = {
      {"uvA", jsonMin},
      {"uvB", jsonMax}
    };
  }
}
