#pragma once

#include <string>
#include <map>

#include <nlohmann/json.hpp>

namespace Huenicorn
{
  /**
   * @brief Provides an abstraction around HTTP requests and returns JSON data structs
   * 
   */
  class IHttpClient
  {
  public:
    using Headers = std::multimap<std::string, std::string>;

    virtual ~IHttpClient(){}

    /**
     * @brief Performs a HTTP(S) request and returns a JSON response
     * 
     * @param url Target URL
     * @param method HTTP method
     * @param body HTTP request body
     * @param headers HTTP request headers
     * @return nlohmann::json JSON response
     */
    virtual nlohmann::json sendRequest(const std::string& url, const std::string& method, const std::string& body = "", const Headers& headers = {}) = 0;
  };
}

