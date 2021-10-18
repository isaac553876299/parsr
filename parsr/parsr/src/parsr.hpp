

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
		//<< "(child of " << ((node.parent) ? (node.parent->name.empty() ? "root" : node.parent->name) : "none") << ")"
		stream << indent_n(node.indent) << "<" << node.name << node.attributes;
		if (node.text.empty() && node.nodes.empty())
		{
			stream << "/>" << std::endl; // perhaps
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
		unsigned int indent = 1;

		size_t alpha = 0;
		size_t omega = std::string::npos;

		size_t a, b, c, d, e, f, g, h, i;
		std::string x, y, z;

		// [?]: throw ill-formed cases
		// [?]: fill node with attributes if any
		// [X]: check attribute uniqueness
		// [X]: fill node text if any
		// [X]: check only one root node

		bool well_formed = true;
		while (well_formed)
		{
			a = str.find("<", alpha);
			if (well_formed = (a != std::string::npos))
			{
				alpha = a + 1;
				b = str.find(">", alpha);
				if (well_formed = (b != std::string::npos))
				{
					alpha = b + 1;
					if (str[a + 1] == '/')
					{
						if (well_formed = (node2append2->name == str.substr(a + 2, b - (a + 2))))
						{
							node2append2 = node2append2->parent;
							--indent;
						}
					}
					else
					{
						// ERROR = str.find(" ", x + 1, y - (x + 1)); any single character inside "" is [2]
						c = str.find(' ', a + 1);
						if (c != std::string::npos) if (c < b)
						{
							node2append2->nodes.push_back({ node2append2,indent++,str.substr(a + 1, c - (a + 1)) });
							node2append2 = &node2append2->nodes.back();

							d = str.find('=', c + 1);
							if (well_formed = (d != std::string::npos)) if (d < b)
							{
								e = str.find('\"', d + 1);
								if (well_formed = (e != std::string::npos)) if (e < b)
								{
									f = str.find('\"', e + 1);
									if (well_formed = (f != std::string::npos)) if (f < b)
									{
										node2append2->attributes.push_back({ str.substr(c + 1, d - (c + 1)), str.substr(e + 1, f - (e + 1)) });
									}
								}
							}

							if (str[b - 1] == '/')
							{
								node2append2 = node2append2->parent;
								--indent;
							}
						}
						else
						{
							if (str[b - 1] == '/')
							{
								node2append2->nodes.push_back({ node2append2,indent/*++*/,str.substr(a + 1, b - (a + 2)) });
							}
							else
							{
								node2append2->nodes.push_back({ node2append2,indent++,str.substr(a + 1, b - (a + 1)) });
								node2append2 = &node2append2->nodes.back();
							}
						}
					}
				}
			}
		}
		std::cout << std::endl << node << std::endl;
		// clear();
		// paste

		return true;
	}

};

#endif

