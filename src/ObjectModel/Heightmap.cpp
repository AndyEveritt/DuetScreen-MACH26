/*
 * Heightmap.cpp
 *
 *  Created on: 26 Mar 2024
 *      Author: andy
 */

#include "Debug.h"

#include "Heightmap.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Directories.h"
#include "utils/csv.h"
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>

namespace OM
{
	static std::string s_currentHeightmapName;
	static std::map<std::string, std::shared_ptr<Heightmap>> s_heightmapCache;
	static std::string s_emptyStr = "";

	static std::string GetLocalFilePath(const std::string& filename)
	{
		return utils::format("/tmp/heightmaps/%s", filename.c_str());
	}

	HeightmapMeta::HeightmapMeta()
	{
		Reset();
	}

	HeightmapMeta::~HeightmapMeta() {}

	void HeightmapMeta::Reset()
	{
		m_isValid = false;
		m_axis[0] = "X";
		m_axis[1] = "Y";
		m_min[0] = 0.0f;
		m_min[1] = 0.0f;
		m_max[0] = 0.0f;
		m_max[1] = 0.0f;
		m_radius = 0.0f;
		m_spacing[0] = 0.0f;
		m_spacing[1] = 0.0f;
		m_samples[0] = 0;
		m_samples[1] = 0;
		m_recipSpacing[0] = 0.0f;
		m_recipSpacing[1] = 0.0f;
	}

	void HeightmapMeta::Parse(const std::string& meta)
	{
		utils::CSV doc(meta, true);
		const std::vector<std::string> headers = doc.GetHeaders();
		for (const std::string& header : headers)
		{
			LOG_DBG("Header: \"{:s}\"", header.c_str());
		}

		doc.GetCell("axis0", 0, m_axis[0]);
		doc.GetCell("axis1", 0, m_axis[1]);

		doc.GetCell("min0", 0, m_min[0]);
		doc.GetCell("min1", 0, m_min[1]);
		doc.GetCell("max0", 0, m_max[0]);
		doc.GetCell("max1", 0, m_max[1]);

		doc.GetCell("radius", 0, m_radius);

		doc.GetCell("spacing0", 0, m_spacing[0]);
		doc.GetCell("spacing1", 0, m_spacing[1]);

		doc.GetCell("num0", 0, m_samples[0]);
		doc.GetCell("num1", 0, m_samples[1]);

		CheckValidity();

		if (IsValid())
		{
			LOG_DBG("Axes: {:s}, {:s}", m_axis[0].c_str(), m_axis[1].c_str());
			LOG_DBG("Min: {:g}, {:g}", m_min[0], m_min[1]);
			LOG_DBG("Max: {:g}, {:g}", m_max[0], m_max[1]);
			LOG_DBG("Radius: {:g}", m_radius);
			LOG_DBG("Spacing: {:g}, {:g}", m_spacing[0], m_spacing[1]);
			LOG_DBG("Samples: {:d}, {:d}", m_samples[0], m_samples[1]);
		}
		else
		{
			LOG_WARN("Heightmap meta data is invalid");
		}
	}

	std::shared_ptr<Move::Axis> HeightmapMeta::GetAxis(size_t index) const
	{
		const std::string& label = m_axis[index];
		auto axis = Move::GetAxisByLetter(label[0]);
		if (axis == nullptr)
		{
			LOG_ERROR("Axis {:s} not found", label.c_str());
		}
		return axis;
	}

	void HeightmapMeta::CheckValidity()
	{
		if (m_max[0] - m_min[0] < MinRange || m_spacing[0] < MinSpacing || m_max[1] - m_min[1] < MinRange ||
			m_spacing[1] < MinSpacing)
		{
			m_isValid = false;
		}
		else
		{
			m_isValid = GetNumSamples() != 0 && (m_radius < 0.0 || m_radius >= 1.0) && m_axis[0] != m_axis[1];

			if (m_isValid)
			{
				m_recipSpacing[0] = 1.0 / m_spacing[0];
				m_recipSpacing[1] = 1.0 / m_spacing[1];
			}
		}
	}

	Heightmap::Heightmap(const std::string& filename)
		: m_fileName(filename)
	{
	}

	void Heightmap::Reset()
	{
		m_heightmap.clear();
		meta.Reset();
		m_minError = 0.0f;
		m_maxError = 0.0f;
		m_meanError = 0.0f;
		m_stdDev = 0.0f;
		m_area = 0.0f;
	}

