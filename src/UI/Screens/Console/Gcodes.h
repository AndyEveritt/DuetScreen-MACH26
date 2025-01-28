/*
 * Gcodes.h
 *
 *  Created on: 2 Jan 2024
 *      Author: Andy Everitt
 */

#pragma once

#include <cstddef>
#include <vector>

struct gcode
{
	const char* gcode;
	const char* helpText;
};

class Gcodes
{
  public:
	static const size_t getGcodeCount();
	static const gcode* getGcode(size_t index);

  private:
	Gcodes();
	static Gcodes& get()
	{
		static Gcodes instance;
		return instance;
	}
	const std::vector<gcode> m_gcodes;
};
