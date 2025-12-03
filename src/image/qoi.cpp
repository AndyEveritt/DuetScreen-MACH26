#include "qoi.h"
#include "Debug.h"
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wconversion"

#define QOI_DEBUG 0
#if QOI_DEBUG
#  define qoi_dbg(fmt, ...)                                                                                            \
	  do                                                                                                               \
	  {                                                                                                                \
		  printf("%s(%d): " fmt, __FUNCTION__, __LINE__, __VA_ARGS__);                                                 \
	  } while (0)
#else
#  define qoi_dbg(fmt, ...)                                                                                            \
	  do                                                                                                               \
	  {                                                                                                                \
	  } while (0)
#endif

#ifndef QOI_MALLOC
#  define QOI_MALLOC(sz) malloc(sz)
#  define QOI_FREE(p) free(p)
#endif
#ifndef QOI_ZEROARR
#  define QOI_ZEROARR(a) memset((a), 0, sizeof(a))
#endif

#define QOI_OP_INDEX 0x00 /* 00xxxxxx */
#define QOI_OP_DIFF 0x40  /* 01xxxxxx */
#define QOI_OP_LUMA 0x80  /* 10xxxxxx */
#define QOI_OP_RUN 0xc0	  /* 11xxxxxx */
#define QOI_OP_RGB 0xfe	  /* 11111110 */
#define QOI_OP_RGBA 0xff  /* 11111111 */

#define QOI_MASK_2 0xc0 /* 11000000 */

#define QOI_COLOR_HASH(C) (C.rgba.r * 3 + C.rgba.g * 5 + C.rgba.b * 7 + C.rgba.a * 11)
#define QOI_MAGIC                                                                                                      \
	(((unsigned int)'q') << 24 | ((unsigned int)'o') << 16 | ((unsigned int)'i') << 8 | ((unsigned int)'f'))
#define QOI_HEADER_SIZE 14

/* 2GB is the max file size that this implementation can safely handle. We guard
against anything larger than that, assuming the worst case with 5 bytes per
pixel, rounded down to a nice clean value. 400 million pixels ought to be
enough for anybody. */
#define QOI_PIXELS_MAX ((unsigned int)400000000)

static const unsigned char qoi_padding[8] = {0, 0, 0, 0, 0, 0, 0, 1};

static void qoi_write_32(unsigned char* bytes, int* p, unsigned int v)
{
	bytes[(*p)++] = (0xff000000 & v) >> 24;
	bytes[(*p)++] = (0x00ff0000 & v) >> 16;
	bytes[(*p)++] = (0x0000ff00 & v) >> 8;
	bytes[(*p)++] = (0x000000ff & v);
}

static unsigned int qoi_read_32(const unsigned char* bytes, int* p)
{
	unsigned int a = bytes[(*p)++];
	unsigned int b = bytes[(*p)++];
	unsigned int c = bytes[(*p)++];
	unsigned int d = bytes[(*p)++];
	return a << 24 | b << 16 | c << 8 | d;
}