	bool Heightmap::LoadFromDuet()
	{
		Reset();
		std::string csvContents;
		if (!Comm::DUET.DownloadFile((Directories::GetSystemDirectory() + m_fileName).c_str(), csvContents))
		{
			LOG_ERROR("Failed to download heightmap file {:s}", m_fileName);
			return false;
		}

		if (csvContents.find("RepRapFirmware height map") == std::string::npos)
		{
			LOG_WARN("CSV file \"{:s}\" not a heightmap", m_fileName);
			return false;
		}

		// Write the file to disk
		std::string localFilePath = GetLocalFilePath(m_fileName);
		std::ofstream file(localFilePath.c_str(), std::ios::out | std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open file {:s} for writing", localFilePath.c_str());
			return false;
		}

		LOG_INFO("Writing heightmap to {:s}", localFilePath.c_str());
		file.write(csvContents.c_str(), csvContents.length());

		if (!ParseMeta(csvContents))
		{
			LOG_ERROR("Failed to parse meta data for heightmap {:s}", m_fileName.c_str());
			return false;
		}

		if (!ParseData(csvContents))
		{
			LOG_ERROR("Failed to parse data for heightmap {:s}", m_fileName.c_str());
			return false;
		}

		return true;
	}

	const Heightmap::Point* Heightmap::GetPoint(size_t x, size_t y) const
	{
		if (x >= GetWidth() || y >= GetHeight())
		{
			LOG_ERROR("Invalid point {:d}, {:d}, heightmap size ({:d}, {:d})", x, y, GetWidth(), GetHeight());
			return nullptr;
		}
		return &m_heightmap[y * GetWidth() + x];
	}

	bool Heightmap::ParseMeta(const std::string& csvContents)
	{
		LOG_INFO("Parsing meta data for heightmap {:s}", m_fileName.c_str());
		size_t metaStart = utils::findInstance(csvContents, "\n", 1);
		size_t metaEnd = utils::findInstance(csvContents, "\n", 3);
		if (metaStart == std::string::npos || metaEnd == std::string::npos)
		{
			LOG_ERROR("Corrupt heightmap file {:s}", m_fileName.c_str());
			return false;
		}

		std::string metaStr = csvContents.substr(metaStart, metaEnd - metaStart);
		LOG_DBG("Meta:\n{:s}", metaStr.c_str());

		meta.Parse(metaStr);
		return true;
	}

	bool Heightmap::ParseData(const std::string& csvContents)
	{
		LOG_INFO("Parsing data for heightmap {:s}", m_fileName.c_str());
		size_t dataStart = utils::findInstance(csvContents, "\n", 3) + 1;
		if (dataStart == std::string::npos)
		{
			LOG_ERROR("Corrupt heightmap file {:s}", m_fileName.c_str());
			return false;
		}

		bool parseError = false;
		std::string dataStr = csvContents.substr(dataStart);
		LOG_DBG("Data:\n{:s}", dataStr.c_str());
		utils::CSV doc(dataStr, false);

		m_heightmap.clear();
		m_minError = 9999.9f;
		m_maxError = -9999.9f;

		size_t rows = doc.GetRowCount();
		size_t cols = doc.GetColumnCount();

		if (rows != GetHeight() || cols != GetWidth())
		{
			LOG_ERROR("Heightmap size mismatch: {:d} != {:d} or {:d} != {:d}", rows, GetHeight(), cols, GetWidth());
			return false;
		}

		// Resize the 1D vector to hold all points
		m_heightmap.resize(rows * cols);

		double errorSum = 0.0f;
		double errorSqrSum = 0.0f;
		double xMin = 9999.9f;	// Used to calculate area
		double yMin = 9999.9f;	// Used to calculate area
		double xMax = -9999.9f; // Used to calculate area
		double yMax = -9999.9f; // Used to calculate area

		for (size_t rowIdx = 0; rowIdx < rows; rowIdx++)
		{
			for (size_t colIdx = 0; colIdx < cols; colIdx++)
			{
				size_t index = rowIdx * cols + colIdx;
				Point& point = m_heightmap[index];
				std::string val;
				point.x = meta.GetMin(0) + colIdx * meta.GetSpacing(0);
				point.y = meta.GetMin(1) + rowIdx * meta.GetSpacing(1);
				if (point.x < xMin)
				{
					LOG_DBG("New xMin: {:g}", point.x);
					xMin = point.x;
				}
				if (point.x > xMax)
				{
					LOG_DBG("New xMax: {:g}", point.x);
					xMax = point.x;
				}
				if (point.y < yMin)
				{
					LOG_DBG("New yMin: {:g}", point.y);
					yMin = point.y;
				}
				if (point.y > yMax)
				{
					LOG_DBG("New yMax: {:g}", point.y);
					yMax = point.y;
				}
				if (!doc.GetCell(colIdx, rowIdx, val))
				{
					LOG_ERROR("Failed to get cell {:d}, {:d}", colIdx, rowIdx);
					point.isNull = true;
					parseError = true;
				}
				utils::removeCharFromString(val, ' ');
				if (val == "0")
				{
					point.isNull = true;
				}
				point.z = strtod(val.c_str(), NULL);

				LOG_DBG("Cell {:d}, {:d}{:s}: ({:g}, {:g}, {:g}) raw=\"{:s}\"",
						colIdx,
						rowIdx,
						point.isNull ? "[INVALID]" : "",
						point.x,
						point.y,
						point.z,
						val.c_str());
				if (point.z < m_minError)
				{
					m_minError = point.z;
				}
				if (point.z > m_maxError)
				{
					m_maxError = point.z;
				}
				errorSum += point.z;
				errorSqrSum += point.z * point.z;
			}
		}

		LOG_DBG("xMin={:g}, xMax={:g}, yMin={:g}, yMax={:g}", xMin, xMax, yMin, yMax);
		m_area =
			meta.GetRadius() > 0 ? meta.GetRadius() * meta.GetRadius() * M_PI : std::abs((xMax - xMin) * (yMax - yMin));
		m_meanError = errorSum / (rows * cols);
		m_stdDev = sqrt(errorSqrSum * GetPointCount() - errorSum * errorSum) / GetPointCount();

		if (rows * cols != m_heightmap.size())
		{
			LOG_WARN("Heightmap size mismatch: {:d} != {:d}", rows * cols, m_heightmap.size());
			parseError = true;
		}

		LOG_DBG("Heightmap: {:d} rows, {:d} cols, area={:g} mm^2, minError={:g} mm, maxError={:g} mm, meanError={:g} "
				"mm, stdDev=%.3f mm",
				rows,
				cols,
				m_area,
				m_minError,
				m_maxError,
				m_meanError,
				m_stdDev);

		return !parseError;
	}

