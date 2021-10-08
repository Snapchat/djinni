package com.snapchat.djinni;

import java.util.Objects;

public abstract class Outcome<Result, Error> {

    public interface ResultHandler<R, Result> {
        public R apply(Result r);
    }
    public interface ErrorHandler<R, Error> {
        public R apply(Error e);
    }
    
    // No default construction. This object can only be created via the static
    // factory methods.
    private Outcome() {}

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null) return false;
        if (!(o instanceof Outcome)) return false;
        Outcome other = (Outcome) o;
        return match(x -> x.equals(other.resultOr(null)),
                     x -> x.equals(other.errorOrNull()));
    }

    @Override
    public int hashCode() {
        Class cls = getClass();
        return match(x -> Objects.hash(cls, 1, x),
                     x -> Objects.hash(cls, 0, x));
    }
    
    // Provide access to result or error through functions
    public abstract <R> R match(ResultHandler<R, Result> handleResult,
                                ErrorHandler<R, Error> handleError);

    // Returns either result or default value
    public Result resultOr(Result defaultResult) {
        return this.match(x -> x, x -> defaultResult);
    }

    public Error errorOrNull() {
        return this.match(x -> null, x -> x);
    }
    
    // Construct Outcome from result
    public static <Result, Error> Outcome<Result, Error> fromResult(Result value) {
        return new Outcome<Result, Error>() {
            @Override
            public <R> R match(ResultHandler<R, Result> handleResult,
                               ErrorHandler<R, Error> handleError) {
                return handleResult.apply(value);
            }
        };
    }
    // Construct Outcome from error
    public static <Result, Error> Outcome<Result, Error> fromError(Error error) {
        return new Outcome<Result, Error>() {
            @Override
            public <R> R match(ResultHandler<R, Result> handleResult,
                               ErrorHandler<R, Error> handleError) {
                return handleError.apply(error);
            }
        };
    }
}
