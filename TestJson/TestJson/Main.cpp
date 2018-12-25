#include <string>
#include "../../Json.h"
#include <iostream>

std::string ReadAllData(const std::string& url) {
	std::ifstream ifile(url);
	std::stringstream buf;
	buf << ifile.rdbuf();
	return buf.str();
}

int main() {

	{
		auto str = ReadAllData("../json/testJsonObj.json");
		auto obj = static_cast<std::shared_ptr<LLD::Json::JsonObject>>(LLD::Json::Parse(str));

		std::string str2 = obj->GetString("name");
		double age = obj->GetNumber("age");
	}
	{
		auto str = ReadAllData("../json/testJsonArray.json");
		auto obj = static_cast<std::shared_ptr<LLD::Json::JsonObject>>(LLD::Json::Parse(str));

		std::string str2 = obj->GetString("name");
		double age = obj->GetNumber("age");
	}
	//std::cout << "name:" << str2;
	std::cin.get();
	return 0;
}