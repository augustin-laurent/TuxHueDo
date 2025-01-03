#pragma once

#include <string>

#include <Huenicorn/IHttpClient.hpp>

//#ifdef USE_CURL
  #include <Huenicorn/CurlHttpClient.hpp>
  namespace Huenicorn
  {
    using ClientImpl = CurlHttpClient;
  }
//#elif defined(USE_CURLPP)
//#else
//  #error "No HTTP client implementation defined"
//#endif

namespace Huenicorn
{
  class HttpRequestUtils
  {
  public:
    using Headers = IHttpClient::Headers;

    static nlohmann::json sendRequest(const std::string& url, const std::string& method, const std::string& body = {}, const Headers& headers = {});

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