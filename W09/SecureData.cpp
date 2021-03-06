// Name: Aria Avazkhani
// Seneca Student ID: 134465160
// Seneca email: aavazkhani@myseneca.ca
// Date of completion: 2018-11-22
//
// I confirm that the content of this file is created by me,
// with the exception of the parts provided to me by my professor.


// Workshop 9 - Multi-Threading
// SecureData.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <functional>
#include "SecureData.h"

using namespace std;

namespace w9 {

	void converter(char* t, char key, int n, const Cryptor& c) {
		for (int i = 0; i < n; i++)
			t[i] = c(t[i], key);
	}

	SecureData::SecureData(const char* file, char key, ostream* pOfs)
	{
		ofs = pOfs;

		// open text file
		fstream input(file, std::ios::in);
		if (!input)
			throw string("\n***Failed to open file ") +
			string(file) + string(" ***\n");

		// copy from file into memory
		input.seekg(0, std::ios::end);
		nbytes = (int)input.tellg() + 1;

		text = new char[nbytes];

		input.seekg(ios::beg);
		int i = 0;
		input >> noskipws;
		while (input.good())
			input >> text[i++];
		text[nbytes - 1] = '\0';
		*ofs << "\n" << nbytes - 1 << " bytes copied from file "
			<< file << " into memory (null byte added)\n";
		encoded = false;

		// encode using key
		code(key);
		*ofs << "Data encrypted in memory\n";
	}

	SecureData::~SecureData() {
		delete[] text;
	}

	void SecureData::display(std::ostream& os) const {
		if (text && !encoded)
			os << text << std::endl;
		else if (encoded)
			throw std::string("\n***Data is encoded***\n");
		else
			throw std::string("\n***No data stored***\n");
	}

	void SecureData::code(char key)
	{
		// TODO: rewrite this function to use at least two threads
		//         to encrypt/decrypt the text.

		int half = nbytes / 2;
		thread th1(bind(converter, text, key, half, Cryptor()));
		thread th2(bind(converter, text + half, key, half, Cryptor()));

		th1.join();
		th2.join();

		encoded = !encoded;
	}

	void SecureData::backup(const char* file) {
		if (!text)
			throw std::string("\n***No data stored***\n");
		else if (!encoded)
			throw std::string("\n***Data is not encoded***\n");
		else
		{
			// TODO: open a binary file for writing

			fstream l_fileIn(file, ios::out | ios::binary);
			if (!l_fileIn)
				throw string("\n***Failed to open file ") +
				string(file) + string(" ***\n");

			// TODO: write data into the binary file
			//         and close the file

			l_fileIn.write(text, nbytes);
			l_fileIn.close();

		}
	}

	void SecureData::restore(const char* file, char key) {
		// TODO: open binary file for reading

		fstream l_fileOut(file, ios::in | ios::binary);
		if (!l_fileOut)
			throw string("\n***Failed to open file ") +
			string(file) + string(" ***\n");

		// TODO: - allocate memory here for the file content

		nbytes = 0;
		l_fileOut >> std::noskipws;
		while (l_fileOut.good()) {
			char key;
			l_fileOut >> key;
			nbytes++;
		}
		l_fileOut.clear();
		l_fileOut.seekg(0, ios::beg);
		text = new char[nbytes];

		// TODO: - read the content of the binary file
		nbytes--;
		l_fileOut.read(text, nbytes);
		text[nbytes] = '\0';

		*ofs << "\n" << nbytes << " bytes copied from binary file "
			<< file << " into memory.\n";

		encoded = true;

		// decode using key
		code(key);

		*ofs << "Data decrypted in memory\n\n";
	}

	std::ostream& operator<<(std::ostream& os, const SecureData& sd) {
		sd.display(os);
		return os;
	}
}
