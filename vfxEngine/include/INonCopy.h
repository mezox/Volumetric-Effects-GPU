#pragma once

class INonCopy
{
public:
	virtual ~INonCopy() {}

	const INonCopy& operator=(const INonCopy& rOther) = delete;
	INonCopy(const INonCopy& rOther) = delete;

protected:
	INonCopy() {}
};