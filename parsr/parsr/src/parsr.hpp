

/*
// https://www.w3schools.com/xml
pre-defined entity references: (&lt; <)(&gt; >)(&amp; &)(&apos; ')(&quot; ").
ignore <!-- comment -->.
ignore prolog <?xml version="1.0" encoding="UTF-8"?>
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
	for (auto& i : list)
	{
		std::cout << i;
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

		std::cout << std::endl << file_name_path << " (" << -1 << " bytes # " << elapsed.count() << " seconds)" << std::endl << str << std::endl;

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
		parsr_node* node_ptr = &node;

		size_t alpha = 0;
		size_t omega = std::string::npos;

		bool well_formed = true;
		while (well_formed)
		{
			size_t a = str.find_first_of("<", alpha);
			if (a != std::string::npos)
			{
				size_t b = str.find_first_of(">", a);
				if (b != std::string::npos)
				{
					node_ptr->nodes.push_back({ str.substr(a + 1, b - (a + 1)) });
					node_ptr = &node_ptr->nodes.back();

					alpha = b + 1;
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

		clear();
		// paste

		std::cout << node << std::endl;

		return true;
	}

};


int parsr_main()
{
	parsr_document doc("../kira_engine/src/example.xml");
	return 0;
}


#endif

