

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
	std::string name;
	std::string text;
	std::list<parsr_attribute> attributes;
	std::list<parsr_node> nodes;

	void clear()
	{
		name.clear();
		text.clear();
		attributes.clear();
		nodes.clear();
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_node& node)
	{
		return stream << "<" << node.name << node.attributes << ">" << node.text << node.nodes;
	}
};

struct parsr_document
{
	std::string name;
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
		clear();
	}

	void clear()
	{
		name.clear();
		root.clear();
	}

	bool load(const std::string file_name_path)
	{
		auto start_clock = std::chrono::high_resolution_clock::now();

		std::fstream file(file_name_path, std::ios::in); // could use ifstream // don't have to close()
		if (!file.is_open())
		{
			return false;
		}
		std::string str, str2;
		while (std::getline(file, str2)) str += str2;
		/*std::stringstream sstream;
		sstream << file.rdbuf();
		std::string str = sstream.str();*/

		auto finish_clock = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = finish_clock - start_clock;

		std::cout << std::endl << file_name_path << " (" << -1 << " bytes # " << elapsed.count() << " seconds)" << std::endl << str << std::endl << std::endl;

		if (parse_string(str))
		{
			name = file_name_path;
			std::cout << *this << std::endl;
			return true;
		}

		return false;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream << std::endl << document.name << " (" << -1 << " bytes # " << -1 << " nodes # " << -1 << " attributes)" << std::endl << document.root << std::endl;
	}

	bool parse_string(const std::string& str)
	{
		parsr_node node;
		parsr_node* node2append2 = &node;
		parsr_node* tmp = node2append2;

		size_t alpha = 0;
		size_t omega = std::string::npos;

		size_t a, b, c, d;
		std::string x, y, z;

		// TODO:
		// TODO: achieve proper nesting
		// TODO: throw ill-formed cases
		// TODO: fill node with attributes if any
		// TODO: check attribute uniqueness
		// TODO: fill node text if any
		// TODO: check only one root node
		bool well_formed = true;
		while (well_formed)
		{
			a = str.find("<", alpha);
			if (a != std::string::npos)
			{
				alpha = a + 1;
				b = str.find(">", alpha);
				if (b != std::string::npos)
				{
					alpha = b + 1;
					if (str[a + 1] == '/')
					{
						y = str.substr(a + 2, b - (a + 2));
						std::cout << "found tail " << y << std::endl;
						if (node2append2->name == y)
						{
							node2append2 = node2append2->parent;
							std::cout << "--> pop " << node2append2->nodes.back().name << std::endl;
							node2append2->nodes.pop_back();
						}
						else
						{
							well_formed = false;
						}
					}
					else if (str[b - 1] == '/')
					{
						z = str.substr(a + 1, b - (a + 2));
						std::cout << "++> push " << z << std::endl;
						node2append2->nodes.push_back({ node2append2,z });
						node2append2 = &node2append2->nodes.back();
						std::cout << "--> pop " << node2append2->name << std::endl;
						node2append2 = node2append2->parent;
						node2append2->nodes.pop_back();
					}
					else
					{
						x = str.substr(a + 1, b - (a + 1));
						std::cout << "++> push " << x << std::endl;
						node2append2->nodes.push_back({ node2append2,x });
						node2append2 = &node2append2->nodes.back();
					}
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
		std::cout << std::endl << node << std::endl;
		// clear();
		// paste

		return true;
	}

};

#endif

