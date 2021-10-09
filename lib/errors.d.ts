export class MmdbError extends Error {
    constructor(message: string, code: number);

    code: number;
}

export class GetAddrError extends Error {
    constructor(message: string, code: number);

    code: number;
}
