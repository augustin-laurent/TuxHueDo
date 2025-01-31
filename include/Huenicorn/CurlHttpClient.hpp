#pragma once

#include <Huenicorn/IHttpClient.hpp>

#include <string>
#include <memory>

#include <curl/curl.h>


namespace Huenicorn
{
  /**
   * @brief Provides an abstraction around HTTP requests and returns JSON data structs
   * 
   */
  class CurlHttpClient : public IHttpClient
  {
    class CurlDeleter
    {
    public:
      void operator()(CURL* curl) const
      {
        curl_easy_cleanup(curl);
      }
    };

    struct CurlSlistDeleter
    {
      void operator()(curl_slist* slist) const
      {
        curl_slist_free_all(slist);
      }
    };


    using UniqueCurlHandle = std::unique_ptr<CURL, CurlDeleter>;
    using UniqueCurlSlist = std::unique_ptr<curl_slist, CurlSlistDeleter>;


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
    virtual std::optional<Response> sendRequest(const std::string& url, const std::string& method, const std::string& body = "", const Headers& headers = {}) override;
  };
}
