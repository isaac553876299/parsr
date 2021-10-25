

#ifndef PARSR
#define PARSR

#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <chrono>

#include <sstream>

#

const std::string indent_n(size_t n)
{
	return std::string(2 * n, ' ');
}

template<class T>
std::ostream& operator<< (std::ostream& stream, const std::list<T>& list)
{
	for (const auto& i : list)
	{
		stream << i;
	}

	return stream;
}

struct parsr_attribute
{
	std::string name;
	std::string value;

	void clear()
	{
		name.clear();
		value.clear();
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_attribute& attribute)
	{
		return stream << " " << attribute.name << "=\"" << attribute.value << "\"";
	}
};

struct parsr_node
{
	parsr_node* parent = nullptr;
	unsigned int indent = 0;
	std::string name;
	std::list<parsr_attribute> attributes;
	std::string text;
	std::list<parsr_node> nodes;

	void clear()
	{
		// parent?
		name.clear();
		attributes.clear();
		text.clear();
		nodes.clear();
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_node& node)
	{
		stream << indent_n(node.indent) << "<" << node.name << node.attributes;
		if (node.text.empty() && node.nodes.empty())
		{
			stream << "/>" << std::endl; // < /> perhaps < ></ >
		}
		else
		{
			stream << ">" << node.text << std::endl << node.nodes << indent_n(node.indent) << "</" << node.name << ">" << std::endl;
		}
		return stream;
	}
};

struct parsr_document
{
	parsr_node root;

	parsr_document()
	{
		// clear();
	}

	parsr_document(const std::string file_name_path)
	{
		// clear();
		load(file_name_path);
	}

	~parsr_document()
	{
		// clear();
	}

	void clear()
	{
		root.clear();
	}

	bool load(const std::string file_name_path)
	{
		auto start_clock = std::chrono::high_resolution_clock::now();

		std::fstream file(file_name_path, std::ios::in); // using ifstream needn't close()
		if (!file.is_open())
		{
			std::cout << "error: open file" << std::endl;
			return false;
		}
		std::string str, str2;
		while (std::getline(file, str2)) str += str2 += '\n';
		/*std::stringstream sstream;
		sstream << file.rdbuf();
		std::string str = sstream.str();*/

		//auto finish_clock = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<float> elapsed = finish_clock - start_clock;

		bool parsed = parse_string(str);
		
		std::cout << std::endl << "\x1B[34mfile \033[0m" << file_name_path
			<< (parsed ? " \x1B[32mwell_formed\033[0m" : " \x1B[31mill_formed\033[0m")
			<< " (" << -1 << " \x1B[35mbytes\033[0m # "
			<< (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " \x1B[36mseconds\033[0m)" << std::endl
			<< str << *this << std::endl;

		return parsed;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream
			<< std::endl << "\x1B[33mdocument \033[0m("
			<< -1 << " \x1B[35mbytes\033[0m # "
			<< -1 << " \x1B[31mnodes\033[0m # "
			<< -1 << " \x1B[31mattributes\033[0m)"
			<< std::endl << document.root << std::endl;
	}

	bool parse_string(const std::string& str)
	{
		parsr_node node;
		parsr_node* node2append2 = &node;
		unsigned int indent = 1;

		const size_t MAX_TOKENS = 100;
		size_t tokens[MAX_TOKENS];
		for (auto& i : tokens) i = std::string::npos;
		size_t ii = 0;
		for (size_t i = 0; i < std::string::npos && ii < MAX_TOKENS; i = str.find_first_of("</ =\">", i + 1))
		{
			tokens[ii++] = (i); printf("%9d [%c]\n", i, str[i]);
		}
		std::cout << std::string(9, '-') << " tokens " << std::string(9, '-') << std::endl;
		bool well_formed = true;
		// TODO: tokens[i] != std::string::npos
		for (size_t i = 0; i < MAX_TOKENS && tokens[i] != std::string::npos && well_formed; ++i)
		{
			if (str[tokens[i]] == '<')
			{
				if (str[tokens[i + 1]] == '/')
				{
					if (str[tokens[i + 2]] == '>')
					{
						if (tokens[i + 1] == tokens[i] + 1)
						{
							std::string x = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							if (node2append2->name == x)
							{
								std::cout << "</ > #" << x << "#" << std::endl;
								node2append2 = node2append2->parent;
								--indent;
								i += 2;
							}
							else
							{
								std::cout << "ill </ >" << std::endl;
								well_formed = false;
							}
						}
						else if (tokens[i + 1] == tokens[i + 2] - 1)
						{
							std::string x = str.substr(tokens[i] + 1, tokens[i + 2] - 1 - (tokens[i] + 1));
							std::cout << "< /> #" << x << "#" << std::endl;
							node2append2->nodes.push_back({ node2append2,indent/*++*/,x });
							i += 2;
						}
						else
						{
							std::cout << "ill < />" << std::endl;
							well_formed = false;
						}
					}
					else
					{
						well_formed = false;
					}
				}
				else if (str[tokens[i + 1]] == '>')
				{
					std::string x = str.substr(tokens[i] + 1, tokens[i + 1] - (tokens[i] + 1));
					std::cout << "< > #" << x << "#" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,x });
					node2append2 = &node2append2->nodes.back();
					++i;
				}
				else if (str[tokens[i + 1]] == ' ')
				{
					std::string x = str.substr(tokens[i] + 1, tokens[i + 1] - (tokens[i] + 1));
					std::cout << "< > #" << x << "#" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,x });
					node2append2 = &node2append2->nodes.back();
					while (str[tokens[i + 1]] == ' ')
					{
						if (str[tokens[i + 2]] == '=' && str[tokens[i + 3]] == '\"' && str[tokens[i + 4]] == '\"')
						{
							std::string n = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							std::cout << "= #" << n << "#" << std::endl;
							std::string v = str.substr(tokens[i + 3] + 1, tokens[i + 4] - (tokens[i + 3] + 1));
							std::cout << "\"\" #" << v << "#" << std::endl;
							node2append2->attributes.push_back({ n,v });
							i += 4;
						}
					}
					if (str[tokens[i + 1]] == '/')
					{
						if (str[tokens[i + 2]] == '>')
						{
							std::string x = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							std::cout << "< /> #" << x << "#" << std::endl;
							node2append2 = node2append2->parent;
							--indent;
							i += 2;
						}
					}
				}
				else
				{
					std::cout << "ill < >" << std::endl;
					well_formed = false;
				}
			}
		}
		
		well_formed = true;
		if (well_formed)
		{
			clear();
			root = node;
		}

		return true;
	}

};

#endif

