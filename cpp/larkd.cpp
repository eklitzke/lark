#include "larkd.h"

int main(int argc, char **argv) {
  int port = 9090;
  string databaseName = "files.db";
  shared_ptr<SQLite3Store> store(new SQLite3Store());
  store->connect(databaseName);
  shared_ptr<LarkServiceHandler> handler(new LarkServiceHandler(store));
  shared_ptr<TProcessor> processor(new LarkServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  //TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

