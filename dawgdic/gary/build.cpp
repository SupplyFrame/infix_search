#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <dawgdic/dawg-builder.h>
#include <dawgdic/guide-builder.h>
#include <dawgdic/dictionary-builder.h>

using namespace std;
int main() {
  dawgdic::DawgBuilder dawg_builder;
  string line;
  int i=0;
  while(getline(cin,line)){
    if(!dawg_builder.Insert(line.data())) return 1;
    if(i%100000==0) cerr<<i<<" lines read.\n";
    ++i;
  }
  //assert(dawg_builder.Insert("apple"));
  //assert(dawg_builder.Insert("banana"));
  //assert(dawg_builder.Insert("brain"));
  //assert(dawg_builder.Insert("cherry"));
  //assert(dawg_builder.Insert("durian"));
//  assert(!dawg_builder.Insert("green\0apple", 11, 0));
//  assert(dawg_builder.Insert("green\0apple"));
//  assert(dawg_builder.Insert("mandarin orange", 8, 0));
//  assert(dawg_builder.Insert("mandarin"));

  dawgdic::Dawg dawg;
  dawg_builder.Finish(&dawg);

  cerr<<"Building dictionary\n";
  dawgdic::Dictionary dawg_dic;
  dawgdic::DictionaryBuilder::Build(dawg, &dawg_dic);

  ofstream dic_output("ihs.dic");
  dawg_dic.Write(&dic_output);
  dic_output.close();

  cerr<<"Building Guide\n";
  dawgdic::Guide guide;
  if(!dawgdic::GuideBuilder::Build(dawg,dawg_dic, &guide)){
    cerr<<"Can't build the guide dictionary.\n";
    return 1;
  }
  ofstream guide_output("ihs.guide");
  guide.Write(&guide_output);
  guide_output.close();


//  assert(dawg_dic.Contains("apple"));
//  assert(dawg_dic.Contains("cherry"));
//  assert(dawg_dic.Contains("durian"));
//  assert(dawg_dic.Contains("green"));
//  assert(dawg_dic.Contains("mandarin"));

  return 0;
}