void* qoi_encode(const void* data, const qoi_desc* desc, int* out_len)
{
	int i, max_size, p, run;
	int px_len, px_end, px_pos, channels;
	unsigned char* bytes;
	const unsigned char* pixels;
	rgba_t index[64];
	rgba_t px, px_prev;

	if (data == NULL || out_len == NULL || desc == NULL || desc->width == 0 || desc->height == 0 ||
		desc->channels < 3 || desc->channels > 4 || desc->colorspace > 1 ||
		desc->height >= QOI_PIXELS_MAX / desc->width)
	{
		return NULL;
	}

	max_size = desc->width * desc->height * (desc->channels + 1) + QOI_HEADER_SIZE + sizeof(qoi_padding);

	p = 0;
	bytes = (unsigned char*)QOI_MALLOC(max_size);
	if (!bytes)
	{
		return NULL;
	}

	qoi_write_32(bytes, &p, QOI_MAGIC);
	qoi_write_32(bytes, &p, desc->width);
	qoi_write_32(bytes, &p, desc->height);
	bytes[p++] = desc->channels;
	bytes[p++] = desc->colorspace;

	pixels = (const unsigned char*)data;

	QOI_ZEROARR(index);

	run = 0;
	px_prev.rgba.r = 0;
	px_prev.rgba.g = 0;
	px_prev.rgba.b = 0;
	px_prev.rgba.a = 255;
	px = px_prev;

	px_len = desc->width * desc->height * desc->channels;
	px_end = px_len - desc->channels;
	channels = desc->channels;

	for (px_pos = 0; px_pos < px_len; px_pos += channels)
	{
		if (channels == 4)
		{
			px = *(rgba_t*)(pixels + px_pos);
		}
		else
		{
			px.rgba.r = pixels[px_pos + 0];
			px.rgba.g = pixels[px_pos + 1];
			px.rgba.b = pixels[px_pos + 2];
		}

		if (px.v == px_prev.v)
		{
			run++;
			if (run == 62 || px_pos == px_end)
			{
				bytes[p++] = QOI_OP_RUN | (run - 1);
				run = 0;
			}
		}
		else
		{
			int index_pos;

			if (run > 0)
			{
				bytes[p++] = QOI_OP_RUN | (run - 1);
				run = 0;
			}

			index_pos = QOI_COLOR_HASH(px) % 64;

			if (index[index_pos].v == px.v)
			{
				bytes[p++] = QOI_OP_INDEX | index_pos;
			}
			else
			{
				index[index_pos] = px;

				if (px.rgba.a == px_prev.rgba.a)
				{
					signed char vr = px.rgba.r - px_prev.rgba.r;
					signed char vg = px.rgba.g - px_prev.rgba.g;
					signed char vb = px.rgba.b - px_prev.rgba.b;

					signed char vg_r = vr - vg;
					signed char vg_b = vb - vg;

					if (vr > -3 && vr < 2 && vg > -3 && vg < 2 && vb > -3 && vb < 2)
					{
						bytes[p++] = QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
					}
					else if (vg_r > -9 && vg_r < 8 && vg > -33 && vg < 32 && vg_b > -9 && vg_b < 8)
					{
						bytes[p++] = QOI_OP_LUMA | (vg + 32);
						bytes[p++] = (vg_r + 8) << 4 | (vg_b + 8);
					}
					else
					{
						bytes[p++] = QOI_OP_RGB;
						bytes[p++] = px.rgba.r;
						bytes[p++] = px.rgba.g;
						bytes[p++] = px.rgba.b;
					}
				}
				else
				{
					bytes[p++] = QOI_OP_RGBA;
					bytes[p++] = px.rgba.r;
					bytes[p++] = px.rgba.g;
					bytes[p++] = px.rgba.b;
					bytes[p++] = px.rgba.a;
				}
			}
		}
		px_prev = px;
	}

	for (i = 0; i < (int)sizeof(qoi_padding); i++)
	{
		bytes[p++] = qoi_padding[i];
	}

	*out_len = p;
	return bytes;
}

int qoi_decode_init(qoi_desc* desc)
{
	if (!desc)
	{
		return -1;
	}

	memset(desc, 0, sizeof(*desc));

	return 0;
}

// returns header in desc
// number of bytes read on success, negative number on error
int qoi_decode_header(const void* data, int size, qoi_desc* desc)
{
	const unsigned char* bytes;
	unsigned int header_magic;
	int p = 0;

	if (data == NULL || desc == NULL || desc->decoder_state != qoi_decoder_header || size < QOI_HEADER_SIZE)
	{
		return -1;
	}

	bytes = (const unsigned char*)data;

	header_magic = qoi_read_32(bytes, &p);
	desc->width = qoi_read_32(bytes, &p);
	desc->height = qoi_read_32(bytes, &p);
	desc->channels = bytes[p++];
	desc->colorspace = bytes[p++];

	if (desc->width == 0 || desc->height == 0 || desc->channels < 3 || desc->channels > 4 || desc->colorspace > 1 ||
		header_magic != QOI_MAGIC || desc->height >= QOI_PIXELS_MAX / desc->width)
	{
		return -2;
	}

	desc->start.rgba.r = 0;
	desc->start.rgba.g = 0;
	desc->start.rgba.b = 0;
	desc->start.rgba.a = 255;

	desc->run = 0;

	desc->pixels_count = 0;
	QOI_ZEROARR(desc->index);

	desc->decoder_state = qoi_decoder_body;

	return p;
}

#include "stdio.h"

