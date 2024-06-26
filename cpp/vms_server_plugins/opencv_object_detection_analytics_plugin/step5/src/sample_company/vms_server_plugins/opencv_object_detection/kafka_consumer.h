#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <librdkafka/rdkafkacpp.h>

class KafkaConsumer {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    KafkaConsumer(const std::string& brokers, const std::string& topic);
    ~KafkaConsumer();

    void start();
    void stop();
    void setMessageCallback(MessageCallback callback);

private:
    void consume();

    std::string brokers_;
    std::string topic_;
    std::unique_ptr<RdKafka::KafkaConsumer> consumer_;
    std::thread consumeThread_;
    std::atomic<bool> running_;
    MessageCallback messageCallback_;
};

#endif // KAFKA_CONSUMER_H
