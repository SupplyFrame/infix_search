import java.io.IOException;
import java.util.List;
import java.util.ArrayList;
import org.apache.thrift.TException;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TJSONProtocol;
import org.apache.thrift.protocol.TSimpleJSONProtocol;
import org.apache.thrift.transport.TSimpleFileTransport;
import org.apache.thrift.transport.TFileTransport;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TTransportException;
import infix.RemoteInfixSearch;

public class Client{

  private String masterHost;
  private int masterPort;
  private static final int COMMAND_REGISTER = 0;
  private static final int COMMAND_QUERY = 1;
  private static final int COMMAND_DEREGISTER = 2;

  public Client(String masterHost, int masterPort){
    this.masterHost = masterHost;
    this.masterPort = masterPort;
  }

  private TTransport transport = null;
  private TTransport fileTransport = null;

  private RemoteInfixSearch.Client initJSONClient()
  throws TTransportException,IOException{
    fileTransport = new TSimpleFileTransport("debug.txt",true,true);
    fileTransport.open();
    TProtocol protocol = new TJSONProtocol(fileTransport);
    RemoteInfixSearch.Client client = new RemoteInfixSearch.Client(protocol);
    return client;
  }

  private RemoteInfixSearch.Client initClient()
  throws TTransportException{
    transport = new TSocket(masterHost,masterPort);
    transport.open();
    TProtocol protocol = new TBinaryProtocol(transport);
    RemoteInfixSearch.Client client = new RemoteInfixSearch.Client(protocol);
    return client;
  }

  private void finalizeClient(){
    if(transport==null) transport.close();
    if(fileTransport==null) fileTransport.close();
  }

  public static void main(String[] args){
    try{
      if(args.length==0){
        System.err.println("Usage: --master-host <hostname> --master-port <port> [OPTIONS]");
        System.err.println("  OPTIONS:");
        System.err.println("  --register_worker	<worker_host> <worker_port>");
        System.err.println("  --send_query		<query>");
        System.exit(1);
      }
      int command = -1;
      int arg = 0;
      String masterHost = "localhost"; 
      int masterPort = 9090;
      String workerHost = "localhost"; 
      int workerPort = 9091;
      String query = "69PCBSR34";
      while(arg<args.length){
        if (args[arg].equals("--master-host")){
          masterHost = args[++arg];
        }else if (args[arg].equals("--master-port")){
          masterPort = Integer.parseInt(args[++arg]);
        }else if (args[arg].equals("--register-worker")){
          command = Client.COMMAND_REGISTER;
          workerHost = args[++arg];
          workerPort = Integer.parseInt(args[++arg]);
        }else if (args[arg].equals("--deregister-worker")){
          command = Client.COMMAND_DEREGISTER;
          workerHost = args[++arg];
          workerPort = Integer.parseInt(args[++arg]);
        }else if (args[arg].equals("--send-query")){
          command = Client.COMMAND_QUERY;
          query = args[++arg];
        }
        ++arg;
      }
      System.err.println("Master host and port "+masterHost+" "+masterPort+". Command is "+command);
      Client client = new Client(masterHost,masterPort);
      RemoteInfixSearch.Client remoteInfixSearchClient = client.initJSONClient();
      if(command == Client.COMMAND_REGISTER){

        System.err.println("Registering worker on host "+workerHost+" with port "+workerPort);
        remoteInfixSearchClient.registerService(workerHost,workerPort);
      }else if(command == Client.COMMAND_DEREGISTER){

        System.err.println("De-registering worker on host "+workerHost+" with port "+workerPort);
        remoteInfixSearchClient.deregisterService(workerHost,workerPort);
      }else if(command == Client.COMMAND_QUERY){
        System.err.println("Sending query "+query);
        List<String> res = remoteInfixSearchClient.findMatches(query);
        for(String r: res){
          System.out.println(r);
        }
      }
      client.finalizeClient();
    }catch(TException x){
      x.printStackTrace();
    }catch(IOException x){
      x.printStackTrace();
    }
  }
}
