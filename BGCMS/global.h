#ifndef GLOBAL_H
#define GLOBAL_H

#include <bgmrpcclient.h>
#include <callgraph.h>

// static NS_BGMRPCClient::BGMRPCClient BGMRPCClient;

extern NS_BGMRPCClient::BGMRPCClient bgmrpcClient;

QIcon resIcon(const QString& type);

#endif  // GLOBAL_H
