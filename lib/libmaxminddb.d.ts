/*
 * MmdbReader & related interfaces
 */

export class MmdbReader<E extends MmdbEntry = MmdbEntry> {
    static open<E extends MmdbEntry = MmdbEntry>(options: MmdbReaderOpenOptions): Promise<MmdbReader<E>>;

    private constructor();

    get fileName(): string;

    get fileSize(): number;

    get depth(): number;

    get metadata(): MmdbMetadata;

    close(): Promise<boolean>;

    lookup(ip: string): Promise<MmdbLookupResult<E>>;
}

export interface MmdbReaderOpenOptions {
    filename: string;
}

export interface MmdbMetadata {
    get nodeCount(): number;

    get recordSize(): number;

    get ipVersion(): number;

    get databaseType(): string;

    get languages(): ReadonlyArray<string>;

    get binaryFormatMajorVersion(): number;

    get binaryFormatMinorVersion(): number;

    get buildEpoch(): number;

    get description(): ReadonlyArray<MmdbDescription>;
}

export interface MmdbDescription {
    get language(): string;

    get description(): string;
}

export interface MmdbLookupResult<E extends MmdbEntry = MmdbEntry> {
    get netmask(): number;

    get entry(): undefined | E;
}

export type MmdbEntry = Record<string, any>;

/*
 * Constants
 */

export const MMDB_LIB_VERSION: string;

export const MMDB_FILE_OPEN_ERROR: number;
export const MMDB_CORRUPT_SEARCH_TREE_ERROR: number;
export const MMDB_INVALID_METADATA_ERROR: number;
export const MMDB_IO_ERROR: number;
export const MMDB_OUT_OF_MEMORY_ERROR: number;
export const MMDB_UNKNOWN_DATABASE_FORMAT_ERROR: number;
export const MMDB_INVALID_DATA_ERROR: number;
export const MMDB_INVALID_LOOKUP_PATH_ERROR: number;
export const MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR: number;
export const MMDB_INVALID_NODE_NUMBER_ERROR: number;
export const MMDB_IPV6_LOOKUP_IN_IPV4_DATABASE_ERROR: number;
