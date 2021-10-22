

/*
// https://www.w3schools.com/xml
// https://www.tutorialspoint.com/xml/index.htm
<?xml version = "1.0" encoding = "UTF-8" standalone = "no" ?>
attributes either single or double quotes
character entity references: (&lt; <)(&gt; >)(&amp; &)(&apos; ')(&quot; ")(&#number;)
<!-- comment -->.
<![CDATA[ ignored ]]>
*/

#ifndef PARSR
#define PARSR

#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <chrono>

#include <sstream>

#

const std::string indent_n(unsigned int n)
{
	std::string r;
	for (unsigned int i = 0; i < n; ++i)
	{
		r += std::string(2, ' ');
	}
	return r;
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
	std::string text;
	std::list<parsr_attribute> attributes;
	std::list<parsr_node> nodes;

	void clear()
	{
		// parent?
		name.clear();
		text.clear();
		attributes.clear();
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
		
		std::cout << std::endl << "\x1B[34mfile \033[0m"<<file_name_path
			<< (parsed ? " \x1B[32mwell_formed\033[0m" : " \x1B[31mill_formed\033[0m")
			<< " (" << -1 << " \x1B[35mbytes\033[0m # "
			<< (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " \x1B[36mseconds\033[0m)" << std::endl
			<< str << *this << std::endl;

		return parsed;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream << std::endl << "\x1B[33mdocument \033[0m(" << -1 << " \x1B[35mbytes\033[0m # " << -1
			<< " \x1B[31mnodes\033[0m # " << -1 << " \x1B[31mattributes\033[0m)" << std::endl << document.root << std::endl;
	}

	bool parse_string(const std::string& str)
	{
		parsr_node node;
		parsr_node* node2append2 = &node;
		unsigned int indent = 1;

		size_t alpha = 0;
		size_t omega = std::string::npos;

		size_t a, b, c, d, e, f, g, h, i;
		std::string x, y, z;

		// ERROR = str.find(" ", x + 1, y - (x + 1)); any single character inside "" is [2]

		const size_t MAX_TOKENS = 100;
		size_t tokens[MAX_TOKENS];
		for (auto& i : tokens) i = std::string::npos;
		size_t ii = 0;
		for (size_t i = 0; i < std::string::npos && ii < MAX_TOKENS; i = str.find_first_of("</>", i + 1))
		{
			tokens[ii++] = (i);
		}
		bool well_formed = true;
		for (size_t i = 0; i < MAX_TOKENS && well_formed; ++i)
		{
			if (str[tokens[i]] == '<')
			{
				if (str[tokens[i + 1]] == '/')
				{
					if (str[tokens[i + 2]] == '>')
					{
						if (tokens[i + 1] == tokens[i] + 1)
						{
							if (node2append2->name == str.substr(tokens[i + 1], tokens[i + 2]))
							{
								std::cout << "</ >" << std::endl;
								node2append2 = node2append2->parent;
								--indent;
								++i;
							}
							else
							{
								well_formed = false;
							}
						}
						else if (tokens[i + 1] == tokens[i + 2] - 1)
						{
							std::cout << "< />" << std::endl;
							node2append2->nodes.push_back({ node2append2,indent/*++*/,str.substr(tokens[i],tokens[i + 2]) });
							++i;
						}
						else
						{
							well_formed = false;
						}
					}
					else
					{
						well_formed = false;
					}
				}
				if (str[tokens[i + 1]] == '>')
				{
					std::cout << "< >" << std::endl;
					node2append2->nodes.push_back({ node2append2,indent++,str.substr(tokens[i],tokens[i + 1]) });
					node2append2 = &node2append2->nodes.back();
					++i;
				}
				else
				{
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

