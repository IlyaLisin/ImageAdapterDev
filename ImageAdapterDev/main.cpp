#include "ImageAdapter.h"

int main()
{
	ImageAdapter *adapter = new ImageAdapter();

	adapter->SendImage("image_byte_array", 50, "guid");
	adapter->StartLearn("guid", 50);
	adapter->~ImageAdapter();
	
	return 1;
}