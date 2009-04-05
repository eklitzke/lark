#import "lark_service_handler.h"

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<LarkServiceHandler> handler(new LarkServiceHandler());
  shared_ptr<TProcessor> processor(new LarkServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

