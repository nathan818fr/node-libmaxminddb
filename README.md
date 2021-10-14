# libmaxminddb

[![npm version](https://img.shields.io/npm/v/libmaxminddb.svg)](https://www.npmjs.com/package/libmaxminddb)
![TypeScript](https://img.shields.io/badge/%3C%2F%3E-TypeScript-%230074c1.svg)
[![Test Status](https://img.shields.io/github/workflow/status/nathan818fr/node-libmaxminddb/Test)](https://github.com/nathan818fr/node-libmaxminddb/actions/workflows/test.yml)

Node.js bindings for [libmaxminddb](https://github.com/maxmind/libmaxminddb). \
To read MaxMind Databases with the best performance (using memory-mapped files)!

This module provides its own TypeScript declarations (.d.ts).

## Installation

```bash
npm install libmaxminddb
```

## Examples

```javascript
const {MmdbReader} = require('libmaxminddb');

MmdbReader.open({filename: 'GeoIP2-Country-Test.mmdb'}).then(async (mmdb) => {
    const {netmask, entry} = await mmdb.lookup('50.114.1.1');
    console.log(entry);
});
```

For a more detailed example look at [demo.js](./docs/examples/demo.js)
(or [demo.ts](./docs/examples/demo.ts) for TypeScript).

## Documentation

For detailed API Documentation, see:
_TODO_

## Advanced libmaxminddb Installation

_TODO: Explain compilation process, system package usage, prebuild binaries, etc_

## Testing

To run the test suite, you first need to clone the submodules.

```bash
git submodule init
git submodule update --recursive
```

Then install the dependencies and run `yarn test`:

```bash
yarn install
yarn test
```

## Contributing

Contributions are welcome.
It is recommended to open an issue before introducing new features to discuss them.

## Versioning

We use [SemVer](http://semver.org/) for versioning.
For the versions available, see the [tags on this repository](https://github.com/nathan818fr/node-libmaxminddb/tags).

## Authors

-   [Nathan Poirier](https://github.com/nathan818fr) &lt;nathan@poirier.io&gt;

## License

This project is licensed under the Apache-2.0 License.
See the [LICENSE](./LICENSE) file for details.
