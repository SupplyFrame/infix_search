// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "RemoteInfixSearch.h"
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TFileTransport.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TFileTransport.h>
#include <thrift/transport/TSimpleFileTransport.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/threadpool.hpp>
#include <boost/thread/thread.hpp>

using namespace ::apache::thrift;
using namespace ::apache::thrift::concurrency;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace boost::threadpool;

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

struct delegator_t{

  string key;
  thrift_container_t container;
  vector<string> matches;

  delegator_t(thrift_container_t container,string key):container(container),key(key){} 

  void operator()(){
    try {
      printf("Trying host %s port %d\n",container.host.data(),container.port);
      container.transport->open();
      container.client->findMatches(matches,key);
      container.transport->close();
    }catch(TException & tx){
      cout<<"ERROR: "<<tx.what()<<endl;
    }
  }
};

class RemoteInfixSearchHandler : virtual public RemoteInfixSearchIf {
 private:
  search_t search;
  int port;
  int service_type;
  string dataset;
  host_list_t host_list;  

  void findMatchesThreaded(std::vector<std::string> & _return, const std::string& key,list<delegator_t *> & delegator_list) {
    pool threadpool(delegator_list.size());
    for(list<delegator_t *>::iterator it = delegator_list.begin();it!=delegator_list.end();++it){
      delegator_t * delegator = *it;
      threadpool.schedule(boost::ref(*delegator));
    }
  }

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

  void start_http_server(){
    shared_ptr<RemoteInfixSearchHandler> handler(this);
    shared_ptr<TProcessor> processor(new RemoteInfixSearchProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(8080));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TJSONProtocolFactory());
    shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(10,20);
    shared_ptr<ThreadFactory> threadFactory(new PosixThreadFactory());
    threadManager->threadFactory(threadFactory);
    threadManager->start();
    TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory,threadManager);
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
        list<delegator_t *> delegator_list;
        for(host_list_t::iterator it = host_list.begin();it!=host_list.end();++it){
          thrift_container_t container = *it;
          delegator_t * delegator = new delegator_t(container,key);
          delegator_list.push_back(delegator);
        }
        findMatchesThreaded(_return,key,delegator_list);
        for(list<delegator_t *>::iterator it = delegator_list.begin();
        it!=delegator_list.end();++it){
          delegator_t * delegator = *it;
          for(vector<string>::iterator it = delegator->matches.begin();
          it!=delegator->matches.end();++it){
             _return.push_back(*it);
          }
          delete(delegator);
//          try {
//            printf("Trying host %s port %d\n",container.host.data(),container.port);
//            container.transport->open();
//            vector<string> matches;
//            container.client->findMatches(matches,key);
//            container.transport->close();
//            for(vector<string>::iterator it = matches.begin();it!=matches.end();++it){
//              _return.push_back(*it);
//            }
//          }catch(TException & tx){
//            cout<<"ERROR: "<<tx.what()<<endl;
//          }
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
  boost::shared_ptr<TTransport> socket(new TSimpleFileTransport("debug.txt",true,false));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TJSONProtocol(transport));
  boost::shared_ptr<RemoteInfixSearchClient> client(new RemoteInfixSearchClient(protocol));
  transport->open();
  client->registerService("localhost",8080);
  transport->close();
  
}
