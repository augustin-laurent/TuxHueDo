#pragma once

#include <string>
#include <memory>

#include <nlohmann/json.hpp>
#include <curl/curl.h>


namespace Huenicorn
{
  /**
   * @brief Provides an abstraction around HTTP requests and returns JSON data structs
   * 
   */
  class RequestUtils
  {
    class CurlDeleter
    {
    public:
      void operator()(CURL* curl) const
      {
        curl_easy_cleanup(curl);
      }
    };


    using UniqueCurlHandle = std::unique_ptr<CURL, CurlDeleter>;

  public:
    using Headers = std::multimap<std::string, std::string>;

    /**
     * @brief Performs a HTTP(S) request and returns a JSON response
     * 
     * @param url Target URL
     * @param method HTTP method
     * @param body HTTP request body
     * @param headers HTTP request headers
     * @return nlohmann::json JSON response
     */
    static nlohmann::json sendRequest(const std::string& url, const std::string& method, const std::string& body = "", const Headers& headers = {});


  private:
    static void _ensureInitialisation();

    static UniqueCurlHandle s_handle;
  };
}
