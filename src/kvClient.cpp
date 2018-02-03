#include <iostream>
using namespace std;

#include "service/SettingsClient.h"

int main() {
	SettingsClient settings(grpc::CreateChannel("34.197.180.158:50051", grpc::InsecureChannelCredentials()));

	std::cout << "calling set()" << std::endl;
	set_response s_response;
	set_request s_request;
	kv::proto::data* data = new kv::proto::data();
	data->set_key("999");
	data->set_value("yyy");
	s_request.set_allocated_req(data);
	settings.set(s_request, &s_response);
	std::cout << "set response: " << s_response.error() << std::endl;

	std::cout << "calling get()" << std::endl;
	get_response g_response;
	get_request g_request;
	g_request.set_key("999");
	settings.get(g_request, &g_response);
	std::cout << "get response: " << g_response.req().value().c_str() << std::endl;

	return 0;
}
