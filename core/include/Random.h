#ifndef RANDOM_H_
#define RANDOM_H_

class Random
{
public:
	static float NextFloat();
	static void SeedWithTime();

private:
	Random();
	~Random();

};

#endif
