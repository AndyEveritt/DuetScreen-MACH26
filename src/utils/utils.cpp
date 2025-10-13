/*
 * utils.cpp
 *
 *  Created on: 26 Jan 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "DebugCommands.h"
#include "utils.h"

namespace utils
{

	/**
	 * Executes a command and returns the output as a string.
	 *
	 * @param cmd The command to be executed.
	 * @return The output of the command as a string.
	 */
	std::string exec(const char* cmd)
	{
		char buffer[128];
		std::string result = "";
		std::string errorText = "";
		FILE* pipe = popen(cmd, "r");
		if (!pipe)
		{
			LOG_ERROR("popen({:s}) failed!", cmd);
			return "";
		}
		while (!feof(pipe))
		{
			if (fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}
		int status = pclose(pipe);
		if (status != 0)
		{
			errorText = fmt::format("Command failed with status: {:d}", status);
		}
		return result + "\n" + errorText;
	}

	size_t removeCharFromString(std::string& nString, char c)
	{
		std::string::size_type pos;
		while (1)
		{
			pos = nString.find(c);
			if (pos != std::string::npos)
			{
				nString.erase(pos, 1);
			}
			else
			{
				break;
			}
		}
		return (size_t)nString.size();
	}

	void replaceSubstring(std::string& nString, const std::string& target, const std::string& replacement)
	{
		size_t position = nString.find(target); // Find the first occurrence of the target substring

		while (position != std::string::npos)
		{
			nString.replace(
				position, target.length(), replacement); // Replace the target substring with the replacement string
			position = nString.find(target, position + replacement.length()); // Find the next occurrence, if any
		}
	}

	std::vector<std::string> splitString(const std::string& str, const std::string& delimiter)
	{
		std::vector<std::string> tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = str.find(delimiter, prev);
			if (pos == std::string::npos)
				pos = str.length();

			std::string token = str.substr(prev, pos - prev);
			if (!token.empty())
				tokens.push_back(token);

			prev = pos + delimiter.length();
		} while (pos < str.length() && prev < str.length());

		return tokens;
	}

	size_t findInstance(std::string_view str, std::string_view target, size_t instance)
	{
		size_t pos = 0;
		size_t count = 0;
		while (count < instance)
		{
			pos = str.find(target, pos);
			if (pos == std::string::npos)
			{
				return std::string::npos;
			}
			count++;
			pos++;
		}
		return pos;
	}
} // namespace utils
