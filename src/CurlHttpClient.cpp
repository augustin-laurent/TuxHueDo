#include <Huenicorn/CurlHttpClient.hpp>

#include <stdexcept>

#include <curl/easy.h>

#include <Huenicorn/Logger.hpp>


namespace Huenicorn
{
  size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
  }


  nlohmann::json CurlHttpClient::sendRequest(const std::string& url, const std::string& method, const std::string& body, const Headers& headers)
  {
    auto handle = std::unique_ptr<CURL, CurlDeleter>(curl_easy_init());
    if(!handle){
      Logger::error("Failed to initialize CURL handle");
      throw std::runtime_error("CURL initialization failed");
    }

    nlohmann::json jsonBody = nlohmann::json::object();

    curl_easy_setopt(handle.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle.get(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(handle.get(), CURLOPT_TIMEOUT, 1);


    if(body.size() > 0){
      curl_easy_setopt(handle.get(), CURLOPT_POSTFIELDS, body.c_str());
      curl_easy_setopt(handle.get(), CURLOPT_POSTFIELDSIZE, body.length());
    }

    UniqueCurlSlist concatenatedHeaders{nullptr};
    if(!headers.empty()){
      // Disable ssl checks for the sake of getting data without trouble
      curl_easy_setopt(handle.get(), CURLOPT_SSL_VERIFYPEER, false);
      curl_easy_setopt(handle.get(), CURLOPT_SSL_VERIFYHOST, false);

      for(const auto& header : headers){
        std::string concat = header.first + ": " + header.second;
        concatenatedHeaders.reset(curl_slist_append(concatenatedHeaders.release(), concat.c_str()));
      }

      curl_easy_setopt(handle.get(), CURLOPT_HTTPHEADER, concatenatedHeaders.get());
    }

    std::string responseString;
    curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &responseString);

    nlohmann::json jsonResponse = {};
    try{
      CURLcode code = curl_easy_perform(handle.get());

      curl_easy_setopt(handle.get(), CURLOPT_HTTPHEADER, nullptr);

      if(code != CURLE_OK){
        Logger::error("HTTP request failed: " + std::string(curl_easy_strerror(code)));
        jsonResponse["errors"] = {{"message", "Request failed"}};
        return jsonResponse;
      }

      jsonResponse = nlohmann::json::parse(responseString);
    }
    catch(const nlohmann::json::exception& e){
      jsonResponse["errors"] = {{"message", "Invalid JSON response"}};
      Logger::error(e.what());
    }
    catch(const std::exception& e){
      jsonResponse["errors"] = {{"message", "Unexpected error"}};
      Logger::error("Unexpected exception: " + std::string(e.what()));
    }

    return jsonResponse;
  }
}