	double Heightmap::GetInterpolatedPoint(double axis0, double axis1, bool extrapolate) const
	{
		// Last grid point
		const double xLast = meta.GetMin(0) + (meta.GetSamples(0) - 1) * meta.GetSpacing(0);
		const double yLast = meta.GetMin(1) + (meta.GetSamples(1) - 1) * meta.GetSpacing(1);

		// Clamp to rectangle so InterpolateXY will always have valid parameters
		const double fEPSILON = 0.01;
		bool outOfBounds = false;
		if (axis0 < meta.GetMin(0))
		{
			outOfBounds = true;
			axis0 = meta.GetMin(0);
		}
		if (axis1 < meta.GetMin(1))
		{
			outOfBounds = true;
			axis1 = meta.GetMin(1);
		}
		if (axis0 > xLast - fEPSILON)
		{
			outOfBounds = true;
			axis0 = xLast - fEPSILON;
		}
		if (axis1 > yLast - fEPSILON)
		{
			outOfBounds = true;
			axis1 = yLast - fEPSILON;
		}

		if (!extrapolate && outOfBounds)
			return std::numeric_limits<double>::quiet_NaN();

		const double xf = (axis0 - meta.GetMin(0)) * meta.GetRecipSpacing(0);
		const double xFloor = floor(xf);
		const int32_t xIndex = (int32_t)xFloor;
		const double yf = (axis1 - meta.GetMin(1)) * meta.GetRecipSpacing(1);
		const double yFloor = floor(yf);
		const int32_t yIndex = (int32_t)yFloor;

		double value;
		if (!InterpolateAxis0Axis1(xIndex, yIndex, xf - xFloor, yf - yFloor, value))
		{
			return std::numeric_limits<double>::quiet_NaN();
		}
		return value;
	}

