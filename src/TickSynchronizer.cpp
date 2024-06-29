#include <Huenicorn/TickSynchronizer.hpp>

#include <thread>


namespace Huenicorn
{
  TickSynchronizer::TickSynchronizer(Timing::TimeUnitType tickInterval):
  m_tickInterval(tickInterval)
  {
    std::fill(m_loadRateHistory, m_loadRateHistory + LoadRateHistorySize, Timing::Duration{0.0});
  }


  Timing::TimeUnit TickSynchronizer::tickInterval() const
  {
    return m_tickInterval;
  }


  const TickSynchronizer::Excess& TickSynchronizer::lastExcess() const
  {
    return m_lastExcess;
  }


  float TickSynchronizer::loadRate(bool asPercents) const
  {
    return asPercents ? m_loadRate * 100 : m_loadRate;
  }


  void TickSynchronizer::setTickInterval(Timing::TimeUnitType tickInterval)
  {
    m_tickInterval = Timing::Duration{tickInterval};
  }


  void TickSynchronizer::start()
  {
    m_timePoint = Timing::ClockType::now();
  }


  bool TickSynchronizer::sync()
  {
    int factor = 1 + _syncWithTick(m_timePoint);
    m_timePoint += (m_tickInterval * factor);

    return factor <= 2;
  }


  Timing::TimeUnitType TickSynchronizer::_syncWithTick(const Timing::TimePoint& startTime)
  {
    // Measure
    Timing::TimePoint now = Timing::ClockType::now();
    m_loadRate = _computeLoad(startTime, now);

    // Sync
    if(m_tickInterval == Timing::TimeUnit::zero()){
      return 0;
    }

    Timing::TimePoint next = startTime + m_tickInterval;

    if(next > now){
      std::this_thread::sleep_until(next);
    }
    else{
      Timing::Duration excess = now - (startTime + m_tickInterval);
      float ratio = excess / m_tickInterval;

      m_lastExcess = {excess, ratio};
      return m_lastExcess.rate;
    }

    return 0;
  }
}
