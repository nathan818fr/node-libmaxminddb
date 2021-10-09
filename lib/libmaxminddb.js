const {logInstallError} = require('../install/log');
const {modulePath} = require('../install/vars-runtime');
const errors = require('./errors');

function load() {
    try {
        return require(modulePath);
    } catch (err) {
        logInstallError(err);
        process.exit(1);
    }
}

function init(lib) {
    if (typeof lib._ImportFromNode === 'function') {
        lib._ImportFromNode(errors);
        delete lib._ImportFromNode;

        promisify(lib.MmdbReader, 'open');
        promisify(lib.MmdbReader.prototype, 'close');
        promisify(lib.MmdbReader.prototype, 'lookup');
    }
    return lib;
}

function promisify(obj, key) {
    const method = obj[key];
    obj[key] = function (...args) {
        return new Promise((resolve, reject) => {
            args.push((err, res) => (!err ? resolve(res) : reject(err)));
            method.apply(this, args);
        });
    };
}

module.exports = init(load());
