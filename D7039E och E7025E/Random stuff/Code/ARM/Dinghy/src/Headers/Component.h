class Component
{
public:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void Sleep() = 0;
	virtual void WakeUp() = 0;
	virtual int Poll() = 0;
};
