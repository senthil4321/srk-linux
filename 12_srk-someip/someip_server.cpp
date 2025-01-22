#include <iostream>
#include <vsomeip/vsomeip.hpp>

class SomeIPServer {
public:
    SomeIPServer() : app_(vsomeip::runtime::get()->create_application("Service")) {
        app_->init();
        app_->register_message_handler(
            SERVICE_ID, INSTANCE_ID, METHOD_ID,
            std::bind(&SomeIPServer::on_message, this, std::placeholders::_1));
        app_->offer_service(SERVICE_ID, INSTANCE_ID);
    }

    void start() {
        std::cout << "Starting SomeIP server..." << std::endl;
        app_->start();
    }

private:
    void on_message(const std::shared_ptr<vsomeip::message> &msg) {
        std::cout << "Received message: ";
        auto payload = msg->get_payload();
        auto data = payload->get_data();
        std::cout.write(reinterpret_cast<const char*>(data.get()), payload->get_length());
        std::cout << std::endl;
        // Process the message
    }

    std::shared_ptr<vsomeip::application> app_;
    static constexpr vsomeip::service_t SERVICE_ID = 0x1234;
    static constexpr vsomeip::instance_t INSTANCE_ID = 0x5678;
    static constexpr vsomeip::method_t METHOD_ID = 0x0421;
};

int main() {
    std::cout << "Starting SomeIP server..." << std::endl;
    SomeIPServer server;
    server.start();
    return 0;
}
