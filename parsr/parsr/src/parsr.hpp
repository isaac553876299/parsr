

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

	std::string to_string() const
	{
		std::string str;

		str += " ";
		str += name;
		str += "=\"";
		str += value;
		str += "\"";

		return str;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_attribute& attribute)
	{
		return stream << attribute.to_string();
	}

	int to_int() const
	{
		int r = 0;
		std::stringstream s(value);
		s >> r;
		return r;
	}
	float to_float() const
	{
		float r = 0;
		std::stringstream s(value);
		s >> r;
		return r;
	}
	bool to_bool() const
	{
		bool r = 0;
		std::stringstream s(value);
		s >> r;
		return r;
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

	std::string to_string() const
	{
		std::string str;
		
		str += std::string(indent * 2, ' ');
		str += "<";
		str += name;
		for (auto& a : attributes)
		{
			str += a.to_string();
		}
		if (text.empty() && nodes.empty())
		{
			str += "/>";// < /> perhaps < ></ >
		}
		else
		{
			str += ">";
			str += "\n";
			if (!text.empty())
			{
				str += text;
				str += "\n";
			}
			for (auto& n : nodes)
			{
				str += n.to_string();
			}
			str += std::string(indent * 2, ' ');
			str += "</";
			str += name;
			str += ">";
		}
		str += "\n";

		return str;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_node& node)
	{
		return stream << node.to_string();
	}

	const parsr_node* child(std::string tag) const
	{
		for (const auto& i : nodes)
		{
			if (i.name == tag)
			{
				return &i;
			}
		}
		return 0;
	}
	const parsr_attribute* attribute(std::string tag) const
	{
		for (const auto& i : attributes)
		{
			if (i.name == tag)
			{
				return &i;
			}
		}
		return 0;
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

	bool load(const std::string file_name_path, const bool debug_info = false)
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
		file.close();

		bool parsed = parse_string(str, debug_info);
		
		std::cout << std::endl << "\x1B[34mload\033[0m " << file_name_path
			<< (parsed ? " \x1B[32mwell_formed\033[0m" : " \x1B[31mill_formed\033[0m")
			<< " (" << (str.capacity()) << " bytes # "
			<< (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " seconds)" << std::endl;
		if (debug_info)
		{
			std::cout << str << *this << std::endl;
		}

		return parsed;
	}

	bool save(const std::string file_name_path, const bool debug_info = false)
	{
		auto start_clock = std::chrono::high_resolution_clock::now();

		std::fstream file(file_name_path, std::ios::out | std::ios::trunc);
		if (!file.is_open())
		{
			std::cout << "error: open file" << std::endl;
			return false;
		}

		std::string str = root.to_string();

		bool parsed = !str.empty();
		if (parsed)
		{
			file << str;
		}
		file.close();//ofstream?

		std::cout << std::endl << "\x1B[36msave\033[0m " << file_name_path
			<< " (" << (str.capacity()) << " bytes # "
			<< (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " seconds)" << std::endl;
		if (debug_info)
		{
			std::cout << str << std::endl;
		}

		return parsed;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream
			<< std::endl << "\x1B[33mdocument\033[0m" << " ("
			<< document.size() << " bytes # "
			<< document.count_nodes() << " nodes # "
			<< document.count_attributes() << " attributes)"
			<< std::endl << document.root << std::endl;
	}

	bool parse_string(const std::string& str, const bool debug_info = false)
	{
		// throw ill_formed
		// unique attributes
		// single root
		// check find methods ""''[2]
		// find text

		parsr_node node;
		parsr_node* node2append2 = &node;
		unsigned int indent = 0;

		const size_t MAX_TOKENS = 4096;
		size_t tokens[MAX_TOKENS];
		for (auto& i : tokens) i = std::string::npos;
		size_t ii = 0;
		for (size_t i = 0; i < std::string::npos && ii < MAX_TOKENS; i = str.find_first_of("</ =\">", i + 1))
		{
			tokens[ii++] = (i); if (debug_info) printf(" %d[%c]", i, str[i]);
		}
		if (debug_info) std::cout << std::endl << std::string(9, '-') << " \x1B[35mtokens\033[0m " << std::string(9, '-') << std::endl;

		std::string x, y, z;

		bool well_formed = true;
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
								if (debug_info) std::cout << "</ > #" << x << "#" << std::endl;
								node2append2 = node2append2->parent;
								--indent;
								i += 2;
							}
							else
							{
								if (debug_info) std::cout << "ill </ >" << std::endl;
								well_formed = false;
							}
						}
						else if (tokens[i + 1] == tokens[i + 2] - 1)
						{
							std::string x = str.substr(tokens[i] + 1, tokens[i + 2] - 1 - (tokens[i] + 1));
							if (debug_info) std::cout << "< /> #" << x << "#" << std::endl;
							node2append2->nodes.push_back({ node2append2,indent/*++*/,x });
							i += 2;
						}
						else
						{
							if (debug_info) std::cout << "ill < />" << std::endl;
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
					if (debug_info) std::cout << "< > #" << x << "#" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,x });
					node2append2 = &node2append2->nodes.back();
					++i;
				}
				else if (str[tokens[i + 1]] == ' ')
				{
					std::string x = str.substr(tokens[i] + 1, tokens[i + 1] - (tokens[i] + 1));
					if (debug_info) std::cout << "< > #" << x << "#" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,x });
					node2append2 = &node2append2->nodes.back();
					while (str[tokens[i + 1]] == ' ')
					{
						if (str[tokens[i + 2]] == '=' && str[tokens[i + 3]] == '\"' && str[tokens[i + 4]] == '\"')
						{
							std::string n = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							std::string v = str.substr(tokens[i + 3] + 1, tokens[i + 4] - (tokens[i + 3] + 1));
							if (debug_info) std::cout << "= #" << n << "# \"\" #" << v << "#" << std::endl;
							node2append2->attributes.push_back({ n,v });
							i += 4;
						}
					}
					if (str[tokens[i + 1]] == '/')
					{
						if (str[tokens[i + 2]] == '>')
						{
							std::string x = str.substr(tokens[i + 1] + 1, tokens[i + 2] - (tokens[i + 1] + 1));
							if (debug_info) std::cout << "< /> #" << x << "#" << std::endl;
							node2append2 = node2append2->parent;
							--indent;
							i += 2;
						}
					}
				}
				else
				{
					if (debug_info) std::cout << "ill < >" << std::endl;
					well_formed = false;
				}
			}
		}

		well_formed = true;
		node.clear();
		node2append2 = &node;
		parsr_node* original = &node;
		size_t alpha = 0;
		while (1)
		{
			if (size_t a = str.find_first_of("<", alpha) != std::string::npos)
			{
				if (size_t b = str.find_first_of(">", a + 1) != std::string::npos)
				{
					for (size_t c = a + 1; c != std::string::npos && c < b; c = str.find_first_of(" ", c + 1))
					{
						if (size_t d = str.find_first_of("=", c + 1) != std::string::npos)
						{
							if (size_t e = str.find_first_of("\"", d + 1) != std::string::npos)
							{
								if (size_t f = str.find_first_of("\"", e + 1) != std::string::npos)
								{
									node2append2->attributes.push_back({ str.substr(c + 1,d - 1 - (c + 1)),str.substr(e + 1,f - 1 - (e + 1)) });
								}
							}
						}
					}
					{
						if (str[b - 1] == '/' && str[a + 1] != '/')
						{
							node2append2->nodes.push_back({ node2append2,0,str.substr(a + 1,b - 2) });
						}
						/*else*/if (str[a + 1] == '/' && str[b - 1] != '/')
						{
							if (node2append2->name == str.substr(a + 2, b - 1))
							{
							}
						}
						else
						{
							node2append2->nodes.push_back({ node2append2,0,str.substr(a + 1,b - 1) });
							node2append2 = &node2append2->nodes.back();
						}
					}
				}
			}
		}

		if (well_formed)
		{
			clear();
			root = node.nodes.front();
		}

		return well_formed;
	}

};

#endif

