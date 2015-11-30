#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<vector>

#include <dawgdic/dawg.h>
#include <dawgdic/completer.h>
#include <dawgdic/dawg-builder.h>
#include <dawgdic/dictionary-builder.h>
#include <dawgdic/guide-builder.h>

using namespace std;
int main(int argc, char * argv[]) {
  //dawgdic::DictionaryBuilder::Build(dawg, &dawg_dic);

  cerr<<"Loading dictionary\n";
  dawgdic::Dictionary dawg_dic;
  ifstream dic_input("ihs.dic");
  dawg_dic.Read(&dic_input);
  dic_input.close();

  cerr<<"Loading Guide\n";
  dawgdic::Guide guide;
  ifstream guide_input("ihs.guide");
  guide.Read(&guide_input);
  guide_input.close();

  dawgdic::Completer completer(dawg_dic,guide);

  vector<string> prefixes;
  string line;
  while(getline(cin,line)){
    prefixes.push_back(line);
  }
  for(vector<string>::iterator it=prefixes.begin();it!=prefixes.end();++it){
    const char * prefix = it->data();
    cerr<<"Searching for prefix "<<prefix<<endl;
    dawgdic::BaseType index = dawg_dic.root();
    if (!dawg_dic.Follow(prefix,&index)){
      cerr<<"Could not be followed. Exiting.\n";
      return 1;
    }
    completer.Start(index,prefix);
    while(true){ 
    //for(int i = 0; i<10; ++i){
      //cerr<<"At "<<i<<endl;
      if(!completer.Next()) break;
      cout<<completer.key()<<": "<<completer.value()<<endl;
    }
  }
  //for(const char * p = text; *p!='\0';++p){
//    dawgdic::BaseType index = dawg_dic.root();
//    cerr<<"here\n";
//    for(const char * q = p; *q!='\0';++q){
//      cerr<<"here 2\n";
//      if(!dawg_dic.Follow(*q,&index)) {
//        cerr<<"Could not be followed for "<<*p<<" and "<<*q<<endl;
//        break;
//      }else{
//        cerr<<"Following for "<<*p<<" and "<<*q<<endl;
//      }

//      if(dawg_dic.has_value(index)){
//        cout<<'('<<(p-text)<<", "<<(q+1-p) << "," << dawg_dic.value(index)<<"): ";
//        cout.write(p,q+1-p)<<endl;
//      }
//    }
//  }
  //const char * exact = "apple";
  //if (dawg_dic.Contains(exact)){
    //cerr<<exact<<" was found.\n";
  //}else{
    //cerr<<exact<<" was not found.\n";
  //}


  return 0;
}
