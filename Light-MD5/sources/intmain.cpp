#include <iostream>
#include "lightMD5.h" // template constexpr require c++17: -std=c++17, if you can't use std::string md5_process(uint8_t*,size_t);

	int main(){	
	uint8_t *uint8_ptr = new uint8_t[4];
	uint8_ptr[0] = 109;
	uint8_ptr[1] = 105;
	uint8_ptr[2] = 107;
	uint8_ptr[3] = 105;
	std::cout<<md5(uint8_ptr, 4)<<"\n";
	delete[] uint8_ptr;

	uint8_t uint8_array[] = {109,105,107,105};
	std::cout<<md5(uint8_array, sizeof(uint8_array))<<"\n"; // <-- == md5_process(uint8_array, ...)
	
	const char *char_ptr = "miki"; 
	std::cout<<md5(char_ptr,strlen(char_ptr))<<"\n";        // <-- == md5_process((uint8_t*)char_ptr, ...)
	
	char char_array[] = "miki"; 
	std::cout<<md5(char_array,strlen(char_array))<<"\n";    // <-- == md5_process((uint8_t*)char_array, ...)
	
	std::string my_str = "miki"; 
	std::cout<<md5(my_str,my_str.length());                 // <-- == md5_process((uint8_t*)my_str.c_str(), ...)
	 
	return 0;
	}
	
