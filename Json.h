#pragma once
#include <map>
#include <string>
#include <String>
#include <Property>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <limits>
namespace LLD {
	namespace Json {

		class Json {
		protected:
			enum Type{
				None,
				Object,
				Array,
				Property,
				String,
				Integer,
				Number
			};
			
			PropertyReadWrite(Type, _Type, None)
		};

		class JsonString : public Json {
		public:
			JsonString() { Set_Type(String); }

			PropertyReadWrite(std::string, Value, "")
		};

		class JsonInteger : public Json {
		public:
			JsonInteger() { Set_Type(Integer); }

			PropertyReadWrite(int, Value, 0)
		};

		class JsonNumber :public Json {
		public:
			JsonNumber() { Set_Type(Number); }

			PropertyReadWrite(double, Value, 0.0f)
		};
		
		class JsonProperty : public Json {
		public:
			JsonProperty() { Set_Type(Property); }

			PropertyReadWriteNoInit(std::string, Key);
			PropertyReadWriteNoInit(std::shared_ptr<Json>, Value);
		};
		class JsonObject : public Json {
		public:
			JsonObject(){ Set_Type(Object); }

			PropertyObjectReadWriteNoInit(std::vector<std::shared_ptr<JsonProperty>>, Propertys);

		public:
			double GetNumber(const std::string &key) {
				for (auto property : Propertys) {
					if (property->GetKey() == key) {
						auto json = property->GetValue().get();
						auto number = static_cast<JsonNumber*>(json);
						assert(number);
						return number->GetValue();
					}
				}
				return std::numeric_limits<double>::min();
			}

			std::string GetString(const std::string &key) {
				for (auto property : Propertys) {
					if (property->GetKey() == key) {
						auto json = property->GetValue().get();
						auto str = static_cast<JsonString*>(json);
						assert(str);
						return str->GetValue();
					}
				}
			}
		};

		class JsonArray : public Json {
		public:
			JsonArray() { Set_Type(Array); }

			PropertyObjectReadWriteNoInit(std::vector<std::shared_ptr<JsonObject>>, Propertys);
		};

		std::shared_ptr<JsonNumber> ParseJsonNumber(const LLD::String & jsonStr, unsigned int& idx) {
			double d;
			sscanf_s(jsonStr.substr(idx).c_str(), "%lf", &d);
			std::stringstream ss;
			ss << d;
			idx += ss.str().length();

			idx = jsonStr.NextMeaningfulCharacter(idx);
			if (jsonStr.at(idx) == ',') {
				idx += 1;
			}
			assert(idx > 0);

			auto number = std::make_shared<JsonNumber>();
			number->SetValue(d);
			return number;
		}

		std::shared_ptr<JsonString> ParseJsonString(const LLD::String & jsonStr, unsigned int& idx) {
			auto end = jsonStr.find("\"", idx + 1);
			assert(end > idx);
			auto str = std::make_shared<JsonString>();
			str->SetValue(jsonStr.substr(idx + 1, end - idx - 1));

			idx = end + 2;
			return str;
		}
		std::shared_ptr<JsonObject> ParseJsonObj(const LLD::String & jsonStr, unsigned int& idx);
		std::shared_ptr<JsonArray> ParseJsonArr(const LLD::String & jsonStr, unsigned int& idx);

		std::shared_ptr<Json> ParseJsonValue(const LLD::String & jsonStr, unsigned int& idx) {
			if (jsonStr.IsNumber(idx)) {
				return ParseJsonNumber(jsonStr, idx);
			}

			idx = jsonStr.NextMeaningfulCharacter(idx);
			assert(idx >= 0);
			LLD::String::DataType t = jsonStr.at(idx);

			LLD::String::Super tmp = "\"";
			LLD::String::DataType tmp_t = tmp.at(0);

			if (t == tmp_t) {
				return ParseJsonString(jsonStr, idx);
			}
			tmp = "{";
			tmp_t = tmp.at(0);
			if (t == tmp_t) {
				return ParseJsonObj(jsonStr, idx);
			}
			tmp = "[";
			tmp_t = tmp.at(0);
			if (t == tmp_t) {
				return ParseJsonArr(jsonStr, idx);
			}
			assert(false);
		}

		std::shared_ptr<JsonProperty> ParseJsonProperty(const LLD::String & jsonStr, unsigned int& idx) {
			auto property = std::make_shared<JsonProperty>();

			int meaningful_idx = jsonStr.NextMeaningfulCharacter(idx);
			int next_idx = jsonStr.find("\"", meaningful_idx + 1);

			property->SetKey(jsonStr.substr(meaningful_idx + 1, next_idx - meaningful_idx - 1));

			next_idx = jsonStr.find(":", next_idx);

			idx = jsonStr.NextMeaningfulCharacter(next_idx + 1);
			
			auto value = ParseJsonValue(jsonStr, idx);
			property->SetValue(value);

			return property;
		}

		std::shared_ptr<JsonObject> ParseJsonObj(const LLD::String & jsonStr, unsigned int& idx) {
			if (jsonStr.at(idx) == '{') {
				++idx;
			}

			idx = jsonStr.NextMeaningfulCharacter(idx);
			assert(idx >= 0);

			auto object = std::make_shared<JsonObject>();
			while (jsonStr.at(idx) != '}') {
				LLD::String::Super tmp = "\"";
				LLD::String::DataType tmp_t = tmp.at(0);
				LLD::String::DataType t = jsonStr.at(idx);
				if (t == tmp_t) {
					auto property = ParseJsonProperty(jsonStr, idx);
					object->GetPropertysReference().push_back(property);
				}
				else {
					assert(false);
				}
				idx = jsonStr.NextMeaningfulCharacter(idx);
			}
			idx++;
			return object;
		}

		std::shared_ptr<JsonArray> ParseJsonArr(const LLD::String & jsonStr, unsigned int& idx) {
			if (jsonStr.at(idx) == '[') {
				++idx;
			}
			idx = jsonStr.NextMeaningfulCharacter(idx);
			assert(idx >= 0);

			auto arr = std::make_shared<JsonArray>();

			while (jsonStr.at(idx) != ']') {

				LLD::String::Super tmp = "{";
				LLD::String::DataType tmp_t = tmp.at(0);
				LLD::String::DataType t = jsonStr.at(idx);
				if (t == tmp_t) {
					auto obj = ParseJsonObj(jsonStr, idx);
					arr->GetPropertysReference().push_back(obj);
				}
				else if(jsonStr.at(idx) == ','){
					++idx;
				}
				else {
					assert(false);
				}

				idx = jsonStr.NextMeaningfulCharacter(idx);
			}
			++idx;
			return arr;
		}

		std::shared_ptr<JsonObject> Parse(const std::string & jsonStr) {
			unsigned int idx = 0;
			return ParseJsonObj(jsonStr, idx);
		}

	}
}