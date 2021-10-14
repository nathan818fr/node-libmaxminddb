const path = require('path');
const {fileURLToPath} = require('url');
const {inspect} = require('util');
const {logInstallError} = require('./log');
const {fs, semverGte, urlToUniqueFilename, httpDownload, rmrf, extractTarGz, urlWithoutSecrets} = require('./utils');
const varsBuild = require('./vars-build');

async function main() {
    const {
        platform,
        globalLibmaxminddbVersion,
        useGlobalLibmaxminddb,
        prebuildLibmaxminddbUrl,
        vendorPath,
        cachePath,
        libC,
    } = varsBuild;
    console.log('libmaxminddb environment:\n', inspect(varsBuild, false, null, true));

    if (useGlobalLibmaxminddb) {
        console.log(`Using globally-installed libmaxminddb v${globalLibmaxminddbVersion}`);
        return;
    }

    if (await fs.exists(vendorPath)) {
        console.log(`Using existing vendored libmaxminddb '${vendorPath}'`);
        if (checkVendor(prebuildLibmaxminddbUrl, libC, vendorPath)) {
            return;
        } else {
            console.log('The existing vendored libmaxminddb does not match the requested version!');
        }
    }

    if (prebuildLibmaxminddbUrl) {
        console.log(`Using prebuild libmaxminddb '${prebuildLibmaxminddbUrl}'`);
        const prebuildFile = await retrieveUrl(prebuildLibmaxminddbUrl, cachePath);
        await extractVendor(prebuildLibmaxminddbUrl, prebuildFile, vendorPath, {strip: 1});
        checkVendor(prebuildLibmaxminddbUrl, libC, vendorPath);
        return;
    }

    throw new Error(`Unable to compile libmaxminddb automatically: there is no prebuild binary for ${platform}.`);
}

async function retrieveUrl(url, cachePath) {
    if (url.startsWith('file://')) {
        return fileURLToPath(url);
    }

    if (url.startsWith('http://') || url.startsWith('https://')) {
        const file = path.join(cachePath, urlToUniqueFilename(url));
        if (await fs.exists(file)) {
            console.log(`Using cached file '${file}'`);
        } else {
            console.log(`Downloading ...`);
            await httpDownload(url, file);
            console.log(`Done! Downloaded to '${file}'`);
        }
        return file;
    }

    throw new Error(`Unsupported URL: '${url}'`);
}

async function extractVendor(vendorUrl, vendorFile, dstDir, opts) {
    console.log('Extracting ...');

    await rmrf(dstDir);
    await extractTarGz(vendorFile, dstDir, opts);

    const vendorContent = JSON.stringify(
        {
            publicUrl: urlWithoutSecrets(vendorUrl),
            localFile: vendorFile,
        },
        null,
        2
    );
    await fs.writeFile(path.join(dstDir, '_VENDOR.json'), vendorContent, 'utf8');

    console.log(`Done! Extracted to '${dstDir}'`);
}

function checkVendor(vendorUrl, libC, vendorPath) {
    if (libC.family) {
        let vendorLibcVersion;
        try {
            vendorLibcVersion = fs
                .readFileSync(path.join(vendorPath, libC.family.toUpperCase() + '_VERSION.txt'), 'utf8')
                .trim();
        } catch (err) {
            if (err.code !== 'ENOENT') {
                throw err;
            }
        }
        if (vendorLibcVersion && !semverGte(libC.version, vendorLibcVersion)) {
            throw new Error(`Incompatible ${libC.family} ${libC.version} (requires >= ${vendorLibcVersion})`);
        }
    }

    let varsVendor;
    try {
        varsVendor = JSON.parse(fs.readFileSync(path.join(vendorPath, '_VENDOR.json'), 'utf8'));
    } catch (err) {
        throw new Error(
            `Unable to read vendor variables: ${err.message}.\nTry to delete the vendor folder: '${vendorPath}'`
        );
    }
    console.log('libmaxminddb vendor variables:\n', inspect(varsVendor, false, null, true));

    return varsVendor.publicUrl === urlWithoutSecrets(vendorUrl);
}

main().catch((err) => {
    logInstallError(err);
    process.exit(1);
});
