

#ifndef PARSR
#define PARSR

#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <sstream>

#

template<class T>
std::ostream& operator<< (std::ostream& stream, const std::list<T>& list)
{
	for (const auto& i : list) stream << i;
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

	std::string to_string() const
	{
		return (" " + name + "=\"" + value + "\"");
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_attribute& attribute)
	{
		return stream << attribute.to_string();
	}

};

struct parsr_node
{
	parsr_node* parent = nullptr;
	std::string name;
	std::list<parsr_attribute> attributes;
	std::string text;
	std::list<parsr_node> children;

	void clear()
	{
		// parent¿
		name.clear();
		attributes.clear();
		text.clear();
		children.clear();
	}

	std::string to_string(unsigned int indent) const
	{
		std::string str = std::string(indent * 2, ' ') + "<" + name;
		for (auto& a : attributes) str += a.to_string();
		if (text.empty() && children.empty()) str += "/>\n";// < /> perhaps < ></ >
		else
		{
			str += ">\n";
			if (!text.empty()) str += std::string((indent + 1) * 2, ' ') + text + "\n";
			for (auto& n : children) str += n.to_string(indent + 1);
			str += std::string(indent * 2, ' ') + "</" + name + ">\n";
		}
		return str;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_node& node)
	{
		return stream << node.to_string(0);
	}

	const parsr_node& child(const std::string& tag) const
	{
		for (const auto& i : children)
			if (i.name == tag)
				return i;
		return parsr_node{};// static¿
	}

	const parsr_attribute& attribute(const std::string& tag) const
	{
		for (const auto& i : attributes)
			if (i.name == tag)
				return i;
		return parsr_attribute{};// static¿
	}

};

struct parsr_document
{
	parsr_node root;

	parsr_document()
	{
		// clear¿
	}

	parsr_document(const std::string& file_name_path)
	{
		load(file_name_path);
	}

	~parsr_document()
	{
		clear();
	}

	void clear()
	{
		root.clear();
	}

	bool load(const std::string& file_name_path)
	{
		std::fstream file(file_name_path, std::ios::in);
		if (!file.is_open()) return false;
		std::stringstream sstream;
		sstream << file.rdbuf();
		file.close();// needn't¿
		std::string str = sstream.str();
		return parse_string(str);
	}

	bool save(const std::string& file_name_path)
	{
		std::string str = root.to_string(0);
		if (!str.empty())
		{
			std::fstream file(file_name_path, std::ios::out | std::ios::trunc);
			if (file.is_open())
			{
				file << str;
				file.close();// needn't¿
				return true;
			}
		}
		return false;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream << document.root << std::endl;
	}

	bool parse_string(const std::string& str)
	{
		//TODOS:
		// check find methods ""''[2]
		// check ill_formed

		parsr_node node;
		parsr_node* node2append2 = &node;

		bool well_formed = true;

		size_t cursor = 0;
		size_t a, b, c, d, e, f, g, h;

		while ((a = str.find_first_of("<", cursor)) != std::string::npos && node2append2 != nullptr)
		{
			if ((b = str.find_first_of(">", a + 1)) != std::string::npos)
			{
				cursor = b + 1;
				{
					if (str[a + 1] == '/'/*&& str[b - 1] != '/'*/)
					{
						std::string s = str.substr(a + 2, b - (a + 2));
						if (node2append2->name == str.substr(a + 2, b - (a + 2)))
						{
							node2append2 = node2append2->parent;// mind node2append2->children.pushback
							if (node2append2 == &node)
							{
								break;// single root
							}
						}
					}
					else
					{
						node2append2->children.push_back({ node2append2, });
						node2append2 = &node2append2->children.back();
						for (c = str.find_first_of(" ", a + 1); c != std::string::npos && c < b; c = str.find_first_of(" ", c + 1))
						{
							if ((d = str.find("=\"", c + 1)) != std::string::npos)
							{
								if ((e = str.find_first_of("\"", d + 2)) != std::string::npos)
								{
									if (node2append2->attribute(str.substr(c + 1, d - (c + 1))).name.empty())
									{
										node2append2->attributes.push_back({ str.substr(c + 1,d - (c + 1)),str.substr(d + 2,e - (d + 2)) });
									}
								}
							}
						}
						node2append2->name = str.substr(a + 1, (node2append2->attributes.empty() ? b : str.find_first_of(" ", a + 1)) - (a + 1));
						if (str[b - 1] == '/'/*&& str[a + 1] != '/'*/)
						{
							if (node2append2->attributes.empty())
							{
								node2append2->name.pop_back();
							}
							node2append2 = node2append2->parent;
						}
						else
						{
							if ((f = str.find("<", cursor)) != std::string::npos)
							{
								node2append2->text = str.substr(cursor, f - cursor);
								//node2append2->text.erase(remove(node2append2->text.begin(), node2append2->text.end(), '\n'), node2append2->text.end());
								if ((g = node2append2->text.find_first_not_of(" \n")) != std::string::npos)
								{
									if ((h = node2append2->text.find_last_not_of(" \n")) != std::string::npos)
									{
										node2append2->text = node2append2->text.substr(g, h - g + 1);
									}
								}
							}
						}
					}
				}
			}
		}
		well_formed = (node2append2 == &node);
		if (well_formed)
		{
			clear();
			//learn if move-constructor is an option here
			root = node.children.front();
		}

		return well_formed;
	}

};

#endif

