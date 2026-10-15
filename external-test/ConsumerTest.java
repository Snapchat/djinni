package consumer;

public final class ConsumerTest {
    public static void main(String[] args) {
        Message message = new Message(new Shared(Status.READY, 42));
        if (message.getPayload().getStatus() != Status.READY ||
                message.getPayload().getValue() != 42) {
            throw new AssertionError("Generated record did not preserve its fields");
        }
    }
}
