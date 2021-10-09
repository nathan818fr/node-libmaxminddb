const {MmdbReader, MMDB_LIB_VERSION} = require('./lib');
const {inspect} = require('util');

async function main() {
    let mmdb = await MmdbReader.open({filename: __dirname + '/test/maxmind-db/test-data/GeoIP2-City-Test.mmdb'});
    console.log('MmdbReader opened:', inspect(mmdb, false, null, true));

    for (const ip of ['1.2.3.4', '81.2.69.160']) {
        const res = await mmdb.lookup(ip);
        console.log('MmdbReader lookup (' + ip + '):', inspect(res, false, null, true));
    }

    console.log(await mmdb.close());
    console.log(await mmdb.close());

    console.log({MMDB_LIB_VERSION});

    if (global.gc) {
        // Test Garbage Collection
        mmdb = null;
        await new Promise((resolve) => setTimeout(resolve, 1000));
        global.gc();
        await new Promise((resolve) => setTimeout(resolve, 1000));
    }
}

main().catch((err) => {
    console.error(err);
    process.exit(1);
});
