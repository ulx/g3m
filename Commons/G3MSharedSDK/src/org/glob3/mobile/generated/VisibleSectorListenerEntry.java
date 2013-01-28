package org.glob3.mobile.generated; 
public class VisibleSectorListenerEntry
{
  private VisibleSectorListener _listener;
  private final long _stabilizationIntervalInMS;

  private Sector _lastSector;
  private long _whenNotifyInMS;

  private ITimer _timer;

  private ITimer getTimer()
  {
	if (_timer == null)
	{
	  _timer = IFactory.instance().createTimer();
	}
	return _timer;
  }


  public VisibleSectorListenerEntry(VisibleSectorListener listener, TimeInterval stabilizationInterval)
  {
	  _listener = listener;
	  _stabilizationIntervalInMS = stabilizationInterval.milliseconds();
	  _lastSector = null;
	  _timer = null;
	  _whenNotifyInMS = 0;

  }

  public final void tryToNotifyListener(Sector visibleSector)
  {
	if (_stabilizationIntervalInMS == 0)
	{
	  if ((_lastSector == null) || (!_lastSector.isEqualsTo(visibleSector)))
	  {
		if (_lastSector != null)
			_lastSector.dispose();
		_lastSector = new Sector(visibleSector);
		_listener.onVisibleSectorChange(_lastSector);
	  }
	}
	else
	{
	  final long now = getTimer().now().milliseconds();

	  if ((_lastSector == null) || (!_lastSector.isEqualsTo(visibleSector)))
	  {
		if (_lastSector != null)
			_lastSector.dispose();
		_lastSector = new Sector(visibleSector);
		_whenNotifyInMS = now + _stabilizationIntervalInMS;
	  }

	  if (_whenNotifyInMS != 0)
	  {
		if (now >= _whenNotifyInMS)
		{
		  _listener.onVisibleSectorChange(_lastSector);

		  _whenNotifyInMS = 0;
		}
	  }
	}

  }

  public void dispose()
  {
	if (_listener != null)
		_listener.dispose();

	if (_timer != null)
	{
	  IFactory.instance().deleteTimer(_timer);
	}
  }
}