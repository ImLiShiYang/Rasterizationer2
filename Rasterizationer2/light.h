#ifndef LIGHT_H
#define LIGHT_H

#include <string>


class Light
{
public:
	virtual ~Light() = default;
	virtual std::string getType() = 0;
};

#endif // !LIGHT_H


