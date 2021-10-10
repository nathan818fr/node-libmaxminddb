# node-libmaxminddb

## Examples

```bash
npm install libmaxminddb
```

```javascript
const {MmdbReader} = require('libmaxminddb');

MmdbReader
    .open({filename: 'GeoIP2-Country-Test.mmdb'})
    .then(async (mmdb) => {
        const {netmask, entry} = await mmdb.lookup('50.114.1.1');
        console.log(entry);
    });
```

For a more detailed example look at [demo.js](./docs/examples/demo.js)
(or [demo.ts](./docs/examples/demo.ts) for TypeScript).
