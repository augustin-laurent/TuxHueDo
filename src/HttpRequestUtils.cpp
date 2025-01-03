#include <Huenicorn/HttpRequestUtils.hpp>


namespace Huenicorn
{
  std::optional<ClientImpl> HttpRequestUtils::s_client = std::nullopt;

  nlohmann::json HttpRequestUtils::sendRequest(const std::string& url, const std::string& method, const std::string& body, const Headers& headers)
  {
    return _ensureInit().sendRequest(url, method, body, headers);
  }
}
