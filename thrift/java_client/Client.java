import java.util.List;
import org.apache.thrift.TException;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;


public class Client{
  public static void main(String[] args){
    try{
      TTransport transport;
      transport = new TSocket("localhost",9090);
      transport.open();
      TProtocol protocol = new TBinaryProtocol(transport);
      remote_infix_search.Client client = new remote_infix_search.Client(protocol);
      client.ping();
      List<String> res = client.find_matches("69PCBSR34");
      for(String r: res){
        System.out.println(r);
      }
      

      transport.close();
    }catch(TException x){
      x.printStackTrace();
    }
  }
}
