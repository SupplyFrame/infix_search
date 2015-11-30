#include<fstream>
#include <dawgdic/dawg-builder.h>
#include <dawgdic/dictionary-builder.h>

//using namespace std;

// Inserts keys into a simple dawg.
dawgdic::DawgBuilder dawg_builder;
dawg_builder.Insert("apple");
dawg_builder.Insert("orange");

// Finishes building a simple dawg.
dawgdic::Dawg dawg;
dawg_builder.Finish(&dawg);

// Builds a dictionary from a simple dawg.
dawgdic::Dictionary dic;
dawgdic::DictionaryBuilder::Build(dawg, &dic);

// Writes a dictionary into a file "test.dic".
std::ofstream dic_file("test.dic", ios::binary);
dic.Write(&dic_file);
