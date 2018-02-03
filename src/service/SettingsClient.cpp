#include "SettingsClient.h"

Status SettingsClient::get(const get_request& request, get_response* response) {
	ClientContext context;
	Status status = stub_->get(&context, request, response);
	return status;
}

Status SettingsClient::set(const set_request& request, set_response* response) {
	ClientContext context;
	Status status = stub_->set(&context, request, response);
	return status;
}
