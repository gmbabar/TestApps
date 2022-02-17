// https://stackoverflow.com/questions/20100059/boost-gzip-compressor-and-gzip-decompressor/20143445

#include <zlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>


int main () {

	std::string stringToBeCompressed = "quick brown fox jumps over the lazy dog";

	// Compression
	std::string compressedString;
	{
		filtering_ostream compressingStream;
		compressingStream.push(boost::iostreams::gzip_compressor(gzip_params(gzip::best_compression)));
		compressingStream.push(boost::iostreams::back_inserter(compressedString));
		compressingStream << stringToBeCompressed;
		boost::iostreams::close(compressingStream);
	}
	std::cout << "Pre-compression: " << stringToBeCompressed << std::endl;
	std::cout << "Postcompression: " << compressedString << std::endl;

	// Decompression 
	std::string decompressedString;
	{
		boost::iostreams::filtering_ostream decompressingStream;
		decompressingStream.push(boost::iostreams::gzip_decompressor());
		decompressingStream.push(boost::iostreams::back_inserter(decompressedString));
		decompressingStream << stringToBeDecompressed;
		boost::iostreams::close(decompressingStream);
	}

	std::cout << "Decompressed: " << decompressedString << std::endl;
}


