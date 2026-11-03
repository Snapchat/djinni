package example.shared;

public final class TreeProbe {
    public static void main(String[] args) {
        if (Shared.class.getName().isEmpty() || Status.class.getName().isEmpty()) {
            throw new AssertionError("Generated tree sources were not available");
        }
    }
}
