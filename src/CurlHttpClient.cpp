#include <Huenicorn/CurlHttpClient.hpp>

#include <stdexcept>

#include <curl/easy.h>

#include <Huenicorn/Logger.hpp>


namespace Huenicorn
{
  CurlHttpClient::UniqueCurlHandle CurlHttpClient::s_handle = nullptr;

  size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
  }


  void CurlHttpClient::_ensureInitialisation()
  {
    if(s_handle){
      return;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    s_handle = std::unique_ptr<CURL, CurlDeleter>(curl_easy_init());
  }


  nlohmann::json CurlHttpClient::sendRequest(const std::string& url, const std::string& method, const std::string& body, const Headers& headers)
  {
    _ensureInitialisation();

    nlohmann::json jsonBody = nlohmann::json::object();

    curl_easy_setopt(s_handle.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(s_handle.get(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(s_handle.get(), CURLOPT_TIMEOUT, 1);


    if(body.size() > 0){
      curl_easy_setopt(s_handle.get(), CURLOPT_POSTFIELDS, body.c_str());
      curl_easy_setopt(s_handle.get(), CURLOPT_POSTFIELDSIZE, body.length());
    }

    curl_slist* concatenatedHeaders = nullptr;
    if(headers.size() > 0){
      // Disable ssl checks for the sake of getting data without trouble
      curl_easy_setopt(s_handle.get(), CURLOPT_SSL_VERIFYPEER, false);
      curl_easy_setopt(s_handle.get(), CURLOPT_SSL_VERIFYHOST, false);

      for(const auto& header : headers){
        std::string concat = header.first + ": " + header.second;
        concatenatedHeaders = curl_slist_append(concatenatedHeaders, concat.c_str());
      }

      curl_easy_setopt(s_handle.get(), CURLOPT_HTTPHEADER, concatenatedHeaders);
    }

    std::string responseString;
    curl_easy_setopt(s_handle.get(), CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(s_handle.get(), CURLOPT_WRITEDATA, &responseString);

    nlohmann::json jsonResponse = {};
    try{
      CURLcode code = curl_easy_perform(s_handle.get());

      if(code != 0){
        Logger::error("HTTP request failed");
        jsonResponse["errors"] = {{}};
        return jsonResponse;
      }

      jsonResponse = nlohmann::json::parse(responseString);

      curl_slist_free_all(concatenatedHeaders);
    }
    catch(const nlohmann::json::exception& e){
      Logger::error(e.what());
    }

    return jsonResponse;
  }
}
