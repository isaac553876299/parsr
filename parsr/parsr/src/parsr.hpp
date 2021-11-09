

#ifndef PARSR
#define PARSR

#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <chrono>

#include <sstream>

#

const std::string test =
"<node>\n"
"  <subnode one=\"1\" two=\"1.99\">\n"
"    <subsubnode/>\n"
"  </subnode>\n"
"</node>\n";

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
		return (" " + name + "=\"" + value + "\"");
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
		
		str += std::string(indent * 2, ' ') + "<" + name;
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
			str += ">\n";
			if (!text.empty())
			{
				str += text + "\n";
			}
			for (auto& n : nodes)
			{
				str += n.to_string();
			}
			str += std::string(indent * 2, ' ') + "</" + name + ">";
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
	//TODO: cannot return nullptr
	//must change focus
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

	bool load(const std::string file_name_path, bool debug_info = false)
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
		std::ifstream filer(file_name_path, std::ios::ate);
		if (filer)
		{
			auto fileSize = filer.tellg();
			filer.seekg(std::ios::beg);
			std::string content(fileSize, 0);
			filer.read(&content[0], fileSize);

			std::unique_ptr<char[]> content2(new char[fileSize]);
			//make_unique
		}
		bool parsed = parse_string(str, debug_info);
		
		std::cout << "\n\033[4mload\033[0m " << file_name_path
			<< (parsed ? " \x1B[32mwell_formed\033[0m" : " \x1B[31mill_formed\033[0m") << " (" << (str.size()) << " bytes # "
			<< (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " seconds)" << std::endl;
		if (debug_info)
		{
			std::cout << str << *this;
		}

		return parsed;
	}

	bool save(const std::string file_name_path, bool debug_info = false)
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

		std::cout << "\033[4msave\033[0m " << file_name_path << " (" << (str.size()) << " bytes # "
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
			<< "\n\033[4mdocument\033[0m" << " ("
			<< document.size() << " bytes # "
			<< document.count_nodes() << " nodes # "
			<< document.count_attributes() << " attributes)\n"
			<< document.root << std::endl;
	}

	bool parse_string(const std::string& str, bool debug_info = false)
	{
		//TODOS:
		// throw ill_formed
		// unique attributes
		// single root
		// check find methods ""''[2]
		// find text

		//improve debug_info behaviour

		parsr_node node;
		parsr_node* node2append2 = &node;
		unsigned int indent = 0;
		
		bool well_formed = true;

		size_t cursor = 0;
		size_t a, b, c, d, e, f;

		while ((a = str.find_first_of("<", cursor)) != std::string::npos)
		{
			if ((b = str.find_first_of(">", a + 1)) != std::string::npos)
			{
				cursor = b + 1;
				{
					//TODO: change logic to push empty node first
					//then fill its name....
					if (str[a + 1] == '/'/*&& str[b - 1] != '/'*/)
					{
						if (node2append2->name == str.substr(a + 2, b - 1))
						{
							node2append2 = node2append2->parent;
							--indent;
						}
					}
					else
					{
						node2append2->nodes.push_back({ node2append2,indent++,str.substr(a + 1,b - 0 - (a + 1)) });
						node2append2 = &node2append2->nodes.back();
						if (debug_info) std::cout << "#" << node2append2->name << "#" << std::endl;
						for (c = str.find_first_of(" ", a + 1); c != std::string::npos && c < b; c = str.find_first_of(" ", c + 1))
						{
							if ((d = str.find_first_of("=", c + 1)) != std::string::npos)
							{
								if ((e = str.find_first_of("\"", d + 1)) != std::string::npos)
								{
									if ((f = str.find_first_of("\"", e + 1)) != std::string::npos)
									{
										node2append2->attributes.push_back({ str.substr(c + 1,d - 0 - (c + 1)),str.substr(e + 1,f - 0 - (e + 1)) });
										if (debug_info) std::cout << "#" << node2append2->attributes.back() << "#" << std::endl;
									}
								}
							}
						}
						if (!node2append2->attributes.empty())
						{
							node2append2->name = str.substr(a + 1, str.find_first_of(" ", a + 1) - 0 - (a + 1));
							if (debug_info) std::cout << "#" << node2append2->name << "#" << std::endl;
						}
						if (str[b - 1] == '/'/*&& str[a + 1] != '/'*/)
						{
							if (node2append2->attributes.empty())
							{
								if (debug_info) std::cout << "#" << node2append2->name << "#" << std::endl;
								node2append2->name.pop_back();
								if (debug_info) std::cout << "#" << node2append2->name << "#" << std::endl;
							}
							node2append2 = node2append2->parent;
							--indent;
						}
					}
				}
			}
		}

		if (well_formed)
		{
			clear();
			//learn if move-constructor is an option here
			root = node.nodes.front();
		}

		return well_formed;
	}

};

#endif

