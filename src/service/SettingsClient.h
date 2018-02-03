#ifndef SERVICE_SETTINGSCLIENT_H_
#define SERVICE_SETTINGSCLIENT_H_

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "kv.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using namespace kv::proto;

class SettingsClient {
public:
	SettingsClient(std::shared_ptr<Channel> channel) : stub_(kv::proto::Settings::NewStub(channel)) {};
	std::unique_ptr<kv::proto::Settings::Stub> stub_;

	Status get(const get_request& request, get_response* response);
	Status set(const set_request& request, set_response* response);
};

#endif /* SERVICE_SETTINGSCLIENT_H_ */
