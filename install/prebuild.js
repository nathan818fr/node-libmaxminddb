const path = require('path');
const {fileURLToPath} = require('url');
const {inspect} = require('util');
const {logInstallError} = require('./log');
const {fs, semverGte, urlToUniqueFilename, httpDownload, extractTarGz, urlWithoutSecrets} = require('./utils');
const varsBuild = require('./vars-build');
const {
    platform,
    globalLibmaxminddbVersion,
    useGlobalLibmaxminddb,
    prebuildLibmaxminddbUrl,
    vendorPath,
    cachePath,
    libC,
} = varsBuild;

async function main() {
    console.log('libmaxminddb environment:', inspect(varsBuild, false, null, true));

    if (useGlobalLibmaxminddb) {
        console.log(`Using globally-installed libmaxminddb v${globalLibmaxminddbVersion}`);
        return;
    }

    if (await fs.exists(vendorPath)) {
        console.log(`Using existing vendored libmaxminddb '${vendorPath}'`);
        checkVendor();
        return;
    }

    if (prebuildLibmaxminddbUrl) {
        console.log(`Using prebuild libmaxminddb '${prebuildLibmaxminddbUrl}'`);
        const prebuildFile = await retrieveUrl(prebuildLibmaxminddbUrl);
        await extractVendor(prebuildLibmaxminddbUrl, prebuildFile, vendorPath, {strip: 1});
        checkVendor();
        return;
    }

    throw new Error(`Unable to compile libmaxminddb automatically: there is no prebuild binary for ${platform}.`);
}

async function retrieveUrl(url) {
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

function checkVendor() {
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
}

main().catch((err) => {
    logInstallError(err);
    process.exit(1);
});
