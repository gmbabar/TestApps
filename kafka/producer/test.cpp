
#include "kafka/KafkaProducer.h"

using namespace kafka::clients;

int main () {
    std::string brokers = "sv-awoh-kafka1.na.bluefirecap.net:9092,sv-awoh-kafka2.na.bluefirecap.net:9092";
    kafka::Topic topic  = "v1.app.test";
    kafka::Properties props ({{"bootstrap.servers",  brokers}, {"enable.idempotence", "true"}, });
    KafkaProducer producer(props);

    std::string data = “????”;
    // The ProducerRecord doesn't own `data`, it is just a thin wrapper
    auto record = producer::ProducerRecord(topic, kafka::NullKey, kafka::Value(data.c_str(), line.size()));
    producer.send(record,
                 [](const producer::RecordMetadata& metadata, const kafka::Error& error) {
                      if (!error) {
                          std::cout << "% Message delivered: " << metadata.toString() << std::endl;
                      } else {
                          std::cerr << "% Message delivery failed: " << error.message() << std::endl;
                      }
                 },
                 KafkaProducer::SendOption::ToCopyRecordValue);
}

