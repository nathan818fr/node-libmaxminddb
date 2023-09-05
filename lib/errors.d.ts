/** Represents an error that occurred while using the MaxMind DB library. */
export class MmdbError extends Error {
    /**
     * @param message A message describing the error.
     * @param code The error code (see MMDB_*_ERROR constants).
     */
    constructor(message: string, code: number);

    /** The error code (see MMDB_*_ERROR constants). */
    code: number;
}

/** Represents an error that occurred while trying to get the address of a hostname. */
export class GetAddrError extends Error {
    /**
     * @param message A message describing the error.
     * @param code The error code (see gai_error).
     */
    constructor(message: string, code: number);

    /** The error code (see gai_error). */
    code: number;
}
