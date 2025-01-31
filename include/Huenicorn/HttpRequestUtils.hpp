#pragma once

#include <string>
#include <optional>

//#include <Huenicorn/IHttpClient.hpp>

#include <Huenicorn/CurlHttpClient.hpp>
namespace Huenicorn
{
  using ClientImpl = CurlHttpClient;
}

namespace Huenicorn
{
  class HttpRequestUtils
  {
  public:
    using Headers = IHttpClient::Headers;

    static std::optional<IHttpClient::Response> sendRequest(const std::string& url, const std::string& method, const std::string& body = {}, const Headers& headers = {});

  private:
    static ClientImpl& _ensureInit()
    {
      if(!s_client.has_value()){
        return s_client.emplace();
      }
      
      return s_client.value();
    }

    static std::optional<ClientImpl> s_client;
  };
}