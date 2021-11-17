

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
"  first\n"
"  <subnode one=\"1\" two=\"1.99\" one=\"repeat\">\n"
"    second\n"
"    <subsubnode/>\n"
"  </subnode>\n"
"</node>\n";

#define mydebug(m,x) {std::cout<<"#"<<m<<">>"<<x<<"#"<<std::endl;}

template<class T>
std::ostream& operator<< (std::ostream& stream, const std::list<T>& list)
{
	for (const auto& i : list)
		stream << i;
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

} static const null_parsr_attribute;

struct parsr_node
{
	parsr_node* parent = nullptr;
	std::string name;
	std::list<parsr_attribute> attributes;
	std::string text;
	std::list<parsr_node> children;

	void clear()
	{
		// parent?
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
			if (!text.empty()) str += text + "\n";
			for (auto& n : children) str += n.to_string(indent + 1);
			str += std::string(indent * 2, ' ') + "</" + name + ">\n";
		}

		return str;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_node& node)
	{
		return stream << node.to_string(0);
	}

	const parsr_node* child(const std::string& tag) const
	{
		for (const auto& i : children)
			if (i.name == tag)
				return &i;

		return &null_parsr_node;
	}
	//mayb bettr 2 scope nulls into node & attribute classes
	const parsr_attribute* attribute(const std::string& tag) const
	{
		for (const auto& i : attributes)
			if (i.name == tag)
				return &i;

		return &null_parsr_attribute;
	}

} static const null_parsr_node;

struct parsr_document
{
	parsr_node root;

	parsr_document()
	{
		//clear();
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
		auto start_clock = std::chrono::high_resolution_clock::now();

		std::fstream file(file_name_path, std::ios::in); // using ifstream needn't close()
		if (!file.is_open()) return false;

		//std::string str, str2;
		//while (std::getline(file, str2)) str += str2;//+ '\n';
		std::stringstream sstream;
		sstream << file.rdbuf();
		std::string str = sstream.str();

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
		bool loaded = parse_string(test);

		std::cout << "\n\033[4mload\033[0m " << file_name_path << (loaded ? " \x1B[32mwell_formed\033[0m" : " \x1B[31mill_formed\033[0m")
			<< " (" << (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " seconds)" << std::endl
			<< str << *this;

		return loaded;
	}

	bool save(const std::string& file_name_path)
	{
		auto start_clock = std::chrono::high_resolution_clock::now();

		std::string str = root.to_string(0);
		bool saved = !str.empty();

		if (saved)
		{
			std::fstream file(file_name_path, std::ios::out | std::ios::trunc);
			if (!file.is_open()) return false;
			file << str;
			file.close();//ofstream?
		}

		std::cout << "\033[4msave\033[0m " << file_name_path
			<< " (" << (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_clock)).count() << " seconds)" << std::endl
			<< str << std::endl;

		return saved;
	}

	friend std::ostream& operator<< (std::ostream& stream, const parsr_document& document)
	{
		return stream << "\n\033[4mdocument\033[0m\n" << document.root << std::endl;
	}

	bool parse_string(const std::string& str)
	{
		//TODOS:
		// check find methods ""''[2]
		// throw ill_formed
		// single root
		// check text

		parsr_node node;
		parsr_node* node2append2 = &node;

		bool well_formed = true;

		size_t cursor = 0;
		size_t a, b, c, d, e, f;

		while ((a = str.find_first_of("<", cursor)) != std::string::npos)
		{
			if ((b = str.find_first_of(">", a + 1)) != std::string::npos)
			{
				cursor = b + 1;
				{
					if (str[a + 1] == '/'/*&& str[b - 1] != '/'*/)
					{
						if (node2append2->name == str.substr(a + 2, b - 1))
						{
							node2append2 = node2append2->parent;
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
									if (node2append2->attribute(str.substr(c + 1, d - (c + 1)))->name.empty())
									{
										node2append2->attributes.push_back({ str.substr(c + 1,d - (c + 1)),str.substr(d + 2,e - (d + 2)) });
										mydebug("attribute", node2append2->attributes.back());
									}
								}
							}
						}
						node2append2->name = str.substr(a + 1, (node2append2->attributes.empty() ? b : str.find_first_of(" ", a + 1)) - (a + 1));
						mydebug("node", node2append2->name);
						if (str[b - 1] == '/'/*&& str[a + 1] != '/'*/)
						{
							if (node2append2->attributes.empty())
							{
								mydebug("node/", node2append2->name);
								node2append2->name.pop_back();
								mydebug("node-/", node2append2->name);
							}
							node2append2 = node2append2->parent;
						}
						else
						{
							if ((f = str.find("<", cursor)) != std::string::npos)
							{
								node2append2->text = str.substr(cursor, f - cursor);
								mydebug("text", node2append2->text);
								node2append2->text.erase(remove(node2append2->text.begin(), node2append2->text.end(), '\n'), node2append2->text.end());
								mydebug("text", node2append2->text);
								if (node2append2->text.find_first_not_of(" "/*\n"*/) == std::string::npos)
								{
									node2append2->text.clear();
								}
							}
						}
					}
				}
			}
		}
		//well_formed = (indent == 0);
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

