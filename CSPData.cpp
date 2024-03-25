#include "CSPData.h"


CSPData::CSPData(std::string filename)
{
	int numPatterns{};
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file " << filename << std::endl;
		exit(1);
	}
    
    std::string line{};
    std::getline(file, line);
    numPatterns = std::stoi(line);
    std::getline(file, line);
    capacity = std::stoi(line);
    
    while (std::getline(file, line))
    {
        int length{}, demand{};
		std::stringstream ss(line);
        ss >> length >> demand;
        orders.push_back({ Order{length, demand} });
    }
}

std::vector<Pattern> CSPData::GenerateInitialPatterns() const
{
	std::vector<Pattern> initialPatterns(orders.size(), Pattern(orders.size(), 0));
    for (int i = 0; i < orders.size(); i++)
    {
		initialPatterns[i][i] = capacity / orders[i].length;
	}
    return initialPatterns;
}