class MmdbError extends Error {
    constructor(message, code) {
        super(message);
        this.code = code;
    }
}

class GetAddrError extends Error {
    constructor(message, code) {
        super(message);
        this.code = code;
    }
}

module.exports = {MmdbError, GetAddrError};
