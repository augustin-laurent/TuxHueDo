#include <thread>
#include <memory>
#include <csignal>

#include <Huenicorn/Version.hpp>
#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Logger.hpp>
#include <Huenicorn/PlatformSelector.hpp>


/**
 * @brief Wrapper around threaded application
 * 
 */
class Application
{
public:
  void start()
  {
    m_core = std::make_unique<Huenicorn::HuenicornCore>(Huenicorn::Version, Huenicorn::platformAdapter.getConfigFilePath());
    m_applicationThread.emplace([&](){
      m_core->start();
    });

    m_applicationThread.value().join();
    m_applicationThread.reset();
    m_core.reset();
  }


  void stop()
  {
    if(!m_core){
      return;
    }

    m_core->stop();
  }


private:
  std::unique_ptr<Huenicorn::HuenicornCore> m_core;
  std::optional<std::thread> m_applicationThread;
};


Application app;


void signalHandler(int signal)
{
  if(signal == SIGTERM || signal == SIGINT || signal == SIGTSTP){
    Huenicorn::Logger::log("Closing application");
    app.stop();
  }
}


int main()
{
  Huenicorn::Logger::log("Starting Huenicorn version ", Huenicorn::Version);

  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);
  signal(SIGTSTP, signalHandler);

  app.start();
  Huenicorn::Logger::log("Huenicorn terminated properly");

  return 0;
}
