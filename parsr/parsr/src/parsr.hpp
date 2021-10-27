

#ifndef PARSR
#define PARSR

#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <chrono>

#include <sstream>

#

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
	
	size_t size() const
	{
		size_t r = 0;
		
		return r;
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

	size_t size() const
	{
		size_t r = 0;
		
		return r;
	}

	size_t count_nodes() const
	{
		size_t r = 0;
		r += nodes.size();
		for (const auto& i : nodes)
		{
			r += i.count_nodes();
		}
		return r;
	}
	size_t count_attributes() const
	{
		size_t r = 0;
		r += attributes.size();
		for (const auto& i : nodes)
		{
			r += i.count_attributes();
		}
		return r;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_node& node)
	{
		stream << std::string(node.indent * 2, ' ') << "<" << node.name << node.attributes;
		if (node.text.empty() && node.nodes.empty())
		{
			stream << "/>" << std::endl; // < /> perhaps < ></ >
		}
		else
		{
			stream << ">" << node.text << std::endl << node.nodes << std::string(node.indent * 2, ' ') << "</" << node.name << ">" << std::endl;
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

	size_t size() const
	{
		size_t r = 0;
		
		return r;
	}

	size_t count_nodes() const
	{
		size_t r = 0;
		r += root.count_nodes();
		return r;
	}
	size_t count_attributes() const
	{
		size_t r = 0;
		r += root.count_attributes();
		return r;
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

		bool parsed = parse_string(str);
		
		std::cout << std::endl << (char)(201) << "\x1B[34mfile\033[0m" << (char)(187) << " " << file_name_path
			<< (parsed ? " \x1B[32mwell_formed\033[0m" : " \x1B[31mill_formed\033[0m")
			<< " (" << (str.capacity()) << " \x1B[35mbytes\033[0m # "
			<< (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " \x1B[36mseconds\033[0m)" << std::endl
			<< (char)(200) << std::string(sizeof("file") - 1, 205) << (char)(188) << std::endl
			/*<< str */ << *this << std::endl;

		return parsed;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream
			<< std::endl << (char)(201) << "\x1B[33mdocument\033[0m" << (char)(187) << " ("
			<< document.size() << " \x1B[35mbytes\033[0m # "
			<< document.count_nodes() << " \x1B[31mnodes\033[0m # "
			<< document.count_attributes() << " \x1B[31mattributes\033[0m)"
			<< std::endl << (char)(200) << std::string(sizeof("document") - 1, 205) << (char)(188)
			<< std::endl << document.root << std::endl;
	}

	bool parse_string(const std::string& str)
	{
		// throw ill_formed
		// unique attributes
		// single root
		// check find methods ""''[2]

		bool debug = true;

		parsr_node node;
		parsr_node* node2append2 = &node;
		unsigned int indent = 1;

		const size_t MAX_TOKENS = 4096;
		size_t tokens[MAX_TOKENS];
		for (auto& i : tokens) i = std::string::npos;
		size_t ii = 0;
		for (size_t i = 0; i < std::string::npos && ii < MAX_TOKENS; i = str.find_first_of("</ =\">", i + 1))
		{
			tokens[ii++] = (i); if (debug) printf("%9d [%c]", i, str[i]); if (debug) if (ii % 5 == 0) std::cout << std::endl;
		}
		if (debug) std::cout << std::endl << std::string(9, '-') << " tokens " << std::string(9, '-') << std::endl;

		std::string x, y, z;

		bool well_formed = true;
		for (size_t i = 0; i < MAX_TOKENS && tokens[i] != std::string::npos && well_formed; ++i)
		{
			switch (str[tokens[i]])
			{
			case '<':

				break;
			case '>':

				break;
			case '/':

				break;
			case ' ':

				break;
			default:

				break;
			}
			if (1 && str[tokens[i]] == '<')
			{
				x = str.substr(tokens[i] + 1, tokens[i + 1] - (tokens[i] + 1));
				if (debug) std::cout << "< #" << x << "#" << std::endl;
				node2append2->nodes.push_back({ node2append2,indent++,x });
				node2append2 = &node2append2->nodes.back();
				well_formed = false;
				if (str[tokens[i + 1]] == '>')
				{
					if (debug) std::cout << "> #" << x << "#" << std::endl;
					well_formed = true;
					//++i;
				}
				if (str[tokens[i + 1]] == '/')
				{
					if (str[tokens[i + 2]] == '>')
					{
						well_formed = false;
						if (tokens[i + 1] == tokens[i + 2] - 1)
						{
							if (debug) std::cout << "/> #" << x << "#" << std::endl;
							node2append2 = node2append2->parent;
							node2append2->nodes.back().indent -= 1;
							well_formed = true;
						}
						else if (tokens[i + 1] == tokens[i] + 1)
						{
							x = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							if (node2append2->name == x)
							{
								if (debug) std::cout << "</ #" << x << "#" << std::endl;
								node2append2 = node2append2->parent;
								--indent;
								well_formed = true;
							}
						}
						i += 2;
					}
				}
				while (str[tokens[i + 1]] == ' ')
				{
					well_formed = false;
					if (str[tokens[i + 2]] == '=' && str[tokens[i + 3]] == '\"' && str[tokens[i + 4]] == '\"')
					{
						y = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
						z = str.substr(tokens[i + 3] + 1, tokens[i + 4] - (tokens[i + 3] + 1));
						if (debug) std::cout << "= #" << y << "# \"\" #" << z << "#" << std::endl;
						node2append2->attributes.push_back({ y,z });
						i += 3;//3?
						well_formed = true;
					}
					++i;
				}
			}

			if (0 && str[tokens[i]] == '<')
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
								if (debug) std::cout << "</ > #" << x << "#" << std::endl;
								node2append2 = node2append2->parent;
								--indent;
								i += 2;
							}
							else
							{
								if (debug) std::cout << "ill </ >" << std::endl;
								well_formed = false;
							}
						}
						else if (tokens[i + 1] == tokens[i + 2] - 1)
						{
							std::string x = str.substr(tokens[i] + 1, tokens[i + 2] - 1 - (tokens[i] + 1));
							if (debug) std::cout << "< /> #" << x << "#" << std::endl;
							node2append2->nodes.push_back({ node2append2,indent/*++*/,x });
							i += 2;
						}
						else
						{
							if (debug) std::cout << "ill < />" << std::endl;
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
					if (debug) std::cout << "< > #" << x << "#" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,x });
					node2append2 = &node2append2->nodes.back();
					++i;
				}
				else if (str[tokens[i + 1]] == ' ')
				{
					std::string x = str.substr(tokens[i] + 1, tokens[i + 1] - (tokens[i] + 1));
					if (debug) std::cout << "< > #" << x << "#" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,x });
					node2append2 = &node2append2->nodes.back();
					while (str[tokens[i + 1]] == ' ')
					{
						if (str[tokens[i + 2]] == '=' && str[tokens[i + 3]] == '\"' && str[tokens[i + 4]] == '\"')
						{
							std::string n = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							std::string v = str.substr(tokens[i + 3] + 1, tokens[i + 4] - (tokens[i + 3] + 1));
							if (debug) std::cout << "= #" << n << "# \"\" #" << v << "#" << std::endl;
							node2append2->attributes.push_back({ n,v });
							i += 4;
						}
					}
					if (str[tokens[i + 1]] == '/')
					{
						if (str[tokens[i + 2]] == '>')
						{
							std::string x = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							if (debug) std::cout << "< /> #" << x << "#" << std::endl;
							node2append2 = node2append2->parent;
							--indent;
							i += 2;
						}
					}
				}
				else
				{
					if (debug) std::cout << "ill < >" << std::endl;
					well_formed = false;
				}
			}
		}
		
		if (well_formed)
		{
			clear();
			root = node;
		}

		return well_formed;
	}

};

#endif

