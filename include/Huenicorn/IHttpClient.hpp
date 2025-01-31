#pragma once

#include <string>
#include <map>
#include <optional>

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

    class Response
    {
    public:
      Response(const std::string& response):
      m_response(response)
      {}

      const std::string& asString() const
      {
        return m_response;
      }

      const nlohmann::json asJson() const
      {
        return nlohmann::json::parse(m_response);
      }

    private:
      const std::string m_response{};
    };

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
    virtual std::optional<Response> sendRequest(const std::string& url, const std::string& method, const std::string& body = "", const Headers& headers = {}) = 0;
  };
}

