#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/remote_infix_search.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;


int main(){
  boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  remote_infix_searchClient client(protocol); 
  try {
    transport->open();
    client.ping();
    cout << "ping()" << endl;
    vector<string> res;
    client.find_matches(res,"69PCBSR34");
    for(int i=0;i<res.size();++i){
      cout<<i<<":"<<res[i]<<endl;
    }
  }catch(TException & tx){
    cout<<"ERROR: "<<tx.what()<<endl;
  }

}
