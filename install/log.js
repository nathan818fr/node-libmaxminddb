const {installUrl} = require('./vars-runtime');

function logInstallError(err) {
    console.error(
        [
            '',
            'Something went wrong installing the "libmaxminddb" module',
            '',
            err.message,
            '',
            `Consult the installation documentation: ${installUrl}`,
            '',
        ].join('\n')
    );
}

module.exports = {logInstallError};
