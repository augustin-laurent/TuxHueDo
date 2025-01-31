#include <Huenicorn/HttpRequestUtils.hpp>

#include <Huenicorn/Logger.hpp>

namespace Huenicorn
{
  std::optional<ClientImpl> HttpRequestUtils::s_client = std::nullopt;

  std::optional<IHttpClient::Response> HttpRequestUtils::sendRequest(const std::string& url, const std::string& method, const std::string& body, const Headers& headers)
  {
    try{
      return _ensureInit().sendRequest(url, method, body, headers);
    }
    catch(const std::exception& e){
      Logger::error(e.what());
      return std::nullopt;
    }
  }
}
