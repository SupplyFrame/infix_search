// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "RemoteInfixSearch.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::infix;
using namespace std;
#include "../../cpp/node.hpp"
#include "../../cpp/search.hpp"


struct thrift_container_t{
  string host;
  int port;
  boost::shared_ptr<TTransport> socket;
  boost::shared_ptr<TTransport> transport;
  boost::shared_ptr<TProtocol> protocol;
  boost::shared_ptr<RemoteInfixSearchClient> client;
};

typedef list<thrift_container_t > host_list_t;

class RemoteInfixSearchHandler : virtual public RemoteInfixSearchIf {
 private:
  search_t search;
  int port;
  int service_type;
  string dataset;
  host_list_t host_list;  

 public:

  static const int TYPE_MASTER = 0; 
  static const int TYPE_SLAVE = 1;

  RemoteInfixSearchHandler(int service_type,int port, string dataset) {
    // Your initialization goes here
    this->service_type = service_type;
    this->port = port;
    this->dataset = dataset;
  }


  void start_server(){
    if(service_type ==  TYPE_SLAVE){
      search.read_input(dataset.data());
    }
    shared_ptr<RemoteInfixSearchHandler> handler(this);
    shared_ptr<TProcessor> processor(new RemoteInfixSearchProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
  }

  int32_t ping() {
    // Your implementation goes here
    printf("ping\n");
    return 0;
  }

  void findMatches(std::vector<std::string> & _return, const std::string& key) {
    // Your implementation goes here
    printf("findMatches\n");
    if(service_type ==  TYPE_SLAVE){
      search.find_path(key,_return);
      for(int i=0;i<_return.size();++i){
        _return[i] = search.extract_row(_return[i]);
      }
    }else{
      if(host_list.size()>0){
        _return.clear();
        for(host_list_t::iterator it = host_list.begin();it!=host_list.end();++it){
          thrift_container_t container = *it;
          try {
            printf("Trying host %s port %d\n",container.host.data(),container.port);
            container.transport->open();
            vector<string> matches;
            container.client->findMatches(matches,key);
            container.transport->close();
            for(vector<string>::iterator it = matches.begin();it!=matches.end();++it){
              _return.push_back(*it);
            }
          }catch(TException & tx){
            cout<<"ERROR: "<<tx.what()<<endl;
          }
        }
      }
    }
  }

  int32_t registerService(const std::string& host, const int32_t port) {
    // Your implementation goes here
    printf("registerService\n");
    pair<string,int> host_tuple(host,port);
    boost::shared_ptr<TTransport> socket(new TSocket(host,port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    boost::shared_ptr<RemoteInfixSearchClient> client(new RemoteInfixSearchClient(protocol));
    thrift_container_t container;
    container.host = host;
    container.port = port;
    container.socket = socket;
    container.transport = transport;
    container.protocol = protocol;
    container.client = client;
    bool found = false;
    for(host_list_t::iterator it = host_list.begin();it!=host_list.end();++it){
      thrift_container_t container = *it;
      if(container.host.compare(host)==0 && container.port==port){
        printf("Found a matching entry\n");
        found = true;
        break;
      }
    }
    if(!found) host_list.push_back(container);
    printf("There are now %u workers\n",(uint)host_list.size());
  }

  int32_t deregisterService(const std::string& host, const int32_t port) {
    // Your implementation goes here
    printf("deregisterService\n");
    pair<string,int> host_tuple(host,port);
    host_list_t::iterator it = host_list.begin();
    while(it!=host_list.end()){
      thrift_container_t container = *it;
      if(container.host.compare(host)==0 && container.port==port){
        printf("Found a matching entry\n");
        it = host_list.erase(it);
      }else{
        ++it;
      }
    }
    printf("There are now %u workers\n",(uint)host_list.size());
  }

};

int main(int argc, char **argv) {
  int port = -1;
  int service_type = -1;
  if (argc<2){
    cerr<<"Usage: service_cpp [OPTION]\n";
    cerr<<"Starts the part number search service as a client or server\n\n";
    cerr<<"  --type	type of service: '0' for master or '1' for slave\n";
    cerr<<"  --port	port to listen on\n";
    cerr<<"  --dataset	path of dataset\n";
    return 1;
  }
  int arg=1;
  string dataset="";
  while(arg<argc){
    printf("At str %s\n",argv[arg]);
    if(strcmp(argv[arg],"--port")==0){
      port = atoi(argv[++arg]);
    }else if(strcmp(argv[arg],"--type")==0){
      service_type = atoi(argv[++arg]);
    }else if(strcmp(argv[arg],"--dataset")==0){
      dataset = argv[++arg];
    }
    ++arg;
  }
  printf("Configuration is service %d port %d dataset %s\n",service_type,port,dataset.data());
  RemoteInfixSearchHandler handler(service_type,port,dataset);
  handler.start_server();
  return 0;
}
