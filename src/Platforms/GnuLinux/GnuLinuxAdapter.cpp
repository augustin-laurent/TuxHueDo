#include <Huenicorn/Platforms/GnuLinux/GnuLinuxAdapter.hpp>

#include <pwd.h>
#include <unistd.h>

#include <Huenicorn/Logger.hpp>

#ifdef PIPEWIRE_GRABBER_AVAILABLE
#include <Huenicorn/Platforms/GnuLinux/PipewireGrabber.hpp>
#endif
#ifdef X11_GRABBER_AVAILABLE
#include <Huenicorn/Platforms/GnuLinux/X11Grabber.hpp>
#endif


namespace Huenicorn
{
  std::filesystem::path GnuLinuxAdapter::getConfigFilePath() const
  {
    const char* homeDir;
    if((homeDir = getenv("HOME")) == NULL){
      homeDir = getpwuid(getuid())->pw_dir;
    }

    return std::filesystem::path(homeDir) / ".config/huenicorn";
  }


  std::string GnuLinuxAdapter::getUsername() const{
    return std::string(getlogin());
  }



  void GnuLinuxAdapter::openWebBrowser(const std::string& url) const
  {
    if(system(std::string("xdg-open " + url).c_str()) != 0){
      Logger::error("Failed to open browser");
    }
  }


  SharedGrabber GnuLinuxAdapter::_createGrabber(Config* config) const
  {
    std::string sessionType = std::getenv("XDG_SESSION_TYPE");

#ifdef PIPEWIRE_GRABBER_AVAILABLE
    if(sessionType == "wayland"){
      auto grabber = std::make_shared<PipewireGrabber>(config);
      Logger::log("Started Pipewire grabber.");
      return grabber;
    }
#endif

#ifdef X11_GRABBER_AVAILABLE
    if(sessionType == "x11"){
      auto grabber = std::make_shared<X11Grabber>(config);
      Logger::log("Started X11 grabber.");
      return grabber;
    }
#endif

    return nullptr;
  }
}
