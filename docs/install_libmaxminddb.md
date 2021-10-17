# Advanced libmaxminddb installation instructions

This package contains only bindings for the libmaxminddb library,
the library itself is needed to build and run the bindings.

## Global package (a.k.a. System package)

By default, if a globally-installed libmaxminddb development package (>= 1.2.0)
is present on your system, it will be used.

It can be located using `pkg-config --modversion libmaxminddb`.

You can compile-it yourself from sources;
or easily install it on your system from your package manager:

-   Debian / Ubuntu: `sudo apt-get install libmaxminddb-dev`
-   CentOS: `sudo yum install libmaxminddb-devel`

If you don't want to use the global package even if it is present, you can:

-   Set the [npm package config](https://docs.npmjs.com/cli/v7/configuring-npm/package-json#config)
    `libmaxminddb_ignore_global` to true
-   (or) Set the environment variable `NODE_LIBMAXMINDDB_IGNORE_GLOBAL` to true

... and then run `npm install` again.

## Prebuilt binaries

If the global package is not available, then a prebuilt version will be used
(provided by [nathan818fr/libmaxminddb-build](https://github.com/nathan818fr/libmaxminddb-build)).

Ready-compiled libmaxminddb binaries are provided for use on the most common platforms:

-   Linux x64 (glibc >= 2.24, musl >= 1.1.24)
-   Linux ARM64 (glibc >= 2.28, musl >= 1.1.24)
-   Linux ARMv7 (glibc >= 2.28)
-   Linux ARMv6 (glibc >= 2.24)
-   macOS x64 (>= 11.0)
-   macOS ARM64 (a.k.a. Apple M1)
-   Windows x64
-   Windows x86

If you want to choose the prebuilt version to use, you can:

-   Set the [npm package config](https://docs.npmjs.com/cli/v7/configuring-npm/package-json#config)
    `libmaxminddb_version` (eg. to `1.6.0-1`)
-   (or) Set the environment variable `NODE_LIBMAXMINDDB_VERSION` (eg. to `1.6.0-1`)

... and then run `npm install` again.

## Custom binaries

Otherwise, you can use your own prebuilt version.

You need to create a `.tar.gz` archive with the following structure:

-   `libmaxminddb/maxminddb.h`
-   `libmaxminddb/maxminddb_config.h`
-   (linux/macos) `libmaxminddb/libmaxminddb.a`
-   (windows) `libmaxminddb/maxminddb.lib`

To use it, set ...

-   the [npm package config](https://docs.npmjs.com/cli/v7/configuring-npm/package-json#config)
    `libmaxminddb_version`
-   (or) the environment variable `NODE_LIBMAXMINDDB_VERSION`

... to a URL of your file.

Supported protocols:

-   `http:` eg. `http://domain.tld/libmaxminddb.tar.gz`
-   `https:` eg. `https://domain.tld/libmaxminddb.tar.gz`
-   `file:` eg. `file:///home/build/libmaxminddb.tar.gz`
    (see [url.fileURLToPath](https://nodejs.org/api/url.html#url_url_fileurltopath_url))

Note that each occurrence of `{{platform}}` in your URL will be replaced by the
[current platform](https://github.com/nathan818fr/node-libmaxminddb/blob/fe73af0ca52e1479f7d359725d0c9352748bb260/install/vars-build.js#L32).
