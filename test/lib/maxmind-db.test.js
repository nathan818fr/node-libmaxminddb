const {MmdbReader} = require('../../lib');

const testDbDir = __dirname + '/../maxmind-db/test-data';

function testDb(name, ips) {
    test('Decode ' + name, async () => {
        let mmdb = await MmdbReader.open({filename: testDbDir + '/' + name});
        let entries;
        try {
            entries = await Promise.all(ips.map((ip) => mmdb.lookup(ip).then((res) => res.entry)));
        } finally {
            await mmdb.close();
        }
        expect({metadata: mmdb.metadata, entries}).toMatchSnapshot();
    });
}

testDb('MaxMind-DB-test-decoder.mmdb', ['1.1.1.0', '0.0.0.0', '255.255.255.255']);
testDb('MaxMind-DB-test-pointer-decoder.mmdb', ['1.0.0.0', '1.1.1.0']);
testDb('MaxMind-DB-test-nested.mmdb', ['1.1.1.0']);
testDb('MaxMind-DB-test-metadata-pointers.mmdb', []);
