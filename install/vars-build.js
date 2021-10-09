const os = require('os');
const path = require('path');
const detectLibc = require('detect-libc');
const {semverGte, spawnSync, shellEsc} = require('./utils');

const env = process.env;
const minLibmaxminddbVersion = '1.2.0'; // included
const prebuildPlatforms = [
    // see https://github.com/nathan818fr/libmaxminddb-build
    'darwin-arm64v8',
    'darwin-x64',
    'linux-arm64v8',
    'linux-armv6',
    'linux-armv7',
    'linux-x64',
    'linuxmusl-arm64v8',
    'linuxmusl-x64',
    'win32-arm64v8',
    'win32-ia32',
    'win32-x64',
];

function getPackageConfig(key) {
    return (
        env['NODE_' + key.toUpperCase()] ||
        env['npm_package_config_' + key] ||
        require('../package.json').config[key] ||
        ''
    ).toString();
}

function getPlatform() {
    const arch = env.npm_config_arch || process.arch;
    const platform = env.npm_config_platform || process.platform;
    const alternativeLibc = platform === 'linux' && detectLibc.isNonGlibcLinux ? detectLibc.family : '';

    let platformId = `${platform}${alternativeLibc}`;
    if (arch === 'arm') {
        platformId += `-armv${env.npm_config_arm_version || process.config.variables.arm_version || '7'}`;
    } else if (arch === 'arm64') {
        platformId += `-arm64v${env.npm_config_arm_version || '8'}`;
    } else {
        platformId += `-${arch}`;
    }

    return platformId;
}

function getPkgConfigPath() {
    if (process.platform === 'win32') {
        return '';
    }
    const brewPkgConfigPath = spawnSync('which brew >/dev/null 2>&1 && eval $(brew --env) && echo $PKG_CONFIG_LIBDIR');
    return [brewPkgConfigPath.trim(), env.PKG_CONFIG_PATH, '/usr/local/lib/pkgconfig', '/usr/lib/pkgconfig']
        .filter((p) => !!p)
        .join(':');
}

function getGlobalLibmaxminddbVersion(pkgConfigPath) {
    if (process.platform === 'win32') {
        return '';
    }
    return spawnSync(`PKG_CONFIG_PATH=${shellEsc(pkgConfigPath)} pkg-config --modversion libmaxminddb`).trim();
}

function shouldUseGlobalLibmaxminddb(globalLibmaxminddbVersion) {
    const libmaxminddbIgnoreGlobal = getPackageConfig('libmaxminddb_ignore_global').toLowerCase() === 'true';
    return (
        !libmaxminddbIgnoreGlobal &&
        !!globalLibmaxminddbVersion &&
        semverGte(globalLibmaxminddbVersion, minLibmaxminddbVersion)
    );
}

function getPrebuildLibmaxminddbUrl(platform) {
    const libmaxminddbVersion = getPackageConfig('libmaxminddb_version');

    if (libmaxminddbVersion.indexOf('://') !== -1) {
        return libmaxminddbVersion.replaceAll('{{platform}}', platform);
    }

    if (prebuildPlatforms.indexOf(platform) === -1) {
        return '';
    }
    return (
        `https://github.com/nathan818fr/libmaxminddb-build/releases/download/` +
        `v${libmaxminddbVersion}/libmaxminddb_${libmaxminddbVersion}_${platform}.tar.gz`
    );
}

function getVendorPath() {
    return path.join(__dirname, '..', 'vendor');
}

function getCachePath() {
    const npmCachePath =
        env.npm_config_cache || (env.APPDATA ? path.join(env.APPDATA, 'npm-cache') : path.join(os.homedir(), '.npm'));
    return path.join(npmCachePath, '_libmaxminddb');
}

const platform = getPlatform();
const pkgConfigPath = getPkgConfigPath();
const globalLibmaxminddbVersion = getGlobalLibmaxminddbVersion(pkgConfigPath);
module.exports = {
    pkgConfigPath,
    libC: {family: detectLibc.family, version: detectLibc.version},
    platform,
    globalLibmaxminddbVersion,
    useGlobalLibmaxminddb: shouldUseGlobalLibmaxminddb(globalLibmaxminddbVersion),
    prebuildLibmaxminddbUrl: getPrebuildLibmaxminddbUrl(platform),
    vendorPath: getVendorPath(),
    cachePath: getCachePath(),
};
