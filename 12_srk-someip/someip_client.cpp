#include <iostream>
#include <vsomeip/vsomeip.hpp>
#include <thread>
#include <chrono>

class SomeIPClient {
public:
    SomeIPClient() : app_(vsomeip::runtime::get()->create_application("Client")) {
        app_->init();
        app_->register_state_handler(
            std::bind(&SomeIPClient::on_state, this, std::placeholders::_1));
        app_->request_service(SERVICE_ID, INSTANCE_ID);
    }

    void start() {
        app_->start();
    }

private:
    void on_state(vsomeip::state_type_e state) {
        if (state == vsomeip::state_type_e::ST_REGISTERED) {
            send_message_periodically();
        }
    }

    void send_message_periodically() {
        while (true) {
            send_message();
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Send message every 2 seconds
        }
    }

    void send_message() {
        auto msg = vsomeip::runtime::get()->create_request();
        msg->set_service(SERVICE_ID);
        msg->set_instance(INSTANCE_ID);
        msg->set_method(METHOD_ID);

        std::string message = "Hello";
        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(std::vector<vsomeip::byte_t>(message.begin(), message.end()));
        msg->set_payload(payload);

        app_->send(msg);
    }

    std::shared_ptr<vsomeip::application> app_;
    static constexpr vsomeip::service_t SERVICE_ID = 0x1234;
    static constexpr vsomeip::instance_t INSTANCE_ID = 0x5678;
    static constexpr vsomeip::method_t METHOD_ID = 0x0421;
};

int main() {
    SomeIPClient client;
    client.start();
    return 0;
}
