
#include "Json.h"


Object::Object(){
  this->type = OBJECT;
}

Array::Array(){
  this->type = ARRAY;
}

Integer::Integer(){
  this->type = INTEGER;
}

Real::Real(){
  this->type = REAL;
}

String::String(){
  this->type = STRING;
}

Boolean::Boolean(){
  this->type = BOOLEAN;
}


Boolean::Boolean(bool b){
  this->type = BOOLEAN;
  val = b;
}

String::String(std::string s){
  this->type = STRING;
  val = s;
}

Real::Real(double d){
  this->type = REAL;
  val = d;
}

Integer::Integer(long long l){
  this->type = INTEGER;
  val = l;
}

std::ostream& operator<<(std::ostream& os, JsonObject* obj){
  Object* o;
  Array* a;
  Integer* i;
  Real* r;
  String* s;
  Boolean* b;

  switch(obj->type){
    case OBJECT:
      o = (Object*) obj;
      os << *o;
      break;
    case ARRAY:
      a = (Array*) obj;
      os << *a;
      break;
    case INTEGER:
      i = (Integer*) obj;
      os << *i;
      break;
    case REAL:
      r = (Real*) obj;
      os << *r;
      break;
    case STRING:
      s = (String*) obj;
      os << *s;
      break;
    case BOOLEAN:
      b = (Boolean*) obj;
      os << *b;
  }
  return os;
}


std::ostream& operator<<(std::ostream& os, Object& obj){
  unsigned int i;
  os << "{\n";

  for(i=0; i < obj.obj.size()-1 ; ++i){
    os  << "\"" << obj.order[i] << "\": " << obj.obj[obj.order[i]] << ",\n";
  }
  os << "\"" << obj.order[i] << "\": " << obj.obj[obj.order[i]];
  os << "\n}";
  return os;
}

std::ostream& operator<<(std::ostream& os, Array& obj){
  unsigned int i;
  os << "[ ";

  for(i=0; i < obj.arr.size()-1 ; ++i){
    os << obj.arr[i] << ", ";
  }
  os << obj.arr[i] << " ]";
  return os;
}

std::ostream& operator<<(std::ostream& os, Real& obj){
  os << obj.val;
  return os;
}

std::ostream& operator<<(std::ostream& os, Integer& obj){
  os << obj.val;
  return os;
}

std::ostream& operator<<(std::ostream& os, String& obj){
  os << "\"" << obj.val << "\"";
  return os;
}

std::ostream& operator<<(std::ostream& os, Boolean& obj){
  if(obj.val){
    os << "true";
  }else{
    os << "false";
  }
  return os;
}


void Object::add(std::string key, JsonObject* val){
  this->obj[key] = val;
  this->order.push_back(key);
}


std::string Object::toString(){
  unsigned int i;
  std::string os;
  os = os +  "{\n";

  for(i=0; i < obj.size()-1 ; ++i){
    os  = os + "\"" + order[i] + "\": " + obj[order[i]]->toString() + ",\n";
  }
  os = os + "\"" + order[i] + "\": " + obj[order[i]]->toString();
  os = os + "\n}";
  return os;
}

std::string Array::toString(){
  unsigned int i;
  std::string os;
  os = os + "[ ";

  for(i=0; i < arr.size()-1 ; ++i){
    os = os + arr[i]->toString() + ", ";
  }
  os = os + arr[i]->toString() + " ]";
  return os;
}

std::string Real::toString(){
  return std::to_string(val);
}

std::string Integer::toString(){
  return std::to_string(val);
}

std::string String::toString(){
  return "\"" + val + "\"";
}

std::string Boolean::toString(){
  if(val)
    return "true";
  else
    return "false";
}


bool issp(char c){
  return std::isspace(static_cast<unsigned char>(c));
}

bool isLimit(char c){
  return c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',';
}

std::string getStr(std::ifstream &stream){
  char c;
  std::string s;

  stream.get(c);
  while(c != '\"'){
    s = s + c;
    stream.get(c);
  }

  s = s + c;
  return s;
}

std::string getValue(std::ifstream& stream){
  char c;
  std::string s;

  stream.get(c);
  while(!isLimit(c) && ! issp(c)){
    s = s + c;
    stream.get(c);
  }

  stream.seekg(-1, stream.cur);

  return s;
}

std::string getNextWord(std::ifstream &stream){
  char c;
  std::string s;
  stream.get(c);
  while (issp(c))        // loop getting single characters
    stream.get(c);

  if(isLimit(c)){
    return std::string() + c;
  }

  if(c == '\"'){
    return "\"" + getStr(stream);
  }

  s = s + c;
  s = s + getValue(stream);

  return s;

}

JsonObject* jsonParse(std::ifstream &stream){
  std::string str;
  std::string str_tmp;
  JsonObject* obj_ptr;

  str = getNextWord(stream);
  if(str == "{"){

    obj_ptr = new Object();
    do{
      str_tmp = getNextWord(stream);

      if(str_tmp == "}")
        return obj_ptr;
      if(str_tmp == ",")
        str_tmp = getNextWord(stream);

      getNextWord(stream);    //:
      ( (Object*)obj_ptr)->add(str_tmp.substr(1,str_tmp.size()-2), jsonParse(stream));

    }while(str_tmp != "}");

  }

  else if( str[0] == '\"' ){
    obj_ptr = new String(str.substr(1,str.size()-2));
  }
  else if( str == "false")
    obj_ptr = new Boolean(false);
  else if( str == "true")
    obj_ptr = new Boolean(true);
  else{
    obj_ptr = new Real(std::stod(str));
  }

  return obj_ptr;
}


JsonObject* jsonParseFile(std::string file_path){

  std::ifstream input_file;
  input_file.open(file_path);

  JsonObject* o = jsonParse(input_file);
  input_file.close();
  return o;

}
