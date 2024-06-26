#include "kafka_consumer.h"
#include <iostream>

KafkaConsumer::KafkaConsumer(const std::string& brokers, const std::string& topic)
    : brokers_(brokers), topic_(topic), running_(false) {
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("metadata.broker.list", brokers_, errstr);

    consumer_.reset(RdKafka::KafkaConsumer::create(conf, errstr));
    if (!consumer_) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
    }

    delete conf;
}

KafkaConsumer::~KafkaConsumer() {
    stop();
}

void KafkaConsumer::start() {
    if (running_) return;

    running_ = true;
    consumeThread_ = std::thread(&KafkaConsumer::consume, this);
}

void KafkaConsumer::stop() {
    if (!running_) return;

    running_ = false;
    if (consumeThread_.joinable()) {
        consumeThread_.join();
    }
}

void KafkaConsumer::setMessageCallback(MessageCallback callback) {
    messageCallback_ = callback;
}

void KafkaConsumer::consume() {
    while (running_) {
        std::unique_ptr<RdKafka::Message> msg(consumer_->consume(1000));
        if (msg->err() == RdKafka::ERR_NO_ERROR) {
            if (messageCallback_) {
                messageCallback_(std::string(static_cast<const char*>(msg->payload()), msg->len()));
            }
        } else if (msg->err() != RdKafka::ERR__TIMED_OUT) {
            std::cerr << "Consume error: " << msg->errstr() << std::endl;
        }
    }
}
