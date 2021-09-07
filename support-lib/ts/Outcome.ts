export interface ResultOutcome<T> {
    result: T;
}

export interface ErrorOutcome<E> {
    error: E;
}

export type Outcome<T, E> = ResultOutcome<T> | ErrorOutcome<E>;
