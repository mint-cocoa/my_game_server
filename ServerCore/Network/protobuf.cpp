#include <grpcpp/grpcpp.h>
#include "../../protobuf/example.grpc.pb.h" // example.proto에서 생성된 헤더 파일

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using example::ExampleService;
using example::MessageRequest;
using example::MessageResponse;

example::MessageRequest CreateMessageRequest(const std::string& message) {
    example::MessageRequest request;
    request.set_message(message);
    return request;
}