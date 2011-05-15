#include <iostream>
#include <string>
#include <vector>

using namespace std;

class column{
	const vector<string>& board;
	int col;
public:
	column(const vector<string>& _board, int _col)
		: board(_board), col(_col) {}
	size_t size() const {return board.size();}
	char operator[] (size_t i) const {return board[i][col];}
};

template <class T>
void encode_line(const T& line)
{
	int block = 0;
	bool has_blocks = false;
	for (size_t i=0; i<line.size(); ++i)
	{
		switch (line[i])
		{
		case '.':
			if (block != 0)
			{
				if (has_blocks)
					cout << " ";
				else 
					has_blocks = true;
				cout << block;
				block = 0;
			}
			break;
		case '#':
			block += 1;
			break;
		default:
			cerr << "Unexpected char \'" << line[i] << "\' (#"<<(int)line[i]<<")"<< endl;
			exit(1);
		}
	}
	if (block != 0)
	{
		if (has_blocks)
			cout << " ";
		else 
			has_blocks = true;
		cout << block;
	}
	if (!has_blocks)
		cout << "0";
	cout << endl;
}

void main(int argc, char** argv)
{
	vector<string> board;
	string line;
	while (getline(cin, line))
	{
		if (line.empty() ||
			line.size() >= 2 &&
			line[0] == '/' && line[1] == '/')
		{
			cout << line << endl;
		}
		else
		{
			board.push_back(line);
			if (line.size() != board[0].size())
			{
				cerr << "invalid line size" << endl;
				exit(1);
			}
		}
	}
	for (size_t col = 0; col < board[0].size(); ++col)
		encode_line(column(board, col));
	for (size_t row = 0; row < board.size(); ++row)
		encode_line(board[row]);
}