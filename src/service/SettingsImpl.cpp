#include <iostream>
#include "SettingsImpl.h"
#include <aws/core/Aws.h>

using namespace std;

SettingsImpl::SettingsImpl() {
	Aws::InitAPI(awsSdkOptions);
	cout << "AWS Init" << endl;

	Aws::Client::ClientConfiguration clientConfig;
	dynamoClient = std::make_shared<Aws::DynamoDB::DynamoDBClient>(clientConfig);
	kmsClient = std::make_shared<Aws::KMS::KMSClient>();
}

SettingsImpl::~SettingsImpl() {
	Aws::ShutdownAPI(awsSdkOptions);
}

Status SettingsImpl::get(ServerContext* context, const get_request* request, get_response* response) {
	cout << "get() from server" << endl;

	Aws::DynamoDB::Model::AttributeValue hashKey;
	hashKey.SetS(request->key().c_str());
	Aws::DynamoDB::Model::GetItemRequest req;
	req.AddKey("key", hashKey);
	req.SetTableName("kvTable");
	const Aws::DynamoDB::Model::GetItemOutcome& result = dynamoClient.get()->GetItem(req);
	if (!result.IsSuccess()) {
		std::cout << "Failed to get item: " << result.GetError().GetMessage() << endl;
		return Status::CANCELLED;
	}

	const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& item = result.GetResult().GetItem();
	if (!item.size()) {
		std::cout << "not found" << endl;
		return Status::CANCELLED;
	}

	Aws::Utils::ByteBuffer cipherText = item.find("value")->second.GetB();
	Aws::Utils::ByteBuffer plainText;
	if (!decrypt(cipherText, plainText)) {
		std::cout << "decrypt() error" << endl;
		return Status::CANCELLED;
	}

	std::string value = (char*) plainText.GetUnderlyingData();
	value.resize(plainText.GetLength());

	kv::proto::data* data = new kv::proto::data();
	data->set_key(item.find("key")->second.GetS().data());
	data->set_value(value);
	response->set_allocated_req(data);

	std::cout << "value was returned" << endl;
	return Status::OK;
}

bool SettingsImpl::decrypt(Aws::Utils::ByteBuffer& cipherText, Aws::Utils::ByteBuffer& plainText) {
	Aws::KMS::Model::DecryptRequest kmsRequest;
	//kmsRequest.setEncryptionContext(Collections.singletonMap("key", key));
	Aws::KMS::Model::DecryptOutcome outcome = kmsClient.get()->Decrypt(kmsRequest.WithCiphertextBlob(cipherText));
	plainText = outcome.GetResult().GetPlaintext();
	return outcome.IsSuccess();

	//    final Map<String, AttributeValue> encryptedItem = ddb.getItem(TABLE, Collections.singletonMap(EMAIL, new AttributeValue().withS(email))).getItem();
	//    final Map<String, AttributeValue> item = cryptor.decryptAllFieldsExcept(encryptedItem, ddbCtx, EMAIL);
}

Status SettingsImpl::set(ServerContext* context, const set_request* request, set_response* response) {
	cout << "set() from server" << endl;
	Aws::DynamoDB::Model::PutItemRequest pir;
	pir.SetTableName("kvTable");
	pir.AddItem("key", Aws::DynamoDB::Model::AttributeValue(request->req().key().c_str()));
	Aws::Utils::ByteBuffer cipherText;
	Aws::String plainText(request->req().value().c_str());
	if (!encrypt(plainText, cipherText)) {
		std::cout << "encrypt() error" << std::endl;
		response->set_error(set_response_error_t_internal);
		return Status::CANCELLED;
	}
	Aws::DynamoDB::Model::AttributeValue value;
	value.SetB(cipherText);
	pir.AddItem("value", value);

	const Aws::DynamoDB::Model::PutItemOutcome result = dynamoClient.get()->PutItem(pir);
	if (!result.IsSuccess()) {
		std::cout << "result error: " << result.GetError().GetMessage() << std::endl;
		response->set_error(set_response_error_t_internal);
		return Status::CANCELLED;
	}

	std::cout << "Item inserted!" << std::endl;
	response->set_error(set_response_error_t_ok);
	return Status::OK;
}

bool SettingsImpl::encrypt(const Aws::String& plainText, Aws::Utils::ByteBuffer& cipherText) {
	Aws::KMS::Model::EncryptRequest kmsRequest;
	Aws::Utils::ByteBuffer buffer((unsigned char*) plainText.data(), plainText.length());
	const Aws::String keyId{"7a1757f3-fb59-4867-a4d9-11ce281455da"};
	kmsRequest.SetKeyId(keyId);
	kmsRequest.SetPlaintext(buffer);
	//kmsRequest.setEncryptionContext(Collections.singletonMap("key", key));
	Aws::KMS::Model::EncryptOutcome outcome = kmsClient.get()->Encrypt(kmsRequest);
	cipherText = outcome.GetResult().GetCiphertextBlob();
	return outcome.IsSuccess();

	//    final Map<String, AttributeValue> item = new HashMap<>();
	//    item.put(EMAIL, new AttributeValue().withS(email));
	//    item.put(ADDRESS, new AttributeValue().withS(address));
	//    final Map<String, AttributeValue> encryptedItem = cryptor.encryptAllFieldsExcept(item, ddbCtx, EMAIL);
}

