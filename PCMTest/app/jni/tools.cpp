#include"tools.h"


int get_ADTS_frame(unsigned char* buffer, size_t buf_size, unsigned char* data, size_t* data_size)
{
	size_t size = 0;
	if (!buffer || !data || !data_size)
	{
		return -1;
	}
	while (1)
	{
		if (buf_size < 7)
		{
			return -1;
		}
		if ((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0))
		{
			size |= ((buffer[3] & 0x03) << 11);     //high 2 bit
			size |= buffer[4] << 3;                 //middle 8 bit
			size |= ((buffer[5] & 0xe0) >> 5);      //low 3bit
			break;
		}
		--buf_size;
		++buffer;
	}

	if (buf_size < size)
	{
		return -1;
	}
	memcpy(data, buffer, size);
	*data_size = size;
	return 0;
}