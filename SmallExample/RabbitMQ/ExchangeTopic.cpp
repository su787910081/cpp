#include<SimpleAmqpClient/SimpleAmqpClient.h>
#include<SimpleAmqpClient/BasicMessage.h>

// strUri = "amqp://guest:guest@192.168.30.11:8820/test"
// strUri = "amqp://[帐户名]:[密码]@[服务主机以及端口]/[虚拟机目录]
bool PublishExchangeTopic(const std::string strUri, const std::string &strTopicExchange)
{
    // 连接到rabbitMQ 服务器
    AmqpClient::Channel::ptr_t channel = 
        AmqpClient::Channel::CreateFromUri(strUri);
    if (channel == nullptr)
    {
        std::cout << "channel is nullptr" << std::endl;
        return false;
    }

    // 创建指定类型的交换机，生产者将消息写往该交换机。
    channel->DeclareExchange(strDirectExchange, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC);

    while (true)
    {
        // 可输入例如 "disk.info 666" 或者 "any.warning 123"
        // 或者 "some.error 404" 表示信息严重程度和内容
        std::cout << "请输入[routing_key1.routing_key2] [message]" << std::endl;

        std::string severity;
        std::string message;
        std::cin >> severity;
        std::cin >> message;

        // 消息的severity 作为routing_key
        channel->BasicPublish(strTopicExchange, severity,
            AmqpClient::BasicMessage::Create(message));

        std::cout << "[X] send " << severity << ": " << message << std::endl;
    }
}

void ReceiveTopicExchange(const std::string &strUri, const std::string &strTopicExchange)
{
    AmqpClient::Channel::ptr_t channel = 
        AmqpClient::Channel::CreateFromUri(strUri);
    if (channel == nullptr)
    {
        return ;
    }

    // 创建指定类型的交换机，我们将从该交换机中读取消息，只有在不存在的时候才会创建。
    channel->DeclareExchange(strTopicExchange, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC);

    std::string strQName = "queue_topic");
    // 第一个参数为空，则系统默认生成随机名称
    // 第三个参数(durable)表明队列 是否持久化
    // durable：true、false。 true：服务器重启会保留下来Exchange。
    // 警告：仅设置此选项，不代表消息持久化。即不保证重启后消息还在。
    channel->DeclareQueue(strQName, false, true, false, false);

    // 队列绑定我们感 兴趣的routing_key, 表示 我们只接收这些routing_key 相关的消息。
    channel->BindQueue(strQName, strTopicExchange, "*.info");
    channel->BindQueue(strQName, strTopicExchange, "disk.*");

    // 得到消费者的相关标记，用于订阅rabbitMQ 上的指定消息。
    // 将第4个参数改为false,开启消息确认。  
    // 需要调用channel->BasicAck(xxx); 
    // 服务器上面的消息才会被清除，否则将一直保留在rabbitMQ 服务端 。
    // 将第5个参数改为false,取消独占队列。
    std::string strConsumer = channel->BasicConsume(strQName, "", true, false, false, 1);

    while (true)
    {
        AmqpClient::Envelope::ptr_t envelope = 
            channel->BasicConsumeMessage(strConsumer);

        std::string severity = envelope->RoutingKey();
        std::string buffer = envelope->Message()->Body();

        std::cout << "[Y] receive " << severity << ": " << buffer << std::endl;

        channel->BasicAck(envelope);
    }

    channel->BasicCancel(strConsumer);
}
