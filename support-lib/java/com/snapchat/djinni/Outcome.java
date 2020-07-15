package com.snapchat.djinni;

import java.util.function.Function;
import java.util.Objects;
import java.util.Optional;

public abstract class Outcome<Result, Error> {
    // No default construction. This object can only be created via the static
    // factory methods.
    private Outcome() {}

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null) return false;
        if (!(o instanceof Outcome)) return false;
        Outcome other = (Outcome) o;
        return match(x -> x.equals(other.result().orElse(null)),
                     x -> x.equals(other.error().orElse(null)));
    }

    @Override
    public int hashCode() {
        Class cls = getClass();
        return match(x -> Objects.hash(cls, 1, x),
                     x -> Objects.hash(cls, 0, x));
    }
    
    // Provide access to result or error through functions
    public abstract <R> R match(Function<? super Result, ? extends R> handleResult,
                                Function<? super Error, ? extends R> handleError);

    // Provide access to result as optional
    public Optional<Result> result() {
        return this.match(Optional::of, x -> Optional.empty());
    }
    // Returns either result or default value
    public Result resultOr(Result defaultResult) {
        return this.match(x -> x, x -> defaultResult);
    }
    // Provide access to error as optional
    public Optional<Error> error() {
        return this.match(x -> Optional.empty(), Optional::of);
    }
    
    // Construct Outcome from result
    public static <Result, Error> Outcome<Result, Error> fromResult(Result value) {
        return new Outcome<Result, Error>() {
            @Override
            public <R> R match(Function<? super Result, ? extends R> handleResult,
                               Function<? super Error, ? extends R> handleError) {
                return handleResult.apply(value);
            }
        };
    }
    // Construct Outcome from error
    public static <Result, Error> Outcome<Result, Error> fromError(Error error) {
        return new Outcome<Result, Error>() {
            @Override
            public <R> R match(Function<? super Result, ? extends R> handleResult,
                               Function<? super Error, ? extends R> handleError) {
                return handleError.apply(error);
            }
        };
    }
}
