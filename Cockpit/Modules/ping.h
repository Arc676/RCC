#ifndef PING_MODULE_H
#define PING_MODULE_H

class PingModule {
	double latency = 0;
	char response  = 0;

public:
	void render();
};

#endif