// return negative number on error or processed bytes from data
// pixels return number of decoded pixel_count (qoi_rgba_t)
int qoi_decode_body(qoi_desc* desc, const void* data, int size, void* buffer, int buffer_size, int* pixel_count)
{
	const unsigned char* bytes;
	unsigned char* pixels;
	rgba_t px;
	int px_pos;
	int p = 0;

	if (desc == NULL || data == NULL || size == 0 || desc->decoder_state != qoi_decoder_body || buffer == NULL ||
		buffer_size < desc->channels || pixel_count == NULL)
	{
		return -3;
	}

	if (desc->width == 0 || desc->height == 0 || desc->channels < 3 || desc->channels > 4 || desc->colorspace > 1 ||
		desc->height >= QOI_PIXELS_MAX / desc->width)
	{
		return -4;
	}

	// restore last pixel for chunk
	px = desc->start;

	pixels = (unsigned char*)buffer;
	*pixel_count = 0;

	bytes = (const unsigned char*)data;
	for (px_pos = 0; px_pos < buffer_size && p < size; px_pos += sizeof(rgba_t))
	{

		if (desc->pixels_count >= desc->width * desc->height)
		{
			// everything decoded
			break;
		}
		if (desc->run > 0)
		{
			desc->run--;
		}
		else if (p < size)
		{
			int b1 = bytes[p];

			// check if command is complete
			if ((b1 == QOI_OP_RGB && p + 4 > size) || (b1 == QOI_OP_RGBA && p + 5 > size) ||
				((b1 & QOI_MASK_2) == QOI_OP_LUMA && p + 2 > size))
			{
				desc->last_bytes_size = size - p;

				LOG_DBG("incomplete command {:02x} {:d} {:d} {:d}", b1, desc->last_bytes_size, p, size);

				if (desc->last_bytes_size > sizeof(desc->last_bytes_size))
				{
					LOG_DBG("buffer too small");
					return -5;
				}

				memcpy(desc->last_bytes, &bytes[p], desc->last_bytes_size);
				desc->decoder_state = qoi_decoder_body_last;
				p = size;
				break;
			}

			p++;

			if (b1 == QOI_OP_RGB)
			{
				px.rgba.r = bytes[p++];
				px.rgba.g = bytes[p++];
				px.rgba.b = bytes[p++];
			}
			else if (b1 == QOI_OP_RGBA)
			{
				px.rgba.r = bytes[p++];
				px.rgba.g = bytes[p++];
				px.rgba.b = bytes[p++];
				px.rgba.a = bytes[p++];
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_INDEX)
			{
				px = desc->index[b1];
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_DIFF)
			{
				px.rgba.r += ((b1 >> 4) & 0x03) - 2;
				px.rgba.g += ((b1 >> 2) & 0x03) - 2;
				px.rgba.b += (b1 & 0x03) - 2;
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_LUMA)
			{
				int b2 = bytes[p++];
				int vg = (b1 & 0x3f) - 32;
				px.rgba.r += vg - 8 + ((b2 >> 4) & 0x0f);
				px.rgba.g += vg;
				px.rgba.b += vg - 8 + (b2 & 0x0f);
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_RUN)
			{
				desc->run = (b1 & 0x3f);
			}

			desc->index[QOI_COLOR_HASH(px) % 64] = px;
		}

		*(rgba_t*)(pixels + px_pos) = px;

		(*pixel_count)++;
		desc->pixels_count++;
	}

	if (desc->pixels_count == desc->width * desc->height)
		desc->decoder_state = qoi_decoder_done;

	// save start pixel for next chunk
	desc->start = px;

	return p;
}

int qoi_decode_body_last(qoi_desc* desc, const void* data, int size, void* buffer, int buffer_size, int* pixel_count)
{

	if (!desc || desc->last_bytes_size == 0 || desc->last_bytes_size > sizeof(desc->last_bytes) ||
		size < (int)sizeof(rgba_t))
	{
		return -10;
	}

	unsigned char data_last[sizeof(desc->last_bytes) + 1];
	int data_last_size = 0;
	int b1 = desc->last_bytes[0];

	memcpy(data_last, desc->last_bytes, desc->last_bytes_size);

	if (b1 == QOI_OP_RGB)
	{
		data_last_size = 4;
	}
	else if (b1 == QOI_OP_RGBA)
	{
		data_last_size = 5;
	}
	else if ((b1 & QOI_MASK_2) == QOI_OP_LUMA)
	{
		data_last_size = 2;
	}
	else
	{
		return -11;
	}

	memcpy(&data_last[desc->last_bytes_size], data, data_last_size - desc->last_bytes_size);
	// TODO cleanup internal state handling
	desc->decoder_state = qoi_decoder_body;

	int res = qoi_decode_body(desc, data_last, data_last_size, buffer, buffer_size, pixel_count);
	if (res != data_last_size)
	{
		return -12;
	}

	if (res < 0)
	{
		return -13;
	}

	res -= desc->last_bytes_size;

	desc->last_bytes_size = 0;

	return res;
}

// negative on error
// number of bytes read of data and pixels decoded in pixel_count
int qoi_decode_chunked(qoi_desc* desc, const void* data, int size, void* buffer, int buffer_size, int* pixel_count)
{
	int res;

	if (!desc || !pixel_count)
	{
		return -1;
	}

	*pixel_count = 0;

	switch (desc->decoder_state)
	{
	case qoi_decoder_header:
		res = qoi_decode_header(data, size, desc);
		break;
	case qoi_decoder_body_last:
		res = qoi_decode_body_last(desc, data, size, buffer, buffer_size, pixel_count);
		break;
	case qoi_decoder_body:
		res = qoi_decode_body(desc, data, size, buffer, buffer_size, pixel_count);
		break;
	case qoi_decoder_done:
		return 0;
	default:
		return -2;
	}

	return res;
}

