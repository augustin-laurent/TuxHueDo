#pragma once

#include <Huenicorn/IPlatformAdapter.hpp>

#include <filesystem>


namespace Huenicorn
{
  class Config;

  class GnuLinuxAdapter : public IPlatformAdapter
  {
  public:
    virtual std::filesystem::path getConfigFilePath() const override;
    virtual std::string getUsername() const override;
    virtual void openWebBrowser(const std::string& url) const override;

  protected:
    virtual SharedGrabber _createGrabber(Config* config) const override;
  };
}
