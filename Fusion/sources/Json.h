#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>

class JsonObject;
class Object;
class Array;
class Real;
class Integer;
class String;
class Boolean;

std::ostream& operator<<(std::ostream& os, JsonObject* obj);
std::ostream& operator<<(std::ostream& os, Object& obj);
std::ostream& operator<<(std::ostream& os, Array& obj);
std::ostream& operator<<(std::ostream& os, Real& obj);
std::ostream& operator<<(std::ostream& os, Integer& obj);
std::ostream& operator<<(std::ostream& os, String& obj);
std::ostream& operator<<(std::ostream& os, Boolean& obj);

bool issp(char c);
std::string getNextWord(std::ifstream &stream);
JsonObject* jsonParse(std::ifstream &stream);
JsonObject* jsonParseFile(std::string file_path);

bool isLimit(char c);
std::string getStr(std::ifstream &stream);
std::string getValue(std::ifstream& stream);

enum JsonType {
  OBJECT,
  ARRAY,
  INTEGER,
  REAL,
  STRING,
  BOOLEAN
};

class JsonObject{

public:
  JsonType type;
    virtual std::string toString() =0;
};

class Object: public JsonObject{

public:
  std::map<std::string, JsonObject*> obj;
  std::vector<std::string> order;
  Object();
  void add(std::string, JsonObject*);
  friend std::ostream& operator<<(std::ostream& os, Object& obj);

    std::string toString();
};

class Array: public JsonObject{
public:
  std::vector<JsonObject*> arr;
  Array();
    friend std::ostream& operator<<(std::ostream& os, Array& obj);
    std::string toString();
};

class Integer: public JsonObject{
public:
  Integer();
  Integer(long long);
  long long val;
    friend std::ostream& operator<<(std::ostream& os, Integer& obj);
    std::string toString();
};

class Real: public JsonObject{
public:
  Real();
  Real(double);
  double val;
    friend std::ostream& operator<<(std::ostream& os, Real& obj);
    std::string toString();
};

class String: public JsonObject{

public:
  String();
  String(std::string);

  std::string val;
  friend std::ostream& operator<<(std::ostream& os, String& obj);
  std::string toString();
};

class Boolean: public JsonObject{

public:
  Boolean();
  Boolean(bool);

  bool val;
  friend std::ostream& operator<<(std::ostream& os, Boolean& obj);
  std::string toString();
};

#endif