enum qoi_decoder_state qoi_decode_state_get(qoi_desc* desc)
{
	if (!desc)
	{
		return qoi_decoder_error;
	}

	return desc->decoder_state;
}

void* qoi_decode(const void* data, int size, qoi_desc* desc, int channels)
{
	const unsigned char* bytes;
	unsigned int header_magic;
	unsigned char* pixels;
	rgba_t index[64];
	rgba_t px;
	int px_len, chunks_len, px_pos;
	int p = 0, run = 0;

	if (data == NULL || desc == NULL || (channels != 0 && channels != 3 && channels != 4) ||
		size < QOI_HEADER_SIZE + (int)sizeof(qoi_padding))
	{
		return NULL;
	}

	bytes = (const unsigned char*)data;

	header_magic = qoi_read_32(bytes, &p);
	desc->width = qoi_read_32(bytes, &p);
	desc->height = qoi_read_32(bytes, &p);
	desc->channels = bytes[p++];
	desc->colorspace = bytes[p++];

	if (desc->width == 0 || desc->height == 0 || desc->channels < 3 || desc->channels > 4 || desc->colorspace > 1 ||
		header_magic != QOI_MAGIC || desc->height >= QOI_PIXELS_MAX / desc->width)
	{
		return NULL;
	}

	if (channels == 0)
	{
		channels = desc->channels;
	}

	px_len = desc->width * desc->height * channels;
	pixels = (unsigned char*)QOI_MALLOC(px_len);
	if (!pixels)
	{
		return NULL;
	}

	QOI_ZEROARR(index);
	px.rgba.r = 0;
	px.rgba.g = 0;
	px.rgba.b = 0;
	px.rgba.a = 255;

	chunks_len = size - (int)sizeof(qoi_padding);
	for (px_pos = 0; px_pos < px_len; px_pos += channels)
	{
		if (run > 0)
		{
			run--;
		}
		else if (p < chunks_len)
		{
			int b1 = bytes[p++];

			if (b1 == QOI_OP_RGB)
			{
				px.rgba.r = bytes[p++];
				px.rgba.g = bytes[p++];
				px.rgba.b = bytes[p++];
			}
			else if (b1 == QOI_OP_RGBA)
			{
				px.rgba.r = bytes[p++];
				px.rgba.g = bytes[p++];
				px.rgba.b = bytes[p++];
				px.rgba.a = bytes[p++];
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_INDEX)
			{
				px = index[b1];
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_DIFF)
			{
				px.rgba.r += ((b1 >> 4) & 0x03) - 2;
				px.rgba.g += ((b1 >> 2) & 0x03) - 2;
				px.rgba.b += (b1 & 0x03) - 2;
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_LUMA)
			{
				int b2 = bytes[p++];
				int vg = (b1 & 0x3f) - 32;
				px.rgba.r += vg - 8 + ((b2 >> 4) & 0x0f);
				px.rgba.g += vg;
				px.rgba.b += vg - 8 + (b2 & 0x0f);
			}
			else if ((b1 & QOI_MASK_2) == QOI_OP_RUN)
			{
				run = (b1 & 0x3f);
			}

			index[QOI_COLOR_HASH(px) % 64] = px;
		}

		if (channels == 4)
		{
			*(rgba_t*)(pixels + px_pos) = px;
		}
		else
		{
			pixels[px_pos + 0] = px.rgba.r;
			pixels[px_pos + 1] = px.rgba.g;
			pixels[px_pos + 2] = px.rgba.b;
		}
	}

	return pixels;
}

#ifndef QOI_NO_STDIO
#  include <stdio.h>

int qoi_write(const char* filename, const void* data, const qoi_desc* desc)
{
	FILE* f = fopen(filename, "wb");
	int size;
	void* encoded;

	if (!f)
	{
		return 0;
	}

	encoded = qoi_encode(data, desc, &size);
	if (!encoded)
	{
		fclose(f);
		return 0;
	}

	fwrite(encoded, 1, size, f);
	fclose(f);

	QOI_FREE(encoded);
	return size;
}

void* qoi_read(const char* filename, qoi_desc* desc, int channels)
{
	FILE* f = fopen(filename, "rb");
	int size, bytes_read;
	void *pixels, *data;

	if (!f)
	{
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size <= 0)
	{
		fclose(f);
		return NULL;
	}
	fseek(f, 0, SEEK_SET);

	data = QOI_MALLOC(size);
	if (!data)
	{
		fclose(f);
		return NULL;
	}

	bytes_read = fread(data, 1, size, f);
	fclose(f);

	pixels = qoi_decode(data, bytes_read, desc, channels);
	QOI_FREE(data);
	return pixels;
}

#endif /* QOI_NO_STDIO */