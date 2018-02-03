Requirements:

- Packer [https://www.packer.io/downloads.html]
- AWS CLI [https://docs.aws.amazon.com/en_us/cli/latest/userguide/installing.html]
- Access Key ID/Secret with AdministratorAccess permission

Setup:

- aws configure --profile kv-user
- aws cloudformation create-stack \
	--profile kv-user \
	--stack-name kv-stack \
	--region us-east-1 \
	--template-body file://CloudFormationTemplate.yaml \
	--parameters file://CloudFormationTemplateParameters.json \
	--capabilities CAPABILITY_IAM

Todo:

- From template-body to template-url 