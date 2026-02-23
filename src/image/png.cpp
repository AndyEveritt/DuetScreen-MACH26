#include "Debug.h"

#include "png.h"
#include <algorithm>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

PNG::PNG()
	: m_imageFileName(nullptr)
	, m_imageFile(nullptr)
{
}

PNG::PNG(const char* imageFileName)
	: m_imageFileName(imageFileName)
{
	Open();
}

PNG::~PNG()
{
	Close();
}

bool PNG::New(const char* imageFileName)
{
	Close();
	m_imageFileName = imageFileName;
	dataSize = 0;
	return Open();
}

bool PNG::Open()
{
	m_imageFile = fopen(m_imageFileName, "wb");
	if (m_imageFile == nullptr)
	{
		LOG_ERROR("Failed to open file {:s}", m_imageFileName);
	}
	return m_imageFile != nullptr;
}

bool PNG::Close()
{
	if (m_imageFile == nullptr)
	{
		return true;
	}
	LOG_DBG("Closing file {:s}", m_imageFileName);
	if (fflush(m_imageFile) != 0)
	{
		LOG_ERROR("Failed to fflush file {:s}: {}", m_imageFileName, errno);
		/* attempt to continue to close */
	}

	int fd = fileno(m_imageFile);
	if (fd != -1)
	{
		if (fsync(fd) != 0)
		{
			LOG_ERROR("Failed to fsync file {:s}: {}", m_imageFileName, errno);
			/* attempt to continue to close */
		}
	}

	if (fclose(m_imageFile) != 0)
	{
		LOG_ERROR("Failed to close file {:s}", m_imageFileName);
		return false;
	}
	m_imageFile = nullptr;
	return true;
}

size_t PNG::appendData(unsigned char data[], int size)
{
	if (!IsOpen())
	{
		LOG_WARN("File {:s} not open", m_imageFileName);
		return 0;
	}
	if (size <= 0)
	{
		return 0;
	}

	size_t totalWritten = 0;
	while (totalWritten < static_cast<size_t>(size))
	{
		size_t toWrite = static_cast<size_t>(size) - totalWritten;
		size_t written = fwrite(data + totalWritten, 1, toWrite, m_imageFile);
		if (written == 0)
		{
			if (ferror(m_imageFile))
			{
				LOG_ERROR("Failed to write PNG data to {:s}: {}", m_imageFileName, errno);
			}
			break;
		}
		totalWritten += written;
	}

	dataSize += totalWritten;
	return totalWritten;
}
