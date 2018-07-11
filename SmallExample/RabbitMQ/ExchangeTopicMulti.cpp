#include<SimpleAmqpClient/SimpleAmqpClient.h>
#include<SimpleAmqpClient/BasicMessage.h>

// 消息发送
bool PublishExchangeTopicMulti(const std::string &strUri)
{
    AmqpClient::Channel::ptr_t channel = 
        AmqpClient::Channel::CreateFromUri(strUri);

    if (channel == nullptr)
    {
        return false;
    }

    // 声明交换机，若不存在则创建 
    std::string strTopicExchange1 = "topic_exchange_1";
    std::string strTopicExchange2 = "topic_exchange_2";
    channel->DeclareExchange(strTopicExchange1, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC);
    channel->DeclareExchange(strTopicExchange2, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC);

    while (true)
    {
        // 可输入例如 "topic_exchange_1 disk.info 666"
        // "topic_exchange_2 any.warning 123"
        std::cout << "请输入[exchange] [routing_key1.routing_key2] [message]: " << std::endl;
    
        std::string strExchange;
        std::string severity;
        std::string message;
        std::cin >> strExchange;
        std::cin >> severity;
        std::cin >> message;

        channel->BasicPublish(strExchange, severity,
            AmqpClient::BasicMessage::Create(message));

        std::cout << "[X] to " << strExchange << ", send " 
            << severity << ": " << message << std::endl;
    }

}


void ReceiveTopicExchangeMulti(const std::string &strUri)
{
    AmqpClient::Channel::ptr_t channel = 
        AmqpClient::Channel::CreateFromUri(strUri);

    if (channel == nullptr)
    {
        return ;
    }

    // 这里我们声明两个交换机，类型均为topic , 我们将通过不同的队列从两个交换机中取消息。
    // 这里交换机的名称需要与发送端的保持一致。
    std::string strTopicExchange1 = "topic_exchange_1";
    std::string strTopicExchange2 = "topic_exchange_2";
    channel->DeclareExchange(strTopicExchange1, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC);
    channel->DeclareExchange(strTopicExchange2, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC);

    // 这里我们声明了三个队列，第一个队列从交换机1 取数据第二三个队列从交换机2 取数据；
    // 但是第二三个队列所绑定的routing_key 有所不同。
    // 当然了，routing_key 也可以相同，这样的话相同routing_key 的消息就会在两个队列中都出现。
    std::string strTopicQueue_1 = "topic_queue_1";
    std::string strTopicQueue_2 = "topic_queue_2";
    std::string strTopicQueue_3 = "topic_queue_3";
    // 第一个参数若为空，则系统默认生成随机队列名称
    // 第三个参数 表明队列是否持久的。true: 服务器重启将会保留该Exchange,
    // 警告，若只设置此项，不代表消息的持久化。即不保证重启后消息还在。
    channel->DeclareQueue(strTopicQueue_1, false, true, false, false);
    channel->DeclareQueue(strTopicQueue_2, false, true, false, false);
    channel->DeclareQueue(strTopicQueue_3, false, true, false, false);

    // 队列绑定我们感兴趣的routing_key, 表示我们只接收这些routing_key 相关的消息。
    channel->BindQueue(strTopicQueue_1, strTopicExchange1, "*.info");
    channel->BindQueue(strTopicQueue_1, strTopicExchange1, "disk.*");
    channel->BindQueue(strTopicQueue_1, strTopicExchange1, "info.error");

    // 在交换机2 上面我们绑定了队列2 和队列3 。但是它们所关心的routing_key 不同。
    channel->BindQueue(strTopicQueue_3, strTopicExchange2, "*.info");
    channel->BindQueue(strTopicQueue_3, strTopicExchange2, "disk.*");
    channel->BindQueue(strTopicQueue_2, strTopicExchange2, "info.error");

    // 创建消费者标志，这个在后面会告诉 channel 我们需要哪些队列中的相关routing_key 的消息。
    // BasicConsume() 第五个参数是指该消息是否以独占的方式处理，若是则不允许第二个消费者绑定到该队列 上，
    // 若否，则多个消费者同时绑定到该队列，那么 在该队列上的消息将随机分配到某一个消费者。
    // 即，同一个消息将不会同时出现 在两个消费者身上。
    std::string strFlagConsume_1 = "tab_consume_1";
    std::string strFlagConsume_2 = "tab_consume_2";
    std::string strFlagConsume_3 = "tab_consume_3";
    channel->BasicConsume(strTopicQueue_1, strFlagConsume_1, true, false, true);
    channel->BasicConsume(strTopicQueue_2, strFlagConsume_2, true, false, true);
    channel->BasicConsume(strTopicQueue_3, strFlagConsume_3, true, false, true);
    // BasicConsume() 的第4 个参数为false 表示，我们需要主动ack 服务器才将该消息清除。

    std::vector<std::string> vecFlagConsume;
    vecFlagConsume.push_back(strFlagConsume_1);
    vecFlagConsume.push_back(strFlagConsume_2);
    vecFlagConsume.push_back(strFlagConsume_3);

    while (true)
    {
        AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage(vecFlagConsume);

        std::string strExchange = envelope->Exchange();
        std::string strFlagConsume = envelope->ConsumerTag();
        std::string severity = envelope->RoutingKey();
        std::string buffer = envelope->Message()->body();

        std::cout << "[Y] exchange: " << strExchange << ", flagconsume: " << strflagConsume 
            << ", receive " << severity << ": " << buffer << std::endl;

        channel->BasicAck(envelope);
    }

    for (size_t i = 0; i < vecFlagConsume.size(); ++i)
        channel->BasicCancel(vecFlagConsume[i]);
}
