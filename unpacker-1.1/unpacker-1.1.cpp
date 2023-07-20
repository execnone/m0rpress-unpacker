#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>

bool doesFileExist(const std::string& filePath);
unsigned char* ReadFileFromOffset(const std::string& filePath, std::size_t offset, std::size_t& dataSize);
void WriteFile(const std::string& filePath, const unsigned char* content, std::size_t size);

unsigned char* De_M0rCryptData(unsigned char* encryptedData, std::size_t size, const char* inputKey, std::size_t keySize);
char De_M0rCryptByte(char plainByte, const char* inputKey, std::size_t size);
const char* GetM0rKey(const std::string& filePath, int offset, std::size_t count);
const char* decryptString(const char* input);

int keyOffset = 0x4C28;
int packOffset = 0x5C00;

int main(int argc, char* argv[])
{
	printf("\nm0rpress 1.1 unpacker | github.com/execnone\n");
	if (argc < 3)
	{
		printf("[!] usage: unpacker.exe inputfile outputfile\n\n");
		return 1;
	}

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    if (doesFileExist(inputFile))
    {
		const char* key = GetM0rKey(inputFile, keyOffset, 100);

		std::size_t size;
		unsigned char* bytes = De_M0rCryptData(ReadFileFromOffset(inputFile, packOffset, size), size, key, strlen(key));

		WriteFile(outputFile, bytes, size);
		printf("[!] successfully unpacked file : %s\n\n", outputFile);
    }
	else
	{
		printf("[!] cannot open file : %s\n\n", inputFile);
		return 1;
	}

	return 0;
}

void WriteFile(const std::string& filePath, const unsigned char* content, std::size_t size)
{
	std::ofstream file(filePath, std::ios::binary);

	if (!file)
	{
		return;
	}

	file.write(reinterpret_cast<const char*>(content), size);
}

bool doesFileExist(const std::string& filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

unsigned char* ReadFileFromOffset(const std::string& filePath, std::size_t offset, std::size_t& dataSize)
{
	std::ifstream file(filePath, std::ios::binary);

	if (!file)
	{
		return nullptr;
	}

	file.seekg(offset, std::ios::beg); // Belirtilen ofsete dosya konumunu ayarlayın

	std::size_t fileSize = static_cast<std::size_t>(file.seekg(0, std::ios::end).tellg()); // Dosya boyutunu alın
	std::size_t remainingSize = fileSize - offset; // Geriye kalan verinin boyutunu hesaplayın

	unsigned char* content = new unsigned char[remainingSize]; // Bellekte yer ayırın

	file.seekg(offset, std::ios::beg); // Ofsete geri dönün
	file.read(reinterpret_cast<char*>(content), remainingSize); // Dosyanın geri kalanını okuyun

	dataSize = remainingSize; // Veri boyutunu aktarın

	return content;
}

char De_M0rCryptByte(char plainByte, const char* inputKey, std::size_t size)
{
	for (std::size_t i = 0; i < size; i++)
	{
		if (i % 2 == 0)
		{
			plainByte -= static_cast<int>(inputKey[i]);
		}
		else
		{
			plainByte += static_cast<int>(inputKey[i]);
		}
	}

	return plainByte;
}

unsigned char* De_M0rCryptData(unsigned char* encryptedData, std::size_t size, const char* inputKey, std::size_t keySize)
{
	unsigned char* decryptedData = new unsigned char[size];
	std::memcpy(decryptedData, encryptedData, size);

	for (std::size_t i = 0; i < size; i++)
	{
		decryptedData[i] = De_M0rCryptByte(decryptedData[i], inputKey, keySize);
	}

	return decryptedData;
}

const char* GetM0rKey(const std::string& filePath, int offset, std::size_t count)
{
	std::ifstream file(filePath);

	if (!file) {
		std::cerr << "[!] cannot open file!" << std::endl;
		exit(1);
	}

	file.seekg(offset);

	std::vector<char> content;
	char ch;
	while (file.get(ch) && content.size() < count) {
		content.push_back(ch);
	}

	// Bellekte dinamik olarak bir alan oluşturur ve içeriğini kopyalar
	char* result = new char[content.size() + 1];
	std::copy(content.begin(), content.end(), result);
	result[content.size()] = '\0'; // Sonlandırma karakteri ekle

	return result;
}

const char* decryptString(const char* input)
{
	size_t length = strlen(input);
	char* decryptedString = new char[length + 1];
	decryptedString[length] = '\0';

	for (size_t i = 0; i < length; ++i)
	{
		decryptedString[i] = (input[i] - 30);
	}

	return decryptedString;
}
