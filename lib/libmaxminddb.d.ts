import {MmdbError, GetAddrError} from './errors';

/**
 * Represents a reader for MaxMind DB databases.
 *
 * @template E The type of the entries in the database.
 */
export class MmdbReader<E extends MmdbEntry = MmdbEntry> {
    /**
     * Opens a MaxMind DB database and returns a reader for it.
     *
     * @param options The options for opening the database.
     * @returns A promise that resolves with a new reader on success; or a promise that rejects with an {@link MmdbError}
     *   on failure.
     */
    static open<E extends MmdbEntry = MmdbEntry>(options: MmdbReaderOpenOptions): Promise<MmdbReader<E>>;

    private constructor();

    /**
     * Gets the name of the database file which was opened, as passed to {@link MmdbReader.open}.
     *
     * Note: This property remains accessible after the reader has been closed.
     */
    get fileName(): string;

    /**
     * Gets the size of the database in bytes.
     *
     * Note: This property remains accessible after the reader has been closed.
     */
    get fileSize(): number;

    /**
     * Gets the depth of the database search tree.
     *
     * Note: This property remains accessible after the reader has been closed.
     */
    get depth(): number;

    /**
     * Gets the metadata for the database.
     *
     * Note: This property remains accessible after the reader has been closed.
     */
    get metadata(): MmdbMetadata;

    /**
     * Closes the reader and releases any associated resources.
     *
     * This method is called automatically when the reader is garbage collected, so it is not necessary to call it
     * explicitly. However it is recommended to call it explicitly when you are done with the reader, so that resources
     * are released promptly.
     *
     * It's safe to call this method multiple times, even if the reader has already been closed.
     *
     * @returns A promise that resolves with a boolean indicating whether the reader was closed (true) or was already
     *   closed (false). The promise will never reject.
     */
    close(): Promise<boolean>;

    /**
     * Looks up an IP address in the database.
     *
     * @param ip The IP address to look up.
     * @returns A promise that resolves to a {@link MmdbLookupResult} on success (whether or not the IP address was
     *   found); or a promise that rejects with {@link GetAddrError} if the IP address is invalid; or a promise that
     *   rejects with {@link MmdbError} if the lookup fails for any other reason.
     */
    lookup(ip: string): Promise<MmdbLookupResult<E>>;
}

/** Options for opening a MaxMind DB database with {@link MmdbReader.open}. */
export interface MmdbReaderOpenOptions {
    /** The path to the MaxMind DB file to open. */
    filename: string;
}

/** Interface representing metadata information about a MaxMind DB database. */
export interface MmdbMetadata {
    /** Gets the number of nodes in the search tree. */
    get nodeCount(): number;

    /** Gets the size of each record in bytes. */
    get recordSize(): number;

    /** Gets the IP version of the database (either 4 or 6). */
    get ipVersion(): number;

    /** Gets the type of the database (e.g. "GeoIP2-City"). */
    get databaseType(): string;

    /** Gets an array of language codes supported by the database. */
    get languages(): ReadonlyArray<string>;

    /** Gets the major version number of the binary format of the database. Should be 2 for all current databases. */
    get binaryFormatMajorVersion(): number;

    /** Gets the minor version number of the binary format of the database. */
    get binaryFormatMinorVersion(): number;

    /** Gets the unix epoch time when the database was built (in seconds). */
    get buildEpoch(): number;

    /** Gets an array of descriptions of the database. */
    get description(): ReadonlyArray<MmdbDescription>;
}

/** Interface representing a description of a MaxMind DB database. */
export interface MmdbDescription {
    /** The language of the description. */
    get language(): string;

    /** The description text. */
    get description(): string;
}

/**
 * Represents the result of a lookup in a MaxMind DB file.
 *
 * @template E The type of the entry returned by the lookup.
 */
export interface MmdbLookupResult<E extends MmdbEntry = MmdbEntry> {
    /**
     * Gets the netmask of the record found in the database.
     *
     * For example, if you look up the address `1.1.1.1` in an IPv4 database and the returned netmask is `16`, then the
     * address is part of the `1.1.0.0/16` subnet in the database.
     */
    get netmask(): number;

    /** Gets the entry found in the database; or `undefined` if no entry was found. */
    get entry(): undefined | E;
}

export type MmdbEntry = Record<string, any>;

/*
 * Constants
 */

/** The version of the MaxMind DB library used by this module (e.g. "1.7.1"). */
export const MMDB_LIB_VERSION: string;

/** Error code: there was an error trying to open the MaxMind DB file. */
export const MMDB_FILE_OPEN_ERROR: number;

/**
 * Error code: looking up an IP address in the search tree gave us an impossible result. The database is damaged or was
 * generated incorrectly or there is a bug in the libmaxminddb code.
 */
export const MMDB_CORRUPT_SEARCH_TREE_ERROR: number;

/**
 * Error code: something in the database is wrong. This includes missing metadata keys as well as impossible values
 * (like an ip_version of 7).
 */
export const MMDB_INVALID_METADATA_ERROR: number;

/** Error code: an IO operation failed. */
export const MMDB_IO_ERROR: number;

/** Error code: a memory allocation call (malloc, etc.) failed. */
export const MMDB_OUT_OF_MEMORY_ERROR: number;

/**
 * Error code: the database metadata indicates that it's major version is not 2. This library can only handle major
 * version 2.
 */
export const MMDB_UNKNOWN_DATABASE_FORMAT_ERROR: number;

/**
 * Error code: an entry in the data section contains invalid data. For example, a `uint16` field is claiming to be more
 * than 2 bytes long. The database is probably damaged or was generated incorrectly.
 */
export const MMDB_INVALID_DATA_ERROR: number;

export const MMDB_INVALID_LOOKUP_PATH_ERROR: number;

export const MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR: number;

export const MMDB_INVALID_NODE_NUMBER_ERROR: number;

/** Error code: an IPv6 address was looked up in an IPv4-only database. */
export const MMDB_IPV6_LOOKUP_IN_IPV4_DATABASE_ERROR: number;
