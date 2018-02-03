#ifndef SERVICE_SETTINGSIMPL_H_
#define SERVICE_SETTINGSIMPL_H_

#include "kv.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace kv::proto;

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/kms/KMSClient.h>
#include <aws/kms/model/EncryptRequest.h>
#include <aws/kms/model/EncryptResult.h>
#include <aws/kms/model/DecryptRequest.h>
#include <aws/kms/model/DecryptResult.h>

class SettingsImpl final : public kv::proto::Settings::Service {
public:
	SettingsImpl();
	~SettingsImpl();

    Status set(ServerContext* context, const set_request* request, set_response* response) override;
    Status get(ServerContext* context, const get_request* request, get_response* response) override;

private:
	Aws::SDKOptions awsSdkOptions;
	std::shared_ptr<Aws::DynamoDB::DynamoDBClient> dynamoClient;
	std::shared_ptr<Aws::KMS::KMSClient> kmsClient;

	bool encrypt(const Aws::String& plainText, Aws::Utils::ByteBuffer& cipherText);
	bool decrypt(Aws::Utils::ByteBuffer& cipherText, Aws::Utils::ByteBuffer& plainText);
};

#endif /* SERVICE_SETTINGSIMPL_H_ */
