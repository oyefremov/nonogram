#include <iostream>
#include <map>
#include <string>

using namespace std;

void generate(int cx, int cy, int seed, int probability)
{
	cout << "// size " << cx << ", " << cy << endl;
	cout << "// seed " << seed << endl;
	cout << "// prob " << probability << "%" << endl;
	srand(seed);
	for (int y = 0; y < cy; ++y)
	{
		for (int x = 0; x < cx; ++x)
			cout.put(rand() % 100 < probability ? '#' : '.');
		cout << endl;
	}
}

int main(int argc, char** argv)
{
	typedef std::map<std::string, int*> args_map_t;
	args_map_t args_map;

	int size = 20;
	int seed = 1;
	int probability = 55;

	args_map["-size"] = &size;
	args_map["-seed"] = &seed;
	args_map["-prob"] = &probability;
	args_map["-probability"] = &probability;

	if (argc == 2 && strcmp(argv[1], "-?"))
	{
			std::cout 
				<< "-size [size]" << endl
				<< "-seed [seed]" << endl
				<< "-probabilit [probabilit]" << endl;
			return 1;
	}

	++argv;

	while (argc > 2)
	{
		char* arg = argv[0];
		char* val = argv[1];
		argv += 2;
		argc -= 2;
		args_map_t::iterator p = args_map.find(arg);
		if (p == args_map.end())
		{
			std::cerr << "Unsupported argument " << arg << std::endl;
			return 1;
		}
		int intVal = atoi(val);
		if (intVal <= 0)
		{
			std::cerr << "Invalid value " << val << " for argument " << arg << std::endl;
			return 1;
		}
		*(p->second) = intVal;
	}
	if (argc == 2)
	{
		std::cerr << "Invalid argument " << *argv << std::endl;
		return 1;
	}
	generate(size, size, seed, probability);	
}