	bool Heightmap::InterpolateAxis0Axis1(
		size_t axis0Index, size_t axis1Index, double axis0Frac, double axis1Frac, double& result) const
	{
		const uint32_t indexX0Y0 = GetMapIndex(axis0Index, axis1Index); // (X0,Y0)
		const uint32_t indexX1Y0 = indexX0Y0 + 1;						// (X1,Y0)
		const uint32_t indexX0Y1 = indexX0Y0 + meta.GetSamples(0);		// (X0 Y1)
		const uint32_t indexX1Y1 = indexX0Y1 + 1;						// (X1,Y1)

		const float xyFrac = axis0Frac * axis1Frac;

		if (indexX1Y1 >= m_heightmap.size())
		{
			LOG_ERROR(
				"Invalid heightmap index {:d}, {:d}. Max index = {:d}", axis0Index, axis1Index, m_heightmap.size() - 1);
			return 0.0;
		}

		Point points[4] = {
			m_heightmap[indexX0Y0], m_heightmap[indexX1Y0], m_heightmap[indexX0Y1], m_heightmap[indexX1Y1]};

		for (Point& point : points)
		{
			if (point.isNull)
			{
				result = 0.0f;
				return false;
			}
		}

		result = (points[0].z * (1.0 - axis0Frac - axis1Frac + xyFrac)) + (points[1].z * (axis0Frac - xyFrac)) +
				 (points[2].z * (axis1Frac - xyFrac)) + (points[3].z * xyFrac);
		return true;
	}

	const std::string& GetHeightmapNameAt(int index)
	{
		std::vector<std::shared_ptr<FileSystem::FileSystemItem>> filenames = GetHeightmapFiles();
		if (index < 0 || index >= (int)filenames.size())
		{
			LOG_ERROR("Invalid heightmap index {:d}", index);
			return s_emptyStr;
		}
		std::shared_ptr<FileSystem::FileSystemItem> item = filenames[index];
		if (item == nullptr)
		{
			LOG_ERROR("Filesystem item at index {:d} is null", index);
			return s_emptyStr;
		}
		return item->GetName();
	}

	void SetCurrentHeightmap(const std::string& filename)
	{
		size_t pos = filename.find_last_of('/');
		if (pos != std::string::npos)
		{
			s_currentHeightmapName = filename.substr(pos + 1);
		}
		else
		{
			s_currentHeightmapName = filename;
		}
		LOG_DBG("Set current heightmap to \"{:s}\" ({:s})", s_currentHeightmapName.c_str(), filename.c_str());
	}

	void ClearCurrentHeightmap()
	{
		s_currentHeightmapName = "";
	}

	/* Sends command to Duet to use the heightmap called `filename` */
	void LoadHeightmap(const char* filename)
	{
		LOG_INFO("Loading heightmap {:s}", filename);
		Comm::DUET.SendGcodef("G29 S1 P\"%s\"\n", filename);
	}

	/* Sends command to Duet to unload the heightmap */
	void UnloadHeightmap()
	{
		LOG_INFO("Unloading heightmap");
		Comm::DUET.SendGcode("G29 S2\n");
	}

	void ToggleHeightmap(const char* filename)
	{
		if (s_currentHeightmapName != filename)
		{
			LoadHeightmap(filename);
			return;
		}
		UnloadHeightmap();
		ClearCurrentHeightmap();
	}

	const std::string& GetCurrentHeightmap()
	{
		return s_currentHeightmapName;
	}

	std::shared_ptr<Heightmap> GetHeightmapData(const std::string& filename)
	{
		auto it = s_heightmapCache.find(filename);
		if (it == s_heightmapCache.end())
		{
			std::shared_ptr<Heightmap> heightmap = std::make_shared<Heightmap>(filename);
			s_heightmapCache[filename] = heightmap;
			return s_heightmapCache[filename];
		}
		return it->second;
	}

	size_t ClearHeightmapCache()
	{
		size_t count = s_heightmapCache.size();
		s_heightmapCache.clear();
		return count - s_heightmapCache.size();
	}

	void RequestHeightmapFiles(std::function<void()> callback)
	{
		LOG_DBG("Requesting heightmap files from Duet");
		FileSystem::RequestFiles(Directories::DirectoryType::SYSTEM, "", callback);
		ClearHeightmapCache();
	}

	std::vector<std::shared_ptr<FileSystem::FileSystemItem>> GetHeightmapFiles()
	{
		const std::vector<std::shared_ptr<FileSystem::FileSystemItem>>& files = FileSystem::GetItems();
		std::vector<std::shared_ptr<FileSystem::FileSystemItem>> csvFiles;

		for (const auto& item : files)
		{
			if (item->GetType() != FileSystem::FileSystemItemType::file)
				continue;

			// Ignore non CSV files
			if (item->GetName().rfind(".csv\0") == std::string::npos)
				continue;

			if (item->GetName().find(DEFAULT_FILAMENTS_FILE) != std::string::npos)
				continue;

			csvFiles.push_back(item);
		}

		return csvFiles;
	}
} // namespace OM
