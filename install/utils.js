const {URL, format: formatUrl} = require('url');
const {createHash} = require('crypto');
const path = require('path');
const simpleGet = require('simple-get');
const os = require('os');
const stream = require('stream');
const zlib = require('zlib');
const tar = require('tar-fs');
const {promisify} = require('util');
const cp = require('child_process');
const cbFs = require('fs');
const fs = {
    ...cbFs,
    copyFile: promisify(cbFs.copyFile),
    exists: promisify(cbFs.exists),
    mkdir: promisify(cbFs.mkdir),
    mkdtemp: promisify(cbFs.mkdtemp),
    rename: promisify(cbFs.rename),
    unlink: promisify(cbFs.unlink),
    writeFile: promisify(cbFs.writeFile),
};

function semverGte(left, right) {
    left = semverParse(left);
    right = semverParse(right);
    for (let i = 0; i < 3; ++i) {
        if (left[i] > right[i]) {
            break;
        }
        if (left[i] < right[i]) {
            return false;
        }
    }
    return true;
}

function semverParse(version) {
    version = version.split(/[^0-9.]/, 2)[0].split('.');
    return [parseInt(version[0]) || 0, parseInt(version[1]) || 0, parseInt(version[2]) || 0];
}

function urlToUniqueFilename(url) {
    url = new URL(url);
    const hash = createHash('sha1')
        .update(formatUrl(url, {auth: false, fragment: false}))
        .digest('hex');
    const safeFilename = url.pathname.substr(url.pathname.lastIndexOf('/') + 1).replace(/[^a-zA-Z0-9_.-]/g, '');
    return hash + '_' + safeFilename;
}

function urlWithoutSecrets(url) {
    url = new URL(url);
    delete url.search;
    delete url.searchParams;
    return formatUrl(url, {auth: false, fragment: false});
}

async function fsSilentUnlink(file) {
    try {
        await fs.unlink(file);
    } catch (ignored) {}
}

async function httpDownload(url, dstFile) {
    const res = await httpGet(url);
    if (res.statusCode !== 200) {
        throw new Error(`Unexpected HTTP status '${url}': ${res.statusCode} ${res.statusMessage}`);
    }

    const tmpDir = await fs.mkdtemp(path.join(os.tmpdir(), 'libmmdb-'));
    const tmpFile = path.join(tmpDir, path.basename(url));
    try {
        await httpPipe(res, fs.createWriteStream(tmpFile));
    } catch (err) {
        await fsSilentUnlink(tmpFile);
        throw err;
    }

    await fs.mkdir(path.dirname(dstFile), {recursive: true});
    try {
        await fs.rename(tmpFile, dstFile);
    } catch (err) {
        await fs.copyFile(tmpFile, dstFile);
        await fsSilentUnlink(tmpFile);
    }
}

function httpGet(opts) {
    return new Promise((resolve, reject) => {
        simpleGet(opts, (err, res) => (err ? reject(err) : resolve(res)));
    });
}

function httpPipe(httpRes, dstStream) {
    return new Promise((resolve, reject) => {
        httpRes
            .on('error', (err) => dstStream.destroy(err))
            .on('close', () => {
                if (!httpRes.complete) {
                    dstStream.destroy(new Error('Connection was terminated'));
                }
            })
            .pipe(dstStream);
        dstStream.on('error', reject).on('close', resolve);
    });
}

function extractTarGz(archiveFile, dstDir, opts) {
    return new Promise((resolve, reject) => {
        stream.pipeline(fs.createReadStream(archiveFile), new zlib.Gunzip(), tar.extract(dstDir, opts), (err) =>
            err ? reject(err) : resolve()
        );
    });
}

function spawnSync(command) {
    return cp.spawnSync(command, {encoding: 'utf8', shell: true, stdio: ['ignore', 'pipe', 'inherit']}).stdout || '';
}

function shellEsc(arg) {
    return "'" + arg.replace("'", "'\\''") + "'";
}

module.exports = {
    fs,
    semverGte,
    urlToUniqueFilename,
    urlWithoutSecrets,
    httpDownload,
    extractTarGz,
    spawnSync,
    shellEsc,
};
