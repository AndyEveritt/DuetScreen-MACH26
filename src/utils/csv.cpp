/*
 * csv.cpp
 *
 *  Created on: 10 Apr 2024
 *      Author: andy
 */

#include "Debug.h"

#include "csv.h"
#include <sstream>

namespace utils
{
	CSV::CSV(const std::string& csvContents, bool hasHeaders)
		: m_hasHeaders(hasHeaders)
	{
		size_t col = 0;
		size_t row = 0;
		size_t dataOffset = hasHeaders ? 1 : 0;
		bool inQuote = false;

		LOG_INFO("Parsing CSV: hasHeaders={:d}", hasHeaders);
		LOG_DBG("CSV contents: {:s}", csvContents.c_str());

		for (size_t i = 0; i < csvContents.size(); i++)
		{
			const char c = csvContents[i];
			const char n = i < csvContents.size() - 1 ? csvContents[i + 1] : 0;

			LOG_VERBOSE("c={:c}, n={:c}, inQuote={:d}, col={:d}, row={:d}", c, n, inQuote, col, row);

			// Make sure we have enough rows and columns
			if (hasHeaders && row == 0)
			{
				if (m_headers.size() <= col)
				{
					LOG_VERBOSE("Adding col {:d} to headers", col);
					m_headers.push_back("");
				}
			}
			else
			{
				if (m_data.size() <= row - dataOffset)
				{
					LOG_VERBOSE("Adding row {:d}", row - dataOffset);
					m_data.push_back(std::vector<std::string>());
				}
				if (m_data[row - dataOffset].size() <= col)
				{
					LOG_VERBOSE("Adding col {:d} to m_data[{:d}]", col, row - dataOffset);
					m_data[row - dataOffset].push_back("");
				}
			}

			// Deal with escaped quotes
			if (inQuote && c == '"' && n == '"')
			{
				if (hasHeaders && row == 0)
				{
					m_headers[col] += c;
				}
				else
				{
					m_data[row][col] += c;
				}
				i++;
				continue;
			}

			// Deal with start and end of a field
			if (c == '"')
			{
				inQuote = !inQuote;
				continue;
			}

			// Check if we can move on to the next field
			if (c == ',' && !inQuote)
			{
				col++;
				continue;
			}

			// Check if we can move on to the next row
			if (c == '\n' && !inQuote)
			{
				col = 0;
				row++;
				continue;
			}

			if (c == '\r' && !inQuote)
			{
				continue;
			}

			// Add the character to the current field
			if (hasHeaders && row == 0)
			{
				m_headers[col] += c;
			}
			else
			{
				m_data[row - dataOffset][col] += c;
			}
		}

		if (m_data.size() > 0 && m_data[m_data.size() - 1].size() == 0)
		{
			m_data.pop_back();
		}
		LOG_DBG("CSV: {:d} rows, {:d} cols", m_data.size(), m_data.size() > 0 ? m_data[0].size() : 0);
	}

	const std::vector<std::string>& CSV::GetHeaders() const
	{
		return m_headers;
	}

	size_t CSV::GetRowCount() const
	{
		return m_data.size();
	}

	size_t CSV::GetColumnCount() const
	{
		return m_data.size() > 0 ? m_data[0].size() : 0;
	}

	bool CSV::GetCell(const std::string& header, size_t row, std::string& val) const
	{
		if (!m_hasHeaders)
		{
			LOG_WARN("No headers in CSV");
			return false;
		}

		for (size_t i = 0; i < m_headers.size(); i++)
		{
			if (m_headers[i] == header)
			{
				return GetCell(i, row, val);
			}
		}

		LOG_WARN("Header \"{:s}\" not found", header.c_str());
		return false;
	}

	bool CSV::GetCell(size_t col, size_t row, std::string& val) const
	{
		if (row >= m_data.size())
		{
			LOG_WARN("Row {:d} out of range, rows={:d}", row, m_data.size());
			return false;
		}

		if (col >= m_data[row].size())
		{
			LOG_WARN("Column {:d} out of range, row[{:d}].size()={:d}", col, row, m_data[row].size());
			return false;
		}

		val = m_data[row][col];
		return true;
	}

	bool CSV::GetCell(const std::string& header, size_t row, size_t& val) const
	{
		std::string str;
		if (!GetCell(header, row, str))
		{
			return false;
		}

		val = strtoul(str.c_str(), NULL, 10);
		return true;
	}

	bool CSV::GetCell(size_t col, size_t row, size_t& val) const
	{
		std::string str;
		if (!GetCell(col, row, str))
		{
			return false;
		}

		val = strtoul(str.c_str(), NULL, 10);
		return true;
	}

	bool CSV::GetCell(const std::string& header, size_t row, double& val) const
	{
		std::string str;
		if (!GetCell(header, row, str))
		{
			return false;
		}

		val = strtod(str.c_str(), NULL);
		return true;
	}

	bool CSV::GetCell(size_t col, size_t row, double& val) const
	{
		std::string str;
		if (!GetCell(col, row, str))
		{
			return false;
		}

		val = strtod(str.c_str(), NULL);
		return true;
	}

} // namespace utils
