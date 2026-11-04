package example.shared;

public final class ExplicitTreeProbe {
    public static void main(String[] args) {
        if (Shared.class.getName().isEmpty() || Status.class.getName().isEmpty()) {
            throw new AssertionError("Explicit generated files were not available");
        }
    }
}
