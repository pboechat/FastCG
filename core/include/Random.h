#ifndef RANDOM_H
#define RANDOM_H

class Random
{
public:
	static float NextFloat();
	static void Seed();

private:
	Random();
	~Random();

};

#endif
